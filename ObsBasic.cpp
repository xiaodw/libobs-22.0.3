#include "ObsBasic.h"
#include "ObsMain.h"
#include "ObsPlatform.h"
#include "util/dstr.h"
#include "util/platform.h"

#define VOLUME_METER_DECAY_FAST        23.53
#define VOLUME_METER_DECAY_MEDIUM      11.76
#define VOLUME_METER_DECAY_SLOW        8.57
#define DL_OPENGL "obs_opengl"
#define DL_D3D11 "obs_d3d11"

ObsBasic::ObsBasic()
{

}

ObsBasic::~ObsBasic()
{

}

#define STARTUP_SEPARATOR \
	"==== Startup complete ==============================================="
#define SHUTDOWN_SEPARATOR \
	"==== Shutting down =================================================="

#define UNSUPPORTED_ERROR \
	"Failed to initialize video:\n\nRequired graphics API functionality " \
	"not found.  Your GPU may not be supported."

#define UNKNOWN_ERROR \
	"Failed to initialize video.  Your GPU may not be supported, " \
	"or your graphics drivers may need to be updated."

void ObsBasic::InitObs()
{
    if (!InitBasicConfig())
        throw "Failed to load basic.ini";
    if (!ResetAudio())
        throw "Failed to initialize audio";

    int ret = ResetVideo();
    switch (ret) {
    case OBS_VIDEO_MODULE_NOT_FOUND:
        throw "Failed to initialize video:  Graphics module not found";
    case OBS_VIDEO_NOT_SUPPORTED:
        throw UNSUPPORTED_ERROR;
    case OBS_VIDEO_INVALID_PARAM:
        throw "Failed to initialize video:  Invalid parameters";
    default:
        if (ret != OBS_VIDEO_SUCCESS)
            throw UNKNOWN_ERROR;
    }

    /* load audio monitoring */
#if defined(_WIN32) || defined(__APPLE__) || HAVE_PULSEAUDIO
    const char *device_name = config_get_string(m_basicConfig, "Audio",
        "MonitoringDeviceName");
    const char *device_id = config_get_string(m_basicConfig, "Audio",
        "MonitoringDeviceId");

    obs_set_audio_monitoring_device(device_name, device_id);

    blog(LOG_INFO, "Audio monitoring device:\n\tname: %s\n\tid: %s",
        device_name, device_id);
#endif

    blog(LOG_INFO, "---------------------------------");
    obs_load_all_modules();
    blog(LOG_INFO, "---------------------------------");
    obs_log_loaded_modules();
    blog(LOG_INFO, "---------------------------------");
    obs_post_load_modules();

    CheckForSimpleModeX264Fallback();

    blog(LOG_INFO, "ResetOutputs\n");
    ResetOutputs();

    blog(LOG_INFO, "InitService\n");
    InitService();
}

bool ObsBasic::ResetAudio()
{
    struct obs_audio_info ai;
    ai.samples_per_sec = config_get_uint(m_basicConfig, "Audio",
        "SampleRate");

    const char *channelSetupStr = config_get_string(m_basicConfig,
        "Audio", "ChannelSetup");

    if (strcmp(channelSetupStr, "Mono") == 0)
        ai.speakers = SPEAKERS_MONO;
    else if (strcmp(channelSetupStr, "2.1") == 0)
        ai.speakers = SPEAKERS_2POINT1;
    else if (strcmp(channelSetupStr, "4.0") == 0)
        ai.speakers = SPEAKERS_4POINT0;
    else if (strcmp(channelSetupStr, "4.1") == 0)
        ai.speakers = SPEAKERS_4POINT1;
    else if (strcmp(channelSetupStr, "5.1") == 0)
        ai.speakers = SPEAKERS_5POINT1;
    else if (strcmp(channelSetupStr, "7.1") == 0)
        ai.speakers = SPEAKERS_7POINT1;
    else
        ai.speakers = SPEAKERS_STEREO;

    return obs_reset_audio(&ai);
}

bool ObsBasic::InitService()
{
    m_service = obs_service_create("rtmp_common", "default_service", nullptr,
        nullptr);
    if (!m_service)
        return false;
    obs_service_release(m_service);
    return true;
}

