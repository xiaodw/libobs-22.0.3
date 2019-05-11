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

    //���ó�ʼ����Ƶ
    bool ResetVideo();

    bool ResetVideoSize(const ObsSize& canvasSize,const ObsSize& outSize);

    //������ƵԴ
    OBSSource CreateSource(const char *id, const char *name, ObsSourceConfig* config);

    //��������
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
    //��Ƶ�ߴ�
    ObsSize m_canvasSize;
    ObsSize m_outSize;

    OBSContext  m_obsContext;
    std::map<std::string,OBSScene> m_scenes;
    OBSScene m_currentScene;

    ConfigFile  m_globalConfig;

};

