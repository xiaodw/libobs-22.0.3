#pragma once

class CRenameDialog :public WindowImplBase
{
public:
    CRenameDialog();
    ~CRenameDialog();

    void ShowDialog(HWND hParent,LPCTSTR title, LPCTSTR text,std::function<bool(CRenameDialog*)> callback)
    {
        m_title = title;
        m_text = text;
        m_callback = callback;
        CreateDuiWindow(hParent, title, WS_POPUPWINDOW | WS_VISIBLE);
        EnableWindow(hParent, false);
        CenterWindow();
    }

    CDuiString GetText() {
        return m_editor->GetText();
    }
    void ShowTip(LPCTSTR text);
    virtual void Notify(TNotifyUI& msg);

protected:
    virtual CDuiString GetSkinFile() { return _T("RenameDialog.xml");}
    virtual UILIB_RESOURCETYPE GetResourceType() const { return RESOURCE_TYPE; }
    virtual CDuiString GetSkinFolder() { return _T(""); }
    LPCTSTR GetWindowClassName() const { return _T("ObsGuiFoundation"); }

    void InitWindow()
    {
        RECT corner = { 5,3,5,7 };
        RECT hole = { 0,0,0,0 };
        m_shadow.SetImage(_T("image/window_shadow.png"), corner, hole);
        m_shadow.Create(m_hWnd);
    }

    virtual void OnFinalMessage(HWND hWnd) {
        HWND hParent = GetParent(hWnd);
        EnableWindow(hParent, true);
        delete this;
    }

private:
    //返回true表示关闭窗口
    std::function<bool(CRenameDialog*)> m_callback;

    CEditUI* m_editor;
    CDuiString m_title;
    CDuiString m_text;
    CWndShadow m_shadow;
};

