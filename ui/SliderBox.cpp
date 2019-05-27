#include "StdAfx.h"
#include "SliderBox.h"

CSliderBox::CSliderBox()
    :m_list(nullptr)
{
}

CSliderBox::~CSliderBox()
{

}

void CSliderBox::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0)
    {

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

