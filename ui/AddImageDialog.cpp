#include "StdAfx.h"
#include "AddImageDialog.h"
#include "MsgBox.h"
#include "api/ObsMain.h"
#include "Utils.h"

CAddImageDialog::CAddImageDialog()
{
}

CAddImageDialog::~CAddImageDialog()
{

}



void CAddImageDialog::Notify(TNotifyUI& msg)
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
            if (url.GetLength() == 0)
            {
                CMsgBox msgBox;
                msgBox.DuiMessageBox(m_hWnd, _T("请添加图片"), _T("提示"), MESSAGE_INFO,FALSE);
                return;
            }

            // 检测文件是否存在
            if (!os_file_exists(ToUtf8(url).c_str()))
            {
                CMsgBox msgBox;
                msgBox.DuiMessageBox(m_hWnd, _T("图片文件不存在"), _T("提示"), MESSAGE_INFO, FALSE);
                return;
            }

            //添加图片
            int alpha = ((CSliderUI*)m_PaintManager.FindControl(_T("SAlpha")))->GetValue();
            ObsMain::Instance()->AddImage(ToUtf8(url).c_str(), alpha);

            Close();
         }
         else if (msg.pSender->GetName() == _T("BSelect"))
         {
             //选择文件
             CDuiString path = OpenSingleFileDialog(m_hWnd, _T("All Formats(*.bmp,*.jpg,*.png,*.gif,*.dds)\0*.bmp;*.jpg;*.png;*.gif;*.dds\0"));
             if(!path.IsEmpty())
                m_PaintManager.FindControl(_T("edit"))->SetText(path);
         }
    }
}

