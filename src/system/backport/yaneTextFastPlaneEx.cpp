//
// yaneTextFastPlaneEx.cpp : Extended text plane for rich text formatting.
//
// This file implements CTextFastPlaneEx, which inherits from CTextFastPlane
// to provide rich text layout and rendering.
//

#include "yaneTextFastPlaneEx.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

// =======================================================================================
// CTextFastPlaneEx Class Implementation
// =======================================================================================

// Constructor: Initializes the base class and other members.
CTextFastPlaneEx::CTextFastPlaneEx(CFastDraw* pFastDraw)
    : CFastPlane(pFastDraw), m_nWrapWidth(0) {
    // The rich text parser and other members are constructed automatically.
    // Base class members (m_Font, m_nTextX, m_nTextY) are also initialized.
	SetDefaultColor(RGB(1,1,1)); // default black
}

// Destructor.
CTextFastPlaneEx::~CTextFastPlaneEx() {
    // Clean up resources if necessary.
}

// Sets the rich text string to be parsed and rendered.
void CTextFastPlaneEx::SetTextRich(const std::string& text) {
    m_strRichText = text;
}

// Sets the base font size for the parser.
void CTextFastPlaneEx::SetBaseFontSize(int nFontSize) {
    m_parser.SetBaseFontSize(nFontSize);
}

// Set default color
void CTextFastPlaneEx::SetDefaultColor(COLORREF rgb) {
    m_parser.SetDefaultColor(rgb);
}

// Update the text and redraw the surface (standard drawing).
LRESULT CTextFastPlaneEx::UpdateText() {
    // We pass false for both antialias and blend flags.
    return UpdateRichText(false, false);
}

// Update the text and redraw with basic antialiasing.
LRESULT CTextFastPlaneEx::UpdateTextA() {
    // We pass true for antialias and false for blend.
    return UpdateRichText(true, false);
}

// Update the text and redraw with full antialiasing and blending.
LRESULT CTextFastPlaneEx::UpdateTextAA() {
    // We pass true for both antialias and blend flags.
    return UpdateRichText(true, true);
}

// The OnDraw method for the base class is not used in this rich text implementation.
LRESULT CTextFastPlaneEx::OnDraw() {
    return 0;
}

