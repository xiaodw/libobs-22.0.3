#include "stdafx.h"
#include "ObsMainFrame.h"
#include "ObsDisplayControl.h"

const TCHAR* const kTitleControlName = _T("apptitle");
const TCHAR* const kCloseButtonControlName = _T("closebtn");
const TCHAR* const kMinButtonControlName = _T("minbtn");
const TCHAR* const kMaxButtonControlName = _T("maxbtn");
const TCHAR* const kRestoreButtonControlName = _T("restorebtn");

CObsMainFrame::CObsMainFrame()
{}

CObsMainFrame::~CObsMainFrame()
{
    PostQuitMessage(0);
}

LPCTSTR CObsMainFrame::GetWindowClassName() const
{
    return _T("ObsGuiFoundation");
}

CControlUI* CObsMainFrame::CreateControl(LPCTSTR pstrClass)
{
    if (_tcscmp(pstrClass, _T("ObsDisplay")) == 0)
    {
        return new CObsDisplayControl();
    }
    return NULL;
}

void CObsMainFrame::OnFinalMessage(HWND hWnd)
{
    WindowImplBase::OnFinalMessage(hWnd);
    delete this;
}

CDuiString CObsMainFrame::GetSkinFile()
{
    return _T("MainFrame.xml");
}

CDuiString CObsMainFrame::GetSkinFolder()
{
    return _T("");
}

UILIB_RESOURCETYPE CObsMainFrame::GetResourceType() const
{
    return UILIB_FILE;
}

LRESULT CObsMainFrame::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#if defined(WIN32) && !defined(UNDER_CE)
    BOOL bZoomed = ::IsZoomed(m_hWnd);
    LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    if (::IsZoomed(m_hWnd) != bZoomed)
    {
        if (!bZoomed)
        {
            CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
            if (pControl) pControl->SetVisible(false);
            pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
            if (pControl) pControl->SetVisible(true);
        }
        else
        {
            CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
            if (pControl) pControl->SetVisible(true);
            pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
            if (pControl) pControl->SetVisible(false);
        }
    }
#else
    return __super::OnSysCommand(uMsg, wParam, lParam, bHandled);
#endif

    return 0;
}

LRESULT CObsMainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CObsMainFrame::ResponseDefaultKeyEvent(WPARAM wParam)
{
    if (wParam == VK_RETURN)
    {
        return FALSE;
    }
    else if (wParam == VK_ESCAPE)
    {
        return TRUE;
    }
    return FALSE;
}

void CObsMainFrame::OnTimer(TNotifyUI& msg)
{
}

void CObsMainFrame::OnExit(TNotifyUI& msg)
{
    Close();
}

void CObsMainFrame::InitWindow()
{}

void CObsMainFrame::OnPrepare(TNotifyUI& msg)
{}

void CObsMainFrame::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0)
    {
        OnPrepare(msg);
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_CLICK) == 0)
    {
        if (_tcsicmp(msg.pSender->GetName(), kCloseButtonControlName) == 0)
        {
            OnExit(msg);
        }
        else if (_tcsicmp(msg.pSender->GetName(), kMinButtonControlName) == 0)
        {
#if defined(UNDER_CE)
            ::ShowWindow(m_hWnd, SW_MINIMIZE);
#else
            SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
#endif
        }
        else if (_tcsicmp(msg.pSender->GetName(), kMaxButtonControlName) == 0)
        {
#if defined(UNDER_CE)
            ::ShowWindow(m_hWnd, SW_MAXIMIZE);
            CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
            if (pControl) pControl->SetVisible(false);
            pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
            if (pControl) pControl->SetVisible(true);
#else
            SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
#endif
        }
        else if (_tcsicmp(msg.pSender->GetName(), kRestoreButtonControlName) == 0)
        {
#if defined(UNDER_CE)
            ::ShowWindow(m_hWnd, SW_RESTORE);
            CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
            if (pControl) pControl->SetVisible(true);
            pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
            if (pControl) pControl->SetVisible(false);
#else
            SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
#endif
        }
        else if (_tcsicmp(msg.pSender->GetName(), _T("btn_menu")) == 0)
        {
            //CMenuWnd* pMenu = new CMenuWnd(m_hWnd);
            //CDuiPoint point = msg.ptMouse;
            //ClientToScreen(m_hWnd, &point);
            //STRINGorID xml(IDR_XML_MENU);
            //pMenu->Init(NULL, xml, _T("xml"), point);
        }
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_TIMER) == 0)
    {
        return OnTimer(msg);
    }
}

LRESULT CObsMainFrame::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return 0;
}