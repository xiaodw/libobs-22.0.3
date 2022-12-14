#include "StdAfx.h"
#include "DeviceSelectDialog.h"
#include "api/ObsMain.h"
#include "util/platform.h"
#include "api/ObsUtils.h"
#include "MsgBox.h"

CDeviceSelectDialog::CDeviceSelectDialog()
    :m_list(nullptr)
{
}

CDeviceSelectDialog::~CDeviceSelectDialog()
{

}

void CDeviceSelectDialog::OnEnumDevice(void* ptr, bool isVideo, const char* name, const char* id)
{
    CDeviceSelectDialog* dialog = (CDeviceSelectDialog*)ptr;
    if (!isVideo)
    {
        CListLabelElementUI* elem = new CListLabelElementUI();
        elem->SetText(CDuiString(name));
        elem->SetUserData(CDuiString(id));
        elem->SetFixedHeight(22);
        dialog->m_list->Add(elem);
    }
}

void CDeviceSelectDialog::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0)
    {
        m_list = (CListUI*)m_PaintManager.FindControl(_T("List"));
        m_list->RemoveAll();

        m_PaintManager.FindControl(_T("LTip"))->SetText(_T("请选择音频设备:"));
        EnumDevice(OnEnumDevice, this);
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_CLICK) == 0)
    {
         if (_tcsicmp(msg.pSender->GetName(), _T("closebtn")) == 0)
        {
             Close();
        }
         if (_tcsicmp(msg.pSender->GetName(), _T("acceptbtn")) == 0)
         {
             int sel = m_list->GetCurSel();
             if (sel < 0)
             {
                 CMsgBox msgBox;
                 msgBox.DuiMessageBox(m_hWnd, _T("您还没有选择设备"), _T("提示"),FALSE);
             }
             else
             {
                 CListLabelElementUI* elem =(CListLabelElementUI*) m_list->GetItemAt(sel);
                 CDuiString name = elem->GetText();
                 CDuiString deviceid = elem->GetUserData();

                 char* nameUtf = nullptr;
                 char* deviceidUtf = nullptr;
                 os_wcs_to_utf8_ptr(name, name.GetLength(), &nameUtf);
                 os_wcs_to_utf8_ptr(deviceid, deviceid.GetLength(), &deviceidUtf);

                 //添加音视频设备
                 ObsMain::Instance()->AddAudio(nameUtf, deviceidUtf);

                 bfree(nameUtf);
                 bfree(deviceidUtf);
                 Close();
             }
         }
         else if (_tcsicmp(msg.pSender->GetName(), _T("BReload")) == 0)
         {
             m_list->RemoveAll();
             EnumDevice(OnEnumDevice, this);
         }
    }

}

