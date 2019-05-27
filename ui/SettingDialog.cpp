#include "StdAfx.h"
#include "api/ObsMain.h"
#include "SettingDialog.h"
#include "MsgBox.h"
#include "Utils.h"
#include <sstream>

CSettingDialog::CSettingDialog()
    :m_tab(nullptr)
{
}

CSettingDialog::~CSettingDialog()
{

}

SIZE videoSize[] = {
    {1920,1080},
    { 1280,720 },
    { 960,540 },
    { 720,480 },
    { 640,360 },
};

uint32_t fps[] = {
    15,20,25,30,40,50,60
};

uint32_t bitrate[] = {
    800,1000,1200,1600,2000
};

void CSettingDialog::InitVideoPage(config_t* config)
{
    bool isPortrait = config_get_bool(config, "Video", "Portrait");
    int base_width = (uint32_t)config_get_uint(config, "Video", "BaseCX");
    int base_height = (uint32_t)config_get_uint(config, "Video", "BaseCY");

    if (isPortrait)
    {
        int temp = base_width;
        base_width = base_height;
        base_height = temp;
    }

    bool modify_size = config_get_bool(config, "Video", "ModifySize");

    if (modify_size)
    {
        m_PaintManager.FindControl<COptionUI>(_T("OVideoSizeModify"))->Selected(true);

        TCHAR buf[32];
        _itot(base_width, buf, 10);

        m_PaintManager.FindControl(_T("EVideoWidth"))->SetText(buf);

        _itot(base_height, buf, 10);
        m_PaintManager.FindControl(_T("EVideoHeight"))->SetText(buf);
    }
    else
    {
        int selIndex = -1;
        for (int i = 0; i < ARRAYSIZE(videoSize); ++i)
        {
            if (videoSize[i].cx == base_width && videoSize[i].cy == base_height)
            {
                selIndex = i;
                break;
            }
        }
        m_PaintManager.FindControl<CComboUI>(_T("CVideoSize"))->SelectItem(selIndex, false, false);
    }


    m_outputCX = (uint32_t)config_get_uint(config, "Video", "OutputCX");
    m_outputCY = (uint32_t)config_get_uint(config, "Video", "OutputCY");

    if (isPortrait)
    {
        int temp = m_outputCX;
        m_outputCX = m_outputCY;
        m_outputCY = temp;
    }

    //设置输出尺寸
    ResetDownscales(base_width, base_height);

    //选择fps
    uint32_t nfps = (uint32_t)config_get_uint(config, "Video", "FPSInt");
    

    for (int i = 0; i < ARRAYSIZE(fps); ++i)
    {
        if (nfps == fps[i])
        {
            m_PaintManager.FindControl<CComboUI>(_T("CFps"))->SelectItem(i, false, false);
            break;
        }
    }

    //选择码率
    uint32_t rate = (uint32_t)config_get_uint(config, "SimpleOutput", "VBitrate");
    for (int i = 0; i < ARRAYSIZE(bitrate); ++i)
    {
        if (rate == bitrate[i])
        {
            m_PaintManager.FindControl<CComboUI>(_T("CBitrate"))->SelectItem(i, false, false);
            break;
        }
    }

    //流编码器
    std::string codec = config_get_string(config, "SimpleOutput", "StreamEncoder");
    m_PaintManager.FindControl<CComboUI>(_T("CCodec"))->SelectItem(codec != "x264", false, false);

}



static std::string ResString(uint32_t cx, uint32_t cy)
{
    std::stringstream res;
    res << cx << "x" << cy;
    return res.str();
}

SIZE StringToRes(const CDuiString& text)
{
    SIZE size = { 0,0 };
    if (!text.IsEmpty())
    {
        LPTSTR pstr = NULL;
        size.cx = _tcstol(text, &pstr, 10);  ASSERT(pstr);
        size.cy = _tcstol(pstr+1, &pstr, 10);  ASSERT(pstr);
    }
    return size;
}

/* some nice default output resolution vals */
static const double scaleValues[] =
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
    3.0
};

static const size_t numVals = sizeof(scaleValues) / sizeof(double);

