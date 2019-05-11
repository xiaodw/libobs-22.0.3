#include "ObsMain.h"
#include "ObsWindow.h"

ObsMain* obsMain = NULL;

#define DL_OPENGL "obs_opengl"
#define DL_D3D11 "obs_d3d11"

ObsMain::ObsMain()
    :m_canvasSize(1280,720),m_outSize(1280, 720),m_obsContext("en-US", nullptr, nullptr)
{
    obs_load_all_modules();
}

ObsMain::~ObsMain()
{
    m_scenes.clear();
    m_currentScene = NULL;
    m_obsContext = NULL;
    obsMain =  nullptr;
}

ObsMain* ObsMain::Instance()
{
    if (!obsMain)
        obsMain = new ObsMain();
    return obsMain;
}

OBSScene ObsMain::GetCurrentScene()
{
    return m_currentScene;
}

OBSScene ObsMain::AddScene(const char* name)
{
    //c指针赋值给c++后需要释放
    obs_scene_t* cscene = obs_scene_create(name);
    if (!cscene)
        return NULL;

    OBSScene scene(cscene);
    obs_scene_release(cscene);

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

void ObsMain::SetCurrentScene(OBSSource scene, bool force,
    bool direct)
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

    obs_source_t* csource = obs_source_create(id, name, settings, hotkey);
    if (!csource)
        return NULL;
    
    OBSSource source(csource);
    obs_source_release(csource);
    return source;
}

bool ObsMain::ResetVideo()
{
    struct obs_video_info ovi;
    ovi.adapter = 0;

    //obs画布尺寸
    ovi.base_width = m_canvasSize.width;
    ovi.base_height = m_canvasSize.height;

    ovi.fps_num = 30000;
    ovi.fps_den = 1001;
    ovi.graphics_module = DL_D3D11;
    ovi.output_format = VIDEO_FORMAT_RGBA;

    //obs输出画面尺寸
    ovi.output_width = m_outSize.width;
    ovi.output_height = m_outSize.height;

    if (obs_reset_video(&ovi) != 0)
    {
        //使用opengl
        ovi.graphics_module = DL_OPENGL;
        return obs_reset_video(&ovi) == 0;
    }
    return true;
}

bool ObsMain::ResetVideoSize(const ObsSize& canvasSize, 
    const ObsSize& outSize)
{
    m_canvasSize = canvasSize;
    m_outSize = outSize;
    return ResetVideo();
}


#ifdef __APPLE__
#define INPUT_AUDIO_SOURCE  "coreaudio_input_capture"
#define OUTPUT_AUDIO_SOURCE "coreaudio_output_capture"
#elif _WIN32
#define INPUT_AUDIO_SOURCE  "wasapi_input_capture"
#define OUTPUT_AUDIO_SOURCE "wasapi_output_capture"
#else
#define INPUT_AUDIO_SOURCE  "pulse_input_capture"
#define OUTPUT_AUDIO_SOURCE "pulse_output_capture"
#endif

const char *ObsMain::InputAudioSource() const
{
    return INPUT_AUDIO_SOURCE;
}

const char *ObsMain::OutputAudioSource() const
{
    return OUTPUT_AUDIO_SOURCE;
}



