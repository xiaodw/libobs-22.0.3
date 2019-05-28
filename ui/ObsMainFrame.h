#pragma once
#include "api/ObsMain.h"
#include "MsgQueue.h"

#define MSG_HANDLE_MSG (WM_USER+12)
#define MSG_UPDATE_AUDIO (WM_USER+13)
#define MSG_UPDATE_TRAY (WM_USER+14)


class CObsDisplayControl;

//obs主界面
class CObsMainFrame : public WindowImplBase,public ObsObserver,public CMsgHandler
{
public:
    CObsMainFrame();
    ~CObsMainFrame();

public:
    LPCTSTR GetWindowClassName() const;
    virtual void OnFinalMessage(HWND hWnd);
    virtual void InitWindow();
    virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
    virtual UILIB_RESOURCETYPE GetResourceType() const;
    virtual CDuiString GetSkinFile();
    virtual CDuiString GetSkinFolder();
    virtual CControlUI* CreateControl(LPCTSTR pstrClass);
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnTrayEvent(UINT uMsg, WPARAM wParam, BOOL& bHandled);

    enum {
        MSG_ADD_SCENE,
        MSG_REMOVE_SCENE,
        MSG_REORDER_SCENE,
        MSG_ACTIVATE_AUDIO_SOURCE,
        MSG_DEACTIVATE_AUDIO_SOURCE,
        MSG_RELOAD_SCENE_ITEM,
        MSG_SCENE_ITEM_SELECT_CHANDED,
        MSG_STREAM_STOPPING,
        MSG_STREAM_STOP,
        MSG_STREAM_START,

        MSG_RECORD_STOPPING,
        MSG_RECORD_STOP,
        MSG_RECORD_START,
        MSG_VIDEO_RESET,

        MSG_DELETE_ELEM,
        MSG_DELETE_SCENE,
    };
    typedef CTypedMsgData<OBSSource> ObsSourceData;
    typedef CTypedMsgData<OBSScene> ObsSceneData;

    //obs回调接口实现
    virtual void OnAddScene(OBSScene source)
    {
        PostMsg(MSG_ADD_SCENE,
            std::make_shared<ObsSceneData>(source));
    }
    virtual void OnRemoveScene(OBSScene source)
    {
        PostMsg(MSG_REMOVE_SCENE, 
            std::make_shared<ObsSceneData>(source));
    }

    virtual void OnReorderScene()
    {
        PostOnce(MSG_REORDER_SCENE);
    }

    virtual void OnActivateAudioSource(OBSSource source)
    {
        PostMsg(MSG_ACTIVATE_AUDIO_SOURCE, 
            std::make_shared<ObsSourceData>(source));
    }

    virtual void OnDeactivateAudioSource(OBSSource source)
    {
        PostMsg(MSG_DEACTIVATE_AUDIO_SOURCE, 
            std::make_shared<ObsSourceData>(source));
    }

    virtual void OnRenameSources(OBSSource source,
        const char* oldName, const char* newName)
    {

    }

    //重新加载sceneitem
    virtual void OnReloadSceneItemList()
    {
        PostOnce(MSG_RELOAD_SCENE_ITEM);
    }

    virtual void OnSceneItemSelectChanged(int index)
    {
        PostMsg(MSG_SCENE_ITEM_SELECT_CHANDED, 
            std::make_shared<CTypedMsgData<int>>(index));
    }

    //推流回调接口
    virtual void OnStreamStopping()
    {
        PostMsg(MSG_STREAM_STOPPING);
    }
    virtual void OnStreamingStart()
    {
        PostMsg(MSG_STREAM_START);
    }
    virtual void OnStreamingStop(int code, const char* error)
    {
        PostMsg(MSG_STREAM_STOP);
    }

    //录制回调接口
    virtual void OnRecordingStart()
    {
        PostMsg(MSG_RECORD_START);
    }

    virtual void OnRecordStopping() 
    {
        PostMsg(MSG_RECORD_STOPPING);
    }

    virtual void OnRecordingStop(int code)
    {
        PostMsg(MSG_RECORD_STOP);
    }

    virtual void OnVideoReset()
    {
        PostMsg(MSG_VIDEO_RESET);
    }

    //显示菜单
    virtual void OnMenu(window_handle_t handle,const ObsPoint& point);

    void PostMsg(unsigned int msgid, std::shared_ptr<CMsgData> data = nullptr)
    {
        m_msgQueue.PostMsg(msgid, this, data);
        PostMessage(MSG_HANDLE_MSG);
    }

    void PostOnce(unsigned int msgid, std::shared_ptr<CMsgData> data = nullptr)
    {
        if (m_msgQueue.CheckMsg(msgid, this))
            return;
        m_msgQueue.PostMsg(msgid, this, data);
        PostMessage(MSG_HANDLE_MSG);
    }
protected:
    virtual void OnMsg(unsigned int msgid, CMsgData* data);

    void Notify(TNotifyUI& msg);
    void OnPrepare(TNotifyUI& msg);
    void OnExit(TNotifyUI& msg);
    void OnTimer(TNotifyUI& msg);

    void AddScene(OBSScene scene);
    void AddSceneItem(OBSSceneItem item);
    void AddNewSceneBtn();
    void UpdateSceneCloseBtnState();

    void UpdateSceneItemName(int index,const CDuiString& name);
    void UpdateSceneItemVisible(int index, bool bVisible);

    void RemoveScene(COptionExUI* opt);

    void ShowSceneItemMenu(HWND hParent,POINT pt,int index);
    void ShowSceneMenu(COptionExUI* opt,POINT pt);

    //更新音频按钮状态
    void UpdateAudioState();
private:
    ObsMain*     m_obs;
    CMsgQueue m_msgQueue;
    CHorizontalLayoutUI* m_sceneList;
    CListUI* m_sceneItemList;
    CWndShadow m_shadow;
    CObsDisplayControl* m_display;
};


