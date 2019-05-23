#include "stdafx.h"
#include "ObsMainFrame.h"
#include "controls/ObsDisplayControl.h"
#include "controls/OptionsEx.h"
#include "api/ObsUtils.h"

#include "NewSceneDialog.h"
#include "WindowSelectDialog.h"
#include "DeviceSelectDialog.h"
#include "MsgBox.h"
#include "PopupList.h"
#include "AddVideoDialog.h"

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
        CDuiString name = msg.pSender->GetText();
        CDuiString tip;
        tip.Format(_T("您确定要删除场景\"%s\"?"), name.GetData());

        //提示是否关闭
        CMsgBox msgBox;
        if (msgBox.DuiMessageBox(m_hWnd, tip,_T("提示")) == IDOK)
        {
            //关闭option
            CHorizontalLayoutUI* layout = (CHorizontalLayoutUI*)msg.pSender->GetParent();
            bool isSel = static_cast<COptionExUI*>(msg.pSender)->IsSelected();
            int index = layout->GetItemIndex(msg.pSender);
            layout->Remove(msg.pSender);

            obs_scene_t* scene = (obs_scene_t*)msg.pSender->GetTag();
            ObsMain::Instance()->RemoveScene(scene);

            if (isSel)
            {
                //移除当前的选择其他的
                CControlUI* ctrl = layout->GetItemAt(index);
                if (!ctrl || _tcscmp(ctrl->GetClass(), kOptionExUIClassName) != 0)
                {
                    ctrl = layout->GetItemAt(index - 1);
                }

                if (ctrl &&  _tcscmp(ctrl->GetClass(), kOptionExUIClassName) == 0)
                {
                    static_cast<COptionExUI*>(ctrl)->Selected(true, true);
                }
            }
        }
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

            CControlUI* nameItem =  elem->FindSubControl(_T("SceneItemName"));
            CDuiString name = nameItem->GetText();

            CDuiString tip;
            tip.Format(_T("您确定要删除\"%s\"?"), name.GetData());

            //提示是否关闭
            CMsgBox msgBox;
            if (msgBox.DuiMessageBox(m_hWnd, tip, _T("提示")) == IDOK)
            {
                ObsSceneItemList& itemList = ObsMain::Instance()->sceneItemList();
                itemList.Remove(elem->GetIndex());
            }
        }
        else if (_tcsicmp(msg.pSender->GetName(), _T("BAddScene")) == 0)
        {
            //添加场景按钮
            CNewSceneDialog* dialog = new CNewSceneDialog();
            dialog->ShowDialog(m_hWnd);
        }
        else if (_tcsicmp(msg.pSender->GetName(), _T("BGame")) == 0)
        {
            CWindowSelectDialog* dialog = new CWindowSelectDialog(true);
            dialog->ShowDialog(m_hWnd);
        }
        else if (_tcsicmp(msg.pSender->GetName(), _T("BWindow")) == 0)
        {
            CWindowSelectDialog* dialog = new CWindowSelectDialog(false);
            dialog->ShowDialog(m_hWnd);
        }
        else if (_tcsicmp(msg.pSender->GetName(), _T("BScreen")) == 0)
        {
            ObsMain::Instance()->AddCaptureScreen(ToUtf8(L"显示器捕获").c_str());
        }
        else if (_tcsicmp(msg.pSender->GetName(), _T("BCamera")) == 0)
        {
            CDeviceSelectDialog* dialog = new CDeviceSelectDialog(true);
            dialog->ShowDialog(m_hWnd);
        }
        else if (_tcsicmp(msg.pSender->GetName(), _T("BMedia")) == 0)
        {
            RECT rc = msg.pSender->GetPos();

            POINT pt = { rc.left,rc.top };
            ClientToScreen(m_hWnd,&pt);

            RECT dest = { pt.x,pt.y - 96,pt.x + 72,pt.y };

            std::vector<CDuiString> items = { _T("视频"),_T("图片"),_T("文字") };

            CPopupList* dialog = new CPopupList(items, [this](int index) {
                switch (index)
                {
                case 0:
                    {
                        CAddVideoDialog* dialog = new CAddVideoDialog();
                        dialog->ShowDialog(this->GetHWND());
                    }
                    break;
                case 1:

                    break;
                case 2:

                    break;
                }
            });
            dialog->ShowDialog(m_hWnd, dest);
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
    switch (msgid)
    {
    case MSG_ADD_SCENE:
        {
            ObsSceneData* scene = static_cast<ObsSceneData*>(data);
            AddScene(scene->data);
            COptionExUI* option = static_cast<COptionExUI*>(m_sceneList->GetItemAt(0));
            option->EnableCloseBtn(m_sceneList->GetCount()>1);

            AddNewSceneBtn();
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

            AddNewSceneBtn();
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
    option->SetCloseBtnWidth(25);
    option->SetCloseBtnNormal(_T("file='image/close.png' source='0,0,25,25' dest='0,5,25,30'"));
    option->SetCloseBtnHot(_T("file='image/close.png' source='25,0,50,25' dest='0,5,25,30'"));
    option->SetCloseBtnPushed(_T("file='image/close.png' source='50,0,75,25' dest='0,5,25,30'"));
    option->SetTextColor(0xffffffff);

    OBSSource source = obs_scene_get_source(scene);

    option->SetTag((UINT_PTR)(obs_scene_t*)scene);
    option->SetText(CDuiString(obs_source_get_name(source)));
    m_sceneList->Add(option);

    if (ObsMain::Instance()->GetCurrentScene() == scene)
    {
        option->Selected(true, false);
    }

}

void CObsMainFrame::AddNewSceneBtn()
{
    CButtonUI* btn = (CButtonUI*)m_sceneList->FindItem(_T("BAddScene"));
    if (btn)
    {
        m_sceneList->SetItemIndex(btn, m_sceneList->GetCount()-1);
    }
    else
    {
        btn = new CButtonUI();
        btn->SetAttributeList(STRINGIZE(name="BAddScene" tooltip="添加场景" height="36" width="36" normalimage="file='image/add.png' source='0,0,16,16' dest='10,10,26,26'" hotimage="file='image/add.png' source='16,0,32,16' dest='10,10,26,26'" pushedimage="file='image/add.png' source='32,0,48,16' dest='10,10,26,26'"));
        m_sceneList->Add(btn);
    }
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
    label->SetName(_T("SceneItemName"));
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