static inline enum obs_scale_type GetScaleType(ConfigFile &basicConfig)
{
    const char *scaleTypeStr = config_get_string(basicConfig,
        "Video", "ScaleType");

    if (astrcmpi(scaleTypeStr, "bilinear") == 0)
        return OBS_SCALE_BILINEAR;
    else if (astrcmpi(scaleTypeStr, "lanczos") == 0)
        return OBS_SCALE_LANCZOS;
    else
        return OBS_SCALE_BICUBIC;
}

static inline enum video_format GetVideoFormatFromName(const char *name)
{
    if (astrcmpi(name, "I420") == 0)
        return VIDEO_FORMAT_I420;
    else if (astrcmpi(name, "NV12") == 0)
        return VIDEO_FORMAT_NV12;
    else if (astrcmpi(name, "I444") == 0)
        return VIDEO_FORMAT_I444;
#if 0 //currently unsupported
    else if (astrcmpi(name, "YVYU") == 0)
        return VIDEO_FORMAT_YVYU;
    else if (astrcmpi(name, "YUY2") == 0)
        return VIDEO_FORMAT_YUY2;
    else if (astrcmpi(name, "UYVY") == 0)
        return VIDEO_FORMAT_UYVY;
#endif
    else
        return VIDEO_FORMAT_RGBA;
}



void ObsBasic::GetFPSCommon(uint32_t &num, uint32_t &den) const
{
    const char *val = config_get_string(m_basicConfig, "Video", "FPSCommon");

    if (strcmp(val, "10") == 0) {
        num = 10;
        den = 1;
    }
    else if (strcmp(val, "20") == 0) {
        num = 20;
        den = 1;
    }
    else if (strcmp(val, "24 NTSC") == 0) {
        num = 24000;
        den = 1001;
    }
    else if (strcmp(val, "25") == 0) {
        num = 25;
        den = 1;
    }
    else if (strcmp(val, "29.97") == 0) {
        num = 30000;
        den = 1001;
    }
    else if (strcmp(val, "48") == 0) {
        num = 48;
        den = 1;
    }
    else if (strcmp(val, "59.94") == 0) {
        num = 60000;
        den = 1001;
    }
    else if (strcmp(val, "60") == 0) {
        num = 60;
        den = 1;
    }
    else {
        num = 30;
        den = 1;
    }
}

void ObsBasic::GetFPSInteger(uint32_t &num, uint32_t &den) const
{
    num = (uint32_t)config_get_uint(m_basicConfig, "Video", "FPSInt");
    den = 1;
}

void ObsBasic::GetFPSFraction(uint32_t &num, uint32_t &den) const
{
    num = (uint32_t)config_get_uint(m_basicConfig, "Video", "FPSNum");
    den = (uint32_t)config_get_uint(m_basicConfig, "Video", "FPSDen");
}

void ObsBasic::GetFPSNanoseconds(uint32_t &num, uint32_t &den) const
{
    num = 1000000000;
    den = (uint32_t)config_get_uint(m_basicConfig, "Video", "FPSNS");
}


void ObsBasic::GetConfigFPS(uint32_t &num, uint32_t &den) const
{
    uint32_t type = config_get_uint(m_basicConfig, "Video", "FPSType");

    if (type == 1) //"Integer"
        GetFPSInteger(num, den);
    else if (type == 2) //"Fraction"
        GetFPSFraction(num, den);
    else if (false) //"Nanoseconds", currently not implemented
        GetFPSNanoseconds(num, den);
    else
        GetFPSCommon(num, den);
}

#ifdef _WIN32
#define IS_WIN32 1
#else
#define IS_WIN32 0
#endif


