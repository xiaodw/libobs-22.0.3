#include "StdAfx.h"
#include "MonitorSelectDialog.h"
#include "api/ObsMain.h"
#include "util/platform.h"
#include "api/ObsUtils.h"
#include "MsgBox.h"

CMonitorSelectDialog::CMonitorSelectDialog()
    :m_list(nullptr)
{
}

CMonitorSelectDialog::~CMonitorSelectDialog()
{

}

void CMonitorSelectDialog::OnEnumMonitor(void* ptr, const char* name, int id)
{
    //初始化
    CListLabelElementUI* elem = new CListLabelElementUI();
    elem->SetText(CDuiString(name));
    elem->SetTag(id);
    elem->SetFixedHeight(22);
    CListUI* list = (CListUI*)ptr;
    list->Add(elem);
}

void CMonitorSelectDialog::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0)
    {
        m_list = (CListUI*)m_PaintManager.FindControl(_T("List"));
        m_list->RemoveAll();
        EnumMonitor(OnEnumMonitor, m_list);
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
                 msgBox.DuiMessageBox(m_hWnd, _T("还没有选择直播屏幕"), _T("提示"),FALSE);
             }
             else
             {
                 CListLabelElementUI* elem =(CListLabelElementUI*) m_list->GetItemAt(sel);
                 CDuiString name = elem->GetText();
                 COptionUI* opt = (COptionUI*)m_PaintManager.FindControl(_T("OCaptureMouse"));
                 ObsMain::Instance()->AddCaptureScreen(ToUtf8(name).c_str(), elem->GetTag(), opt->IsSelected());
                 Close();
             }
         }
         else if (_tcsicmp(msg.pSender->GetName(), _T("BReload")) == 0)
         {
             m_list->RemoveAll();
             EnumMonitor(OnEnumMonitor, m_list);
         }
    }

}

