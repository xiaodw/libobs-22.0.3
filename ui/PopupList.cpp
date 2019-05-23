#include "StdAfx.h"
#include "PopupList.h"

CPopupList::CPopupList(std::vector<CDuiString>& items, std::function<void(int)> callback)
    :m_list(nullptr), m_items(items),m_callback(callback)
{
}

CPopupList::~CPopupList()
{

}

void CPopupList::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0)
    {
        m_list = (CListUI*)m_PaintManager.FindControl(_T("List"));

        for (auto& item : m_items)
        {
            CListLabelElementUI* elem = new CListLabelElementUI();
            elem->SetFixedHeight(32);
            elem->SetText(item);
            m_list->Add(elem);
        }
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_CLICK) == 0)
    {
         if (_tcsicmp(msg.pSender->GetName(), _T("closebtn")) == 0)
        {
             Close();
        }
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_ITEMCLICK) == 0)
    {
        CListLabelElementUI* elem = static_cast<CListLabelElementUI*>(msg.pSender);
        int index = elem->GetIndex();
        
        if (m_callback)
            m_callback(index);
        Close();
    }
}