// This is the core logic that handles rich text parsing, layout, and drawing.
LRESULT CTextFastPlaneEx::UpdateRichText(bool bAntialias, bool bBlend) {
    // We must have a valid wrapping width.
    if (m_nWrapWidth <= 0) {
        return 1;
    }

    // Clear previous layout data before generating a new layout.
    m_vLines.clear();
    m_parser.SetText(m_strRichText);
    m_parser.SetBaseFontSize(m_parser.GetBaseFontSize());
    
    CRichTextLine currentLine;
    // Set initial alignment for the very first line
    currentLine.alignment = m_parser.GetContext().m_nAlign; 

    CRichTextSegment segment;
    int currentLineHeight = 0;
    int totalLayoutHeight = 0;

    // Loop through all text segments to measure and lay them out.
    while (m_parser.GetNextSegment(segment) == 0) {
        // Handle explicit line breaks.
        if (segment.text == "<BR>") {
            currentLine.alignment = m_parser.GetContext().m_nAlign; // Update before pushing
            currentLine.totalHeight = currentLineHeight;
            m_vLines.push_back(currentLine);
            totalLayoutHeight += currentLineHeight;
            
            // Reset for new line
            currentLine = CRichTextLine();
            currentLine.alignment = m_parser.GetContext().m_nAlign;
            currentLineHeight = 0;
            continue;
        }

        // Handle horizontal rule.
        if (segment.text == "<HR>") {
            currentLine.alignment = m_parser.GetContext().m_nAlign;
            currentLine.totalHeight = currentLineHeight;
            m_vLines.push_back(currentLine);
            totalLayoutHeight += currentLineHeight;
            
            currentLine = CRichTextLine();
            currentLine.alignment = m_parser.GetContext().m_nAlign; 
            currentLineHeight = 0;

            CRichTextLine hrLine;
            hrLine.totalHeight = 5; 
            m_vLines.push_back(hrLine);
            totalLayoutHeight += 5;
            continue;
        }

        // Create a temporary CFont object to measure the segment's size.
        CFont tempFont;
        tempFont.SetSize(segment.context.m_nFontSize);
        tempFont.SetFont(segment.context.m_nFontNo);
        tempFont.SetColor(segment.context.m_rgbColor);
        tempFont.SetWeight(segment.context.m_bBold ? 700 : 300);
        tempFont.SetItalic(segment.context.m_bItalic);
        tempFont.SetUnderLine(segment.context.m_bUnderLine);
        tempFont.SetStrikeOut(segment.context.m_bStrikeOut);
        tempFont.SetText(segment.text);

        int segmentWidth, segmentHeight;
        tempFont.GetSize(segmentWidth, segmentHeight);
        
        // If the line is practically empty, adopt the alignment of the 
        // current segment. This allows tags like <RIGHT> at the start of 
        // a line to take effect immediately.
        if (currentLine.totalWidth == 0) {
            currentLine.alignment = segment.context.m_nAlign;
        }

        // Check if the segment fits on the current line. If not, start a new line.
        if (currentLine.totalWidth + segmentWidth > m_nWrapWidth && !currentLine.segments.empty()) {
            currentLine.totalHeight = currentLineHeight;
            m_vLines.push_back(currentLine);
            totalLayoutHeight += currentLineHeight;
            
            currentLine = CRichTextLine();
            currentLine.alignment = m_parser.GetContext().m_nAlign; // Align for the NEW line
            currentLineHeight = 0;
        }
        
        // Add the segment to the current line.
        segment.width = segmentWidth;
        segment.height = segmentHeight;
        currentLine.segments.push_back(segment);
        currentLine.totalWidth += segment.width;
        
        // Update the line's height to be the height of the tallest segment.
        if (segment.height > currentLineHeight) {
            currentLineHeight = segment.height;
        }
    }
    
    // Push the final line if it's not empty.
    if (!currentLine.segments.empty()) {
        // Ensure final line gets latest alignment if it was still empty/processing
        if (currentLine.totalWidth == 0) currentLine.alignment = m_parser.GetContext().m_nAlign;
        
        currentLine.totalHeight = currentLineHeight;
        m_vLines.push_back(currentLine);
        totalLayoutHeight += currentLineHeight;
    }

    // Now that we know the final size, we can resize the actual surface.
    SetSize(m_nWrapWidth, totalLayoutHeight);
    bool createAlphaSurface = bAntialias || bBlend;
    CreateSurface(m_nWrapWidth,totalLayoutHeight, createAlphaSurface);

    // Clear the surface with the background color from the parser context.
    SetFillColor(m_parser.GetContext().m_rgbColorBk);
    Clear();

    // Call the final drawing function.
    DrawLayout(bAntialias, bBlend);
    
    return 0;
}

