#pragma once

class CSettingDialog :public WindowImplBase
{
public:
    CSettingDialog();
    ~CSettingDialog();

    void ShowDialog(HWND hParent)
    {
        CreateDuiWindow(hParent, _T("…Ë÷√"), WS_POPUPWINDOW | WS_VISIBLE);
        EnableWindow(hParent, false);
        CenterWindow();
    }

    void ShowPage(LPCTSTR name) {
        COptionUI* opt = m_PaintManager.FindControl<COptionUI>(name);
        if (opt)
            opt->Selected(true);
    }

    virtual CDuiString GetSkinFile() {
        return _T("SettingDialog.xml");
    }

    virtual void OnFinalMessage(HWND hWnd) {
        HWND hParent = GetParent(hWnd);
        EnableWindow(hParent, true);
        delete this;
    }

    virtual void Notify(TNotifyUI& msg);

    virtual UILIB_RESOURCETYPE GetResourceType() const { return RESOURCE_TYPE; }
    virtual CDuiString GetSkinFolder() { return _T(""); }
    LPCTSTR GetWindowClassName() const { return _T("ObsGuiFoundation"); }

    void InitWindow();

private:
    void InitVideoPage(config_t* config);
    bool SaveVideoPage(config_t* config);

    void InitLivePage(config_t* config);
    bool SaveLivePage(config_t* config);

    void InitAudioPage(config_t* config);
    bool SaveAudioPage(config_t* config);

    void InitRecordPage(config_t* config);
    bool SaveRecordPage(config_t* config);

    void InitOtherPage(config_t* config);
    bool SaveOtherPage(config_t* config);


    void ResetDownscales(uint32_t cx, uint32_t cy);

    void LoadListValues(CComboUI *widget, obs_property_t *prop,
        int index);

    CWndShadow m_shadow;
    CTabLayoutUI* m_tab;

    uint32_t m_outputCX = 1280;
    uint32_t m_outputCY = 720;

    //…˘“Ù «∑Ò…Ë÷√
    bool m_desktopAudioDevice1Changed = false;
    bool m_desktopAudioDevice2Changed = false;

    bool m_auxAudioDevice1Changed = false;
    bool m_auxAudioDevice2Changed = false;
    bool m_auxAudioDevice3Changed = false;
};

