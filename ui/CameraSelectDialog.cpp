#include "StdAfx.h"
#include "api/ObsMain.h"
#include "util/platform.h"
#include "api/ObsUtils.h"
#include "MsgBox.h"
#include "CameraSelectDialog.h"


CCameraSelectDialog::CCameraSelectDialog()
    :m_list(nullptr), m_resolutionList(nullptr),m_prop(nullptr)
{
    m_prop = obs_properties_by_id("dshow_input");
}

CCameraSelectDialog::~CCameraSelectDialog()
{
    if(m_prop)
        obs_properties_destroy(m_prop);
}

void CCameraSelectDialog::EnumDevice()
{
    obs_property_t *property = obs_properties_get(m_prop, "video_device_id");
    if (property)
    {
        size_t  count = obs_property_list_item_count(property);
        for (size_t i = 0; i < count; i++)
        {
            const char *name = obs_property_list_item_name(property, i);
            const char *id = obs_property_list_item_string(property, i);

            CListLabelElementUI* elem = new CListLabelElementUI();
            elem->SetName(_T("CameraItem"));
            elem->SetText(CDuiString(name));
            elem->SetUserData(CDuiString(id));
            elem->SetFixedHeight(22);
            m_list->Add(elem);

            blog(LOG_INFO, "EnumDevice:%s  %s", name, id);
        }
    }
}

void CCameraSelectDialog::AddResolutionItem(const CDuiString& text)
{
    CListLabelElementUI* elem = new CListLabelElementUI();
    elem->SetName(_T("ResolutionItem"));
    elem->SetText(text);
    elem->SetFixedHeight(24);
    m_resolutionList->Add(elem);
}

void CCameraSelectDialog::EnumResolution()
{
    CComboUI* comboList = (CComboUI*)(m_PaintManager.FindControl(_T("CResolution")));
    obs_property_t *resolution = obs_properties_get(m_prop, "resolution");
    size_t  count = obs_property_list_item_count(resolution);

    m_resolutionList->RemoveAll();
    AddResolutionItem(_T("设备默认"));

    for (size_t i = 0; i < count; i++)
    {
        const char *name = obs_property_list_item_name(resolution, i);
        const char *id = obs_property_list_item_string(resolution, i);
        AddResolutionItem(name);
    }
    m_resolutionList->SelectItem(0);
}

void CCameraSelectDialog::Notify(TNotifyUI& msg)
{
    if (msg.sType== DUI_MSGTYPE_WINDOWINIT)
    {
        m_resolutionList = (CComboUI*)m_PaintManager.FindControl(_T("CResolution"));
        m_list = (CListUI*)m_PaintManager.FindControl(_T("List"));
        m_list->RemoveAll();
        EnumDevice();
    }
    else if (msg.sType == DUI_MSGTYPE_ITEMSELECT)
    {
        if (_tcsicmp(msg.pSender->GetName(), _T("List")) == 0)
        {
            CListLabelElementUI* elem = (CListLabelElementUI*)m_list->GetItemAt(msg.wParam);
            std::string deviceid = ToUtf8(elem->GetUserData());

            obs_property_t *property = obs_properties_get(m_prop, "video_device_id");

            obs_data_t* data = obs_data_create();

            obs_data_set_string(data, "video_device_id", deviceid.c_str());
            obs_data_set_string(data, "last_video_device_id", m_lastDeviceId.c_str());
            obs_property_modified(property, data);

            obs_data_release(data);

            EnumResolution();

            m_lastDeviceId = deviceid;
        }

    }
    else if (msg.sType == DUI_MSGTYPE_CLICK)
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

                 //添加音视频设备
                COptionUI* opt =(COptionUI*) m_PaintManager.FindControl(_T("OGreenBkg"));

                ObsMain::CameraInfo info;
                info.greeenBkg = opt->IsSelected();
                info.deviceid = ToUtf8(deviceid);

                if (m_resolutionList->GetCurSel() > 0)
                    info.resolution = ToUtf8(m_resolutionList->GetText());

                ObsMain::Instance()->AddCamera(ToUtf8(name).c_str(), &info);
                 Close();
             }
         }
         else if (msg.pSender->GetName()== _T("BReload"))
         {
             m_list->RemoveAll();
             EnumDevice();
         }
    }

}

