#pragma once

enum enumMessageIcon {
    MESSAGE_SUCCEED = 0,
    MESSAGE_QUESTION,
    MESSAGE_WARNING,
    MESSAGE_ERROR,
    MESSAGE_INFO,
    MESSAGE_ABOUT,
    MESSAGE_MAX,
};

class CMsgBox : public WindowImplBase
{
public:
    CMsgBox();
    ~CMsgBox();

    LPCTSTR GetWindowClassName() const;
    UINT GetClassStyle() const;
    void OnFinalMessage(HWND /*hWnd*/);


    void InitWindow();
    void Notify(TNotifyUI& msg);

    virtual CDuiString GetSkinFolder() { return _T(""); }
    virtual UILIB_RESOURCETYPE GetResourceType() const { return RESOURCE_TYPE; }
    virtual CDuiString GetSkinFile() { return _T("msgbox.xml"); }


    UINT DuiMessageBox(HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption = NULL, UINT uIcon = MESSAGE_INFO, BOOL bOk = TRUE);
 
    UINT TipBox(HWND hParent, LPCTSTR lpText);

    int GetStringLines(CDuiString st, CDuiString &stMax);
protected:
    CButtonUI *m_pButtonCancel;
    CWndShadow m_shadow;
};
