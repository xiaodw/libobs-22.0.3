#include "ObsMain.h"

ObsMain* obsMain = NULL;

ObsMain::ObsMain()
{

}

ObsMain::~ObsMain()
{
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
    OBSScene scene = obs_scene_create(name);
    m_scenes[name] = scene;
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