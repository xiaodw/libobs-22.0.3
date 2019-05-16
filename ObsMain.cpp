#include "ObsMain.h"
#include "ObsWindow.h"
#include "ObsPlatform.h"
#include "util/platform.h"

ObsMain* obsMain = NULL;

ObsMain::ObsMain()
    :m_sceneItemList(this)
{
    InitOBSCallbacks();
}

ObsMain::~ObsMain()
{
    ClearSceneData();
    m_scenes.clear();
    m_currentScene = nullptr;
    obsMain =  nullptr;
    m_signalHandlers.clear();
}

ObsMain* ObsMain::Instance()
{
    if (!obsMain)
        obsMain = new ObsMain();
    return obsMain;
}

OBSScene ObsMain::FindScene(const char* name)
{
    for (auto& sd : m_scenes)
    {
        if (strcmp(sd->name(), name) == 0)
        {
            return sd->scene;
        }
    }
    return NULL;
}

OBSScene ObsMain::GetCurrentScene()
{
    return m_currentScene;
}

void ObsMain::AddScene(const char* name, bool setCurrent)
{
    obs_scene_t  *scene = obs_scene_create(name);
    if(setCurrent)
        SetCurrentScene(scene);
    obs_scene_release(scene);
}

void ObsMain::SetCurrentScene(OBSScene scene)
{
    m_currentScene = scene;
    SetCurrentScene(obs_scene_get_source(scene));
}

void ObsMain::OnAddScene(OBSScene scene)
{
    std::lock_guard<std::mutex> lock(m_lock);

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

    blog(LOG_INFO, "add scene %s\n", obs_source_get_name(source));
    m_scenes.push_back(std::unique_ptr<SceneData>(data));
}

void ObsMain::OnRemoveScene(OBSScene scene)
{
    std::lock_guard<std::mutex> lock(m_lock);
    for (auto i=m_scenes.begin();i!=m_scenes.end();++i)
    {
        if (scene == (*i)->scene)
        {
            i = m_scenes.erase(i);
            break;
        }
    }
}

void ObsMain::SetCurrentScene(OBSSource scene, bool force)
{
    obs_scene_t* _scene= obs_scene_from_source(scene);
    if (_scene && m_currentScene!=_scene)
    {
        m_currentScene = _scene;
        m_sceneItemList.OnSceneChanged();
    }

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

    if (scene == pThis->m_currentScene)
        pThis->m_sceneItemList.OnReorderItems();
}

void ObsMain::SceneItemAdded(void *data, calldata_t *params)
{
    ObsMain* pThis = (ObsMain*)data;
    obs_sceneitem_t *item = (obs_sceneitem_t*)calldata_ptr(params, "item");
    pThis->m_sceneItemList.OnAdd(item);
}

void ObsMain::SceneItemSelected(void *data, calldata_t *params)
{
    ObsMain* pThis = (ObsMain*)data;
    obs_scene_t     *scene = (obs_scene_t*)calldata_ptr(params, "scene");
    obs_sceneitem_t *item = (obs_sceneitem_t*)calldata_ptr(params, "item");

    if(scene == pThis->m_currentScene)
        pThis->m_sceneItemList.OnSelect(item,true);
}

void ObsMain::SceneItemDeselected(void *data, calldata_t *params)
{
    ObsMain* pThis = (ObsMain*)data;
    obs_scene_t     *scene = (obs_scene_t*)calldata_ptr(params, "scene");
    obs_sceneitem_t *item = (obs_sceneitem_t*)calldata_ptr(params, "item");

    if (scene == pThis->m_currentScene)
        pThis->m_sceneItemList.OnSelect(item, false);
}

void ObsMain::SourceCreated(void *data, calldata_t *params)
{
    ObsMain* pThis = (ObsMain*)data;
    obs_source_t *source = (obs_source_t*)calldata_ptr(params, "source");

    obs_scene_t* scene = obs_scene_from_source(source);
    if (scene != NULL)
    {
        pThis->OnAddScene(scene);
    }

    if (pThis->m_observer)
        pThis->m_observer->OnAddScene(source);
}

