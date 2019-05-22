#include "stdafx.h"
#include "ObsMainFrame.h"
#include "controls/ObsDisplayControl.h"
#include "controls/OptionsEx.h"

const TCHAR* const kTitleControlName = _T("apptitle");
const TCHAR* const kCloseButtonControlName = _T("closebtn");
const TCHAR* const kMinButtonControlName = _T("minbtn");
const TCHAR* const kMaxButtonControlName = _T("maxbtn");
const TCHAR* const kRestoreButtonControlName = _T("restorebtn");

CObsMainFrame::CObsMainFrame()
    :m_sceneList(nullptr), m_sceneItemList(nullptr)
{
    ObsMain::Instance()->SetObserver(this);
}

CObsMainFrame::~CObsMainFrame()
{
    ObsMain::Instance()->SetObserver(NULL);
    PostQuitMessage(0);
}

LPCTSTR CObsMainFrame::GetWindowClassName() const
{
    return _T("ObsGuiFoundation");
}

CControlUI* CObsMainFrame::CreateControl(LPCTSTR pstrClass)
{
    if (_tcscmp(pstrClass, _T("ObsDisplay")) == 0)
    {
        return new CObsDisplayControl();
    }
    else if (_tcscmp(pstrClass, _T("OptionEx")) == 0)
    {
        return new COptionExUI();
    }
    return NULL;
}

void CObsMainFrame::OnFinalMessage(HWND hWnd)
{
    WindowImplBase::OnFinalMessage(hWnd);
    delete this;
}

CDuiString CObsMainFrame::GetSkinFile()
{
    return _T("MainFrame.xml");
}

CDuiString CObsMainFrame::GetSkinFolder()
{
    return _T("");
}

UILIB_RESOURCETYPE CObsMainFrame::GetResourceType() const
{
    return UILIB_FILE;
}

LRESULT CObsMainFrame::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#if defined(WIN32) && !defined(UNDER_CE)
    BOOL bZoomed = ::IsZoomed(m_hWnd);
    LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    if (::IsZoomed(m_hWnd) != bZoomed)
    {
        if (!bZoomed)
        {
            CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
            if (pControl) pControl->SetVisible(false);
            pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
            if (pControl) pControl->SetVisible(true);
        }
        else
        {
            CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
            if (pControl) pControl->SetVisible(true);
            pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
            if (pControl) pControl->SetVisible(false);
        }
    }
#else
    return __super::OnSysCommand(uMsg, wParam, lParam, bHandled);
#endif

    return 0;
}

LRESULT CObsMainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CObsMainFrame::ResponseDefaultKeyEvent(WPARAM wParam)
{
    if (wParam == VK_RETURN)
    {
        return FALSE;
    }
    else if (wParam == VK_ESCAPE)
    {
        return TRUE;
    }
    return FALSE;
}

void CObsMainFrame::OnTimer(TNotifyUI& msg)
{
}

void CObsMainFrame::OnExit(TNotifyUI& msg)
{
    Close();
}

void CObsMainFrame::InitWindow()
{
    RECT corner = { 5,3,5,7 };
    RECT hole = { 0,0,0,0 };
    m_shadow.SetImage(_T("image/window_shadow.png"), corner, hole);
    m_shadow.Create(m_hWnd);
}

void CObsMainFrame::OnPrepare(TNotifyUI& msg)
{
    m_sceneList =(CHorizontalLayoutUI*)m_PaintManager.FindControl(_T("SceneList"));
    m_sceneItemList = (CListUI*)m_PaintManager.FindControl(_T("SceneItemList"));

    //加载场景
    ObsMain::Instance()->LoadScene();
}

