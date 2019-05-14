#pragma once
#include <obs.hpp>
#include <vector>
#include <memory>

class ObsVolumeControl
{
public:
    ObsVolumeControl(OBSSource source);
    ~ObsVolumeControl();

    void SetMuted(bool checked);
    void SetVolume(int vol);//0~100
private:
    static void OBSVolumeChanged(void *param, float db);
    static void OBSVolumeLevel(void *data,
        const float magnitude[MAX_AUDIO_CHANNELS],
        const float peak[MAX_AUDIO_CHANNELS],
        const float inputPeak[MAX_AUDIO_CHANNELS]);
    static void OBSVolumeMuted(void *data, calldata_t *calldata);


    OBSSource m_source;
    obs_fader_t     *m_obs_fader;
    obs_volmeter_t  *m_obs_volmeter;
};
