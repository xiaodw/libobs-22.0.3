#pragma once
#include <obs.hpp>
#include <util/util.hpp>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include "ObsConfig.h"
#include "ObsWindowBase.h"
#include "ObsBasic.h"

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
    ObsMain();
    ~ObsMain();

    //全局实例
    static ObsMain* Instance();

    bool InitGlobalConfig();

    //创建视频源
    OBSSource CreateSource(const char *id, const char *name, ObsSourceConfig* config);

    //添加source到当前场景
    bool AddSource(OBSSource source);

    //添加拖拽的文件
    bool AddDropSource(const char *data, DropType image);

    //添加源文件
    bool AddSourceFile(const char* file);

    //场景管理
    OBSScene AddScene(const char* name);
    void RemoveScene(const char* name);
    bool SelectScene(const char* name);
    OBSScene FindScene(const char* name);
    OBSScene GetCurrentScene();

    config_t* globalConfig()const  { return m_globalConfig; }

    //设置当前场景
    void SetCurrentScene(OBSSource scene, bool force = false);

    void TransitionToScene(OBSSource source, bool force = false);

    int GetProfilePath(char *path, size_t size, const char *file) const;
private:

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


    struct SceneData {
        SceneData(OBSScene s)
            :scene(s)
        {
        }
        OBSScene scene;
        std::vector<std::shared_ptr<OBSSignal>> handlers;
    };

    std::map<std::string,std::unique_ptr<SceneData>> m_scenes;
    OBSScene m_currentScene;
    ConfigFile  m_globalConfig;

    std::vector<OBSSignal> m_signalHandlers;
};

inline config_t *GetGlobalConfig() { return ObsMain::Instance()->globalConfig(); }
inline config_t *GetBasicConfig() { return ObsMain::Instance()->basicConfig(); }