// Draws the laid-out lines onto the surface.
void CTextFastPlaneEx::DrawLayout(bool antialias, bool blend) {
    int currentY = 0; //m_nTextY

    for (size_t i = 0; i < m_vLines.size(); ++i) {
        const CRichTextLine& line = m_vLines[i];
		int currentX = 0; //m_nTextX

        // Adjust horizontal position based on alignment.
        if (line.alignment == 1) { // Center
            currentX += (m_nWrapWidth - line.totalWidth) / 2;
        } else if (line.alignment == 2) { // Right
            currentX += m_nWrapWidth - line.totalWidth;
        }
        
        // Draw each segment of the line.
        for (size_t j = 0; j < line.segments.size(); ++j) {
            const CRichTextSegment& segment = line.segments[j];
            
			CTextFastPlane* pTextPtr = new CTextFastPlane;
			CFont* tempFont = pTextPtr->GetFont();
			tempFont->SetSize(segment.context.m_nFontSize);
			tempFont->SetColor(segment.context.m_rgbColor);
			tempFont->SetWeight(segment.context.m_bBold ? FW_BOLD : FW_NORMAL);
			tempFont->SetItalic(segment.context.m_bItalic);
			tempFont->SetUnderLine(segment.context.m_bUnderLine);
			tempFont->SetStrikeOut(segment.context.m_bStrikeOut);
			tempFont->SetText(segment.text);
			tempFont->SetShadowOffset(segment.context.m_nShadowOffset.cx, segment.context.m_nShadowOffset.cy);
			tempFont->SetFont(segment.context.m_nFontNo);

			// TODO: DUMMY MOCKUP (DO IT PROPERLY LATER)
			tempFont->SetLetterSpacing(-1);
			//tempFont->SetHeight(18); // Adjust for desired line spacing, e.g., 15-17 for 12pt font
			//tempFont->SetSize(13);

			if(antialias == false && blend == false)
				pTextPtr->UpdateText();
			if(antialias && blend == false)
				pTextPtr->UpdateTextA();
			if(antialias && blend)
				pTextPtr->UpdateTextAA();
			if(antialias == false && blend)
				printf("wtf");

			BltFast(pTextPtr, currentX, currentY);
            currentX += segment.width;
        }
        currentY += line.totalHeight;
        if (i < m_vLines.size() - 1) {
            currentY += m_parser.GetContext().m_nBlankHeight;
        }
    }
}


// =======================================================================================
// CRichTextParser Class Implementation
// =======================================================================================

// Constructor: Initializes the parser's state.
CRichTextParser::CRichTextParser()
    : m_lpStr(NULL), m_lpTextAdr(NULL), m_nBaseSize(12) {
    m_contextStack.push(CRichTextContext());
    m_context = m_contextStack.top();
}

// Sets the rich text string and resets the parser state.
void CRichTextParser::SetText(const std::string& text) {
    m_text = text;
    m_lpStr = m_text.c_str();
    m_lpTextAdr = m_text.c_str();
    
    // Clear the context stack and push the default context.
    while (!m_contextStack.empty()) m_contextStack.pop();

    // Use the stored default color when initializing the context.
    CRichTextContext defaultContext;
    defaultContext.m_rgbColor = m_defaultColor;
    
    m_contextStack.push(defaultContext);
    m_context = m_contextStack.top();
}

