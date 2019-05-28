#include "StdAfx.h"
#include "TipBox.h"


CTipBox::CTipBox()
{
}

CTipBox::~CTipBox()
{

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

