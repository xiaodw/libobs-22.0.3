#include "StdAfx.h"
#include "Utils.h"
#include <commdlg.h>

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