// Parses and returns the next text segment or tag.
LRESULT CRichTextParser::GetNextSegment(CRichTextSegment& segment) {
    segment.text.clear();
    // Do NOT assign segment.context here. It will be assigned after parsing.
    
    if (*m_lpStr == '\0') {
        return 3;
    }
    
    // Process newlines as explicit line breaks FIRST.
    if (*m_lpStr == '\n' || *m_lpStr == '\r') {
        segment.text = "<BR>";
        while (*m_lpStr == '\n' || *m_lpStr == '\r') {
            m_lpStr++;
        }
        segment.context = m_context; // Assign current context for <BR>
        return 0;
    }

    // Now, handle any other leading whitespace before tags or words.
    SkipSpace(m_lpStr);
    
    // Check for tags
    if (*m_lpStr == '<') {
        LPCSTR start = m_lpStr + 1;
        LPCSTR end = strchr(start, '>');
        if (end == NULL) {
            m_lpStr = m_text.c_str() + m_text.length();
            return 1;
        }

        std::string tagContent(start, end - start);
        m_lpStr = end + 1;

        if (tagContent[0] == '/') {
            if (m_contextStack.size() > 1) {
                m_contextStack.pop();
                m_context = m_contextStack.top();
            }
        } else {
			LPCSTR lpAttr = tagContent.c_str();
	        

			if (IsToken(lpAttr, "CENTER")) {
				m_contextStack.push(m_context);
				m_context.m_nAlign = 1;
			} else if (IsToken(lpAttr, "RIGHT")) {
				m_contextStack.push(m_context);
				m_context.m_nAlign = 2;
			} else if (IsToken(lpAttr, "LEFT")) {
				m_contextStack.push(m_context);
				m_context.m_nAlign = 0;
			} 
			else if (IsToken(lpAttr, "FONT=")) { 
				m_contextStack.push(m_context);
				int nFontNo;
				if (GetStrNum(lpAttr, nFontNo) != 0) return 3;
				m_context.m_nFontNo = nFontNo;
			} 
			else if (IsToken(lpAttr, "SIZE=")) { 
				m_contextStack.push(m_context);
				int fontSizeRate = 0;
				GetStrNum(lpAttr, fontSizeRate);
				m_context.m_nFontSize = m_nBaseSize + fontSizeRate;
			} else if (IsToken(lpAttr, "COLOR=")) { 
				m_contextStack.push(m_context);
				GetStrColor(lpAttr, m_context.m_rgbColor);
			} else if (IsToken(lpAttr, "BOLD")) { 
				m_contextStack.push(m_context);
				m_context.m_bBold = true;
			} else if (IsToken(lpAttr, "ITALIC")) {
				m_contextStack.push(m_context);
				m_context.m_bItalic = true;
			} else if (IsToken(lpAttr, "U")) {
				m_contextStack.push(m_context);
				m_context.m_bUnderLine = true;
			} else if (IsToken(lpAttr, "S")) {
				m_contextStack.push(m_context);
				m_context.m_bStrikeOut = true;
			} else if (IsToken(lpAttr, "BR")) { segment.text = "<BR>"; } 
			else if (IsToken(lpAttr, "HR")) { segment.text = "<HR>"; }
        }
        segment.context = m_context; // Assign the LATEST context after tag processing
        return 0;
    }
    
    // Correctly parse a single word including punctuation.
    std::string word;
    LPCSTR start = m_lpStr;
    
    while (*m_lpStr != '\0' && *m_lpStr != '<' && !isspace(*m_lpStr)) {
        m_lpStr++;
    }
    
    if (m_lpStr > start) {
        segment.text.assign(start, m_lpStr - start);
        
        // After parsing the word, check if it is followed by a space.
        if (*m_lpStr == ' ' || *m_lpStr == '\t') {
            segment.trailingSpace = true;
            m_lpStr++; // Consume the space
        } else {
            segment.trailingSpace = false;
        }
        segment.context = m_context;
        return 0;
    }
    
    return 1;
}

// New helper function to parse attributes within a tag string.
void CRichTextParser::ParseTagAttributes(const std::string& tagContent, CRichTextContext& context) {
    std::string::size_type pos = 0;
    std::string::size_type end = tagContent.length();

    // The tag name is the first word
    std::string::size_type tagNameEnd = tagContent.find_first_of(" \t");
    std::string tagName;
    if (tagNameEnd != std::string::npos) {
        tagName = tagContent.substr(0, tagNameEnd);
        pos = tagContent.find_first_not_of(" \t", tagNameEnd);
    } else {
        tagName = tagContent;
    }

    // Process single-attribute tags
    if (stricmp(tagName.c_str(), "BOLD") == 0) {
        context.m_bBold = true;
    } else if (stricmp(tagName.c_str(), "ITALIC") == 0) {
        context.m_bItalic = true;
    } else if (stricmp(tagName.c_str(), "U") == 0) {
        context.m_bUnderLine = true;
    } else if (stricmp(tagName.c_str(), "S") == 0) {
        context.m_bStrikeOut = true;
    } else if (stricmp(tagName.c_str(), "CENTER") == 0) {
        context.m_nAlign = 1;
    } else if (stricmp(tagName.c_str(), "RIGHT") == 0) {
        context.m_nAlign = 2;
    }

    // Now parse attributes for multi-attribute tags like FONT
    if (stricmp(tagName.c_str(), "FONT") == 0 && pos != std::string::npos) {
        std::string attributes = tagContent.substr(pos);
        LPCSTR lpAttr = attributes.c_str();

        while (*lpAttr != '\0') {
            SkipSpace(lpAttr);
            if (*lpAttr == '\0') break;
            
            if (IsToken(lpAttr, "SIZE=")) {
                int fontSizeRate = 0;
                GetStrNum(lpAttr, fontSizeRate);
                context.m_nFontSize = m_nBaseSize + fontSizeRate;
            } else if (IsToken(lpAttr, "COLOR=")) {
                GetStrColor(lpAttr, context.m_rgbColor);
            }
            // Add other attribute parsers here
            // e.g., else if (IsToken(lpAttr, "BGCOLOR=")) { ... }
            
            // Skip to the next potential attribute
            SkipTo(lpAttr, " ");
        }
    }
}

