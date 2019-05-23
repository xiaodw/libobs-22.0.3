#include "StdAfx.h"
#include "AddVideoDialog.h"
#include "MsgBox.h"
#include <commdlg.h>

CAddVideoDialog::CAddVideoDialog()
{
}

CAddVideoDialog::~CAddVideoDialog()
{

}


CDuiString OpenSingleFileDialog(HWND hwnd, LPCTSTR filter = NULL)
{
    OPENFILENAME ofn;       // common dialog box structure
    TCHAR szFile[MAX_PATH] = { 0 };       // buffer for file name

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    //
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    //
    ofn.lpstrFile[0] = _T('\0');
    ofn.nMaxFile = sizeof(szFile);
    if (filter)
        ofn.lpstrFilter = filter;
    else
        ofn.lpstrFilter = _T("All Files(*.*)\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NODEREFERENCELINKS;

    // Display the Open dialog box. 
    CDuiString fileName;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        fileName = ofn.lpstrFile;
    }
    return fileName;
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
                if (!os_file_exists(ToUtf8(url).c_str()))
                {
                    CMsgBox msgBox;
                    msgBox.DuiMessageBox(m_hWnd, _T("视频文件不存在"), _T("提示"), MESSAGE_INFO, FALSE);
                    return;
                }
            }

            bool loop = ((COptionUI*)m_PaintManager.FindControl(_T("OLoop")))->IsSelected();
            bool active = ((COptionUI*)m_PaintManager.FindControl(_T("OAvtive")))->IsSelected();



            Close();
         }
         else if (msg.pSender->GetName() == _T("BSelect"))
         {
             //选择文件
             CDuiString path = OpenSingleFileDialog(m_hWnd);
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

