#include "stdafx.h"
#include "ObsMainFrame.h"
#include "controls/ObsDisplayControl.h"
#include "controls/OptionsEx.h"
#include "api/ObsUtils.h"

#include "WindowSelectDialog.h"
#include "CameraSelectDialog.h"
#include "MsgBox.h"
#include "PopupList.h"
#include "AddVideoDialog.h"
#include "AddImageDialog.h"
#include "AddTextDialog.h"
#include "RenameDialog.h"
#include "MonitorSelectDialog.h"
#include "SettingDialog.h"
#include "SliderBox.h"

const TCHAR* const kTitleControlName = _T("apptitle");
const TCHAR* const kCloseButtonControlName = _T("closebtn");
const TCHAR* const kMinButtonControlName = _T("minbtn");
const TCHAR* const kMaxButtonControlName = _T("maxbtn");
const TCHAR* const kRestoreButtonControlName = _T("restorebtn");

CObsMainFrame::CObsMainFrame()
    :m_sceneList(nullptr), m_sceneItemList(nullptr), m_display(nullptr)
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
    return RESOURCE_TYPE;
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

    m_sceneList = (CHorizontalLayoutUI*)m_PaintManager.FindControl(_T("SceneList"));
    m_sceneItemList = (CListUI*)m_PaintManager.FindControl(_T("SceneItemList"));
    m_display = (CObsDisplayControl*)m_PaintManager.FindControl(_T("ObsDisplay"));

    config_t* config = GetBasicConfig();
    if (config)
    {
        if (config_get_bool(config, "Output", "AutoRecord"))
        {
            m_PaintManager.FindControl<COptionUI>(_T("OAutoRecord"))->Selected(true, false);
        }
    }
}

void CObsMainFrame::OnPrepare(TNotifyUI& msg)
{


}

