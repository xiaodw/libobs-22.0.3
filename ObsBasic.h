#pragma once
#include <memory>
#include "ObsOutputs.h"
#include <util/util.hpp>

#define Str(s) s

struct RtmpServer
{
    std::string server;
    std::string key;
    bool use_auth = false;
    std::string username;
    std::string password;
};

class ObsObserver
{
public:
    virtual ~ObsObserver() {}

    virtual void OnReorderSources(OBSScene scene) = 0;
    virtual void OnAddSceneItem(OBSSceneItem item) = 0;
    virtual void OnSelectSceneItem(OBSScene scene,OBSSceneItem item,bool sel) = 0;

    virtual void OnAddScene(OBSSource source) =0;
    virtual void OnRemoveScene(OBSSource source) = 0;
    
    virtual void OnActivateAudioSource(OBSSource source) =0;

    virtual void OnDeactivateAudioSource(OBSSource source) = 0;

    virtual void OnRenameSources(OBSSource source,
        const char* oldName,const char* newName) = 0;

    //推流回调接口
    virtual void OnStreamStopping() = 0;
    virtual void OnStreamingStart() = 0;
    virtual void OnStreamingStop(int code, const char* error) = 0;

    //录制回调接口
    virtual void OnRecordingStart() = 0;
    virtual void OnRecordStopping() = 0;
    virtual void OnRecordingStop(int code) = 0;
};


class ObsBasic
{
public:
    ObsBasic();
    ~ObsBasic();

    //初始化obs
    void InitObs();

    obs_service_t *GetService();
    void  SetService(obs_service_t *service);


    bool SetRtmpServer(const RtmpServer& server);

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
    
    void SetTransition(OBSSource transition);

    void EnableDesktopAudio(bool enable);
    void EnableInputAudio(bool enable);

protected:
    bool InitBasicConfig();

    //初始化配置
    bool InitBasicConfigDefaults();
    //初始化默认转换
    void InitDefaultTransitions();

    void GetConfigFPS(uint32_t &num, uint32_t &den) const;
    void GetFPSCommon(uint32_t &num, uint32_t &den) const;
    void GetFPSInteger(uint32_t &num, uint32_t &den) const;
    void GetFPSFraction(uint32_t &num, uint32_t &den) const;
    void GetFPSNanoseconds(uint32_t &num, uint32_t &den) const;

    int  ResetVideo();
    bool ResetAudio();
    void ResetOutputs();

    void ResetAudioDevice(const char *sourceId, const char *deviceId,
        const char *deviceDesc, int channel);

    bool  InitService();

    void CheckForSimpleModeX264Fallback();

    OBSContext  m_context;
    OBSService m_service;
    ConfigFile    m_basicConfig;
    std::unique_ptr<BasicOutputHandler> m_outputHandler;

    OBSSource m_curTransition; //当前场景转换

    bool m_streamingStopping = false;
    bool m_recordingStopping = false;
    bool m_replayBufferStopping = false;

    ObsObserver* m_observer = nullptr;
};


