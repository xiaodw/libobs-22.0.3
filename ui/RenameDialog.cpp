#include "StdAfx.h"
#include "RenameDialog.h"
#include "api/ObsMain.h"
#include "util/platform.h"

CRenameDialog::CRenameDialog()
    :m_editor(NULL)
{

}

CRenameDialog::~CRenameDialog()
{

}

void CRenameDialog::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0)
    {
        m_editor = (CEditUI*)m_PaintManager.FindControl(_T("edit"));
        m_editor->SetText(m_text);
        m_editor->SetAutoSelAll(true);
        m_PaintManager.FindControl(_T("title"))->SetText(m_title);


        m_editor->SetFocus();
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_CLICK) == 0)
    {
        if (_tcsicmp(msg.pSender->GetName(), _T("closebtn")) == 0)
        {
            Close();
        }
        else if (_tcsicmp(msg.pSender->GetName(), _T("acceptbtn")) == 0)
        {
            if (!m_callback || m_callback(this))
                Close();
        }
    }
}

void CRenameDialog::ShowTip(LPCTSTR text)
{
    CControlUI* ctrl = m_PaintManager.FindControl(_T("tiptext"));
    ctrl->SetText(text);
}
