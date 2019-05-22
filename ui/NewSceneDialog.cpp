#include "StdAfx.h"
#include "NewSceneDialog.h"
#include "api/ObsMain.h"
#include "util/platform.h"

CNewSceneDialog::CNewSceneDialog()
{

}

CNewSceneDialog::~CNewSceneDialog()
{

}

void CNewSceneDialog::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0)
    {
        m_PaintManager.FindControl(_T("edit"))->SetFocus();
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_CLICK) == 0)
    {
        if (_tcsicmp(msg.pSender->GetName(), _T("closebtn")) == 0)
        {
            Close();
        }
        else if (_tcsicmp(msg.pSender->GetName(), _T("acceptbtn")) == 0)
        {
            CControlUI* ctrl = m_PaintManager.FindControl(_T("edit"));
            CDuiString text = ctrl->GetText();
            if (text.GetLength() > 0)
            {
                char* utf8 = NULL;
                os_wcs_to_utf8_ptr(text.GetData(), text.GetLength(),&utf8);
                bool nameUsed = false;

                //名称不能重复
                auto& scenes = ObsMain::Instance()->scenes();
                for (auto& data : scenes)
                {
                    if (strcmp(data->name(), utf8) == 0)
                    {
                        nameUsed = true;
                        break;
                    }
                }

                if (!nameUsed)
                {
                    ObsMain::Instance()->AddScene(utf8, true);
                    Close();
                }
                else
                {
                    ShowTip(_T("名称不能重复"));
                }

                bfree(utf8);
            }
            else
            {
                ShowTip(_T("名称不能为空"));
            }
        }
    }

}

void CNewSceneDialog::ShowTip(LPCTSTR text)
{
    CControlUI* ctrl = m_PaintManager.FindControl(_T("tiptext"));
    ctrl->SetText(text);
}
