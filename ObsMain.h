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
    ObsMain();
    ~ObsMain();

    //ȫ��ʵ��
    static ObsMain* Instance();

    void InitObs() override;
    bool InitGlobalConfig();

    //������ƵԴ
    OBSSource CreateSource(const char *id, const char *name, ObsSourceConfig* config);

    //���source����ǰ����
    bool AddSource(OBSSource source);

    //��������ӿ�
    void RemoveSelectedSceneItem();
    std::vector<OBSSceneItem> GetSelectedSceneItem();
    

    //�����ק���ļ�
    bool AddDropSource(const char *data, DropType image);

    //���Դ�ļ�
    bool AddSourceFile(const char* file);

    //��������
    OBSScene FindScene(const char* name);
    OBSScene GetCurrentScene();

    void AddScene(const char* name,bool setCurrent);
    void SetCurrentScene(OBSScene scene);

    //���õ�ǰ����
    void SetCurrentScene(OBSSource scene, bool force = false);


    void Load(const char *file);
    void SaveProject();

    //��ʾ�˵�
    void DoShowMenu(const ObsPoint &pt) {
        if (m_observer)
            m_observer->OnMenu(pt);
    }
private:
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
