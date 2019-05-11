#pragma once
#include <obs.hpp>
#include <util/util.hpp>
#include <map>
#include <string>
#include "ObsConfig.h"
#include "ObsWindowBase.h"
#include "ObsBasic.h"

class ObsWindow;

//管理场景
class ObsMain:public ObsBasic
{
public:
    ObsMain();
    ~ObsMain();

    //全局实例
    static ObsMain* Instance();

    bool InitGlobalConfig();

    //创建视频源
    OBSSource CreateSource(const char *id, const char *name, ObsSourceConfig* config);

    //场景管理
    OBSScene AddScene(const char* name);
    void RemoveScene(const char* name);
    bool SelectScene(const char* name);
    OBSScene FindScene(const char* name);
    OBSScene GetCurrentScene();

    config_t* globalConfig()const  { return m_globalConfig; }

    void SetCurrentScene(OBSSource scene);

    int GetProfilePath(char *path, size_t size, const char *file) const;
private:
    bool InitGlobalConfigDefaults();

    std::map<std::string,OBSScene> m_scenes;
    OBSScene m_currentScene;
    ConfigFile  m_globalConfig;
};

inline config_t *GetGlobalConfig() { return ObsMain::Instance()->globalConfig(); }
inline config_t *GetBasicConfig() { return ObsMain::Instance()->basicConfig(); }