#pragma once

CDuiString OpenSingleFileDialog(HWND hwnd, LPCTSTR filter = NULL);

bool OpenColorSelectDialog(HWND hwnd, COLORREF & rgbCurrent);

CDuiString OpenSelectPathDialog(HWND hwnd);

void OpenFolder(HWND hwnd,LPCTSTR path);

//系统字体枚举器
class CFontsEnumerator
{
public:
#ifdef _UNICODE
    typedef std::vector<std::wstring> FontList;
#else
    typedef std::vector<std::string> FontList;
#endif

    CFontsEnumerator(void);
    ~CFontsEnumerator(void);

    void EnumFont(HDC hdc);
    const FontList& cnFontList() { return m_chFonts; }
    const FontList& enFontList() { return m_enFonts; }

public:
    int DoEnumStep(const ENUMLOGFONTEX *lpelfe, 
        const NEWTEXTMETRICEX *lpntme, unsigned long FontType);

    bool IsExist(LPCTSTR name);

    FontList m_chFonts;//中文字体
    FontList m_enFonts;//英文字体
};