void ObsMain::SourceRemoved(void *data, calldata_t *params)
{
    ObsMain* pThis = (ObsMain*)data;
    obs_source_t *source = (obs_source_t*)calldata_ptr(params, "source");

    obs_scene_t* scene = obs_scene_from_source(source);
    if (scene != NULL)
    {
        pThis->OnRemoveScene(scene);
    }

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

void ObsMain::InitObs()
{
    ObsBasic::InitObs();

    char savePath[512];
    char fileName[512];
    int ret;
    const char *sceneCollection = config_get_string(globalConfig(),
        "Basic", "SceneCollectionFile");

    if (!sceneCollection)
        throw "Failed to get scene collection name";

    ret = snprintf(fileName, 512, "obs-studio/basic/scenes/%s.json",
        sceneCollection);
    if (ret <= 0)
        throw "Failed to create scene collection file name";

   ret = GetConfigPath(savePath, sizeof(savePath), fileName);
    if (ret <= 0)
        throw "Failed to get scene collection json file path";

    Load(savePath);

    //CreateDefaultScene(true);
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


void ObsMain::ClearSceneData()
{
    obs_set_output_source(0, nullptr);
    obs_set_output_source(1, nullptr);
    obs_set_output_source(2, nullptr);
    obs_set_output_source(3, nullptr);
    obs_set_output_source(4, nullptr);
    obs_set_output_source(5, nullptr);

    auto cb = [](void *unused, obs_source_t *source)
    {
        obs_source_remove(source);
        UNUSED_PARAMETER(unused);
        return true;
    };

    obs_enum_sources(cb, nullptr);

    m_scenes.clear();
    m_currentScene = nullptr;
    m_curTransition = nullptr;


    blog(LOG_INFO, "All scene data cleared");
    blog(LOG_INFO, "------------------------------------------------");
}


void ObsMain::CreateDefaultScene(bool firstStart)
{
    ClearSceneData();
    InitDefaultTransitions();

    obs_scene_t  *scene = obs_scene_create(Str("Basic.Scene"));
    if (firstStart)
        InitAudioSources();

    SetCurrentScene(scene);
    obs_scene_release(scene);
}

static void LoadAudioDevice(const char *name, int channel, obs_data_t *parent)
{
    obs_data_t *data = obs_data_get_obj(parent, name);
    if (!data)
        return;

    obs_source_t *source = obs_load_source(data);
    if (source) {
        obs_set_output_source(channel, source);
        obs_source_release(source);
    }

    obs_data_release(data);
}

void ObsMain::LoadSceneListOrder(obs_data_array_t *array)
{
    size_t num = obs_data_array_count(array);

    for (size_t i = 0; i < num; i++) {
        obs_data_t *data = obs_data_array_item(array, i);
        const char *name = obs_data_get_string(data, "name");


        obs_data_release(data);
    }
}


void ObsMain::Load(const char *file)
{
    obs_data_t *data = obs_data_create_from_json_file_safe(file, "bak");
    if (!data) {
        blog(LOG_INFO, "No scene file found, creating default scene");
        CreateDefaultScene(true);
        SaveProject();
        return;
    }

    ClearSceneData();
    InitDefaultTransitions();

    obs_data_array_t *sceneOrder = obs_data_get_array(data, "scene_order");
    obs_data_array_t *sources = obs_data_get_array(data, "sources");
    obs_data_array_t *groups = obs_data_get_array(data, "groups");
    const char *sceneName = obs_data_get_string(data, "current_scene");

    int newDuration = obs_data_get_int(data, "transition_duration");
    if (!newDuration)
        newDuration = 300;

    const char *curSceneCollection = config_get_string(globalConfig(), "Basic", "SceneCollection");

    obs_data_set_default_string(data, "name", curSceneCollection);
    obs_source_t     *curScene;

    LoadAudioDevice(DESKTOP_AUDIO_1, 1, data);
    LoadAudioDevice(DESKTOP_AUDIO_2, 2, data);
    LoadAudioDevice(AUX_AUDIO_1, 3, data);
    LoadAudioDevice(AUX_AUDIO_2, 4, data);
    LoadAudioDevice(AUX_AUDIO_3, 5, data);

    if (!sources) {
        sources = groups;
        groups = nullptr;
    }
    else {
        obs_data_array_push_back_array(sources, groups);
    }

    obs_load_sources(sources, nullptr, nullptr);

    if (sceneOrder)
        LoadSceneListOrder(sceneOrder);

    curScene = obs_get_source_by_name(sceneName);
    
    if (curScene)
    {
        SetCurrentScene(curScene, true);
        obs_source_release(curScene);
    }
    else
    {
        CreateDefaultScene(false);
    }

    obs_data_array_release(sources);
    obs_data_array_release(groups);
    obs_data_array_release(sceneOrder);

    //bool fixedScaling = obs_data_get_bool(data, "scaling_enabled");
    //int scalingLevel = (int)obs_data_get_int(data, "scaling_level");
    //float scrollOffX = (float)obs_data_get_double(data, "scaling_off_x");
    //float scrollOffY = (float)obs_data_get_double(data, "scaling_off_y");

    obs_data_release(data);
}


void ObsMain::SaveProject()
{
    m_projectChanged = true;
    SaveProjectDeferred();
}

void ObsMain::SaveProjectDeferred()
{
    if (!m_projectChanged)
        return;

    m_projectChanged = false;

    const char *sceneCollection = config_get_string(globalConfig(),
        "Basic", "SceneCollectionFile");
    char savePath[512];
    char fileName[512];
    int ret;

    if (!sceneCollection)
        return;

    ret = snprintf(fileName, 512, "obs-studio/basic/scenes/%s.json",
        sceneCollection);
    if (ret <= 0)
        return;

    ret = GetConfigPath(savePath, sizeof(savePath), fileName);
    if (ret <= 0)
        return;

    Save(savePath);
}


obs_data_array_t *ObsMain::SaveSceneListOrder()
{
    obs_data_array_t *sceneOrder = obs_data_array_create();

    for (auto& sc: m_scenes) {
        obs_data_t *data = obs_data_create();
        obs_data_set_string(data, "name", sc->name());
        obs_data_array_push_back(sceneOrder, data);
        obs_data_release(data);
    }
    return sceneOrder;
}

static void SaveAudioDevice(const char *name, int channel, obs_data_t *parent,
    std::vector<OBSSource> &audioSources)
{
    obs_source_t *source = obs_get_output_source(channel);
    if (!source)
        return;

    audioSources.push_back(source);

    obs_data_t *data = obs_save_source(source);

    obs_data_set_obj(parent, name, data);

    obs_data_release(data);
    obs_source_release(source);
}

static obs_data_t *GenerateSaveData(config_t* config,obs_data_array_t *sceneOrder,
    OBSScene &scene, int transitionDuration)
{
    obs_data_t *saveData = obs_data_create();

    std::vector<OBSSource> audioSources;
    audioSources.reserve(5);

    SaveAudioDevice(DESKTOP_AUDIO_1, 1, saveData, audioSources);
    SaveAudioDevice(DESKTOP_AUDIO_2, 2, saveData, audioSources);
    SaveAudioDevice(AUX_AUDIO_1, 3, saveData, audioSources);
    SaveAudioDevice(AUX_AUDIO_2, 4, saveData, audioSources);
    SaveAudioDevice(AUX_AUDIO_3, 5, saveData, audioSources);

    /* -------------------------------- */
    /* save non-group sources           */

    auto FilterAudioSources = [&](obs_source_t *source)
    {
        if (obs_source_is_group(source))
            return false;

        return find(begin(audioSources), end(audioSources), source) ==
            end(audioSources);
    };
    using FilterAudioSources_t = decltype(FilterAudioSources);

    obs_data_array_t *sourcesArray = obs_save_sources_filtered(
        [](void *data, obs_source_t *source)
    {
        return (*static_cast<FilterAudioSources_t*>(data))(source);
    }, static_cast<void*>(&FilterAudioSources));

    /* -------------------------------- */
    /* save group sources separately    */

    /* saving separately ensures they won't be loaded in older versions */
    obs_data_array_t *groupsArray = obs_save_sources_filtered(
        [](void*, obs_source_t *source)
    {
        return obs_source_is_group(source);
    }, nullptr);

    /* -------------------------------- */

    obs_source_t *transition = obs_get_output_source(0);
    obs_source_t *currentScene = obs_scene_get_source(scene);
    const char   *sceneName = obs_source_get_name(currentScene);

    const char *sceneCollection = config_get_string(config,
        "Basic", "SceneCollection");

    obs_data_set_string(saveData, "current_scene", sceneName);
    obs_data_set_array(saveData, "scene_order", sceneOrder);
    obs_data_set_string(saveData, "name", sceneCollection);
    obs_data_set_array(saveData, "sources", sourcesArray);
    obs_data_set_array(saveData, "groups", groupsArray);
    obs_data_array_release(sourcesArray);
    obs_data_array_release(groupsArray);

    obs_data_set_string(saveData, "current_transition",
        obs_source_get_name(transition));
    obs_data_set_int(saveData, "transition_duration", transitionDuration);
    obs_source_release(transition);

    return saveData;
}

void ObsMain::Save(const char *file)
{
    OBSScene scene = GetCurrentScene();
    OBSSource curProgramScene = obs_scene_get_source(scene);

    obs_data_array_t *sceneOrder = SaveSceneListOrder();

    obs_data_t *saveData = GenerateSaveData(globalConfig(), sceneOrder,
        scene, 300);

    if (!obs_data_save_json_safe(saveData, file, "tmp", "bak"))
        blog(LOG_ERROR, "Could not save scene data to %s", file);

    obs_data_release(saveData);
    obs_data_array_release(sceneOrder);
}



static bool get_selected_item(obs_scene_t *scene, obs_sceneitem_t *item, void *param)
{
    std::vector<OBSSceneItem> * items =
        reinterpret_cast<std::vector<OBSSceneItem>*>(param);

    if (obs_sceneitem_is_group(item))
        obs_sceneitem_group_enum_items(item, get_selected_item, param);

    if (obs_sceneitem_selected(item))
    {
        items->push_back(item);
    }
    UNUSED_PARAMETER(scene);
    return true;
}

static bool CenterAlignSelectedItems(obs_scene_t *scene, obs_sceneitem_t *item,
    void *param)
{
    obs_bounds_type boundsType = *reinterpret_cast<obs_bounds_type*>(param);

    if (obs_sceneitem_is_group(item))
        obs_sceneitem_group_enum_items(item, CenterAlignSelectedItems,
            param);
    if (!obs_sceneitem_selected(item))
        return true;

    obs_video_info ovi;
    obs_get_video_info(&ovi);

    obs_transform_info itemInfo;
    vec2_set(&itemInfo.pos, 0.0f, 0.0f);
    vec2_set(&itemInfo.scale, 1.0f, 1.0f);
    itemInfo.alignment = OBS_ALIGN_LEFT | OBS_ALIGN_TOP;
    itemInfo.rot = 0.0f;

    vec2_set(&itemInfo.bounds,
        float(ovi.base_width), float(ovi.base_height));
    itemInfo.bounds_type = boundsType;
    itemInfo.bounds_alignment = OBS_ALIGN_CENTER;

    obs_sceneitem_set_info(item, &itemInfo);

    UNUSED_PARAMETER(scene);
    return true;
}

void ObsMain::FitToScreen()
{
    obs_bounds_type boundsType = OBS_BOUNDS_SCALE_INNER;
    obs_scene_enum_items(GetCurrentScene(), CenterAlignSelectedItems,
        &boundsType);
}

void ObsMain::StretchToScreen()
{
    obs_bounds_type boundsType = OBS_BOUNDS_STRETCH;
    obs_scene_enum_items(GetCurrentScene(), CenterAlignSelectedItems,
        &boundsType);
}

