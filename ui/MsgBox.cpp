#include "StdAfx.h"
#include "MsgBox.h"

const TCHAR wMessageBoxCaption[] = _T("MessageBox_Caption");
const TCHAR wMessageBoxIcon[] = _T("MessageBox_Icon");
const TCHAR wMessageBoxText[] = _T("MessageBox_Text");
const TCHAR wMessageBoxTextLayout[] = _T("MessageBox_TextLayout");
const TCHAR wMessageBoxIconLayout[] = _T("MessageBox_IconLayout");


CMsgBox::CMsgBox() :
    m_pButtonCancel(NULL)
{
}

CMsgBox::~CMsgBox()
{

}

LPCTSTR CMsgBox::GetWindowClassName() const 
{ 
    return _T("ObsGuiFoundation");
}

UINT CMsgBox::GetClassStyle() const 
{ 
    return CS_DBLCLKS;
}


void CMsgBox::OnFinalMessage(HWND /*hWnd*/) 
{ 
    return;
}

void CMsgBox::InitWindow()
{
    m_pButtonCancel = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("cancelbtn")));
    RECT corner = { 5,3,5,7 };
    RECT hole = { 0,0,0,0 };
    m_shadow.SetImage(_T("image/window_shadow.png"), corner, hole);
    m_shadow.Create(m_hWnd);
}

void CMsgBox::Notify(TNotifyUI& msg)
{
    if( msg.sType == _T("click") ) 
    {
        if( msg.pSender->GetName() == _T("acceptbtn")) {
            Close(IDOK);
            return; 
        }
        else {
            Close(IDCANCEL);
            return; 
        }
    }

}

UINT CMsgBox::DuiMessageBox(HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uIcon, BOOL bOk)
{
    Create(hParent, _T(""), UI_WNDSTYLE_DIALOG, WS_EX_WINDOWEDGE);

    if (!bOk && m_pButtonCancel != NULL)  m_pButtonCancel->SetVisible(false);

    CButtonUI *pButton = static_cast<CButtonUI*>(m_PaintManager.FindControl(wMessageBoxIcon));;
    CContainerUI* pIconLayout = static_cast<CContainerUI*>(m_PaintManager.FindControl(wMessageBoxIconLayout));
    CDuiString stImage;
    if (pButton != NULL)
    {
        switch(uIcon)
        {
        case MESSAGE_SUCCEED:
            break;
        case MESSAGE_QUESTION:
            break;
        case MESSAGE_WARNING:
            break;
        case MESSAGE_ERROR:
            break;
        case MESSAGE_INFO:
            break;    
        case MESSAGE_ABOUT:
            break;
        }
        if (stImage.GetLength() != 0)
        {
            pButton->SetBkImage(stImage);
        }
        else{
            pIconLayout->SetVisible(false);
        }
    }

    CTextUI* pCaption_control = static_cast<CTextUI*>(m_PaintManager.FindControl(wMessageBoxCaption));
    if (pCaption_control != NULL && lpCaption)    pCaption_control->SetText(lpCaption);
    
    CContainerUI* pTextLayout = static_cast<CContainerUI*>(m_PaintManager.FindControl(wMessageBoxTextLayout));
    CTextUI* pText_control = static_cast<CTextUI*>(m_PaintManager.FindControl(wMessageBoxText));
    if (pText_control != NULL && pTextLayout != NULL)
    {
        pText_control->SetText(lpText); //设置文本
        RECT T = pText_control->GetPos();
        SIZE m_szClient = m_PaintManager.GetClientSize();
        int nCTextUIWidth = m_szClient.cx - pTextLayout->GetChildPadding() - pTextLayout->GetInset().left - 15;
        CDuiString stMax;
        int nLines = GetStringLines(lpText, stMax);
        SIZE szSpace = { 0 };
        HFONT hOldFont = (HFONT)::SelectObject(m_PaintManager.GetPaintDC(), m_PaintManager.GetFont(0));
        ::GetTextExtentPoint32(m_PaintManager.GetPaintDC(), stMax, stMax.GetLength(), &szSpace);
        ::SelectObject(m_PaintManager.GetPaintDC(), (HGDIOBJ)hOldFont);

        szSpace.cy = (szSpace.cy) * nLines + 105;
        if (pIconLayout->IsVisible())  
            szSpace.cx = szSpace.cx + 100;
        else 
            szSpace.cx = szSpace.cx + 70;

        if (szSpace.cx < 260)
            szSpace.cx = 260;

        //计算 文本大小
        RECT rect;
        GetClientRect(m_hWnd, &rect);
        rect.right = szSpace.cx + rect.left;
        rect.bottom = rect.top + szSpace.cy;
        SetWindowPos (m_hWnd, NULL, rect.left, rect.top, rect.right , rect.bottom, SWP_SHOWWINDOW );
    }

    CenterWindow();
    UINT uRet = ShowModal();
    return uRet;
}


void CMsgBox::TipBox(HWND hParent, LPCTSTR lpText)
{
    DuiMessageBox(hParent, lpText, NULL, MESSAGE_INFO, FALSE);
}


int CMsgBox::GetStringLines(CDuiString st, CDuiString &stMax)
{
    stMax = _T("");
    CDuiString stTmp;
    int nCount = 1;
    for (int i = 0; i < st.GetLength() ; i++)
    {
        if (st[i] == _T('\n'))
        {
            if (stTmp.GetLength() > stMax.GetLength())   stMax = stTmp;
            stTmp = _T("");
            nCount++;
        }else if (st[i] == _T('\r') && ((i + 1) < st.GetLength()) && st[i + 1] == _T('\n'))
        {
            if (stTmp.GetLength() > stMax.GetLength())   stMax = stTmp;
            stTmp = _T("");
            nCount++;  
            i++;
        }
        else
        {
            stTmp += st[i];
        }
    }
    if (stTmp.GetLength() > stMax.GetLength()) {
        stMax = stTmp;
    }
    return nCount;
}

