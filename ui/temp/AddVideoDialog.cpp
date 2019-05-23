#include "StdAfx.h"
#include "AddVideoDialog.h"
#include "MsgBox.h"

CAddVideoDialog::CAddVideoDialog()
    :m_list(nullptr)
{
}

CAddVideoDialog::~CAddVideoDialog()
{

}

void CAddVideoDialog::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0)
    {
        m_list = (CListUI*)m_PaintManager.FindControl(_T("List"));
        m_list->RemoveAll();
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_CLICK) == 0)
    {
         if (_tcsicmp(msg.pSender->GetName(), _T("closebtn")) == 0)
        {
             Close();
        }
         else if (_tcsicmp(msg.pSender->GetName(), _T("acceptbtn")) == 0)
         {
       
                 Close();
         }

    }

}

