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



//管理场景
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

    //全局实例
    static ObsMain* Instance();

    bool InitGlobalConfig();
    bool LoadScene();

    //创建视频源
    OBSSource CreateSource(const char *id, const char *name, obs_data_t* settings = NULL, obs_data_t* hotkey=NULL);

    //添加source到当前场景
    bool AddSource(OBSSource source);

    //添加拖拽的文件
    bool AddDropSource(const char *data, DropType image);

    //添加源文件
    bool AddSourceFile(const char* file);

    //场景管理
    OBSScene FindScene(const char* name);
    OBSScene GetCurrentScene();

    void AddScene(const char* name,bool setCurrent);
    void SetCurrentScene(obs_scene_t* scene);
    void RemoveScene(obs_scene_t* scene);

    //设置当前场景
    void SetCurrentScene(OBSSource scene, bool force = false);


    void Load(const char *file);
    void SaveProject();

    //显示菜单
    void DoShowMenu(const ObsPoint &pt) {
        if (m_observer)
            m_observer->OnMenu(pt);
    }
    void FitToScreen();
    void StretchToScreen();

    //添加屏幕捕捉
    bool AddCaptureScreen(const char* name);

    //name和window可以通过EnumGameProcess获取
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

    //scene回调
    static void SceneReordered(void *data, calldata_t *params);
    static void SceneItemAdded(void *data, calldata_t *params);
    static void SceneItemSelected(void *data, calldata_t *params);
    static void SceneItemDeselected(void *data, calldata_t *params);

    //source操作回调
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

    //场景item维护
    ObsSceneItemList m_sceneItemList;
};

inline config_t *GetGlobalConfig() { return ObsMain::Instance()->globalConfig(); }
inline config_t *GetBasicConfig() { return ObsMain::Instance()->basicConfig(); }
