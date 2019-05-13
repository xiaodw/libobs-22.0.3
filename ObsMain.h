#pragma once
#include <obs.hpp>
#include <util/util.hpp>
#include <map>
#include <string>
#include "ObsConfig.h"
#include "ObsWindowBase.h"
#include "ObsBasic.h"

class ObsWindow;

//������
class ObsMain:public ObsBasic
{
public:
    ObsMain();
    ~ObsMain();

    //ȫ��ʵ��
    static ObsMain* Instance();

    bool InitGlobalConfig();

    //������ƵԴ
    OBSSource CreateSource(const char *id, const char *name, ObsSourceConfig* config);

    //��������
    OBSScene AddScene(const char* name);
    void RemoveScene(const char* name);
    bool SelectScene(const char* name);
    OBSScene FindScene(const char* name);
    OBSScene GetCurrentScene();

    config_t* globalConfig()const  { return m_globalConfig; }

    //���õ�ǰ����
    void SetCurrentScene(OBSSource scene, bool force = false);

    void TransitionToScene(OBSSource source, bool force = false);

    int GetProfilePath(char *path, size_t size, const char *file) const;
private:
    bool InitGlobalConfigDefaults();

    std::map<std::string,OBSScene> m_scenes;
    OBSScene m_currentScene;
    ConfigFile  m_globalConfig;
};

inline config_t *GetGlobalConfig() { return ObsMain::Instance()->globalConfig(); }
inline config_t *GetBasicConfig() { return ObsMain::Instance()->basicConfig(); }