void CObsMainFrame::RemoveScene(COptionExUI* opt)
{
    CDuiString name = opt->GetText();
    CDuiString tip;
    tip.Format(_T("��ȷ��Ҫɾ������\"%s\"?"), name.GetData());

    //��ʾ�Ƿ�ر�
    CMsgBox msgBox;
    if (msgBox.DuiMessageBox(m_hWnd, tip, _T("��ʾ")) == IDOK)
    {
        //�ر�option
        CHorizontalLayoutUI* layout = (CHorizontalLayoutUI*)opt->GetParent();
        bool isSel = static_cast<COptionExUI*>(opt)->IsSelected();
        int index = layout->GetItemIndex(opt);
        layout->Remove(opt);

        obs_scene_t* scene = (obs_scene_t*)opt->GetTag();
        ObsMain::Instance()->RemoveScene(scene);

        if (isSel)
        {
            //�Ƴ���ǰ��ѡ��������
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

void CObsMainFrame::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0)
    {
        OnPrepare(msg);
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_OPTIONCLOSED) == 0)
    {
        RemoveScene((COptionExUI*)msg.pSender);
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_CLICK) == 0)
    {
        CDuiString name = msg.pSender->GetName();
        if (_tcsicmp(name, kCloseButtonControlName) == 0)
        {
            OnExit(msg);
        }
        else if (_tcsicmp(name, kMinButtonControlName) == 0)
        {
#if defined(UNDER_CE)
            ::ShowWindow(m_hWnd, SW_MINIMIZE);
#else
            SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
#endif
        }
        else if (_tcsicmp(name, kMaxButtonControlName) == 0)
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
        else if (_tcsicmp(name, kRestoreButtonControlName) == 0)
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
        else if (_tcsicmp(name, _T("SceneItemRemove")) == 0)
        {
            CListContainerElementUI* elem = static_cast<CListContainerElementUI*>(msg.pSender->GetParent()->GetParent());

            CControlUI* nameItem =  elem->FindSubControl(_T("SceneItemName"));
            CDuiString name = nameItem->GetText();

            CDuiString tip;
            tip.Format(_T("��ȷ��Ҫɾ��\"%s\"?"), name.GetData());

            //��ʾ�Ƿ�ر�
            CMsgBox msgBox;
            if (msgBox.DuiMessageBox(m_hWnd, tip, _T("��ʾ")) == IDOK)
            {
                ObsSceneItemList& itemList = ObsMain::Instance()->sceneItemList();
                itemList.Remove(elem->GetIndex());
            }
        }
        else if (_tcsicmp(name, _T("BAddScene")) == 0)
        {
            CRenameDialog* dialog = new CRenameDialog();
            dialog->ShowDialog(m_hWnd, _T("��������"), _T(""), [](CRenameDialog* dialog) {
                CDuiString text = dialog->GetText();
                if (text.GetLength() > 0)
                {
                    std::string utf8 = ToUtf8(text);
                    bool nameUsed = false;

                    //���Ʋ����ظ�
                    auto& scenes = ObsMain::Instance()->scenes();
                    for (auto& data : scenes)
                    {
                        if (strcmp(data->name(), utf8.c_str()) == 0)
                        {
                            nameUsed = true;
                            break;
                        }
                    }

                    if (!nameUsed)
                    {
                        utf8 = GenerateSourceName(utf8);
                        ObsMain::Instance()->AddScene(utf8.c_str(), true);
                        return true;
                    }
                    else
                    {
                        dialog->ShowTip(_T("���Ʋ����ظ�"));
                    }
                }
                else
                {
                    dialog->ShowTip(_T("���Ʋ���Ϊ��"));
                }
                return false;
            });
        }
        else if (_tcsicmp(name, _T("BGame")) == 0)
        {
            CWindowSelectDialog* dialog = new CWindowSelectDialog(true);
            dialog->ShowDialog(m_hWnd);
        }
        else if (_tcsicmp(name, _T("BWindow")) == 0)
        {
            CWindowSelectDialog* dialog = new CWindowSelectDialog(false);
            dialog->ShowDialog(m_hWnd);
        }
        else if (_tcsicmp(name, _T("BScreen")) == 0)
        {
            CMonitorSelectDialog* dialog = new CMonitorSelectDialog();
            dialog->ShowDialog(m_hWnd);
        }
        else if (_tcsicmp(name, _T("BCamera")) == 0)
        {
            CCameraSelectDialog* dialog = new CCameraSelectDialog();
            dialog->ShowDialog(m_hWnd);
        }
        else if (_tcsicmp(name, _T("BMedia")) == 0)
        {
            RECT rc = msg.pSender->GetPos();

            POINT pt = { rc.left,rc.top };
            ClientToScreen(m_hWnd,&pt);

            RECT dest = { pt.x,pt.y - 96,pt.x + 72,pt.y };

            std::vector<CDuiString> items = { _T("��Ƶ"),_T("ͼƬ"),_T("����") };

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
                    {
                        CAddImageDialog* dialog = new CAddImageDialog();
                        dialog->ShowDialog(this->GetHWND());
                    }
                    break;
                case 2:
                    {
                        CAddTextDialog* dialog = new CAddTextDialog();
                        dialog->ShowDialog(this->GetHWND());
                    }
                    break;
                }
            });
            dialog->ShowDialog(m_hWnd, dest);
        }
        else if (name == _T("BStartLive"))
        {


        }
        else if (name == _T("BSetting"))
        {
            CSettingDialog* dialog = new CSettingDialog();
            dialog->ShowDialog(m_hWnd);
        }
        else if (name == _T("BSoundOpen") || name == _T("BSoundClose"))
        {
            CSliderBox* box = new CSliderBox();
            RECT pos = msg.pSender->GetPos();

            POINT pt = { pos.left + 3,pos.top - 128 };

            ClientToScreen(m_hWnd, &pt);

            box->ShowDialog(m_hWnd,pt, OUTPUT_AUDIO_CHANNEL1);
        }
        else if (name == _T("BMicOpen") || name == _T("BMicClose"))
        {
            CSliderBox* box = new CSliderBox();
            RECT pos = msg.pSender->GetPos();

            POINT pt = { pos.left + 3,pos.top - 128 };

            ClientToScreen(m_hWnd, &pt);

            box->ShowDialog(m_hWnd, pt, INPUT_AUDIO_CHANNEL1);
        }
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_TIMER) == 0)
    {
        return OnTimer(msg);
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_ITEMSELECT) == 0)
    {
        //list itemѡ��
        if (_tcsicmp(msg.pSender->GetName(), _T("SceneItemList")) == 0)
        {
            ObsSceneItemList& itemList = ObsMain::Instance()->sceneItemList();
            itemList.Select(msg.wParam);
        }
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_MENU) == 0)
    {
        CDuiString name = msg.pSender->GetName();
        //list itemѡ��
        if (name ==  _T("SceneItemList"))
        {
            //����item�˵�
            int index = m_sceneItemList->FindItemByPos(msg.ptMouse);
            if (index>=0)
            {
                ShowSceneItemMenu(m_hWnd, msg.ptMouse, index);
            }
        }
        else if (name == _T("Scene"))
        {
            //�����˵�
            ShowSceneMenu((COptionExUI*)msg.pSender, msg.ptMouse);
        }
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_SELECTCHANGED) == 0)
    {
        //option�¼�
        if (_tcsicmp(msg.pSender->GetName(), _T("SceneItemVisible")) == 0)
        {
            //scene item �Ƿ���ʾ
            CListContainerElementUI* elem = static_cast<CListContainerElementUI*>(msg.pSender->GetParent()->GetParent());
            ObsSceneItemList& itemList = ObsMain::Instance()->sceneItemList();
            itemList.SetVisible(elem->GetIndex(), msg.wParam);
        }
        else  if (_tcsicmp(msg.pSender->GetName(), _T("Scene")) == 0)
        {
            //�����л�
            obs_scene_t* scene = (obs_scene_t*) msg.pSender->GetTag();
            ObsMain::Instance()->SetCurrentScene(scene);
        }
        else if (_tcsicmp(msg.pSender->GetName(), _T("OAutoRecord")) == 0)
        {
            config_t* config = GetBasicConfig();
            if (config)
            {
                config_set_bool(config, "Output", "AutoRecord",msg.wParam);
            }
        }
    }
}