void CSettingDialog::ResetDownscales(uint32_t cx, uint32_t cy)
{
    string bestScale;
    int bestPixelDiff = 0x7FFFFFFF;
    int   out_cx = m_outputCX;
    int   out_cy = m_outputCY;

    CComboUI* combo = m_PaintManager.FindControl<CComboUI>(_T("COutSize"));
    combo->RemoveAll();

    for (size_t idx = 0; idx < numVals; idx++) {
        uint32_t downscaleCX = uint32_t(double(cx) / scaleValues[idx]);
        uint32_t downscaleCY = uint32_t(double(cy) / scaleValues[idx]);

        downscaleCX &= 0xFFFFFFFC;
        downscaleCY &= 0xFFFFFFFE;

        string res = ResString(downscaleCX, downscaleCY);

        CComboItemUI* item = new CComboItemUI();
        item->SetText(CDuiString(res.c_str()));
        combo->Add(item);

        /* always try to find the closest output resolution to the
        * previously set output resolution */
        int newPixelCount = int(downscaleCX * downscaleCY);
        int oldPixelCount = int(out_cx * out_cy);
        int diff = abs(newPixelCount - oldPixelCount);

        if (diff < bestPixelDiff) {
            bestScale = res;
            bestPixelDiff = diff;
        }
    }


    //纠正输出尺寸
    if (bestScale == "1920x1080")
    {
        bestScale = "1280x720";
    }

    if (!combo->SelectItemByText(CDuiString(bestScale.c_str()), false, false))
        combo->SelectItem(0, false, false);
}


SIZE GetVideoSize(CPaintManagerUI* mgr,bool * modify=nullptr) {
    int width;
    int height;

    if (mgr->FindControl<COptionUI>(_T("OVideoSizeModify"))->IsSelected())
    {
        width = _ttoi(mgr->FindControl<CEditUI>(_T("EVideoWidth"))->GetText());
        height = _ttoi(mgr->FindControl<CEditUI>(_T("EVideoHeight"))->GetText());

        if (modify)
            *modify = true;
    }
    else
    {
        int sel = mgr->FindControl<CComboUI>(_T("CVideoSize"))->GetCurSel();
        if (sel < 0)
            sel = 0;
        width = videoSize[sel].cx;
        height = videoSize[sel].cy;

        if (modify)
            *modify = false;
    }

    SIZE sz = { width,height };
    return sz;
}

bool CSettingDialog::SaveVideoPage(config_t* config)
{
    int base_width = (uint32_t)config_get_uint(config, "Video", "BaseCX");
    int base_height = (uint32_t)config_get_uint(config, "Video", "BaseCY");
    int basse_fps = (uint32_t)config_get_uint(config, "Video", "FPSInt");
    bool base_portrait = config_get_bool(config,"Video","Portrait");

    bool isPortrait = m_PaintManager.FindControl<COptionUI>(_T("OPortrait"))->IsSelected();

    int width = 0;
    int height = 0;

    bool modify = false;
    SIZE size = GetVideoSize(&m_PaintManager, &modify);
    width = size.cx;
    height = size.cy;
    config_set_bool(config, "Video", "ModifySize", modify);

    if (width <= 0 || width > 4000)
    {
        CMsgBox msgBox;
        msgBox.TipBox(m_hWnd, _T("视频宽度超出范围（0~4000）"));
        return false;
    }

    if (height <= 0 || height > 4000)
    {
        CMsgBox msgBox;
        msgBox.TipBox(m_hWnd, _T("视频高度超出范围（0~4000）"));
        return false;
    }

    if (isPortrait)
    {
        config_set_uint(config, "Video", "BaseCX", height);
        config_set_uint(config, "Video", "BaseCY", width);

        CDuiString text = m_PaintManager.FindControl<CComboUI>(_T("COutSize"))->GetText();
        if (!text.IsEmpty())
        {
            SIZE res = StringToRes(text);
            config_set_uint(config, "Video", "OutputCX", res.cy);
            config_set_uint(config, "Video", "OutputCY", res.cx);
        }
    }
    else
    {
        config_set_uint(config, "Video", "BaseCX", width);
        config_set_uint(config, "Video", "BaseCY", height);

        CDuiString text = m_PaintManager.FindControl<CComboUI>(_T("COutSize"))->GetText();
        if (!text.IsEmpty())
        {
            SIZE res = StringToRes(text);
            config_set_uint(config, "Video", "OutputCX", res.cx);
            config_set_uint(config, "Video", "OutputCY", res.cy);
        }
    }

    //fps设置
    int fpsidx = m_PaintManager.FindControl<CComboUI>(_T("CFps"))->GetCurSel();
    if (fpsidx < 0) fpsidx = 1;
        
    config_set_uint(config, "Video", "FPSInt", fps[fpsidx]);
    config_set_uint(config, "Video", "FPSType",1);
    config_set_uint(config, "SimpleOutput", "VBitrate",bitrate[m_PaintManager.FindControl<CComboUI>(_T("CBitrate"))->GetCurSel()]);

    if(m_PaintManager.FindControl<CComboUI>(_T("CCodec"))->GetCurSel()==0)
        config_set_string(config, "SimpleOutput", "StreamEncoder","x264");
    else
        config_set_string(config, "SimpleOutput", "StreamEncoder", "qsv");

    if (base_width != width || base_height != height || basse_fps != fps[fpsidx] || base_portrait != isPortrait)
    {
        ObsMain::Instance()->VideoSettingChange();
    }
    return true;
}

