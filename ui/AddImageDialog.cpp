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
            //�����Ƶ
            CDuiString url = m_PaintManager.FindControl(_T("edit"))->GetText();
            if (url.GetLength() == 0)
            {
                CMsgBox msgBox;
                msgBox.DuiMessageBox(m_hWnd, _T("�����ͼƬ"), _T("��ʾ"), MESSAGE_INFO,FALSE);
                return;
            }

            // ����ļ��Ƿ����
            if (!os_file_exists(ToUtf8(url).c_str()))
            {
                CMsgBox msgBox;
                msgBox.DuiMessageBox(m_hWnd, _T("ͼƬ�ļ�������"), _T("��ʾ"), MESSAGE_INFO, FALSE);
                return;
            }

            //���ͼƬ
            int alpha = ((CSliderUI*)m_PaintManager.FindControl(_T("SAlpha")))->GetValue();
            ObsMain::Instance()->AddImage(ToUtf8(url).c_str(), alpha);

            Close();
         }
         else if (msg.pSender->GetName() == _T("BSelect"))
         {
             //ѡ���ļ�
             CDuiString path = OpenSingleFileDialog(m_hWnd, _T("All Formats(*.bmp,*.jpg,*.png,*.gif,*.dds)\0*.bmp;*.jpg;*.png;*.gif;*.dds\0"));
             if(!path.IsEmpty())
                m_PaintManager.FindControl(_T("edit"))->SetText(path);
         }
    }
}

