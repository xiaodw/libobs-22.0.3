#pragma once
#include <obs.hpp>
#include <util/util.hpp>
#include <map>
#include <string>
#include "ObsConfig.h"
#include "ObsWindowBase.h"

class ObsWindow;

//管理场景
class ObsMain
{
public:
    ObsMain();
    ~ObsMain();

    //全局实例
    static ObsMain* Instance();

    //重置初始化视频
    bool ResetVideo();

    bool ResetVideoSize(const ObsSize& canvasSize,const ObsSize& outSize);

    //创建视频源
    OBSSource CreateSource(const char *id, const char *name, ObsSourceConfig* config);

    //场景管理
    OBSScene AddScene(const char* name);
    void RemoveScene(const char* name);
    bool SelectScene(const char* name);

    OBSScene GetCurrentScene();

    config_t* config() { return m_globalConfig; }
    const char *InputAudioSource() const;
    const char *OutputAudioSource() const;

    void SetCurrentScene(OBSSource scene, bool force = false,
        bool direct = false);

private:
    //视频尺寸
    ObsSize m_canvasSize;
    ObsSize m_outSize;

    OBSContext  m_obsContext;
    std::map<std::string,OBSScene> m_scenes;
    OBSScene m_currentScene;

    ConfigFile  m_globalConfig;

};

