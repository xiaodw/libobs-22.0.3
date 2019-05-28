#include "StdAfx.h"
#include "TipBox.h"

extern bool GetScreenSize(int *cx, int *cy);

CTipBox* CTipBox::s_lastTipBox = NULL;

CTipBox::CTipBox()
{
    s_lastTipBox = this;
}

CTipBox::~CTipBox()
{
    s_lastTipBox = NULL;
}

void CTipBox::Notify(TNotifyUI& msg)
{
    if (msg.sType==DUI_MSGTYPE_WINDOWINIT)
    {

    }
    else if (msg.sType==DUI_MSGTYPE_CLICK)
    {
        CDuiString name = msg.pSender->GetName();
        if (_tcsicmp(msg.pSender->GetName(), _T("closebtn")) == 0)
        {
             Close();
        }
    }

}

void CTipBox::ShowDialog(LPCTSTR text)
{
    HWND hWnd = CreateDuiWindow(NULL, _T("ÌבÊ¾"), WS_POPUPWINDOW | WS_VISIBLE, WS_EX_TOOLWINDOW);

    m_PaintManager.FindControl(_T("TText"))->SetText(text);

    RECT rcWnd = { 0,0,0,0 };
    GetClientRect(hWnd, &rcWnd);

    int screenWidth = 1920, screenHeight = 1080;
    GetScreenSize(&screenWidth, &screenHeight);

    int wndWidth = rcWnd.right - rcWnd.left + 5;
    int wndHeight = rcWnd.bottom - rcWnd.top + 5;

    HWND hTrayWnd = ::FindWindow(_T("Shell_TrayWnd"), _T(""));
    if (hTrayWnd)
    {
        RECT rect;
        ::GetWindowRect(hTrayWnd, &rect);

        if (rect.top > wndHeight &&  rect.top < screenHeight)
        {
            screenHeight = rect.top;
        }
        else if (rect.left > wndWidth)
        {
            screenWidth = rect.left;
        }
    }

    UINT uFlags = SWP_NOZORDER | SWP_NOSIZE;
    ::SetWindowPos(m_hWnd, NULL, screenWidth - wndWidth, screenHeight - wndHeight, 0, 0, uFlags);
}

void CTipBox::SetText(LPCTSTR text)
{
    m_PaintManager.FindControl(_T("TText"))->SetText(text);
}


void CTipBox::ShowTip(LPCTSTR text)
{
    if (s_lastTipBox)
        s_lastTipBox->SetText(text);
    else
    {
        CTipBox* tipBox = new CTipBox();
        tipBox->ShowDialog(text);
    }
}

