#pragma once
#include <functional>
#include <vector>


class CPopupList :public WindowImplBase
{
public:
    CPopupList(std::vector<CDuiString>& items, std::function<void(int)> callback);
    ~CPopupList();

    void ShowDialog(HWND hParent, RECT rcPos)
    {
        HWND hWnd=Create(hParent, _T("²Ëµ¥"), WS_POPUPWINDOW | WS_VISIBLE,WS_EX_TOOLWINDOW);
        ::MoveWindow(hWnd, rcPos.left, rcPos.top,
            rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, TRUE);
        SetFocus(hWnd);
    }

    virtual CDuiString GetSkinFile() {
        return _T("PopupList.xml");
    }

    virtual void OnFinalMessage(HWND hWnd) {
        HWND hParent = GetParent(hWnd);
        delete this;
    }

    virtual void Notify(TNotifyUI& msg);

    virtual UILIB_RESOURCETYPE GetResourceType() const { return UILIB_FILE; }
    virtual CDuiString GetSkinFolder() { return _T(""); }
    LPCTSTR GetWindowClassName() const { return _T("ObsGuiFoundation"); }

    virtual LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        Close();
        return 0;
    }
private:
    CListUI* m_list;
    std::vector<CDuiString> m_items;
    std::function<void(int)> m_callback;
};

