#pragma once

class ObsWindow;

class CObsDisplayControl : public CControlUI
{
public:
    CObsDisplayControl();
    ~CObsDisplayControl();

    virtual void DoInit();
    virtual void SetPos(RECT rc, bool bNeedInvalidate = true);
    virtual void SetInternVisible(bool bVisible = true);
private:
    ObsWindow* m_window;
};