void CSettingDialog::InitLivePage(config_t* config)
{
    CDuiString text = config_get_string(config, "Stream", "Url");
    m_PaintManager.FindControl(_T("EStreamUrl"))->SetText(text);

    text = config_get_string(config, "Stream", "Name");
    m_PaintManager.FindControl(_T("EStreamName"))->SetText(text);

    text = config_get_string(config, "Stream", "UserName");
    m_PaintManager.FindControl(_T("EUserName"))->SetText(text);

    text = config_get_string(config, "Stream", "UserPwd");
    m_PaintManager.FindControl(_T("EUserPwd"))->SetText(text);

    m_PaintManager.FindControl<COptionUI>(_T("OVerify"))->
        Selected(config_get_bool(config, "Stream", "Auth"));
}

bool CSettingDialog::SaveLivePage(config_t* config)
{
    std::string data =ToUtf8(m_PaintManager.FindControl(_T("EStreamUrl"))->GetText());
    config_set_string(config, "Stream", "Url", data.c_str());

    data = ToUtf8(m_PaintManager.FindControl(_T("EStreamName"))->GetText());
    config_set_string(config, "Stream", "Name", data.c_str());

    data = ToUtf8(m_PaintManager.FindControl(_T("EUserName"))->GetText());
    config_set_string(config, "Stream", "UserName", data.c_str());

    data = ToUtf8(m_PaintManager.FindControl(_T("EUserPwd"))->GetText());
    config_set_string(config, "Stream", "UserPwd", data.c_str());

    config_set_bool(config, "Stream", "Auth", m_PaintManager.FindControl<COptionUI>(_T("OVerify"))->IsSelected());

    ObsMain::Instance()->ServiceSettingChange();
    return true;
}

const char* channelText[] = {
    "Mono",
    "Stereo",
    "2.1",
    "4.0",
    "4.1",
    "5.1",
    "7.1",
};

uint32_t abitrate[] = {
    64,96,128,160,192,224,256,288,320
};


void CSettingDialog::InitAudioPage(config_t* config)
{
    m_PaintManager.FindControl<CComboUI>(_T("CSampleRate"))->SelectItem(config_get_uint(config, "Audio", "SampleRate") == 48000);

    const char *channelSetupStr = config_get_string(config,
        "Audio", "ChannelSetup");

    int index = 1;

    for (int i = 0; i < ARRAYSIZE(channelText); ++i)
    {
        if (strcmp(channelText[i], channelSetupStr) == 0)
        {
            index = i;
           break;
        }
    }
    m_PaintManager.FindControl<CComboUI>(_T("CChannel"))->SelectItem(index);

    //音频码率设置
    uint32_t bitrate = (uint32_t)config_get_uint(config, "SimpleOutput", "ABitrate");
    for (int i = 0; i < ARRAYSIZE(abitrate); ++i)
    {
        if (bitrate == abitrate[i])
        {
            m_PaintManager.FindControl<CComboUI>(_T("CAudioBitrate"))->SelectItem(i, false, false);
            break;
        }
    }


    //音频设备设置
    const char *input_id = ObsMain::Instance()->InputAudioSource();
    const char *output_id = ObsMain::Instance()->OutputAudioSource();

    obs_properties_t *input_props = obs_get_source_properties(input_id);
    obs_properties_t *output_props = obs_get_source_properties(output_id);

    if (input_props) {
        obs_property_t *inputs = obs_properties_get(input_props,
            "device_id");
        LoadListValues(m_PaintManager.FindControl<CComboUI>(_T("CAuxAudioDevice1")), inputs, 3);
        LoadListValues(m_PaintManager.FindControl<CComboUI>(_T("CAuxAudioDevice2")), inputs, 4);
        LoadListValues(m_PaintManager.FindControl<CComboUI>(_T("CAuxAudioDevice3")), inputs, 5);
        obs_properties_destroy(input_props);
    }

    if (output_props) {
        obs_property_t *outputs = obs_properties_get(output_props,
            "device_id");
        LoadListValues(m_PaintManager.FindControl<CComboUI>(_T("CDesktopAudioDevice1")), outputs, 1);
        LoadListValues(m_PaintManager.FindControl<CComboUI>(_T("CDesktopAudioDevice2")), outputs, 2);
        obs_properties_destroy(output_props);
    }

}

