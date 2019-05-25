#pragma once

CDuiString OpenSingleFileDialog(HWND hwnd, LPCTSTR filter = NULL);

bool OpenColorSelectDialog(HWND hwnd, COLORREF & rgbCurrent);

//ϵͳ����ö����
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

    FontList m_chFonts;//��������
    FontList m_enFonts;//Ӣ������
};
