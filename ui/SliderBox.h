#pragma once

class CSliderBox :public WindowImplBase
{
public:
    CSliderBox();
    ~CSliderBox();

    void ShowDialog(HWND hParent,POINT pt)
    {
        CreateDuiWindow(hParent, _T(""), WS_POPUPWINDOW | WS_VISIBLE, WS_EX_TOOLWINDOW);
        UINT uFlags = SWP_NOZORDER | SWP_NOSIZE;
        ::SetWindowPos(m_hWnd, NULL, pt.x, pt.y , 0 , 0 , uFlags);
    }

    virtual CDuiString GetSkinFile() {
        return _T("SliderBox.xml");
    }

    virtual void OnFinalMessage(HWND hWnd) {
        delete this;
    }

    virtual void Notify(TNotifyUI& msg);

    virtual UILIB_RESOURCETYPE GetResourceType() const { return UILIB_FILE; }
    virtual CDuiString GetSkinFolder() { return _T(""); }
    LPCTSTR GetWindowClassName() const { return _T("ObsGuiFoundation"); }

    //如果丢失焦点关闭
    virtual LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        Close();
        return 0;
    }

    void InitWindow()
    {
        RECT corner = { 5,3,5,7 };
        RECT hole = { 0,0,0,0 };
        m_shadow.SetImage(_T("image/window_shadow.png"), corner, hole);
        m_shadow.Create(m_hWnd);
    }
private:
    CListUI* m_list;
    CWndShadow m_shadow;
};

