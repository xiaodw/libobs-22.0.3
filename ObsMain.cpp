#include "ObsMain.h"
#include "ObsWindow.h"
#include "ObsPlatform.h"
#include "util/platform.h"

ObsMain* obsMain = NULL;

ObsMain::ObsMain()
{
    InitOBSCallbacks();
}

ObsMain::~ObsMain()
{
    m_scenes.clear();
    m_currentScene = NULL;
    obsMain =  nullptr;
}

ObsMain* ObsMain::Instance()
{
    if (!obsMain)
        obsMain = new ObsMain();
    return obsMain;
}

OBSScene ObsMain::FindScene(const char* name)
{
    auto find = m_scenes.find(name);
    if (find != m_scenes.end())
        return find->second->scene;
    return NULL;
}

OBSScene ObsMain::GetCurrentScene()
{
    return m_currentScene;
}

OBSScene ObsMain::AddScene(const char* name)
{
    //c指针赋值给c++后需要释放
    OBSScene scene = obs_scene_create(name);
    if (!scene)
        return NULL;

    if (m_scenes.size() == 0)
    {
        m_currentScene = scene;
        SetCurrentScene(obs_scene_get_source(m_currentScene),true);
    }

    SceneData* data = new SceneData(scene);

    obs_source_t *source = obs_scene_get_source(scene);
    signal_handler_t *handler = obs_source_get_signal_handler(source);

    data->handlers.assign({
        std::make_shared<OBSSignal>(handler, "item_add",
        ObsMain::SceneItemAdded, this),
        std::make_shared<OBSSignal>(handler, "item_select",
        ObsMain::SceneItemSelected, this),
        std::make_shared<OBSSignal>(handler, "item_deselect",
        ObsMain::SceneItemDeselected, this),
        std::make_shared<OBSSignal>(handler, "reorder",
        ObsMain::SceneReordered, this),
    });

    m_scenes[name] = std::unique_ptr<SceneData>(data);
    obs_scene_release(scene);
    return scene;
}

void ObsMain::RemoveScene(const char* name)
{
    auto find = m_scenes.find(name);
    if (find != m_scenes.end())
    {
        obs_source_t *source = obs_scene_get_source(find->second->scene);

        //移除scene
        if(source)
            obs_source_remove(source);

        m_scenes.erase(find);

        //自动选中下一个scene
    }
}

bool ObsMain::SelectScene(const char* name)
{
    auto find = m_scenes.find(name);
    if (find != m_scenes.end())
    {
        m_currentScene = find->second->scene;
        SetCurrentScene(obs_scene_get_source(m_currentScene));
        return true;
    }
    return false;
}

void ObsMain::SetCurrentScene(OBSSource scene, bool force)
{
    if (m_curTransition)
    {
        TransitionToScene(scene,force);
    }
    else if (m_currentScene)
    {
        OBSSource actualLastScene = obs_scene_get_source(m_currentScene);
        if (actualLastScene != scene) {
            if (scene)
                obs_source_inc_showing(scene);
            if (actualLastScene)
                obs_source_dec_showing(actualLastScene);
        }
    }
}

void ObsMain::TransitionToScene(OBSSource source, bool force)
{
    obs_scene_t *scene = obs_scene_from_source(source);
    if (!scene)
        return;

    OBSSource transition = obs_get_output_source(0);
    if (!transition)
        return;

    if (force) {
        obs_transition_set(transition, source);
    }
    else {
        int duration = 300;
        obs_transition_start(transition,
            OBS_TRANSITION_MODE_AUTO, duration, source);
    }

    obs_source_release(transition);
}

OBSSource ObsMain::CreateSource(const char *id, const char *name, ObsSourceConfig* config)
{
    obs_data_t* settings = NULL;
    obs_data_t* hotkey = NULL;
    if (config)
    {
        settings = config->settings;
        hotkey = config->hotkey;
    }

    OBSSource source = obs_source_create(id, name, settings, hotkey);
    if (!source)
        return NULL;

    obs_source_release(source);
    return source;
}

bool ObsMain::AddSource(OBSSource source)
{
    if (!m_currentScene || !source)
        return false;
    obs_sceneitem_t* item= obs_scene_add(m_currentScene, source);
    if (item)
    {
        //obs_sceneitem_release(item);
        return true;
    }
    else
        return false;
}