void CObsMainFrame::ShowSceneMenu(COptionExUI* opt, POINT pt)
{
    CMenuWnd* menu = CMenuWnd::ShowMenu(m_hWnd, _T("ScenMenu.xml"), pt, [opt, this](CMenuElementUI* elem) {
        CDuiString name = elem->GetName();
        if (name == _T("Rename"))
        {
            CRenameDialog* dialog = new CRenameDialog();
            dialog->ShowDialog(m_hWnd, _T("������"), name = opt->GetText(), [opt,this](CRenameDialog* dialog) {
                ObsSceneItemList& list = ObsMain::Instance()->sceneItemList();
                CDuiString text = dialog->GetText();
                if (text == opt->GetText())
                    return true;

                if (text.GetLength() > 0)
                {
                    std::string utf8 = ToUtf8(text);

                    bool nameUsed = false;

                    //���Ʋ����ظ�
                    auto& scenes = ObsMain::Instance()->scenes();
                    for (auto& data : scenes)
                    {
                        if (strcmp(data->name(), utf8.c_str()) == 0)
                        {
                            nameUsed = true;
                            break;
                        }
                    }

                    //���Ʋ����ظ�
                    if (nameUsed)
                    {
                        dialog->ShowTip(_T("���Ʋ����ظ�"));
                    }
                    else
                    {
                        //����������
                        obs_scene_t* scene = (obs_scene_t*)opt->GetTag();
                        utf8 = GenerateSourceName(utf8);
                        ObsMain::Instance()->RenameScene(scene,utf8.c_str());

                        opt->SetText(CDuiString(utf8.c_str()));
                        return true;
                    }
                }
                else
                {
                    dialog->ShowTip(_T("���Ʋ���Ϊ��"));
                }
                return false;
            });
        }
        else if (name == _T("Delete"))
        {
            this->PostMsg(MSG_DELETE_SCENE, std::make_shared<CTypedMsgData<COptionExUI*>>(opt));
        }
    });
}

