#include "StdAfx.h"
#include "Utils.h"
#include <commdlg.h>
#include <Shlobj.h>
#include <shellapi.h>

CDuiString OpenSingleFileDialog(HWND hwnd, LPCTSTR filter)
{
    OPENFILENAME ofn;       // common dialog box structure
    TCHAR szFile[MAX_PATH] = { 0 };       // buffer for file name
    TCHAR szCurrentPath[MAX_PATH] = { 0 };
    GetCurrentDirectory(MAX_PATH, szCurrentPath);

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

    SetCurrentDirectory(szCurrentPath);//设置回原来的路径
    return fileName;
}



bool OpenColorSelectDialog(HWND hwnd, COLORREF & rgbCurrent)
{
    CHOOSECOLOR cc;                 // common dialog box structure 
    static COLORREF acrCustClr[16]; // array of custom colors 

    // Initialize CHOOSECOLOR 
    ZeroMemory(&cc, sizeof(cc));
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = hwnd;
    cc.lpCustColors = (LPDWORD)acrCustClr;
    cc.rgbResult = rgbCurrent;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;

    if (ChooseColor(&cc) == TRUE)
    {
        cc.rgbResult;
        rgbCurrent = cc.rgbResult;
        return true;
    }
    else
        return false;
}

CDuiString OpenSelectPathDialog(HWND hwnd)
{
    BROWSEINFO bInfo;
    memset(&bInfo, 0, sizeof(bInfo));
    bInfo.hwndOwner = hwnd;//父窗口
    bInfo.lpszTitle = TEXT("请选择文件夹");
    bInfo.ulFlags = BIF_RETURNONLYFSDIRS
        | BIF_USENEWUI //包含一个编辑框 用户可以手动填写路径 对话框可以调整大小之类的..
        | BIF_UAHINT //带TIPS提示 
        | BIF_DONTGOBELOWDOMAIN //显示网络目录
        ;

    CDuiString  strPath;
    LPITEMIDLIST lpDlist = SHBrowseForFolder(&bInfo);
    if (lpDlist != NULL)//单击了确定按钮  
    {
        TCHAR szPathName[MAX_PATH];
        memset(szPathName, 0, MAX_PATH);
        SHGetPathFromIDList(lpDlist, szPathName);
        strPath = szPathName;
    }
    return strPath;

}

void OpenFolder(HWND hwnd, LPCTSTR path)
{
    //CDuiString file;
    //SHELLEXECUTEINFO shex = { 0 };
    //shex.cbSize = sizeof(SHELLEXECUTEINFO);
    //shex.lpFile = _T("explorer");
    //file = _T(" /select, ");
    //file.Append(path);
    //shex.lpParameters = file;
    //shex.lpVerb = _T("open");
    //shex.nShow = SW_SHOWDEFAULT;
    //shex.lpDirectory = NULL;
    //ShellExecuteEx(&shex);

    CDuiString file = path;

    TCHAR lastChar = file.LastChar();
    if (lastChar != _T('/') && lastChar != _T('\\'))
    {
        file.Append(_T('\\'));
    }
    ShellExecute(hwnd, _T("open"), file, NULL, NULL, SW_SHOW);
}

/////////////////////////////////////////////////////////////////////////////////////
CFontsEnumerator::CFontsEnumerator(void)
{
}


CFontsEnumerator::~CFontsEnumerator(void)
{
}


int CALLBACK EnumProc(const ENUMLOGFONTEX *lpelfe, 
    const NEWTEXTMETRICEX *lpntme, unsigned long FontType, LPARAM lParam)
{
    CFontsEnumerator* pTHIS = reinterpret_cast<CFontsEnumerator*>(lParam);
    return pTHIS->DoEnumStep(lpelfe, lpntme, FontType);
}



void CFontsEnumerator::EnumFont(HDC hdc)
{
    LOGFONT lf;
    memset(&lf, 0, sizeof(lf));
    //lf.lfCharSet	= DEFAULT_CHARSET;

    lf.lfCharSet = GB2312_CHARSET;
    lf.lfFaceName[0] = 0;
    EnumFontFamiliesEx(hdc, &lf, (FONTENUMPROC)EnumProc, reinterpret_cast<LPARAM>(this), 0);//hdc
}


bool CFontsEnumerator::IsExist(LPCTSTR name)
{
    if (name[0]> 0x80)
    {
        CFontsEnumerator::FontList::iterator i = m_chFonts.begin();
        for (; i != m_chFonts.end(); ++i)
        {
            if ((*i).compare(name) == 0)
                return true;
        }
    }
    else
    {
        CFontsEnumerator::FontList::iterator i = m_enFonts.begin();
        for (; i != m_enFonts.end(); ++i)
        {
            if ((*i).compare(name) == 0)
                return true;
        }
    }
    return false;
}

int	CFontsEnumerator::DoEnumStep(const ENUMLOGFONTEX *lpelfe,
    const NEWTEXTMETRICEX *lpntme, unsigned long FontType)
{
    LPCTSTR fontName = lpelfe->elfLogFont.lfFaceName;

    if (IsExist(fontName))
        return 1;
    if (fontName[0] > 0x80)
    {
        m_chFonts.push_back(fontName);
    }
    else
        m_enFonts.push_back(fontName);
    return 1;
}