int ObsBasic::ResetVideo()
{
    if (m_outputHandler && m_outputHandler->Active())
        return OBS_VIDEO_CURRENTLY_ACTIVE;

    struct obs_video_info ovi;
    int ret;

    GetConfigFPS(ovi.fps_num, ovi.fps_den);

    const char *colorFormat = config_get_string(m_basicConfig, "Video",
        "ColorFormat");
    const char *colorSpace = config_get_string(m_basicConfig, "Video",
        "ColorSpace");
    const char *colorRange = config_get_string(m_basicConfig, "Video",
        "ColorRange");

#ifdef WIN32
    ovi.graphics_module = DL_D3D11;
#else
    ovi.graphics_module = DL_OPENGL;
#endif

    ovi.base_width = (uint32_t)config_get_uint(m_basicConfig,
        "Video", "BaseCX");
    ovi.base_height = (uint32_t)config_get_uint(m_basicConfig,
        "Video", "BaseCY");
    ovi.output_width = (uint32_t)config_get_uint(m_basicConfig,
        "Video", "OutputCX");
    ovi.output_height = (uint32_t)config_get_uint(m_basicConfig,
        "Video", "OutputCY");
    ovi.output_format = GetVideoFormatFromName(colorFormat);
    ovi.colorspace = astrcmpi(colorSpace, "601") == 0 ?
        VIDEO_CS_601 : VIDEO_CS_709;
    ovi.range = astrcmpi(colorRange, "Full") == 0 ?
        VIDEO_RANGE_FULL : VIDEO_RANGE_PARTIAL;
    ovi.adapter = config_get_uint(m_basicConfig,"Video", "AdapterIdx");
    ovi.gpu_conversion = true;
    ovi.scale_type = GetScaleType(m_basicConfig);

    if (ovi.base_width == 0 || ovi.base_height == 0) {
        ovi.base_width = 1280;
        ovi.base_height = 720;
        config_set_uint(m_basicConfig, "Video", "BaseCX", 1280);
        config_set_uint(m_basicConfig, "Video", "BaseCY", 720);
    }

    if (ovi.output_width == 0 || ovi.output_height == 0) {
        ovi.output_width = ovi.base_width;
        ovi.output_height = ovi.base_height;
        config_set_uint(m_basicConfig, "Video", "OutputCX",
            ovi.base_width);
        config_set_uint(m_basicConfig, "Video", "OutputCY",
            ovi.base_height);
    }

    ret = obs_reset_video(&ovi);
    if (IS_WIN32 && ret != OBS_VIDEO_SUCCESS) {
        if (ret == OBS_VIDEO_CURRENTLY_ACTIVE) {
            blog(LOG_WARNING, "Tried to reset when "
                "already active");
            return ret;
        }

        /* Try OpenGL if DirectX fails on windows */
        if (astrcmpi(ovi.graphics_module, DL_OPENGL) != 0) {
            blog(LOG_WARNING, "Failed to initialize obs video (%d) "
                "with graphics_module='%s', retrying "
                "with graphics_module='%s'",
                ret, ovi.graphics_module,
                DL_OPENGL);
            ovi.graphics_module = DL_OPENGL;
            ret = obs_reset_video(&ovi);
        }
    }
    return ret;
}

void ObsBasic::StreamDelayStarting(int delay)
{

}
void ObsBasic::StreamStopping()
{
    streamingStopping = true;
}
void ObsBasic::StreamDelayStopping(int delay)
{

}
void ObsBasic::StreamingStart()
{
    streamingStopping = false;
}
void ObsBasic::StreamingStop(int code, const char* error)
{
    streamingStopping = false;
}


void ObsBasic::RecordingStart()
{
    recordingStopping = false;
}

void ObsBasic::RecordStopping()
{
    recordingStopping = true;
}

void ObsBasic::RecordingStop(int code)
{
    streamingStopping = false;
}

void ObsBasic::ReplayBufferStart()
{
    replayBufferStopping = false;
}

void ObsBasic::ReplayBufferStop(int code)
{
    replayBufferStopping = false;
}

void ObsBasic::ReplayBufferStopping()
{
    replayBufferStopping = true;
}

static const double scaled_vals[] =
{
    1.0,
    1.25,
    (1.0 / 0.75),
    1.5,
    (1.0 / 0.6),
    1.75,
    2.0,
    2.25,
    2.5,
    2.75,
    3.0,
    0.0
};


