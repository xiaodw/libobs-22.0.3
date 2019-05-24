#pragma once
#include <obs.hpp>
#include <util/util.hpp>
#include <vector>
#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include "ObsConfig.h"
#include "ObsBasic.h"
#include "ObsWindowBase.h"
#include "ObsSceneItemList.h"

class ObsWindow;

enum DropType {
    DropType_RawText,
    DropType_Text,
    DropType_Image,
    DropType_Media,
};



//������
class ObsMain:public ObsBasic
{
public:
    struct SceneData {
        SceneData(OBSScene s)
            :scene(s)
        {
        }

        ~SceneData()
        {
        }

        const char* name() {
            return obs_source_get_name(obs_scene_get_source(scene));
        }

        OBSScene scene;
        std::vector<std::shared_ptr<OBSSignal>> handlers;
    };

    ObsMain();
    ~ObsMain();

    //ȫ��ʵ��
    static ObsMain* Instance();

    bool InitGlobalConfig();
    bool LoadScene();

    //������ƵԴ
    OBSSource CreateSource(const char *id, const char *name, obs_data_t* settings = NULL, obs_data_t* hotkey=NULL);

    //���source����ǰ����
    bool AddSource(OBSSource source);

    //�����ק���ļ�
    bool AddDropSource(const char *data, DropType image);

    //���Դ�ļ�
    bool AddSourceFile(const char* file);

    //��������
    OBSScene FindScene(const char* name);
    OBSScene GetCurrentScene();

    void AddScene(const char* name,bool setCurrent);
    void SetCurrentScene(obs_scene_t* scene);
    void RemoveScene(obs_scene_t* scene);
    void RenameScene(obs_scene_t* scene,const char* newName);

    //���õ�ǰ����
    void SetCurrentScene(OBSSource scene, bool force = false);


    void Load(const char *file);
    void SaveProject();

    //��ʾ�˵�
    void DoShowMenu(window_handle_t handle, const ObsPoint &pt) {
        if (m_observer)
            m_observer->OnMenu(handle,pt);
    }
    void FitToScreen();

    //�����Ļ��׽
    bool AddCaptureScreen(const char* name);

    //name��window����ͨ��EnumGameProcess��ȡ
    bool AddGameCapture(const char* name,const char* window);
    bool AddWindowCapture(const char* name, const char* window);

    bool AddCamera(const char* name, const char* deviceid,bool enableGreeenBkg);
    bool AddAudio(const char* name, const char* deviceid);

    bool AddImage(const char* path,int alpha);

    struct VideoData {
        std::string name;
        std::string url;
        bool isFile;
        bool isLoop;
        bool isActiveReplay;
    };
    bool AddVideo(const VideoData* video);
    bool AddText(const char* text);

    ObsSceneItemList& sceneItemList() { return m_sceneItemList; }
    std::vector<std::unique_ptr<SceneData>>& scenes() { return m_scenes; }

private:
    OBSScene CheckScene(obs_scene_t* scene);

    void OnAddScene(OBSScene scene);
    void OnRemoveScene(OBSScene scene);

    void ClearSceneData();
    void CreateDefaultScene(bool firstStart);
    void LoadSceneListOrder(obs_data_array_t *array);

    obs_data_array_t *SaveSceneListOrder();
    void Save(const char *file);
    void SaveProjectDeferred();

    //scene�ص�
    static void SceneReordered(void *data, calldata_t *params);
    static void SceneItemAdded(void *data, calldata_t *params);
    static void SceneItemSelected(void *data, calldata_t *params);
    static void SceneItemDeselected(void *data, calldata_t *params);

    //source�����ص�
    static void SourceCreated(void *data, calldata_t *params);
    static void SourceRemoved(void *data, calldata_t *params);
    static void SourceActivated(void *data, calldata_t *params);
    static void SourceDeactivated(void *data, calldata_t *params);
    static void SourceRenamed(void *data, calldata_t *params);


    bool InitGlobalConfigDefaults();
    void InitOBSCallbacks();

    void TransitionToScene(OBSSource source, bool force = false);

    std::mutex m_lock;
    std::vector<std::unique_ptr<SceneData>> m_scenes;
    OBSScene m_currentScene;


    std::vector<OBSSignal> m_signalHandlers;

    bool m_loaded = false;
    bool m_projectChanged = false;

    //����itemά��
    ObsSceneItemList m_sceneItemList;
};

inline config_t *GetGlobalConfig() { return ObsMain::Instance()->globalConfig(); }
inline config_t *GetBasicConfig() { return ObsMain::Instance()->basicConfig(); }
