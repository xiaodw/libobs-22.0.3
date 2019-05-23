#include "StdAfx.h"
#include "AddTextDialog.h"
#include "MsgBox.h"
#include "api/ObsMain.h"
#include "Utils.h"

CAddTextDialog::CAddTextDialog()
{
}

CAddTextDialog::~CAddTextDialog()
{

}

void CAddTextDialog::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0)
    {

    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_CLICK) == 0)
    {
         if (msg.pSender->GetName()==_T("closebtn"))
        {
             Close();
        }
         else if (msg.pSender->GetName()== _T("acceptbtn"))
         {
            
            Close();
         }
    }
}

