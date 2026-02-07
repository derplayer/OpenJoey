#ifndef __yaneGUITextBox_h__
#define __yaneGUITextBox_h__

#include "../../stdafx.h"
#include "yaneGUIParts.h"     // For IGUIParts base class
#include "yaneGUIButton.h"
#include "yaneGUISlider.h"    // For CGUISlider and its base classes
#include "yaneTextFastPlaneEx.h"

namespace yaneuraoGameSDK3rd {
namespace Draw {

// Forward declaration
class CGUITextBox;

// Custom slider listener for the textbox
class CGUITextBoxSliderListener : public CGUINormalSliderListener {
public:
    CGUITextBoxSliderListener();
    void SetTextBox(smart_ptr<CGUITextBox> pv) { m_vTextBox = pv; }

    void SetMinSizeFromGraphic(ISurface* graphic) {
        int sx, sy;
        graphic->GetSize(sx, sy); 
        SetMinSize(sx, sy); 
    }

    // --- ADDED THIS DECLARATION ---
    virtual void GetSliderSize(int nX, int nY, int& sx, int& sy);

    virtual void OnPageUp();
    virtual void OnPageDown();
    virtual void OnPageLeft();
    virtual void OnPageRight();

protected:
    smart_ptr<CGUITextBox> m_vTextBox;
};

enum ScrollDirection {
    SCROLL_UP,
    SCROLL_DOWN
};

class CGUITextBoxArrowButtonListener : public CGUINormalButtonListener {
public:
    CGUITextBoxArrowButtonListener(ScrollDirection direction);
    void SetTextBox(YTL::smart_ptr<CGUITextBox> textBox);
    virtual void OnLBClick(void);

private:
    ScrollDirection m_direction;
    YTL::smart_ptr<CGUITextBox> m_vTextBox;
};

class CGUITextBox : public IGUIParts {
public:
    enum SliderMode {
        NO_SLIDER = -1, 
        VERTICAL_SLIDER = 0,
        HORIZONTAL_SLIDER = 1,
        BOTH_SLIDERS = 2
    };

    CGUITextBox();
    virtual ~CGUITextBox();

    void Create(int x, int y, int width, int height, SliderMode mode = NO_SLIDER);
    void SetSliderGFX(smart_ptr<ISurface> sliderThumbGraphic);
    void SetArrowGFX(smart_ptr<CPlaneLoader> pv, int upIndex, int downIndex);
    void SetSliderLoader(string data, string path);

    void SetText(const string& text);
    string GetText() const;
    void SetFont(smart_ptr<CFont> font); 
    CFont* GetFont(); 

    void SetTextColor(ISurfaceRGB color);
    void SetTextOffset(int x, int y); 

    void SetMargins(int x, int y);
    void GetMargins(int& x, int& y) const;
	void GetSize(int& x, int& y) const;
	void GetArrowSize(int& x, int& y) const;

    void SetBackgroundPlane(smart_ptr<ISurface> pv);

    virtual LRESULT OnSimpleMove(ISurface* lp);
    virtual LRESULT OnSimpleDraw(ISurface* lp);
    virtual void Reset();
    virtual void SetMouse(smart_ptr<CFixMouse> pv);

    void ScrollContent(int dx, int dy); 
    smart_ptr<CGUISlider> GetSlider(){ return m_vSlider; }

    void GetScrollPos(int& x, int& y) const { x = m_nScrollX; y = m_nScrollY; }

    void UpdateTextPlane(); 
    std::string TrimLeadingSpaces(const std::string& s);

protected:
    void CalculateVisibleContentSize(); 

    CTextFastPlaneEx* m_vTextFastPlane; 
    CPlane m_vTextPlane; 

    string m_strCurrentText;
    ISurfaceRGB m_textColor;
    int m_nTextOffsetX;
    int m_nTextOffsetY;

    int m_nMarginX;
    int m_nMarginY;

    smart_ptr<ISurface> m_vBackgroundPlane; 

    SliderMode m_sliderMode;
    smart_ptr<CGUISlider> m_vSlider;
    smart_ptr<CGUITextBoxSliderListener> m_vSliderListener;
    smart_ptr<ISurface> m_vSliderThumbGraphic;

    smart_ptr<CGUIButton> m_vScrollUpButton;
    smart_ptr<CGUIButton> m_vScrollDownButton;
    smart_ptr<CGUITextBoxArrowButtonListener> m_vScrollUpButtonListener;
    smart_ptr<CGUITextBoxArrowButtonListener> m_vScrollDownButtonListener;

    int m_nWidth;  
    int m_nHeight; 
    int m_nSliderStripWidth;  
    int m_nSliderStripHeight; 
	int m_nArrowWidth;  
    int m_nArrowHeight; 
    int m_nContentWidth;  
    int m_nContentHeight; 

    int m_nScrollX; 
    int m_nScrollY; 

    RECT m_rcTextBoxClip; 
};

} // namespace Draw
} // namespace yaneuraoGameSDK3rd

#endif // __yaneGUITextBox_h__