void CObsMainFrame::ShowSceneItemMenu(HWND hParent,POINT pt,int index)
{
    CMenuWnd* menu = CMenuWnd::ShowMenu(hParent, _T("SceneItemMenu.xml"), pt, [index, this](CMenuElementUI* elem) {
        CDuiString name = elem->GetName();
        if (name == _T("FitScreen"))
        {
            ObsMain::Instance()->FitToScreen();
        }
        else if (name == _T("MoveTop"))
        {
            ObsMain::Instance()->sceneItemList().MoveToTop(index);
        }
        else if (name == _T("MoveUp"))
        {
            ObsMain::Instance()->sceneItemList().MoveUp(index);
        }
        else if (name == _T("MoveDown"))
        {
            ObsMain::Instance()->sceneItemList().MoveDown(index);
        }
        else if (name == _T("MoveBottom"))
        {
            ObsMain::Instance()->sceneItemList().MoveToBottom(index);
        }
        else if (name == _T("Rename"))
        {
            name = ObsMain::Instance()->sceneItemList().itemName(index);
            CRenameDialog* dialog = new CRenameDialog();
            dialog->ShowDialog(m_hWnd, _T("������"), name, [index, this](CRenameDialog* dialog) {
                ObsSceneItemList& list = ObsMain::Instance()->sceneItemList();
                CDuiString text = dialog->GetText();
                CDuiString name = list.itemName(index);
                if (text == name)
                    return true;

                if (text.GetLength() > 0)
                {
                    std::string utf8 = ToUtf8(text);
                    bool nameUsed = false;

                    //���Ʋ����ظ�
                    if (obs_get_source_by_name(utf8.c_str()))
                    {
                        dialog->ShowTip(_T("���Ʋ����ظ�"));
                    }
                    else
                    {
                        ObsMain::Instance()->sceneItemList().SetName(index, utf8.c_str());
                        UpdateSceneItemName(index, text);
                        return true;
                    }
                }
                else
                {
                    dialog->ShowTip(_T("���Ʋ���Ϊ��"));
                }
                return false;
            });
        }
        else if (name == _T("Delete"))
        {
            PostMsg(MSG_DELETE_ELEM,
                std::make_shared<CTypedMsgData<int>>(index));
        }
        else if (name == _T("Show"))
        {
             bool bVisible = elem->GetTag() != 1;
             ObsMain::Instance()->sceneItemList().SetVisible(index, bVisible);
             UpdateSceneItemVisible(index, bVisible);
        }
    });


    if (ObsMain::Instance()->sceneItemList().itemVisible(index))
    {
        CMenuElementUI* elem = (CMenuElementUI*)menu->m_pm.FindControl(_T("Show"));
        elem->SetText(_T("����"));
        elem->SetTag(1);
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
    case MSG_VIDEO_RESET:
        {
            m_display->ResetVideo();
        }
        break;

    case MSG_DELETE_ELEM:
        {
            CTypedMsgData<int>*index = static_cast<CTypedMsgData<int>*>(data);
            ObsSceneItemList& itemList = ObsMain::Instance()->sceneItemList();
            const char* nameUtf8 = itemList.itemName(index->data);
            CDuiString tip;
            CDuiString name(nameUtf8);
            tip.Format(_T("��ȷ��Ҫɾ��\"%s\"?"), name.GetData());

            //��ʾ�Ƿ�ر�
            CMsgBox msgBox;
            if (msgBox.DuiMessageBox(m_hWnd, tip, _T("��ʾ")) == IDOK)
            {
                itemList.Remove(index->data);
            }
        }
        break;
    case MSG_DELETE_SCENE:
        {
            CTypedMsgData<COptionExUI*>*option = static_cast<CTypedMsgData<COptionExUI*>*>(data);
            RemoveScene(option->data);
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
    option->SetContextMenuUsed(true);

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
        btn->UseDefaultAttribute(&m_PaintManager,_T("NewSceneBtn"));
        m_sceneList->Add(btn);
    }
}

void CObsMainFrame::UpdateSceneItemName(int index, const CDuiString& name)
{
    CListContainerElementUI* elem = (CListContainerElementUI*)m_sceneItemList->GetItemAt(index);
    if (elem)
        elem->FindSubControl(_T("SceneItemName"))->SetText(name);
}

void CObsMainFrame::UpdateSceneItemVisible(int index, bool bVisible)
{
    CListContainerElementUI* elem = (CListContainerElementUI*)m_sceneItemList->GetItemAt(index);
    if (elem)
        ((COptionUI*)elem->FindSubControl(_T("SceneItemVisible")))->Selected(bVisible,false);
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
    opt->UseDefaultAttribute(&m_PaintManager,_T("SceneItemVisibleOpt"));

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
    btn->UseDefaultAttribute(&m_PaintManager,_T("SceneItemRemoveBtn"));
    hoz->Add(opt);
    hoz->Add(label);
    hoz->Add(btn);

    elem->Add(hoz);

    m_sceneItemList->Add(elem);

    if (bSelected)
        elem->Select(true, false);
}

void CObsMainFrame::OnMenu(window_handle_t handle, const ObsPoint& point)
{
    int index = ObsMain::Instance()->sceneItemList().GetCurrentIndex();

    if (index >= 0)
    {
        POINT pt = { point.x,point.y };
        ShowSceneItemMenu((HWND)handle, pt, index);
    }
}