static inline std::string GetComboData(CComboUI *combo)
{
    CControlUI* ctrl = combo->GetSelectItem();
    if (ctrl)
        return ToUtf8(ctrl->GetUserData());
    return "";
}

bool CSettingDialog::SaveAudioPage(config_t* config)
{
    int samplerate = config_get_uint(config, "Audio", "SampleRate");
    std::string channel = config_get_string(config, "Audio", "ChannelSetup");

    int index = m_PaintManager.FindControl<CComboUI>(_T("CSampleRate"))->GetCurSel();

    int newSamplerate = 4800;
    if (index == 0)
        newSamplerate = 44100;
  
    config_set_uint(config, "Audio", "SampleRate", newSamplerate);

    index = m_PaintManager.FindControl<CComboUI>(_T("CChannel"))->GetCurSel();
    config_set_string(config,"Audio", "ChannelSetup", channelText[index]);

    if (newSamplerate != samplerate || channel != channelText[index])
        ObsMain::Instance()->AudioSettingChange();


    config_set_uint(config, "SimpleOutput", "ABitrate", abitrate[m_PaintManager.FindControl<CComboUI>(_T("CAudioBitrate"))->GetCurSel()]);

    //音频设备设置
    auto UpdateAudioDevice = [this](bool input, CComboUI *combo,
        const char *name, int index)
    {
        ObsMain* obs = ObsMain::Instance();
        obs->ResetAudioDevice(
            input ? obs->InputAudioSource()
            : obs->OutputAudioSource(),
           GetComboData(combo).c_str(),
            Str(name), index);
    };

    if(m_desktopAudioDevice1Changed)
        UpdateAudioDevice(false, m_PaintManager.FindControl<CComboUI>(_T("CDesktopAudioDevice1")),
            "Basic.DesktopDevice1", 1);

    if(m_desktopAudioDevice2Changed)
        UpdateAudioDevice(false, m_PaintManager.FindControl<CComboUI>(_T("CDesktopAudioDevice2")),
        "Basic.DesktopDevice2", 2);

    if(m_auxAudioDevice1Changed)
        UpdateAudioDevice(true, m_PaintManager.FindControl<CComboUI>(_T("CAuxAudioDevice1")),
        "Basic.AuxDevice1", 3);

    if (m_auxAudioDevice2Changed)
        UpdateAudioDevice(true, m_PaintManager.FindControl<CComboUI>(_T("CAuxAudioDevice2")),
        "Basic.AuxDevice2", 4);

    if (m_auxAudioDevice3Changed)
        UpdateAudioDevice(true, m_PaintManager.FindControl<CComboUI>(_T("CAuxAudioDevice3")),
        "Basic.AuxDevice3", 5);
    return true;
}

static inline void LoadListValue(CComboUI *widget, const CDuiString& text, const CDuiString& val,int index = -1)
{
    CComboItemUI* item = new CComboItemUI();
    item->SetText(text);
    item->SetUserData(val);

    if(index>=0)
        widget->AddAt(item,index);
    else
        widget->Add(item);
}