void CObsMainFrame::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0)
    {
        OnPrepare(msg);
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_OPTIONCLOSED) == 0)
    {
        //关闭option
        CHorizontalLayoutUI* layout = (CHorizontalLayoutUI*)msg.pSender->GetParent();
        layout->Remove(msg.pSender);

        obs_scene_t* scene = (obs_scene_t*)msg.pSender->GetTag();
        ObsMain::Instance()->RemoveScene(scene);
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_CLICK) == 0)
    {
        if (_tcsicmp(msg.pSender->GetName(), kCloseButtonControlName) == 0)
        {
            OnExit(msg);
        }
        else if (_tcsicmp(msg.pSender->GetName(), kMinButtonControlName) == 0)
        {
#if defined(UNDER_CE)
            ::ShowWindow(m_hWnd, SW_MINIMIZE);
#else
            SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
#endif
        }
        else if (_tcsicmp(msg.pSender->GetName(), kMaxButtonControlName) == 0)
        {
#if defined(UNDER_CE)
            ::ShowWindow(m_hWnd, SW_MAXIMIZE);
            CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
            if (pControl) pControl->SetVisible(false);
            pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
            if (pControl) pControl->SetVisible(true);
#else
            SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
#endif
        }
        else if (_tcsicmp(msg.pSender->GetName(), kRestoreButtonControlName) == 0)
        {
#if defined(UNDER_CE)
            ::ShowWindow(m_hWnd, SW_RESTORE);
            CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kMaxButtonControlName));
            if (pControl) pControl->SetVisible(true);
            pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(kRestoreButtonControlName));
            if (pControl) pControl->SetVisible(false);
#else
            SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
#endif
        }
        else if (_tcsicmp(msg.pSender->GetName(), _T("btn_menu")) == 0)
        {
            //CMenuWnd* pMenu = new CMenuWnd(m_hWnd);
            //CDuiPoint point = msg.ptMouse;
            //ClientToScreen(m_hWnd, &point);
            //STRINGorID xml(IDR_XML_MENU);
            //pMenu->Init(NULL, xml, _T("xml"), point);
        }
        else if (_tcsicmp(msg.pSender->GetName(), _T("SceneItemRemove")) == 0)
        {
            CListContainerElementUI* elem = static_cast<CListContainerElementUI*>(msg.pSender->GetParent()->GetParent());
            ObsSceneItemList& itemList = ObsMain::Instance()->sceneItemList();
            itemList.Remove(elem->GetIndex());
        }
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_TIMER) == 0)
    {
        return OnTimer(msg);
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_ITEMSELECT) == 0)
    {
        //list item选中
        if (_tcsicmp(msg.pSender->GetName(), _T("SceneItemList")) == 0)
        {
            ObsSceneItemList& itemList = ObsMain::Instance()->sceneItemList();
            itemList.Select(msg.wParam);
        }
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_SELECTCHANGED) == 0)
    {
        //option事件
        if (_tcsicmp(msg.pSender->GetName(), _T("SceneItemVisible")) == 0)
        {
            //scene item 是否显示
            CListContainerElementUI* elem = static_cast<CListContainerElementUI*>(msg.pSender->GetParent()->GetParent());
            ObsSceneItemList& itemList = ObsMain::Instance()->sceneItemList();
            itemList.SetVisible(elem->GetIndex(), msg.wParam);
        }
        else  if (_tcsicmp(msg.pSender->GetName(), _T("Scene")) == 0)
        {
            //场景切换
            obs_scene_t* scene = (obs_scene_t*) msg.pSender->GetTag();
            ObsMain::Instance()->SetCurrentScene(scene);
        }
    }
}

LRESULT CObsMainFrame::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (uMsg == MSG_HANDLE_MSG)
    {
        m_msgQueue.HandleMsg();
        bHandled = TRUE;
    }
    return 0;
}

