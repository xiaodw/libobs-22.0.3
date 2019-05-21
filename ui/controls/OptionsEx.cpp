#include <StdAfx.h>
#include "OptionsEx.h"

namespace DuiLib
{
const TCHAR* const kOptionExUIClassName= _T("OptionEx");


COptionExUI::COptionExUI()
    :m_enableCloseBtn(true),m_closeBtnState(0), m_closeBtnWidth(12)
{
    ZeroMemory(&m_rcCloseBtn, sizeof(m_rcCloseBtn));
}

COptionExUI::~COptionExUI()
{

}

LPCTSTR COptionExUI::GetClass() const
{
    return kOptionExUIClassName;
}

LPVOID COptionExUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, kOptionExUIClassName) == 0)
        return this;
    else
        return COptionUI::GetInterface(pstrName);
}

void COptionExUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("closebtnnormal")) == 0)
    {
        m_closeBtnNormal = pstrValue;
    }
    else if (_tcscmp(pstrName, _T("closebtnhot")) == 0)
    {
        m_closeBtnHot = pstrValue;
    }
    else if (_tcscmp(pstrName, _T("closebtnpushed")) == 0)
    {
        m_closeBtnPushed = pstrValue;
    }
    else if (_tcscmp(pstrName, _T("closebtnwidth")) == 0)
    {
        m_closeBtnWidth =_ttoi(pstrValue);
    }
    else
        COptionUI::SetAttribute(pstrName, pstrValue);
}

void COptionExUI::PaintStatusImage(HDC hDC)
{
    COptionUI::PaintStatusImage(hDC);
    if (!m_enableCloseBtn)
        return;

    //»æÖÆ¹Ø±Õ°´Å¥
    if (IsFocused()) m_closeBtnState |= UISTATE_FOCUSED;
    else m_closeBtnState &= ~UISTATE_FOCUSED;
    if (!IsEnabled()) m_closeBtnState |= UISTATE_DISABLED;
    else m_closeBtnState &= ~UISTATE_DISABLED;

    if ((m_closeBtnState & UISTATE_PUSHED) != 0) {
        if (!DrawCloseImage(hDC, m_closeBtnPushed))
            DrawCloseImage(hDC, m_closeBtnNormal);
    }
    else if ((m_closeBtnState & UISTATE_HOT) != 0) {
        if (!DrawCloseImage(hDC, m_closeBtnHot))
            DrawCloseImage(hDC, m_closeBtnNormal);
    }
    else
        DrawCloseImage(hDC, m_closeBtnNormal);
}

void COptionExUI::DoEvent(TEventUI& event)
{
    if (m_enableCloseBtn)
    {
        if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK)
        {
            if (::PtInRect(&m_rcCloseBtn, event.ptMouse) && IsEnabled()) {
                m_closeBtnState |= UISTATE_PUSHED | UISTATE_CAPTURED;
                Invalidate();
                return;
            }
        }

        if (event.Type == UIEVENT_MOUSEMOVE)
        {
            if ((m_closeBtnState & UISTATE_CAPTURED) != 0) {
                if (::PtInRect(&m_rcCloseBtn, event.ptMouse))
                    m_closeBtnState |= UISTATE_PUSHED;
                else
                    m_closeBtnState &= ~UISTATE_PUSHED;
                Invalidate();
            }
        }
        if (event.Type == UIEVENT_BUTTONUP)
        {
            if ((m_closeBtnState & UISTATE_CAPTURED) != 0) {
                if (::PtInRect(&m_rcCloseBtn, event.ptMouse) && IsEnabled())
                {
                    m_pManager->SendNotify(this, DUI_MSGTYPE_OPTIONCLOSED);
                }
                m_closeBtnState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
                Invalidate();
            }
        }
    }

    //if (!::PtInRect(&m_rcCloseBtn, event.ptMouse))
    {
        COptionUI::DoEvent(event);
    }
}


void COptionExUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    COptionUI::SetPos(rc, bNeedInvalidate);

    int offset = ((m_rcItem.bottom - m_rcItem.top) - m_closeBtnWidth) / 2;

    m_rcCloseBtn.right  = m_rcItem.right - offset;
    m_rcCloseBtn.left = m_rcCloseBtn.right - m_closeBtnWidth;
    m_rcCloseBtn.top = m_rcItem.top;
    m_rcCloseBtn.bottom = m_rcItem.bottom;
}

void COptionExUI::EnableCloseBtn(bool enable) {
    m_enableCloseBtn = enable;
    if (m_enableCloseBtn)
    {
        RECT rc = { 0,0,m_closeBtnWidth,0 };
        SetTextPadding(rc);
    }
    else
    {
        RECT rc = { 0,0,0,0 };
        SetTextPadding(rc);
    }
    Invalidate(); 
}

}//namespace