bool ObsBasic::InitBasicConfigDefaults()
{
    /* use 1920x1080 for new default base res if main monitor is above
    * 1920x1080, but don't apply for people from older builds -- only to
    * new users */
    int  cx = 1920;
    int  cy = 1080;

    /* ----------------------------------------------------- */
    /* move over mixer values in advanced if older config */
    if (config_has_user_value(m_basicConfig, "AdvOut", "RecTrackIndex") &&
        !config_has_user_value(m_basicConfig, "AdvOut", "RecTracks")) {

        uint64_t track = config_get_uint(m_basicConfig, "AdvOut",
            "RecTrackIndex");
        track = 1ULL << (track - 1);
        config_set_uint(m_basicConfig, "AdvOut", "RecTracks", track);
        config_remove_value(m_basicConfig, "AdvOut", "RecTrackIndex");
        config_save_safe(m_basicConfig, "tmp", nullptr);
    }

    /* ----------------------------------------------------- */

    config_set_default_string(m_basicConfig, "Output", "Mode", "Simple");

    config_set_default_string(m_basicConfig, "SimpleOutput", "FilePath",
        GetDefaultVideoSavePath().c_str());
    config_set_default_string(m_basicConfig, "SimpleOutput", "RecFormat",
        "flv");
    config_set_default_uint(m_basicConfig, "SimpleOutput", "VBitrate",
        2500);
    config_set_default_string(m_basicConfig, "SimpleOutput", "StreamEncoder",
        SIMPLE_ENCODER_X264);
    config_set_default_uint(m_basicConfig, "SimpleOutput", "ABitrate", 160);
    config_set_default_bool(m_basicConfig, "SimpleOutput", "UseAdvanced",
        false);
    config_set_default_bool(m_basicConfig, "SimpleOutput", "EnforceBitrate",
        true);
    config_set_default_string(m_basicConfig, "SimpleOutput", "Preset",
        "veryfast");
    config_set_default_string(m_basicConfig, "SimpleOutput", "RecQuality",
        "Stream");
    config_set_default_string(m_basicConfig, "SimpleOutput", "RecEncoder",
        SIMPLE_ENCODER_X264);
    config_set_default_bool(m_basicConfig, "SimpleOutput", "RecRB", false);
    config_set_default_int(m_basicConfig, "SimpleOutput", "RecRBTime", 20);
    config_set_default_int(m_basicConfig, "SimpleOutput", "RecRBSize", 512);
    config_set_default_string(m_basicConfig, "SimpleOutput", "RecRBPrefix",
        "Replay");

    config_set_default_bool(m_basicConfig, "AdvOut", "ApplyServiceSettings",
        true);
    config_set_default_bool(m_basicConfig, "AdvOut", "UseRescale", false);
    config_set_default_uint(m_basicConfig, "AdvOut", "TrackIndex", 1);
    config_set_default_string(m_basicConfig, "AdvOut", "Encoder", "obs_x264");

    config_set_default_string(m_basicConfig, "AdvOut", "RecType", "Standard");

    config_set_default_string(m_basicConfig, "AdvOut", "RecFilePath",
        GetDefaultVideoSavePath().c_str());
    config_set_default_string(m_basicConfig, "AdvOut", "RecFormat", "flv");
    config_set_default_bool(m_basicConfig, "AdvOut", "RecUseRescale",
        false);
    config_set_default_uint(m_basicConfig, "AdvOut", "RecTracks", (1 << 0));
    config_set_default_string(m_basicConfig, "AdvOut", "RecEncoder",
        "none");

    config_set_default_bool(m_basicConfig, "AdvOut", "FFOutputToFile",
        true);
    config_set_default_string(m_basicConfig, "AdvOut", "FFFilePath",
        GetDefaultVideoSavePath().c_str());
    config_set_default_string(m_basicConfig, "AdvOut", "FFExtension", "mp4");
    config_set_default_uint(m_basicConfig, "AdvOut", "FFVBitrate", 2500);
    config_set_default_uint(m_basicConfig, "AdvOut", "FFVGOPSize", 250);
    config_set_default_bool(m_basicConfig, "AdvOut", "FFUseRescale",
        false);
    config_set_default_bool(m_basicConfig, "AdvOut", "FFIgnoreCompat",
        false);
    config_set_default_uint(m_basicConfig, "AdvOut", "FFABitrate", 160);
    config_set_default_uint(m_basicConfig, "AdvOut", "FFAudioTrack", 1);

    config_set_default_uint(m_basicConfig, "AdvOut", "Track1Bitrate", 160);
    config_set_default_uint(m_basicConfig, "AdvOut", "Track2Bitrate", 160);
    config_set_default_uint(m_basicConfig, "AdvOut", "Track3Bitrate", 160);
    config_set_default_uint(m_basicConfig, "AdvOut", "Track4Bitrate", 160);
    config_set_default_uint(m_basicConfig, "AdvOut", "Track5Bitrate", 160);
    config_set_default_uint(m_basicConfig, "AdvOut", "Track6Bitrate", 160);

    config_set_default_bool(m_basicConfig, "AdvOut", "RecRB", false);
    config_set_default_uint(m_basicConfig, "AdvOut", "RecRBTime", 20);
    config_set_default_int(m_basicConfig, "AdvOut", "RecRBSize", 512);

    config_set_default_uint(m_basicConfig, "Video", "BaseCX", cx);
    config_set_default_uint(m_basicConfig, "Video", "BaseCY", cy);

    /* don't allow BaseCX/BaseCY to be susceptible to defaults changing */
    if (!config_has_user_value(m_basicConfig, "Video", "BaseCX") ||
        !config_has_user_value(m_basicConfig, "Video", "BaseCY")) {
        config_set_uint(m_basicConfig, "Video", "BaseCX", cx);
        config_set_uint(m_basicConfig, "Video", "BaseCY", cy);
        config_save_safe(m_basicConfig, "tmp", nullptr);
    }

    config_set_default_string(m_basicConfig, "Output", "FilenameFormatting",
        "%CCYY-%MM-%DD %hh-%mm-%ss");

    config_set_default_bool(m_basicConfig, "Output", "DelayEnable", false);
    config_set_default_uint(m_basicConfig, "Output", "DelaySec", 20);
    config_set_default_bool(m_basicConfig, "Output", "DelayPreserve", true);

    config_set_default_bool(m_basicConfig, "Output", "Reconnect", true);
    config_set_default_uint(m_basicConfig, "Output", "RetryDelay", 10);
    config_set_default_uint(m_basicConfig, "Output", "MaxRetries", 20);

    config_set_default_string(m_basicConfig, "Output", "BindIP", "default");
    config_set_default_bool(m_basicConfig, "Output", "NewSocketLoopEnable",
        false);
    config_set_default_bool(m_basicConfig, "Output", "LowLatencyEnable",
        false);

    int i = 0;
    uint32_t scale_cx = cx;
    uint32_t scale_cy = cy;

    /* use a default scaled resolution that has a pixel count no higher
    * than 1280x720 */
    while (((scale_cx * scale_cy) > (1280 * 720)) && scaled_vals[i] > 0.0) {
        double scale = scaled_vals[i++];
        scale_cx = uint32_t(double(cx) / scale);
        scale_cy = uint32_t(double(cy) / scale);
    }

    config_set_default_uint(m_basicConfig, "Video", "OutputCX", scale_cx);
    config_set_default_uint(m_basicConfig, "Video", "OutputCY", scale_cy);

    /* don't allow OutputCX/OutputCY to be susceptible to defaults
    * changing */
    if (!config_has_user_value(m_basicConfig, "Video", "OutputCX") ||
        !config_has_user_value(m_basicConfig, "Video", "OutputCY")) {
        config_set_uint(m_basicConfig, "Video", "OutputCX", scale_cx);
        config_set_uint(m_basicConfig, "Video", "OutputCY", scale_cy);
        config_save_safe(m_basicConfig, "tmp", nullptr);
    }

    config_set_default_uint(m_basicConfig, "Video", "FPSType", 0);
    config_set_default_string(m_basicConfig, "Video", "FPSCommon", "30");
    config_set_default_uint(m_basicConfig, "Video", "FPSInt", 30);
    config_set_default_uint(m_basicConfig, "Video", "FPSNum", 30);
    config_set_default_uint(m_basicConfig, "Video", "FPSDen", 1);
    config_set_default_string(m_basicConfig, "Video", "ScaleType", "bicubic");
    config_set_default_string(m_basicConfig, "Video", "ColorFormat", "NV12");
    config_set_default_string(m_basicConfig, "Video", "ColorSpace", "601");
    config_set_default_string(m_basicConfig, "Video", "ColorRange",
        "Partial");

    config_set_default_string(m_basicConfig, "Audio", "MonitoringDeviceId",
        "default");
    config_set_default_string(m_basicConfig, "Audio", "MonitoringDeviceName",
        Str("Basic.Settings.Advanced.Audio.MonitoringDevice"
            ".Default"));
    config_set_default_uint(m_basicConfig, "Audio", "SampleRate", 44100);
    config_set_default_string(m_basicConfig, "Audio", "ChannelSetup",
        "Stereo");
    config_set_default_double(m_basicConfig, "Audio", "MeterDecayRate",
        VOLUME_METER_DECAY_FAST);
    config_set_default_uint(m_basicConfig, "Audio", "PeakMeterType", 0);

    return true;
}