void ObsMain::InitOBSCallbacks()
{
    m_signalHandlers.reserve(m_signalHandlers.size() + 6);
    m_signalHandlers.emplace_back(obs_get_signal_handler(), "source_create",
        ObsMain::SourceCreated, this);
    m_signalHandlers.emplace_back(obs_get_signal_handler(), "source_remove",
        ObsMain::SourceRemoved, this);
    m_signalHandlers.emplace_back(obs_get_signal_handler(), "source_activate",
        ObsMain::SourceActivated, this);
    m_signalHandlers.emplace_back(obs_get_signal_handler(), "source_deactivate",
        ObsMain::SourceDeactivated, this);
    m_signalHandlers.emplace_back(obs_get_signal_handler(), "source_rename",
        ObsMain::SourceRenamed, this);
}

void ObsMain::SceneReordered(void *data, calldata_t *params)
{
    ObsMain* pThis = (ObsMain*)data;

    obs_scene_t *scene = (obs_scene_t*)calldata_ptr(params, "scene");

    if(pThis->m_observer)
        pThis->m_observer->OnReorderSources(scene);
}

void ObsMain::SceneItemAdded(void *data, calldata_t *params)
{
    ObsMain* pThis = (ObsMain*)data;
    obs_sceneitem_t *item = (obs_sceneitem_t*)calldata_ptr(params, "item");
    if (pThis->m_observer)
        pThis->m_observer->OnAddSceneItem(item);
}

void ObsMain::SceneItemSelected(void *data, calldata_t *params)
{
    ObsMain* pThis = (ObsMain*)data;
    obs_scene_t     *scene = (obs_scene_t*)calldata_ptr(params, "scene");
    obs_sceneitem_t *item = (obs_sceneitem_t*)calldata_ptr(params, "item");
    if (pThis->m_observer)
        pThis->m_observer->OnSelectSceneItem(scene,item,true);
}

void ObsMain::SceneItemDeselected(void *data, calldata_t *params)
{
    ObsMain* pThis = (ObsMain*)data;
    obs_scene_t     *scene = (obs_scene_t*)calldata_ptr(params, "scene");
    obs_sceneitem_t *item = (obs_sceneitem_t*)calldata_ptr(params, "item");
    if (pThis->m_observer)
        pThis->m_observer->OnSelectSceneItem(scene, item, false);
}

void ObsMain::SourceCreated(void *data, calldata_t *params)
{
    ObsMain* pThis = (ObsMain*)data;
    obs_source_t *source = (obs_source_t*)calldata_ptr(params, "source");
    if (pThis->m_observer)
        pThis->m_observer->OnAddScene(source);
}

void ObsMain::SourceRemoved(void *data, calldata_t *params)
{
    ObsMain* pThis = (ObsMain*)data;
    obs_source_t *source = (obs_source_t*)calldata_ptr(params, "source");
    if (pThis->m_observer)
        pThis->m_observer->OnRemoveScene(source);
}

void ObsMain::SourceActivated(void *data, calldata_t *params)
{
    ObsMain* pThis = (ObsMain*)data;
    obs_source_t *source = (obs_source_t*)calldata_ptr(params, "source");
    uint32_t     flags = obs_source_get_output_flags(source);

    if (flags & OBS_SOURCE_AUDIO && pThis->m_observer)
    {
        pThis->m_observer->OnActivateAudioSource(source);
    }
}

void ObsMain::SourceDeactivated(void *data, calldata_t *params)
{
    ObsMain* pThis = (ObsMain*)data;
    obs_source_t *source = (obs_source_t*)calldata_ptr(params, "source");
    uint32_t     flags = obs_source_get_output_flags(source);

    if (flags & OBS_SOURCE_AUDIO && pThis->m_observer)
    {
        pThis->m_observer->OnDeactivateAudioSource(source);
    }
}

void ObsMain::SourceRenamed(void *data, calldata_t *params)
{
    ObsMain* pThis = (ObsMain*)data;
    obs_source_t *source = (obs_source_t*)calldata_ptr(params, "source");
    const char *newName = calldata_string(params, "new_name");
    const char *prevName = calldata_string(params, "prev_name");

    if (pThis->m_observer)
        pThis->m_observer->OnRenameSources(source, prevName, newName);

    blog(LOG_INFO, "Source '%s' renamed to '%s'", prevName, newName);
}


static std::string GenerateSourceName(const  std::string &base)
{
    std::string name;
    int inc = 0;

    for (;; inc++) {
        name = base;

        if (inc) {
            name += " (";
            name += std::to_string(inc + 1);
            name += ")";
        }

        obs_source_t *source = obs_get_source_by_name(name.c_str());
        if (!source)
            return name;
    }
}


