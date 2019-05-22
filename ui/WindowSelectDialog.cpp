#include "StdAfx.h"
#include "WindowSelectDialog.h"
#include "api/ObsMain.h"
#include "util/platform.h"
#include "api/ObsUtils.h"
#include "MsgBox.h"

CWindowSelectDialog::CWindowSelectDialog(bool selGame)
    :m_isGame(selGame), m_list(nullptr)
{
}

CWindowSelectDialog::~CWindowSelectDialog()
{

}

void CWindowSelectDialog::OnEnumWindow(void* ptr, const char* name, const char* window)
{
    //初始化
    CListLabelElementUI* elem = new CListLabelElementUI();
    elem->SetText(CDuiString(name));
    elem->SetUserData(CDuiString(window));
    elem->SetFixedHeight(22);
    CListUI* list = (CListUI*)ptr;
    list->Add(elem);
}

void CWindowSelectDialog::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0)
    {
        m_list = (CListUI*)m_PaintManager.FindControl(_T("List"));
        m_list->RemoveAll();

        if (m_isGame)
        {
            m_PaintManager.FindControl(_T("LTip"))->SetText(_T("请选择要直播的游戏:"));
            EnumGameWindow(OnEnumWindow,m_list);
        }
        else
        {
            m_PaintManager.FindControl(_T("LTip"))->SetText(_T("请选择要直播的窗口:"));
            EnumWindow(OnEnumWindow, m_list);
        }
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
                 msgBox.DuiMessageBox(m_hWnd, _T("还没有选择直播源"), _T("提示"),FALSE);
             }
             else
             {
                 CListLabelElementUI* elem =(CListLabelElementUI*) m_list->GetItemAt(sel);
                 CDuiString name = elem->GetText();
                 CDuiString window = elem->GetUserData();

                 char* nameUtf = nullptr;
                 char* windowUtf = nullptr;
                 os_wcs_to_utf8_ptr(name, name.GetLength(), &nameUtf);
                 os_wcs_to_utf8_ptr(window, window.GetLength(), &windowUtf);

                 if (m_isGame)
                 {
                     ObsMain::Instance()->AddGameCapture(nameUtf,windowUtf);
                 }
                 else
                 {
                     ObsMain::Instance()->AddWindowCapture(nameUtf, windowUtf);
                 }

                 bfree(nameUtf);
                 bfree(windowUtf);
                 Close();
             }
         }
         else if (_tcsicmp(msg.pSender->GetName(), _T("BReload")) == 0)
         {
             m_list->RemoveAll();
             if (m_isGame)
             {
                 EnumGameWindow(OnEnumWindow, m_list);
             }
             else
                 EnumWindow(OnEnumWindow, m_list);
         }
    }

}

