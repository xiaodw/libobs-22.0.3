#include "StdAfx.h"
#include "AddVideoDialog.h"
#include "MsgBox.h"
#include "api/ObsMain.h"
#include "Utils.h"

CAddVideoDialog::CAddVideoDialog()
{
}

CAddVideoDialog::~CAddVideoDialog()
{

}



void CAddVideoDialog::Notify(TNotifyUI& msg)
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
            //添加视频
            CDuiString url = m_PaintManager.FindControl(_T("edit"))->GetText();
            ObsMain::VideoData data;
            data.url = ToUtf8(url);

            if (url.GetLength() == 0)
            {
                CMsgBox msgBox;
                msgBox.DuiMessageBox(m_hWnd, _T("请添加视频"), _T("提示"), MESSAGE_INFO,FALSE);
                return;
            }

            COptionUI* localFile = (COptionUI*)m_PaintManager.FindControl(_T("OLocal"));
            if (localFile->IsSelected())
            {
                // 检测文件是否存在
                if (!os_file_exists(data.url.c_str()))
                {
                    CMsgBox msgBox;
                    msgBox.DuiMessageBox(m_hWnd, _T("视频文件不存在"), _T("提示"), MESSAGE_INFO, FALSE);
                    return;
                }
            }

            data.isFile = localFile->IsSelected();
            data.isLoop = ((COptionUI*)m_PaintManager.FindControl(_T("OLoop")))->IsSelected();
            data.isActiveReplay = ((COptionUI*)m_PaintManager.FindControl(_T("OAvtive")))->IsSelected();

            ObsMain::Instance()->AddVideo(&data);
            Close();
         }
         else if (msg.pSender->GetName() == _T("BSelect"))
         {
             //选择文件
             CDuiString path = OpenSingleFileDialog(m_hWnd, _T("All Formats(*.mp4,*.mkv,*.rmvb,*.3gp,*.avi,*.flv,*.mpg,*.mov)\0*.mp4;*.mkv;*.rmvb;*.3gp;*.avi;*.flv;*.mpg;*.mov\0"));
             if(!path.IsEmpty())
                m_PaintManager.FindControl(_T("edit"))->SetText(path);
         }
    }
    else  if (_tcsicmp(msg.sType, DUI_MSGTYPE_SELECTCHANGED) == 0)
    {
        if (msg.pSender->GetName() == _T("OLocal"))
        {
               m_PaintManager.FindControl(_T("BSelect"))->SetVisible(msg.wParam);
        }
    }
}