void CObsMainFrame::OnMsg(unsigned int msgid, CMsgData* data)
{
    blog(LOG_INFO, "OnMsg %d", msgid);
    switch (msgid)
    {
    case MSG_ADD_SCENE:
        {
            ObsSceneData* scene = static_cast<ObsSceneData*>(data);
            AddScene(scene->data);
            COptionExUI* option = static_cast<COptionExUI*>(m_sceneList->GetItemAt(0));
            option->EnableCloseBtn(m_sceneList->GetCount()>1);
        }
        break;
    case MSG_REMOVE_SCENE:
        {
            ObsSceneData* scene = static_cast<ObsSceneData*>(data);

            for (int i = 0; i < m_sceneList->GetCount(); ++i)
            {
                COptionExUI* option = static_cast<COptionExUI*>(m_sceneList->GetItemAt(i));
                if (option->GetTag() == (UINT_PTR)(obs_scene_t*)scene->data)
                {
                    m_sceneList->Remove(option);
                    break;
                }
            }

            if (m_sceneList->GetCount() == 1)
            {
                COptionExUI* option = static_cast<COptionExUI*>(m_sceneList->GetItemAt(0));
                option->EnableCloseBtn(false);
            }
        }
        break;
    case MSG_REORDER_SCENE:
        {
            m_sceneList->RemoveAll();
            auto& scenes = ObsMain::Instance()->scenes();
            for (auto& scene : scenes)
            {
                AddScene(scene->scene);
            }
        }
        break;
    case MSG_RELOAD_SCENE_ITEM:
        {
            m_sceneItemList->RemoveAll();
            ObsSceneItemList& itemList = ObsMain::Instance()->sceneItemList();
            for (int i = 0; i < itemList.Count(); ++i)
            {
                AddSceneItem(itemList.Get(i));
            }
        }
        break;
    case MSG_SCENE_ITEM_SELECT_CHANDED:
        {
            CTypedMsgData<int>*index = static_cast<CTypedMsgData<int>*>(data);
            m_sceneItemList->SelectItem(index->data, false, false);
        }
        break;
    default:

        break;
    }

}

void CObsMainFrame::AddScene(OBSScene scene)
{
    COptionExUI* option = new COptionExUI();

    option->SetName(_T("Scene"));
    option->SetGroup(_T("SceneGroup"));
    option->SetFixedWidth(132);

    RECT padding = { 0,0,12,0 };
    option->SetTextPadding(padding);
    option->SetSelectedBkColor(0xff252526);
    option->SetCloseBtnNormal(_T("file='image/side_close.png' source='0,0,12,12' dest='0,12,12,24'"));
    option->SetCloseBtnHot(_T("file='image/side_close.png' source='24,0,36,12' dest='0,12,12,24'"));
    option->SetTextColor(0xffffffff);

    OBSSource source = obs_scene_get_source(scene);

    option->SetTag((UINT_PTR)(obs_scene_t*)scene);
    option->SetText(CDuiString(obs_source_get_name(source)));

    if (ObsMain::Instance()->GetCurrentScene() == scene)
    {
        option->Selected(true, false);
    }
    m_sceneList->Add(option);
}

void CObsMainFrame::AddSceneItem(OBSSceneItem item)
{
    CDuiString name = ObsSceneItemList::itemName(item);
    bool bVisible = ObsSceneItemList::itemVisible(item);
    bool bSelected = ObsSceneItemList::itemSelected(item);

    CListContainerElementUI* elem = new CListContainerElementUI();
    elem->SetFixedHeight(30);
    RECT rcInset = { 10,0,10,0 };
    elem->SetInset(rcInset);

    CHorizontalLayoutUI* hoz = new CHorizontalLayoutUI();
    hoz->SetChildPadding(5);

    COptionUI* opt = new COptionUI();
   opt->SetAttributeList(STRINGIZE(name="SceneItemVisible" width="14" hotimage="file='image/checkbox.png' source='0,0,14,14' dest='0,8,14,22'" normalimage="file='image/checkbox.png' source='14,0,28,14' dest='0,8,14,22'" selectedhotimage="file='image/checkbox.png' source='56,0,70,14' dest='0,8,14,22'" selectedimage="file='image/checkbox.png' source='70,0,84,14' dest='0,8,14,22'" ));

   if (bVisible)
       opt->Selected(true, false);

    CLabelUI* label = new CLabelUI();
    label->SetText(name);
    label->SetTextColor(0xffffff);
    label->SetFont(1);
    label->SetTextStyle(DT_END_ELLIPSIS | DT_VCENTER | DT_SINGLELINE);
    label->SetToolTip(name);
    CButtonUI* btn = new CButtonUI();
    btn->SetAttributeList(STRINGIZE(name="SceneItemRemove" width="12" normalimage="file='image/side_close.png' source='0,0,12,12' dest='0,9,12,21'" hotimage="file='image/side_close.png' source='24,0,36,12' dest='0,9,12,21'"));

    hoz->Add(opt);
    hoz->Add(label);
    hoz->Add(btn);

    elem->Add(hoz);

    m_sceneItemList->Add(elem);

    if (bSelected)
        elem->Select(true, false);
}

