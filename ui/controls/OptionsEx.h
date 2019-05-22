#pragma once

namespace DuiLib
{
extern const TCHAR* const kOptionExUIClassName;// = _T("OptionEx");

#define DUI_MSGTYPE_OPTIONCLOSED 		   (_T("optionclosed"))

class COptionExUI :public COptionUI
{
public:
    COptionExUI();
    ~COptionExUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void PaintStatusImage(HDC hDC);

    void DoEvent(TEventUI& event);

    void SetPos(RECT rc, bool bNeedInvalidate = true);

    void SetCloseBtnNormal(LPCTSTR str) { m_closeBtnNormal = str; }
    void SetCloseBtnHot(LPCTSTR str) { m_closeBtnHot = str; }
    void SetCloseBtnPushed(LPCTSTR str) { m_closeBtnPushed = str; }
    void SetCloseBtnWidth(int width) { m_closeBtnWidth = width; }
    void EnableCloseBtn(bool enable);
private:
    bool DrawCloseImage(HDC hDC, TDrawInfo& drawInfo)
    {
        return CRenderEngine::DrawImage(hDC, m_pManager, m_rcCloseBtn, m_rcPaint, drawInfo);
    }

    bool               m_enableCloseBtn;
    int                   m_closeBtnWidth;
    RECT               m_rcCloseBtn;
    UINT               m_closeBtnState;
    TDrawInfo		m_closeBtnNormal;
    TDrawInfo		m_closeBtnHot;
    TDrawInfo		m_closeBtnPushed;
};

}//namespace