// CStringScanner helper function implementations
bool CRichTextParser::IsToken(LPCSTR& lp, LPCSTR lp2) {
    LPCSTR temp = lp;
    while (*lp2 != '\0' && toupper(*temp) == toupper(*lp2)) {
        temp++;
        lp2++;
    }
    if (*lp2 == '\0') {
        lp = temp;
        return true;
    }
    return false;
}

LRESULT CRichTextParser::SkipTo(LPCSTR& lp, LPCSTR lp2) {
    LPCSTR temp = lp;
    while (*temp != '\0' && !IsToken(temp, lp2)) {
        temp++;
    }
    if (*temp == '\0') {
        lp = temp;
        return 1;
    }
    lp = temp;
    return 0;
}

LRESULT CRichTextParser::SkipTo2(LPCSTR& lp, LPCSTR lp2, char* lp3, size_t buf_size) {
    LPCSTR start = lp;
    if (SkipTo(lp, lp2) != 0) {
        return 1;
    }
    
    size_t len = lp - start;
    if (len >= buf_size) {
        len = buf_size - 1;
    }
    
    memcpy(lp3, start, len);
    lp3[len] = '\0';
    
    return 0;
}

LRESULT CRichTextParser::SkipSpace(LPCSTR& lp) {
    while (*lp == ' ' || *lp == '\t' || *lp == '\n' || *lp == '\r') {
        lp++;
    }
    return (*lp == '\0') ? 1 : 0;
}

LRESULT CRichTextParser::GetStrNum(LPCSTR& lp, int& nRate) {
    SkipSpace(lp);
    bool bNegative = false;
    if (*lp == '-') {
        bNegative = true;
        lp++;
    } else if (*lp == '+') {
        lp++;
    }
    if (!isdigit(*lp)) return 1;
    nRate = 0;
    while (isdigit(*lp)) {
        nRate = nRate * 10 + (*lp - '0');
        lp++;
    }
    if (bNegative) nRate = -nRate;
    return 0;
}

LRESULT CRichTextParser::GetStrColor(LPCSTR& lp, COLORREF& nFontColor) {
    SkipSpace(lp);

    if (*lp == '#') {
        // Handle hex variant
        lp++;
        
        char hex[7];
        for (int i = 0; i < 6; ++i) {
            if (!isxdigit(*lp)) return 1;
            hex[i] = *lp;
            lp++;
        }
        hex[6] = '\0';
        
        unsigned long value = strtoul(hex, NULL, 16);
        nFontColor = RGB((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
        
        return 0;
    } else if (isdigit(*lp)) {
        // Handle comma-separated variant
        int r = 0, g = 0, b = 0;
        if (GetNum(lp, r) != 0) return 1;
        if (*lp != ',') return 1;
        lp++;
        if (GetNum(lp, g) != 0) return 1;
        if (*lp != ',') return 1;
        lp++;
        if (GetNum(lp, b) != 0) return 1;
        
        nFontColor = RGB(r, g, b);
        return 0;
    }

    return 1; // Return an error if the format is not recognized
}

LRESULT CRichTextParser::GetNum(LPCSTR& lp, int& nVal) {
    SkipSpace(lp);
    nVal = 0;
    while (isdigit(*lp)) {
        nVal = nVal * 10 + (*lp - '0');
        lp++;
    }
    return 0;
}

} // end of namespace Draw
} // end of namespace yaneuraoGameSDK3rd