void ObsBasic::ResetOutputs()
{
    const char *mode = config_get_string(m_basicConfig, "Output", "Mode");
    bool advOut = astrcmpi(mode, "Advanced") == 0;

    if (!m_outputHandler || !m_outputHandler->Active()) {
        m_outputHandler.reset();
        m_outputHandler.reset(advOut ?
            CreateAdvancedOutputHandler(this) :
            CreateSimpleOutputHandler(this));
    }
    else {
        m_outputHandler->Update();
    }
}

bool ObsBasic::InitBasicConfig()
{
    char configPath[512];
    int ret = ObsMain::Instance()->GetProfilePath(configPath, sizeof(configPath), "");
    if (ret <= 0) {
        blog(LOG_ERROR, "Failed to get profile path");
        return false;
    }

    if (os_mkdir(configPath) == MKDIR_ERROR) {
        blog(LOG_ERROR, "Failed to create profile path");
        return false;
    }

    ret = ObsMain::Instance()->GetProfilePath(configPath, sizeof(configPath), "basic.ini");
    if (ret <= 0) {
        blog(LOG_ERROR, "Failed to get base.ini path");
        return false;
    }

    int code = m_basicConfig.Open(configPath, CONFIG_OPEN_ALWAYS);
    if (code != CONFIG_SUCCESS) {
        blog(LOG_ERROR, "Failed to open basic.ini: %d", code);
        return false;
    }

    if (config_get_string(m_basicConfig, "General", "Name") == nullptr) {
        const char *curName = config_get_string(ObsMain::Instance()->config(),
            "Basic", "Profile");

        config_set_string(m_basicConfig, "General", "Name", curName);
        m_basicConfig.SaveSafe("tmp");
    }

    return InitBasicConfigDefaults();
}

