#include "ObsMain.h"
#include "ObsWindow.h"
#include "util/platform.h"

ObsMain* obsMain = NULL;

ObsMain::ObsMain()
{
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
        return find->second;
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

    obs_scene_release(scene);

    m_scenes[name] = scene;
    if (m_scenes.size() == 1)
        m_currentScene = scene;
    return scene;
}

void ObsMain::RemoveScene(const char* name)
{
    auto find = m_scenes.find(name);
    if (find != m_scenes.end())
        m_scenes.erase(find);
}

bool ObsMain::SelectScene(const char* name)
{
    auto find = m_scenes.find(name);
    if (find != m_scenes.end())
    {
        m_currentScene = find->second;
        return true;
    }
    return false;
}

void ObsMain::SetCurrentScene(OBSSource scene)
{
    if (m_currentScene)
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
