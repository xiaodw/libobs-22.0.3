#pragma once
#include "api/ObsMain.h"
#include "MsgQueue.h"

#define MSG_HANDLE_MSG (WM_USER+12)


#define STRINGIZE(x) L#x

//obs������
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
    virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    enum {
        MSG_ADD_SCENE,
        MSG_REMOVE_SCENE,
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
    };
    typedef CTypedMsgData<OBSSource> ObsSourceData;
    typedef CTypedMsgData<OBSScene> ObsSceneData;

    //obs�ص��ӿ�ʵ��
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

    //���¼���sceneitem
    virtual void OnReloadSceneItemList()
    {
        PostMsg(MSG_RELOAD_SCENE_ITEM);
    }

    virtual void OnSceneItemSelectChanged(int index)
    {
        PostMsg(MSG_SCENE_ITEM_SELECT_CHANDED, 
            std::make_shared<CTypedMsgData<int>>(index));
    }

    //�����ص��ӿ�
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

    //¼�ƻص��ӿ�
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

    //��ʾ�˵�
    virtual void OnMenu(const ObsPoint& point)
    {

    }

    void PostMsg(unsigned int msgid, std::shared_ptr<CMsgData> data = nullptr)
    {
        m_msgQueue.PostMsg(msgid, this, data);
        PostMessage(MSG_HANDLE_MSG);
    }
protected:
    virtual void OnMsg(unsigned int msgid, CMsgData* data);

    void Notify(TNotifyUI& msg);
    void OnPrepare(TNotifyUI& msg);
    void OnExit(TNotifyUI& msg);
    void OnTimer(TNotifyUI& msg);

    void AddSceneItem(OBSSceneItem item);
private:
    CMsgQueue m_msgQueue;
    CHorizontalLayoutUI* m_sceneList;
    CListUI* m_sceneItemList;
    CWndShadow m_shadow;
};