void CSettingDialog::LoadListValues(CComboUI *widget, obs_property_t *prop,
    int index)
{
    size_t count = obs_property_list_item_count(prop);

    obs_source_t *source = obs_get_output_source(index);
    const char *deviceId = nullptr;
    obs_data_t *settings = nullptr;

    if (source) {
        settings = obs_source_get_settings(source);
        if (settings)
            deviceId = obs_data_get_string(settings, "device_id");
    }

    LoadListValue(widget, _T("停用"), _T("disabled"));

    for (size_t i = 0; i < count; i++) {
        const char *name = obs_property_list_item_name(prop, i);
        const char *val = obs_property_list_item_string(prop, i);
        LoadListValue(widget, name, val);
    }

    if (deviceId) {
        CDuiString var(deviceId);
        CControlUI* ctrl= widget->FindItemByUserData(var);
        if (ctrl) {
            widget->SelectItem(ctrl,false,false);
        }
        else {
            LoadListValue(widget, _T("未知设备"), var,0);
            widget->SelectItem(0, false, false);
        }
    }
    else
    {
        widget->SelectItem(0, false, false);
    }

    if (settings)
        obs_data_release(settings);
    if (source)
        obs_source_release(source);
}

void CSettingDialog::InitRecordPage(config_t* config)
{
    CDuiString data = config_get_string(config, "AdvOut", "RecFilePath");
    m_PaintManager.FindControl(_T("ERecordPath"))->SetText(data);


    data = config_get_string(config, "AdvOut", "RecFormat");
    m_PaintManager.FindControl<CComboUI>(_T("CRecordFormat"))->SelectItemByText(data);
    
}

bool CSettingDialog::SaveRecordPage(config_t* config)
{
    std::string path = ToUtf8(m_PaintManager.FindControl(_T("ERecordPath"))->GetText());
    if (os_get_free_disk_space(path.c_str()) < 1024 * 1024 * 1024)
    {
        CMsgBox msgBox;
        msgBox.TipBox(m_hWnd, _T("所选磁盘空间不足"));
        return false;
    }

    config_set_string(config, "AdvOut", "RecFilePath", path.c_str());

    std::string format = ToUtf8(m_PaintManager.FindControl(_T("CRecordFormat"))->GetText());
    config_set_string(config, "AdvOut", "RecFormat", format.c_str());
    return true;
}


void CSettingDialog::InitOtherPage(config_t* config)
{
    int second = config_get_uint(config, "Output", "DelaySec");
    m_PaintManager.FindControl<COptionUI>(_T("OEnableDealy"))->Selected(config_get_bool(config, "Output", "DelayEnable"), false);

    TCHAR buf[32];
    _itot(second, buf, 10);
    m_PaintManager.FindControl(_T("EDelay"))->SetText(buf);

    //竖屏模式
    m_PaintManager.FindControl<COptionUI>(_T("OPortrait"))->Selected(config_get_bool(config, "Video", "Portrait"), false);

}

bool CSettingDialog::SaveOtherPage(config_t* config)
{
    bool enableDelay = m_PaintManager.FindControl<COptionUI>(_T("OEnableDealy"))->IsSelected();

    config_set_bool(config, "Output", "DelayEnable", enableDelay);

    int second = _ttoi(m_PaintManager.FindControl(_T("EDelay"))->GetText());

    if (second < 0 || second>60)
    {
        CMsgBox msgBox;
        msgBox.TipBox(m_hWnd, _T("直播内容延时时间超出范围(0~60秒)"));
        return false;
    }

    config_set_uint(config, "Output", "DelaySec", second);

    //是否竖屏模式
    config_set_bool(config, "Video", "Portrait", m_PaintManager.FindControl<COptionUI>(_T("OPortrait"))->IsSelected());

    return true;
}

