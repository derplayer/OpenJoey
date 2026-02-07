#include "../../stdafx.h"
#include "yaneGUITextBox.h"

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

namespace yaneuraoGameSDK3rd {
namespace Draw {

///////////////////////////////////////////////////////////////////////////////
// CGUITextBoxArrowButtonListener
///////////////////////////////////////////////////////////////////////////////

CGUITextBoxArrowButtonListener::CGUITextBoxArrowButtonListener(ScrollDirection direction)
    : m_direction(direction) {}

void CGUITextBoxArrowButtonListener::SetTextBox(YTL::smart_ptr<CGUITextBox> textBox) {
    m_vTextBox = textBox;
}

void CGUITextBoxArrowButtonListener::OnLBClick(void) {
    if (m_vTextBox.get()) {
        // FIX: Default to 16 if GetFont() returns NULL or invalid height
        int lineHeight = 16; 
        if (m_vTextBox->GetFont()) {
             int h = m_vTextBox->GetFont()->GetHeight();
             if (h > 0) lineHeight = h;
        }

        if (m_direction == SCROLL_UP) m_vTextBox->ScrollContent(0, -lineHeight);
        else m_vTextBox->ScrollContent(0, lineHeight);
    }
}

///////////////////////////////////////////////////////////////////////////////
// CGUITextBoxSliderListener 
///////////////////////////////////////////////////////////////////////////////

CGUITextBoxSliderListener::CGUITextBoxSliderListener() {
    SetMinSize(15, 15);
}

void CGUITextBoxSliderListener::GetSliderSize(int nX, int nY, int& sx, int& sy) {
    sx = m_nMinX;
    sy = m_nMinY;
}

bool IsLeftButtonDown(YTL::smart_ptr<CGUITextBox> box) {
    if (box.get() == NULL) return false;
    if (box->GetMouse().get() == NULL) return false; 
    int mx, my, mb;
    box->GetMouse()->GetInfo(mx, my, mb);
    return (mb & 2) != 0; 
}

void CGUITextBoxSliderListener::OnPageUp() {
    if (m_vTextBox.get() && IsLeftButtonDown(m_vTextBox)) {
        int step = 50; 
        smart_ptr<CGUISlider> slider = m_vTextBox->GetSlider();
        if (slider.get()) {
            int tx, ty, sx, sy, w, h;
            slider->CalcSliderPos(tx, ty, sx, sy, w, h);
            int mx, my, mb;
            m_vTextBox->GetMouse()->GetInfo(mx, my, mb);

            // Center Alignment Logic
            int thumbCenterY = ty + (sy / 2);
            int screenDist = thumbCenterY - my;
            
            int itemNumX, itemNumY;
            slider->GetItemNum(itemNumX, itemNumY);
            if (h > 0 && screenDist > 0) {
                 long long projectedMove = (long long)screenDist * itemNumY / h;
                 if (projectedMove < step) step = max(1, (int)projectedMove);
            }
        }
        m_vTextBox->ScrollContent(0, -step);
    }
}

void CGUITextBoxSliderListener::OnPageDown() {
    if (m_vTextBox.get() && IsLeftButtonDown(m_vTextBox)) {
        int step = 50;
        smart_ptr<CGUISlider> slider = m_vTextBox->GetSlider();
        if (slider.get()) {
            int tx, ty, sx, sy, w, h;
            slider->CalcSliderPos(tx, ty, sx, sy, w, h);
            int mx, my, mb;
            m_vTextBox->GetMouse()->GetInfo(mx, my, mb);

            // Center Alignment Logic
            int thumbCenterY = ty + (sy / 2);
            int screenDist = my - thumbCenterY; 
            
            int itemNumX, itemNumY;
            slider->GetItemNum(itemNumX, itemNumY);
            if (h > 0 && screenDist > 0) {
                 long long projectedMove = (long long)screenDist * itemNumY / h;
                 if (projectedMove < step) step = max(1, (int)projectedMove);
            }
        }
        m_vTextBox->ScrollContent(0, step);
    }
}

void CGUITextBoxSliderListener::OnPageLeft() {
    if (m_vTextBox.get() && IsLeftButtonDown(m_vTextBox)) {
        m_vTextBox->ScrollContent(-50, 0);
    }
}

void CGUITextBoxSliderListener::OnPageRight() {
    if (m_vTextBox.get() && IsLeftButtonDown(m_vTextBox)) {
        m_vTextBox->ScrollContent(50, 0);
    }
}

///////////////////////////////////////////////////////////////////////////////
// CGUITextBox
///////////////////////////////////////////////////////////////////////////////

CGUITextBox::CGUITextBox()
    : m_nWidth(0), m_nHeight(0), m_nContentWidth(0), m_nContentHeight(0),
      m_nScrollX(0), m_nScrollY(0), m_sliderMode(NO_SLIDER),
      m_nTextOffsetX(0), m_nTextOffsetY(0),
      m_nSliderStripWidth(15), 
      m_nSliderStripHeight(15), 
      m_nMarginX(0), 
      m_nMarginY(0)
{
    m_textColor = ISurface::makeRGB(255, 255, 255, 0);
    ::SetRect(&m_rcTextBoxClip, 0, 0, 0, 0);
}

CGUITextBox::~CGUITextBox() {
}

void CGUITextBox::SetMargins(int x, int y) {
    m_nMarginX = max(0, x);
    m_nMarginY = max(0, y);
    if (m_vTextFastPlane) UpdateTextPlane();
}

void CGUITextBox::GetMargins(int& x, int& y) const {
    x = m_nMarginX;
    y = m_nMarginY;
}

void CGUITextBox::GetSize(int& x, int& y) const {
    x = m_nWidth;
    y = m_nHeight;
}

void CGUITextBox::GetArrowSize(int& x, int& y) const {
    x = m_nArrowWidth;
    y = m_nArrowHeight;
}

void CGUITextBox::SetSliderGFX(smart_ptr<ISurface> sliderThumbGraphic){
    m_vSliderThumbGraphic = sliderThumbGraphic;
    if (m_vSlider.get() && m_vSliderListener.get() && m_vSliderThumbGraphic.get()) {
        CGUINormalSliderListener* pSliderListener = static_cast<CGUINormalSliderListener*>(m_vSliderListener.get());
        m_vSliderListener->SetMinSizeFromGraphic(m_vSliderThumbGraphic.get()); 
        pSliderListener->SetPlane(m_vSliderThumbGraphic);

        int actualThumbWidth = 0;
        int actualThumbHeight = 0;
        m_vSliderThumbGraphic->GetSize(actualThumbWidth, actualThumbHeight); 
        m_nSliderStripWidth = actualThumbWidth;
        m_nSliderStripHeight = actualThumbHeight;

        RECT sliderMovementRect;
        int effectiveWidth = m_nWidth - (m_nMarginX * 2); 
        int effectiveHeight = m_nHeight - (m_nMarginY * 2);
        ::SetRect(&sliderMovementRect, 0, 0, effectiveWidth, effectiveHeight); 

        if (m_sliderMode == VERTICAL_SLIDER || m_sliderMode == BOTH_SLIDERS) {
            sliderMovementRect.left = effectiveWidth - m_nSliderStripWidth; 
            sliderMovementRect.right = effectiveWidth;
            int buttonSize = m_nSliderStripWidth;
            sliderMovementRect.top = buttonSize;
            sliderMovementRect.bottom = effectiveHeight - buttonSize;
            if (m_sliderMode == BOTH_SLIDERS) sliderMovementRect.bottom -= m_nSliderStripHeight; 
        }
        if (m_sliderMode == HORIZONTAL_SLIDER || m_sliderMode == BOTH_SLIDERS) {
            sliderMovementRect.top = effectiveHeight - m_nSliderStripHeight;
            sliderMovementRect.bottom = effectiveHeight;
            if (m_sliderMode == BOTH_SLIDERS) sliderMovementRect.right -= m_nSliderStripWidth; 
        }
        m_vSlider->SetRect(&sliderMovementRect);
    }
}

void CGUITextBox::SetArrowGFX(smart_ptr<CPlaneLoader> pv, int upIndex, int downIndex) {
    if (m_vScrollUpButton.get() && m_vScrollDownButton.get() && pv.get()) {
        YTL::smart_ptr<CGUITextBox> this_ptr(this, false);

        if (m_vScrollUpButtonListener.isNull()) {
            m_vScrollUpButtonListener = YTL::smart_ptr<CGUITextBoxArrowButtonListener>(new CGUITextBoxArrowButtonListener(SCROLL_UP));
            m_vScrollUpButtonListener->SetTextBox(this_ptr);
            m_vScrollUpButton->SetEvent(YTL::smart_ptr_static_cast<CGUIButtonEventListener>(m_vScrollUpButtonListener));
        }
        if (m_vScrollDownButtonListener.isNull()) {
            m_vScrollDownButtonListener = YTL::smart_ptr<CGUITextBoxArrowButtonListener>(new CGUITextBoxArrowButtonListener(SCROLL_DOWN));
            m_vScrollDownButtonListener->SetTextBox(this_ptr);
            m_vScrollDownButton->SetEvent(YTL::smart_ptr_static_cast<CGUIButtonEventListener>(m_vScrollDownButtonListener));
        }

        CGUINormalButtonListener* upListener = static_cast<CGUINormalButtonListener*>(m_vScrollUpButtonListener.get());
        CGUINormalButtonListener* downListener = static_cast<CGUINormalButtonListener*>(m_vScrollDownButtonListener.get());
        pv->SetColorKey(ISurface::makeRGB(255, 255, 255, 0)); 
        upListener->SetPlaneLoader(pv, upIndex);
        downListener->SetPlaneLoader(pv, downIndex);

        m_nArrowWidth = 0;
        m_nArrowHeight = 0;
        CPlane upPlane = pv->GetPlane(upIndex);
        if(upPlane.get()) upPlane->GetSize(m_nArrowWidth, m_nArrowHeight);

        m_vScrollUpButton->SetXY(m_nX + m_nWidth - m_nSliderStripWidth - m_nMarginX, m_nY + m_nMarginY);
        m_vScrollDownButton->SetXY(m_nX + m_nWidth - m_nSliderStripWidth - m_nMarginX, m_nY + m_nHeight - m_nArrowHeight - m_nMarginY);
    }
}

void CGUITextBox::SetSliderLoader(string folder, string path){
        CGUINormalSliderListener* pSliderListener = static_cast<CGUINormalSliderListener*>(m_vSliderListener.get());
        CPlaneLoader m_vPlaneScrollLoader;
        m_vPlaneScrollLoader.SetReadDir(folder);
        if (m_vPlaneScrollLoader.Set(path, false) != 0) {
            OutputDebugStringA("Error: Failed to load slider gfx\n");
            return;
        }
        CPlane pln = m_vPlaneScrollLoader.GetPlane(5);
        smart_ptr<ISurface> plnPtr(pln.get(), false);
        m_vSliderThumbGraphic = plnPtr;
        pSliderListener->SetPlane(m_vSliderThumbGraphic);
}

void CGUITextBox::Create(int x, int y, int width, int height, SliderMode mode) {
    SetXY(x, y);
    m_nWidth = width;
    m_nHeight = height;
    m_sliderMode = mode;

    ::SetRect(&m_rcTextBoxClip, x + m_nMarginX, y + m_nMarginY, x + width - m_nMarginX, y + height - m_nMarginY);

    m_vTextFastPlane = new CTextFastPlaneEx;
    m_vTextPlane = CPlane(m_vTextFastPlane);

    if (m_sliderMode != NO_SLIDER) {
        m_vSlider = YTL::smart_ptr<CGUISlider>(new CGUISlider());
        m_vSliderListener = YTL::smart_ptr<CGUITextBoxSliderListener>(new CGUITextBoxSliderListener());

        YTL::smart_ptr<CGUITextBox> this_ptr(this, false);
        m_vSliderListener->SetTextBox(this_ptr);

        m_vSlider->SetEvent(YTL::smart_ptr_static_cast<CGUISliderEventListener>(m_vSliderListener));
        m_vSlider->SetType(m_sliderMode);
        m_vSlider->SetXY(m_nX + m_nMarginX, m_nY + m_nMarginY);

        if (m_sliderMode == VERTICAL_SLIDER || m_sliderMode == BOTH_SLIDERS) {
            m_vScrollUpButton = YTL::smart_ptr<CGUIButton>(new CGUIButton());
            m_vScrollDownButton = YTL::smart_ptr<CGUIButton>(new CGUIButton());
        }

        RECT tempSliderRect;
        const int defaultSliderStripSize = 15;
        int effectiveWidth = m_nWidth - (m_nMarginX * 2);
        int effectiveHeight = m_nHeight - (m_nMarginY * 2);

        if (m_sliderMode == VERTICAL_SLIDER || m_sliderMode == BOTH_SLIDERS) {
            ::SetRect(&tempSliderRect, effectiveWidth - defaultSliderStripSize, 0 + defaultSliderStripSize, effectiveWidth, effectiveHeight - defaultSliderStripSize);
            RECT upButtonBounds = { effectiveWidth - defaultSliderStripSize, 0, effectiveWidth, defaultSliderStripSize };
            m_vScrollUpButton->SetBounds(upButtonBounds);
            RECT downButtonBounds = { effectiveWidth - defaultSliderStripSize, effectiveHeight - defaultSliderStripSize, effectiveWidth, effectiveHeight };
            m_vScrollDownButton->SetBounds(downButtonBounds);
        } else if (m_sliderMode == HORIZONTAL_SLIDER) {
            ::SetRect(&tempSliderRect, 0, effectiveHeight - defaultSliderStripSize, effectiveWidth, effectiveHeight);
        }
        m_vSlider->SetRect(&tempSliderRect);
    }
}

void CGUITextBox::SetText(const string& text) {
    if (m_strCurrentText != text) {
        m_strCurrentText = text;
        UpdateTextPlane();
    }
}

string CGUITextBox::GetText() const { return m_strCurrentText; }

void CGUITextBox::SetFont(YTL::smart_ptr<CFont> font) {
    if (m_vTextFastPlane != NULL) {
        m_vTextFastPlane->SetBaseFontSize(font->GetSize());
        UpdateTextPlane();
    }
}

CFont* CGUITextBox::GetFont() { return NULL; }

void CGUITextBox::SetTextColor(ISurfaceRGB color) {
    m_textColor = color;
    if (m_vTextFastPlane != NULL) UpdateTextPlane();
}

void CGUITextBox::SetTextOffset(int x, int y) {
    m_nTextOffsetX = x;
    m_nTextOffsetY = y;
}

void CGUITextBox::SetBackgroundPlane(YTL::smart_ptr<ISurface> pv) {
    m_vBackgroundPlane = pv;
}

void CGUITextBox::UpdateTextPlane() {
    if (m_vTextFastPlane != NULL) {
        int textAvailableWidth = m_nWidth - (m_nMarginX * 2);
        if (m_sliderMode == VERTICAL_SLIDER || m_sliderMode == BOTH_SLIDERS) textAvailableWidth -= m_nSliderStripWidth;
        if (textAvailableWidth < 0) textAvailableWidth = 0;

        m_vTextFastPlane->SetWrapWidth(textAvailableWidth);
        m_vTextFastPlane->SetBaseFontSize(13);
        m_vTextFastPlane->SetTextRich(m_strCurrentText);
        m_vTextFastPlane->UpdateText(); 
        CalculateVisibleContentSize(); 
    }

    if (m_vSlider.get()) {
        int effectiveDisplayWidth = m_nWidth - (m_nMarginX * 2);
        int effectiveDisplayHeight = m_nHeight - (m_nMarginY * 2);

        m_nScrollX = max(0, min(m_nScrollX, max(0, m_nContentWidth - effectiveDisplayWidth)));
        m_nScrollY = max(0, min(m_nScrollY, max(0, m_nContentHeight - effectiveDisplayHeight)));

        int itemsX = 1;
        int itemsY = 1;
        if (m_sliderMode == HORIZONTAL_SLIDER || m_sliderMode == BOTH_SLIDERS) itemsX = max(1, m_nContentWidth - effectiveDisplayWidth + 1);
        if (m_sliderMode == VERTICAL_SLIDER || m_sliderMode == BOTH_SLIDERS) itemsY = max(1, m_nContentHeight - effectiveDisplayHeight + 1);

        m_vSlider->SetItemNum(itemsX, itemsY);
        m_vSlider->SetSelectedItem(m_nScrollX, m_nScrollY);
    }
}

void CGUITextBox::CalculateVisibleContentSize() {
    if (m_vTextFastPlane != NULL) m_vTextFastPlane->GetSize(m_nContentWidth, m_nContentHeight);
    else {
        m_nContentWidth = 0;
        m_nContentHeight = 0;
    }
}

LRESULT CGUITextBox::OnSimpleMove(ISurface* lp) {
    LRESULT result = 0;
    int effectiveDisplayHeight = m_nHeight - (m_nMarginY * 2);
    bool needsVerticalScroll = m_nContentHeight > effectiveDisplayHeight;

    if (m_vSlider.get() && m_pvMouse.get()) {
        int oldSliderX, oldSliderY;
        m_vSlider->GetSelectedItem(oldSliderX, oldSliderY);

        result |= m_vSlider->OnSimpleMove(lp);
        
        int newSliderX, newSliderY;
        m_vSlider->GetSelectedItem(newSliderX, newSliderY);

        if (oldSliderX != newSliderX || oldSliderY != newSliderY) {
            m_nScrollX = newSliderX;
            m_nScrollY = newSliderY;
            result |= 1;
        }

        // FIX: Allow wheel scrolling on BOTH sliders + Vertical
        if ((m_sliderMode == VERTICAL_SLIDER || m_sliderMode == BOTH_SLIDERS) && needsVerticalScroll) {
            if (m_vScrollUpButton.get()) result |= m_vScrollUpButton->OnSimpleMove(lp);
            if (m_vScrollDownButton.get()) result |= m_vScrollDownButton->OnSimpleMove(lp);

            RECT mouseCheckRect = {m_nX, m_nY, m_nX + m_nWidth, m_nY + m_nHeight};
            int mx, my;
            GetMouse()->GetXY(mx, my);
            POINT pt = {mx, my};

            if (::PtInRect(&mouseCheckRect, pt)) {
                int scrollDeltaY = 0;
                // Use a modest step size
                int step = 20; 

                if (GetMouse()->IsWheelUp()) scrollDeltaY = -step;
                else if (GetMouse()->IsWheelDown()) scrollDeltaY = step;

                if (scrollDeltaY != 0) {
                    ScrollContent(0, scrollDeltaY);
                    result |= 1;

                    // *** CRITICAL FIX: CONSUME THE EVENT ***
                    // Without this, the wheel flag stays true for 60 frames = HUGE SCROLL
                    GetMouse()->ResetButton(); 
                }
            }
        }
    }
    return result;
}

LRESULT CGUITextBox::OnSimpleDraw(ISurface* lp) {
    int drawX = m_nX + m_nXOffset;
    int drawY = m_nY + m_nYOffset;

    if (!m_vBackgroundPlane.isNull()) {
        SIZE destSize = { m_nWidth, m_nHeight };
        lp->BltNatural(m_vBackgroundPlane.get(), drawX, drawY, &destSize, NULL, &m_rcTextBoxClip, 0);
    }

    if (m_vTextPlane.get()) {
        RECT srcRect;
        ::SetRect(&srcRect, m_nScrollX, m_nScrollY, m_nScrollX + m_nWidth - (m_nMarginX * 2), m_nScrollY + m_nHeight - (m_nMarginY * 2));

        srcRect.right = min(srcRect.right, m_nContentWidth);
        srcRect.bottom = min(srcRect.bottom, m_nContentHeight);
        
        RECT textClippingRect = m_rcTextBoxClip; 
        if (m_sliderMode == VERTICAL_SLIDER || m_sliderMode == BOTH_SLIDERS) textClippingRect.right -= m_nSliderStripWidth;
        if (m_sliderMode == HORIZONTAL_SLIDER || m_sliderMode == BOTH_SLIDERS) textClippingRect.bottom -= m_nSliderStripHeight;
        
        if (textClippingRect.left > textClippingRect.right) textClippingRect.right = textClippingRect.left;
        if (textClippingRect.top > textClippingRect.bottom) textClippingRect.bottom = textClippingRect.top;

        lp->BltNatural(m_vTextPlane.get(), drawX + m_nTextOffsetX + m_nMarginX, drawY + m_nTextOffsetY + m_nMarginY, NULL, &srcRect, &textClippingRect, 0);
    }

    int effectiveDisplayHeight = m_nHeight - (m_nMarginY * 2);
    bool needsVerticalScroll = m_nContentHeight > effectiveDisplayHeight;

    if (m_sliderMode == VERTICAL_SLIDER && needsVerticalScroll) {
        if (m_vScrollUpButton.get()) m_vScrollUpButton->OnSimpleDraw(lp);
        if (m_vScrollDownButton.get()) m_vScrollDownButton->OnSimpleDraw(lp);
        if (m_vSlider.get()) m_vSlider->OnSimpleDraw(lp);
    }
    return 0;
}

void CGUITextBox::Reset() {
    IGUIParts::Reset();
    m_strCurrentText = "";
    m_nScrollX = 0;
    m_nScrollY = 0;
    m_nContentWidth = 0;
    m_nContentHeight = 0;
    m_nMarginX = 0;
    m_nMarginY = 0;

    if (m_vTextFastPlane != NULL) m_vTextFastPlane->UpdateTextAA();
    if (m_vSlider.get()) m_vSlider->Reset();
    if (m_vScrollUpButton.get()) m_vScrollUpButton->Reset();
    if (m_vScrollDownButton.get()) m_vScrollDownButton->Reset();
}

void CGUITextBox::SetMouse(YTL::smart_ptr<CFixMouse> pv) {
    IGUIParts::SetMouse(pv);
    if (m_vSlider.get()) m_vSlider->SetMouse(pv);
    if (m_vScrollUpButton.get()) m_vScrollUpButton->SetMouse(pv);
    if (m_vScrollDownButton.get()) m_vScrollDownButton->SetMouse(pv);
}

void CGUITextBox::ScrollContent(int dx, int dy) {
    m_nScrollX += dx;
    m_nScrollY += dy;

    int effectiveDisplayWidth = m_nWidth - (m_nMarginX * 2);
    int effectiveDisplayHeight = m_nHeight - (m_nMarginY * 2);

    m_nScrollX = max(0, min(m_nScrollX, max(0, m_nContentWidth - effectiveDisplayWidth)));
    m_nScrollY = max(0, min(m_nScrollY, max(0, m_nContentHeight - effectiveDisplayHeight)));

    if (m_vSlider.get()) {
        m_vSlider->SetSelectedItem(m_nScrollX, m_nScrollY);
    }
}

std::string CGUITextBox::TrimLeadingSpaces(const std::string& s) {
    size_t first = s.find_first_not_of(" \t\n\r\f\v");
    if (std::string::npos == first) return s;
    return s.substr(first);
}

} // namespace Draw
} // namespace yaneuraoGameSDK3rd