bool ObsBasic::StreamingActive() const
{
    if (!m_outputHandler)
        return false;
    return m_outputHandler->StreamingActive();
}

bool ObsBasic::Active() const
{
    if (!m_outputHandler)
        return false;
    return m_outputHandler->Active();
}


obs_service_t *ObsBasic::GetService()
{
    if (!m_service) {
        m_service = obs_service_create("rtmp_common", NULL, NULL,
            nullptr);
        obs_service_release(m_service);
    }
    return m_service;
}

void  ObsBasic::SetService(obs_service_t *service)
{
    if (service)
        m_service = service;
}

bool ObsBasic::StartStreaming()
{
    if (m_outputHandler->StreamingActive())
        return false;

    if (!m_outputHandler->StartStreaming(m_service)) {
        blog(LOG_ERROR,"Output.StartStreamFailed");
        return false;
    }

    bool recordWhenStreaming = config_get_bool(GetGlobalConfig(),
        "BasicWindow", "RecordWhenStreaming");
    if (recordWhenStreaming)
        StartRecording();

    bool replayBufferWhileStreaming = config_get_bool(GetGlobalConfig(),
        "BasicWindow", "ReplayBufferWhileStreaming");
    if (replayBufferWhileStreaming)
        StartReplayBuffer();

    return true;
}

void ObsBasic::StopStreaming()
{
    if (m_outputHandler->StreamingActive())
        m_outputHandler->StopStreaming(streamingStopping);

    bool recordWhenStreaming = config_get_bool(GetGlobalConfig(),
        "BasicWindow", "RecordWhenStreaming");
    bool keepRecordingWhenStreamStops = config_get_bool(GetGlobalConfig(),
        "BasicWindow", "KeepRecordingWhenStreamStops");
    if (recordWhenStreaming && !keepRecordingWhenStreamStops)
        StopRecording();

    bool replayBufferWhileStreaming = config_get_bool(GetGlobalConfig(),
        "BasicWindow", "ReplayBufferWhileStreaming");
    bool keepReplayBufferStreamStops = config_get_bool(GetGlobalConfig(),
        "BasicWindow", "KeepReplayBufferStreamStops");
    if (replayBufferWhileStreaming && !keepReplayBufferStreamStops)
        StopReplayBuffer();
}


