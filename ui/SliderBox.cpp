#include "StdAfx.h"
#include "SliderBox.h"
#include "api/ObsMain.h"

CSliderBox::CSliderBox()
    :m_channel(0)
{
}

CSliderBox::~CSliderBox()
{

}

void CSliderBox::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0)
    {
        if (m_channel == OUTPUT_AUDIO_CHANNEL1 || m_channel == OUTPUT_AUDIO_CHANNEL2)
            m_PaintManager.FindControl(_T("ODesktop"))->SetVisible(true);
        else
            m_PaintManager.FindControl(_T("OMic"))->SetVisible(true);

        CSliderUI* slider = m_PaintManager.FindControl<CSliderUI>(_T("Slider"));
        slider->SetValue(ObsMain::Instance()->GetVolume(m_channel));
    }

}

LRESULT CSliderBox::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    CSliderUI* slider = m_PaintManager.FindControl<CSliderUI>(_T("Slider"));

    ObsMain::Instance()->SetVolume(m_channel, slider->GetValue());
    Close();
    return 0;
}