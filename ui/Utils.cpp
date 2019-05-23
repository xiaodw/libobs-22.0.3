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
