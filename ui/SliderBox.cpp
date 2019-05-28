#include "StdAfx.h"
#include "SliderBox.h"
#include "api/ObsMain.h"
#include "ObsMainFrame.h"

CSliderBox::CSliderBox()
    :m_channel(0)
{
}

CSliderBox::~CSliderBox()
{

}

void CSliderBox::Notify(TNotifyUI& msg)
{
    if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
    {
        ObsMain* obs = ObsMain::Instance();
        COptionUI* opt = NULL;

        if (m_channel == OUTPUT_AUDIO_CHANNEL1 || m_channel == OUTPUT_AUDIO_CHANNEL2)
            opt = m_PaintManager.FindControl<COptionUI>(_T("ODesktop"));
        else
            opt = m_PaintManager.FindControl<COptionUI>(_T("OMic"));

        CSliderUI* slider = m_PaintManager.FindControl<CSliderUI>(_T("Slider"));
        slider->SetValue(obs->GetVolume(m_channel));

        opt->SetVisible(true);
        opt->Selected(obs->GetMuted(m_channel));
    }
    else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
    {
        COptionUI* opt = NULL;
        if (m_channel == OUTPUT_AUDIO_CHANNEL1 || m_channel == OUTPUT_AUDIO_CHANNEL2)
            opt = m_PaintManager.FindControl<COptionUI>(_T("ODesktop"));
        else
            opt = m_PaintManager.FindControl<COptionUI>(_T("OMic"));

        ObsMain::Instance()->SetMuted(m_channel, opt->IsSelected());

        //设置主界面按钮状态
        ::PostMessage(GetParent(m_hWnd), MSG_UPDATE_AUDIO, 0, 0);
    }
}

LRESULT CSliderBox::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    CSliderUI* slider = m_PaintManager.FindControl<CSliderUI>(_T("Slider"));

    ObsMain::Instance()->SetVolume(m_channel, slider->GetValue());
    Close();
    return 0;
}