#pragma once

class CWindowSelectDialog :public WindowImplBase
{
public:
    CWindowSelectDialog(bool selGame);
    ~CWindowSelectDialog();

    void ShowDialog(HWND hParent)
    {
        CreateDuiWindow(hParent, _T("—°‘Ò÷±≤•‘¥"), WS_POPUPWINDOW | WS_VISIBLE);
        EnableWindow(hParent, false);
        CenterWindow();
    }

    virtual CDuiString GetSkinFile() {
        return _T("WindowSelectDialog.xml");
    }

    virtual void OnFinalMessage(HWND hWnd) {
        HWND hParent = GetParent(hWnd);
        EnableWindow(hParent, true);
        delete this;
    }

    virtual void Notify(TNotifyUI& msg);

    virtual UILIB_RESOURCETYPE GetResourceType() const { return UILIB_FILE; }
    virtual CDuiString GetSkinFolder() { return _T(""); }
    LPCTSTR GetWindowClassName() const { return _T("ObsGuiFoundation"); }

    void InitWindow()
    {
        RECT corner = { 5,3,5,7 };
        RECT hole = { 0,0,0,0 };
        m_shadow.SetImage(_T("image/window_shadow.png"), corner, hole);
        m_shadow.Create(m_hWnd);
    }

private:
    static void OnEnumWindow(void* ptr, const char* name, const char* window);

    CListUI* m_list;
    CWndShadow m_shadow;
    bool m_isGame;
};