bool ObsMain::AddDropSource(const char *data, DropType image)
{
    obs_data_t *settings = obs_data_create();
    obs_source_t *source = nullptr;
    const char *type = nullptr;
    std::string name;

    switch (image) {
    case DropType_RawText:
        obs_data_set_string(settings, "text", data);
#ifdef _WIN32
        type = "text_gdiplus";
#else
        type = "text_ft2_source";
#endif
        break;
    case DropType_Text:
#ifdef _WIN32
        obs_data_set_bool(settings, "read_from_file", true);
        obs_data_set_string(settings, "file", data);
        name = GetFileName(name);
        type = "text_gdiplus";
#else
        obs_data_set_bool(settings, "from_file", true);
        obs_data_set_string(settings, "text_file", data);
        type = "text_ft2_source";
#endif
        break;
    case DropType_Image:
        obs_data_set_string(settings, "file", data);
        name = GetFileName(name);
        type = "image_source";
        break;
    case DropType_Media:
        obs_data_set_string(settings, "local_file", data);
        name = GetFileName(name);
        type = "ffmpeg_source";
        break;
    }

    if (!obs_source_get_display_name(type)) {
        obs_data_release(settings);
        return false;
    }

    if (name.empty())
        name = obs_source_get_display_name(type);
    source = obs_source_create(type,
        GenerateSourceName(name).c_str(),
        settings, nullptr);

    if (source) {
        OBSScene scene = GetCurrentScene();
        obs_scene_add(scene, source);
        obs_source_release(source);
    }

    obs_data_release(settings);

    return source != nullptr;
}




static const char *textExtensions[] = {
    "txt", "log", nullptr
};

static const char *imageExtensions[] = {
    "bmp", "tga", "png", "jpg", "jpeg", "gif", nullptr
};

static const char *mediaExtensions[] = {
    "3ga", "669", "a52", "aac", "ac3", "adt", "adts", "aif", "aifc",
    "aiff", "amb", "amr", "aob", "ape", "au", "awb", "caf", "dts",
    "flac", "it", "kar", "m4a", "m4b", "m4p", "m5p", "mid", "mka",
    "mlp", "mod", "mpa", "mp1", "mp2", "mp3", "mpc", "mpga", "mus",
    "oga", "ogg", "oma", "opus", "qcp", "ra", "rmi", "s3m", "sid",
    "spx", "tak", "thd", "tta", "voc", "vqf", "w64", "wav", "wma",
    "wv", "xa", "xm", "3g2", "3gp", "3gp2", "3gpp", "amv", "asf", "avi",
    "bik", "crf", "divx", "drc", "dv", "evo", "f4v", "flv", "gvi",
    "gxf", "iso", "m1v", "m2v", "m2t", "m2ts", "m4v", "mkv", "mov",
    "mp2", "mp2v", "mp4", "mp4v", "mpe", "mpeg", "mpeg1", "mpeg2",
    "mpeg4", "mpg", "mpv2", "mts", "mtv", "mxf", "mxg", "nsv", "nuv",
    "ogg", "ogm", "ogv", "ogx", "ps", "rec", "rm", "rmvb", "rpl", "thp",
    "tod", "ts", "tts", "txd", "vob", "vro", "webm", "wm", "wmv", "wtv",
    nullptr
};

bool ObsMain::AddSourceFile(const char* file)
{
    const char **cmp;
    std::string suffix = GetFilePostfix(file);
    bool found = false;

#define CHECK_SUFFIX(extensions, type) \
cmp = extensions; \
while (*cmp) { \
	if (stricmp(*cmp, suffix.c_str()) == 0) { \
		found = AddDropSource(file, type); \
		break; \
	} \
\
	cmp++; \
} \
\
if (found) \
	break;

    do {
        CHECK_SUFFIX(textExtensions, DropType_Text);
        CHECK_SUFFIX(imageExtensions, DropType_Image);
        CHECK_SUFFIX(mediaExtensions, DropType_Media);
    } while (0);

#undef CHECK_SUFFIX

    return found;
}


#ifdef __APPLE__
#define BASE_PATH ".."
#else
#define BASE_PATH ".."
#endif

#define CONFIG_PATH BASE_PATH "/config"

#ifndef OBS_UNIX_STRUCTURE
#define OBS_UNIX_STRUCTURE 0
#endif

bool portable_mode = false;

int GetConfigPath(char *path, size_t size, const char *name)
{
    if (!OBS_UNIX_STRUCTURE && portable_mode) {
        if (name && *name) {
            return snprintf(path, size, CONFIG_PATH "/%s", name);
        }
        else {
            return snprintf(path, size, CONFIG_PATH);
        }
    }
    else {
        return os_get_config_path(path, size, name);
    }
}