void CSettingDialog::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0)
    {
        m_tab = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("TSetting")));

        //初始化配置
        config_t* config= ObsMain::Instance()->basicConfig();
        if (!config)
            return;

        InitVideoPage(config);
        InitLivePage(config);
        InitAudioPage(config);
        InitRecordPage(config);
        InitOtherPage(config);
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_CLICK) == 0)
    {
        CDuiString name = msg.pSender->GetName();

        if (name ==  _T("closebtn"))
        {
             Close();
        }
        else if (name ==  _T("acceptbtn"))
        {

            if (ObsMain::Instance()->StreamActive())
            {
                CMsgBox msgBox;
                msgBox.TipBox(m_hWnd, _T("请停止直播后，点击确定。"));
                return;
            }

            //保存设置
            config_t* config = ObsMain::Instance()->basicConfig();
            if (config)
            {
                if (!SaveVideoPage(config))return;
                if (!SaveLivePage(config))return;
                if (!SaveAudioPage(config))return;
                if (!SaveRecordPage(config))return;
                if (!SaveOtherPage(config))return;
            }

            ObsMain::Instance()->SaveConfig();
            Close();
        }
        else if (name == _T("BSelectRecordPath"))
        {
            CDuiString path = OpenSelectPathDialog(m_hWnd);
            if (!path.IsEmpty())
            {
                m_PaintManager.FindControl(_T("ERecordPath"))->SetText(path);
            }
        }
        else if (name == _T("BOpenRecordPath"))
        {
            CDuiString path = m_PaintManager.FindControl(_T("ERecordPath"))->GetText();
            OpenFolder(m_hWnd,path);
        }
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_SELECTCHANGED) == 0)
    {
        CDuiString name = msg.pSender->GetName();

        if (msg.wParam)
        {
            if (name == _T("OVideo"))
            {
                m_tab->SelectItem(0, false);
            }
            else if (name == _T("OLive"))
            {
                m_tab->SelectItem(1, false);
            }
            else if (name == _T("OSound"))
            {
                m_tab->SelectItem(2, false);
            }
            else if (name == _T("ORecord"))
            {
                m_tab->SelectItem(3, false);
            }
            else if (name == _T("OOther"))
            {
                m_tab->SelectItem(4, false);
            }
        }

         if (name == _T("OVerify"))
        {
            m_PaintManager.FindControl(_T("HUserName"))->SetVisible(msg.wParam);
            m_PaintManager.FindControl(_T("HUserPwd"))->SetVisible(msg.wParam);
        }
        else if (name == _T("OEnableDealy"))
        {
            m_PaintManager.FindControl(_T("EDelay"))->SetEnabled(msg.wParam);
        }
        else if (name == _T("OVideoSizeModify"))
        {
            m_PaintManager.FindControl(_T("EVideoWidth"))->SetEnabled(msg.wParam);
            m_PaintManager.FindControl(_T("EVideoHeight"))->SetEnabled(msg.wParam);
            m_PaintManager.FindControl(_T("CVideoSize"))->SetEnabled(!msg.wParam);

            //重置缩放尺寸
            SIZE sz = GetVideoSize(&m_PaintManager);
            ResetDownscales(sz.cx,sz.cy);
        }
    }
    else if (msg.sType == DUI_MSGTYPE_ITEMSELECT)
    {
        CDuiString name = msg.pSender->GetName();
        if (name == _T("CDesktopAudioDevice1"))
        {
            m_desktopAudioDevice1Changed = true;
        }
        else if (name == _T("CDesktopAudioDevice2"))
        {
            m_desktopAudioDevice2Changed = true;
        }
        else if (name == _T("CAuxAudioDevice1"))
        {
            m_auxAudioDevice1Changed = true;
        }
        else if (name == _T("CAuxAudioDevice2"))
        {
            m_auxAudioDevice2Changed = true;
        }
        else if (name == _T("CAuxAudioDevice3"))
        {
            m_auxAudioDevice3Changed = true;
        }
        else if (name == _T("CVideoSize"))
        {
            int sel = static_cast<CComboUI*>(msg.pSender)->GetCurSel();
            if (sel < 0)
                sel = 0;
            ResetDownscales(videoSize[sel].cx, videoSize[sel].cy);
        }
        else if (name == _T("COutSize"))
        {
            SIZE res = StringToRes(msg.pSender->GetText());
            if (res.cx > 0 && res.cy > 0)
            {
                m_outputCX = res.cx;
                m_outputCY = res.cy;
            }
        }
    }
    else if (msg.sType == DUI_MSGTYPE_KILLFOCUS)
    {
        CDuiString name = msg.pSender->GetName();
        if (name == _T("EVideoWidth") || name==_T("EVideoHeight"))
        {
            SIZE sz = GetVideoSize(&m_PaintManager);
            if(sz.cx>0 && sz.cy>0)
                ResetDownscales(sz.cx, sz.cy);
        }
    }

}

