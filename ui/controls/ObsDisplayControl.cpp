#include "../StdAfx.h"
#include "ObsDisplayControl.h"

CObsDisplayControl::CObsDisplayControl()
    :m_window(ObsWindow::Create())
{

}

CObsDisplayControl::~CObsDisplayControl()
{
    delete m_window;
}

void CObsDisplayControl::DoInit()
{
    RECT temp=GetPos();
    ObsRect rc = { 
        temp.left,
        temp.top, 
        temp.right - temp.left,
        temp.bottom - temp.top 
    };
    m_window->CreateWnd(GetManager()->GetPaintWindow(), rc);
    m_window->CreateDisplay();
}

void CObsDisplayControl::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc,bNeedInvalidate);
    RECT temp = GetPos();
    ObsRect pos = {
        temp.left,
        temp.top,
        temp.right - temp.left,
        temp.bottom - temp.top
    };

    blog(LOG_INFO, "setpos %d,%d,%d,%d", pos.x, pos.y, pos.width, pos.height);
    m_window->SetWindowPos(pos);
}

void CObsDisplayControl::SetInternVisible(bool bVisible)
{
    CControlUI::SetInternVisible(bVisible);

    m_window->SetVisible(bVisible);
    m_window->SetEnable(bVisible);
}