void ObsBasic::ForceStopStreaming()
{
    if (m_outputHandler->StreamingActive())
        m_outputHandler->StopStreaming(true);

    bool recordWhenStreaming = config_get_bool(GetGlobalConfig(),
        "BasicWindow", "RecordWhenStreaming");
    bool keepRecordingWhenStreamStops = config_get_bool(GetGlobalConfig(),
        "BasicWindow", "KeepRecordingWhenStreamStops");
    if (recordWhenStreaming && !keepRecordingWhenStreamStops)
        StopRecording();

    bool replayBufferWhileStreaming = config_get_bool(GetGlobalConfig(),
        "BasicWindow", "ReplayBufferWhileStreaming");
    bool keepReplayBufferStreamStops = config_get_bool(GetGlobalConfig(),
        "BasicWindow", "KeepReplayBufferStreamStops");
    if (replayBufferWhileStreaming && !keepReplayBufferStreamStops)
        StopReplayBuffer();
}


bool ObsBasic::StartRecording()
{
    if (m_outputHandler->RecordingActive())
        return true;
    return m_outputHandler->StartRecording();
}

void ObsBasic::StopRecording()
{
    if (m_outputHandler->RecordingActive())
        m_outputHandler->StopRecording(recordingStopping);
}

static int CountVideoSources()
{
    int count = 0;

    auto countSources = [](void *param, obs_source_t *source)
    {
        if (!source)
            return true;

        uint32_t flags = obs_source_get_output_flags(source);
        if ((flags & OBS_SOURCE_VIDEO) != 0)
            (*reinterpret_cast<int*>(param))++;

        return true;
    };

    obs_enum_sources(countSources, &count);
    return count;
}

bool ObsBasic::StartReplayBuffer()
{
    if (!m_outputHandler || !m_outputHandler->replayBuffer)
        return false;
    if (m_outputHandler->ReplayBufferActive())
        return false;

    if (CountVideoSources() == 0) {
        return false;
    }

    obs_output_t *output = m_outputHandler->replayBuffer;
    obs_data_t *hotkeys = obs_hotkeys_save_output(output);
    obs_data_array_t *bindings = obs_data_get_array(hotkeys,
        "ReplayBuffer.Save");
    size_t count = obs_data_array_count(bindings);
    obs_data_array_release(bindings);
    obs_data_release(hotkeys);

    if (!count) {
        return false;
    }
    return m_outputHandler->StartReplayBuffer();
}

void ObsBasic::StopReplayBuffer()
{
    if (!m_outputHandler || !m_outputHandler->replayBuffer)
        return;

    if (m_outputHandler->ReplayBufferActive())
        m_outputHandler->StopReplayBuffer(replayBufferStopping);
}


void ObsBasic::CheckForSimpleModeX264Fallback()
{
    const char *curStreamEncoder = config_get_string(m_basicConfig,
        "SimpleOutput", "StreamEncoder");
    const char *curRecEncoder = config_get_string(m_basicConfig,
        "SimpleOutput", "RecEncoder");
    bool qsv_supported = false;
    bool amd_supported = false;
    bool nve_supported = false;
    bool changed = false;
    size_t idx = 0;
    const char *id;

    while (obs_enum_encoder_types(idx++, &id)) {
        if (strcmp(id, "amd_amf_h264") == 0)
            amd_supported = true;
        else if (strcmp(id, "obs_qsv11") == 0)
            qsv_supported = true;
        else if (strcmp(id, "ffmpeg_nvenc") == 0)
            nve_supported = true;
    }

    auto CheckEncoder = [&](const char *&name)
    {
        if (strcmp(name, SIMPLE_ENCODER_QSV) == 0) {
            if (!qsv_supported) {
                changed = true;
                name = SIMPLE_ENCODER_X264;
                return false;
            }
        }
        else if (strcmp(name, SIMPLE_ENCODER_NVENC) == 0) {
            if (!nve_supported) {
                changed = true;
                name = SIMPLE_ENCODER_X264;
                return false;
            }
        }
        else if (strcmp(name, SIMPLE_ENCODER_AMD) == 0) {
            if (!amd_supported) {
                changed = true;
                name = SIMPLE_ENCODER_X264;
                return false;
            }
        }

        return true;
    };

    if (!CheckEncoder(curStreamEncoder))
        config_set_string(m_basicConfig,
            "SimpleOutput", "StreamEncoder",
            curStreamEncoder);
    if (!CheckEncoder(curRecEncoder))
        config_set_string(m_basicConfig,
            "SimpleOutput", "RecEncoder",
            curRecEncoder);
    if (changed)
        config_save_safe(m_basicConfig, "tmp", nullptr);
}