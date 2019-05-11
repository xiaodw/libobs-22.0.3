#pragma once
#include <obs.hpp>
#include <util/util.hpp>
#include <map>
#include <string>
#include "ObsConfig.h"
#include "ObsWindowBase.h"

class ObsWindow;

//������
class ObsMain
{
public:
    ObsMain();
    ~ObsMain();

    //ȫ��ʵ��
    static ObsMain* Instance();

    //������ƵԴ
    OBSSource CreateSource(const char *id, const char *name, ObsSourceConfig* config);

    //��������
    OBSScene AddScene(const char* name);
    void RemoveScene(const char* name);
    bool SelectScene(const char* name);
    OBSScene FindScene(const char* name);
    OBSScene GetCurrentScene();

    config_t* config()const  { return m_globalConfig; }
    const char *InputAudioSource() const;
    const char *OutputAudioSource() const;

    void SetCurrentScene(OBSSource scene);

    int GetProfilePath(char *path, size_t size, const char *file) const;
private:
    OBSContext  m_obsContext;
    std::map<std::string,OBSScene> m_scenes;
    OBSScene m_currentScene;
    ConfigFile  m_globalConfig;
};

inline config_t *GetGlobalConfig() { return ObsMain::Instance()->config(); }