int ObsMain::GetProfilePath(char *path, size_t size, const char *file)const
{
    char profiles_path[512];
    const char *profile = config_get_string(globalConfig(),"Basic", "Profile");
    int ret;

    if (!profile)
    {
        //给个默认名
        profile = "profile";
        config_set_string(globalConfig(), "Basic", "Profile", profile);
    }

    if (!path)
        return -1;
    if (!file)
        file = "";

    ret = GetConfigPath(profiles_path, 512, "obs-studio/basic/profiles");
    if (ret <= 0)
        return ret;

    if (!*file)
        return snprintf(path, size, "%s/%s", profiles_path, profile);

    return snprintf(path, size, "%s/%s/%s", profiles_path, profile, file);
}

bool ObsMain::InitGlobalConfig()
{
    char path[512];
    bool changed = false;

    int len = GetConfigPath(path, sizeof(path),
        "obs-studio/global.ini");
    if (len <= 0) {
        return false;
    }

    int errorcode = m_globalConfig.Open(path, CONFIG_OPEN_ALWAYS);
    if (errorcode != CONFIG_SUCCESS) {
        blog(LOG_ERROR, "Failed to open global.ini: %d", errorcode);
        return false;
    }

    if (!config_has_user_value(m_globalConfig, "General", "Pre19Defaults")) {
        uint32_t lastVersion = config_get_int(m_globalConfig, "General",
            "LastVersion");
        bool useOldDefaults = lastVersion &&
            lastVersion < MAKE_SEMANTIC_VERSION(19, 0, 0);

        config_set_bool(m_globalConfig, "General", "Pre19Defaults",
            useOldDefaults);
        changed = true;
    }

    if (!config_has_user_value(m_globalConfig, "General", "Pre21Defaults")) {
        uint32_t lastVersion = config_get_int(m_globalConfig, "General",
            "LastVersion");
        bool useOldDefaults = lastVersion &&
            lastVersion < MAKE_SEMANTIC_VERSION(21, 0, 0);

        config_set_bool(m_globalConfig, "General", "Pre21Defaults",
            useOldDefaults);
        changed = true;
    }


    if (changed)
        config_save_safe(m_globalConfig, "tmp", nullptr);

    return InitGlobalConfigDefaults();
}


#define DEFAULT_LANG "en-US"

bool ObsMain::InitGlobalConfigDefaults()
{
    config_set_default_string(m_globalConfig, "General", "Language",
        DEFAULT_LANG);
    config_set_default_uint(m_globalConfig, "General", "MaxLogs", 10);
    config_set_default_int(m_globalConfig, "General", "InfoIncrement", -1);
    config_set_default_string(m_globalConfig, "General", "ProcessPriority",
        "Normal");
    config_set_default_bool(m_globalConfig, "General", "EnableAutoUpdates",
        true);

#if _WIN32
    config_set_default_string(m_globalConfig, "Video", "Renderer",
        "Direct3D 11");
#else
    config_set_default_string(globalConfig, "Video", "Renderer", "OpenGL");
#endif

    config_set_default_bool(m_globalConfig, "BasicWindow", "PreviewEnabled",
        true);
    config_set_default_bool(m_globalConfig, "BasicWindow",
        "PreviewProgramMode", false);
    config_set_default_bool(m_globalConfig, "BasicWindow",
        "SceneDuplicationMode", true);
    config_set_default_bool(m_globalConfig, "BasicWindow",
        "SwapScenesMode", true);
    config_set_default_bool(m_globalConfig, "BasicWindow",
        "SnappingEnabled", true);
    config_set_default_bool(m_globalConfig, "BasicWindow",
        "ScreenSnapping", true);
    config_set_default_bool(m_globalConfig, "BasicWindow",
        "SourceSnapping", true);
    config_set_default_bool(m_globalConfig, "BasicWindow",
        "CenterSnapping", false);
    config_set_default_double(m_globalConfig, "BasicWindow",
        "SnapDistance", 10.0);
    config_set_default_bool(m_globalConfig, "BasicWindow",
        "RecordWhenStreaming", false);
    config_set_default_bool(m_globalConfig, "BasicWindow",
        "KeepRecordingWhenStreamStops", false);
    config_set_default_bool(m_globalConfig, "BasicWindow",
        "SysTrayEnabled", true);
    config_set_default_bool(m_globalConfig, "BasicWindow",
        "SysTrayWhenStarted", false);
    config_set_default_bool(m_globalConfig, "BasicWindow",
        "SaveProjectors", false);
    config_set_default_bool(m_globalConfig, "BasicWindow",
        "ShowTransitions", true);

    config_set_default_bool(m_globalConfig, "BasicWindow",
        "VerticalVolControl", false);

    config_set_default_bool(m_globalConfig, "BasicWindow",
        "MultiviewMouseSwitch", true);

    config_set_default_bool(m_globalConfig, "BasicWindow",
        "MultiviewDrawNames", true);

    config_set_default_bool(m_globalConfig, "BasicWindow",
        "MultiviewDrawAreas", true);

    return true;
}
