#pragma once
#include <memory>
#include "ObsOutputs.h"
#include <util/util.hpp>

#define Str(s) s

class ObsBasic
{
public:
    ObsBasic();
    ~ObsBasic();

    //初始化obs
    void InitObs();

    void ResetOutputs();
    bool InitBasicConfig();

    obs_service_t *GetService();
    void  SetService(obs_service_t *service);


    bool StreamingActive() const;
    bool Active() const;

    bool StartStreaming();
    void StopStreaming();
    void ForceStopStreaming();
    bool StartRecording();
    void StopRecording();
    bool StartReplayBuffer();
    void StopReplayBuffer();

    //由obsoutput调用接口
    void StreamDelayStarting(int delay);
    void StreamStopping();
    void StreamDelayStopping(int delay);
    void StreamingStart();
    void StreamingStop(int code, const char* error);
    void RecordingStart();
    void RecordStopping();
    void RecordingStop(int code);
    void ReplayBufferStart();
    void ReplayBufferStop(int code);
    void ReplayBufferStopping();

    //basic config
    config_t* basicConfig() { return m_basicConfig; }
    
protected:
    bool InitBasicConfigDefaults();
    void GetConfigFPS(uint32_t &num, uint32_t &den) const;
    void GetFPSCommon(uint32_t &num, uint32_t &den) const;
    void GetFPSInteger(uint32_t &num, uint32_t &den) const;
    void GetFPSFraction(uint32_t &num, uint32_t &den) const;
    void GetFPSNanoseconds(uint32_t &num, uint32_t &den) const;

    int  ResetVideo();
    bool ResetAudio();
    bool  InitService();

    void CheckForSimpleModeX264Fallback();

    OBSContext  m_obsContext;
    OBSService m_service;
    ConfigFile    m_basicConfig;
    std::unique_ptr<BasicOutputHandler> m_outputHandler;

    bool streamingStopping = false;
    bool recordingStopping = false;
    bool replayBufferStopping = false;

};


