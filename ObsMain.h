#pragma once
#include <obs.hpp>
#include <util/util.hpp>
#include <map>
#include <string>

//������
class ObsMain
{
public:
    ObsMain();
    ~ObsMain();

    //ȫ��ʵ��
    static ObsMain* Instance();

    //��������
    OBSScene AddScene(const char* name);
    void RemoveScene(const char* name);
    bool SelectScene(const char* name);

    OBSScene GetCurrentScene();

    config_t* config() { return m_globalConfig; }
    const char *InputAudioSource() const;
    const char *OutputAudioSource() const;

private:
    std::map<std::string,OBSScene> m_scenes;
    OBSScene m_currentScene;

    ConfigFile  m_globalConfig;
    OBSContext  m_obsContext;
};

