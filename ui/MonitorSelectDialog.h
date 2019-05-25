#pragma once

class CMonitorSelectDialog :public WindowImplBase
{
public:
    CMonitorSelectDialog();
    ~CMonitorSelectDialog();

    void ShowDialog(HWND hParent)
    {
        CreateDuiWindow(hParent, _T("Ñ¡Ôñ×ÀÃæ"), WS_POPUPWINDOW | WS_VISIBLE);
        EnableWindow(hParent, false);
        CenterWindow();
    }

    virtual CDuiString GetSkinFile() {
        return _T("MonitorSelectDialog.xml");
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
    static void OnEnumMonitor(void* ptr, const char* name, int id);

    CListUI* m_list;
    CWndShadow m_shadow;
};

