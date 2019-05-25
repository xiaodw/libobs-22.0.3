#include "StdAfx.h"
#include "AddTextDialog.h"
#include "MsgBox.h"
#include "api/ObsMain.h"
#include "Utils.h"

CAddTextDialog::CAddTextDialog()
{
}

CAddTextDialog::~CAddTextDialog()
{

}

void CAddTextDialog::Notify(TNotifyUI& msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0)
    {
        //初始化
        CComboUI* fontList =(CComboUI*) m_PaintManager.FindControl(_T("CFontList"));

        CFontsEnumerator emu;
        emu.EnumFont(m_PaintManager.GetPaintDC());

        for (auto& font : emu.cnFontList())
        {
            CListLabelElementUI* elem = new CListLabelElementUI();
            elem->SetText(font.c_str());
            elem->SetFixedHeight(24);
            fontList->Add(elem);
        }
        fontList->SelectItem(0);

    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_SELECTCHANGED) == 0)
    {
        CDuiString name = msg.pSender->GetName();
        if (name == _T("OSetLayout"))
        {
            COptionUI* opt = (COptionUI*)msg.pSender;
            CHorizontalLayoutUI* layout = (CHorizontalLayoutUI*)m_PaintManager.FindControl(_T("HTextLayout"));

            for (int i = 0; i < layout->GetCount(); ++i)
            {
                layout->GetItemAt(i)->SetEnabled(opt->IsSelected());
            }
        }
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_CLICK) == 0)
    {
         CDuiString name = msg.pSender->GetName();
         if (name ==_T("closebtn"))
        {
             Close();
        }
         else if (name == _T("acceptbtn"))
         {
            //添加文本
             ObsMain::TextData text;

             text.text = ToUtf8(m_PaintManager.FindControl(_T("EText"))->GetText());
             if (text.text.empty())
             {
                 CMsgBox msgbox;
                 msgbox.DuiMessageBox(m_hWnd, _T("请输入文字"));
                 return;
             }

             text.font = ToUtf8(m_PaintManager.FindControl(_T("CFontList"))->GetText());
             text.color = m_PaintManager.FindControl(_T("BFontColor"))->GetTag();
             if (text.color == 0)
                 text.color = 0xff000000;

             text.size = _ttoi(m_PaintManager.FindControl(_T("CFontSize"))->GetText());

             text.bold = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("OBold")))->IsSelected();
             text.italic = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("OItalic")))->IsSelected();
             
             text.extents = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("OSetLayout")))->IsSelected();
             text.extents_cx = _ttoi(m_PaintManager.FindControl(_T("EWidth"))->GetText());
             text.extents_wrap = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("OWrap")))->IsSelected();

             if (text.extents && (text.extents_cx <= 0 || text.extents_cx>4096))
             {
                 CMsgBox msgbox;
                 msgbox.DuiMessageBox(m_hWnd, _T("自定义文本宽度范围(0~4096)"));
                 return;
             }

             int sel = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("CAlign")))->GetCurSel();
             const char* alignText[] = {
                "left",
                "center",
                "right"
             };

             if(sel>=0)
                text.align = alignText[sel];

             sel = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("CValign")))->GetCurSel();
             const char* valignText[] = {
                 "top",
                 "center",
                 "bottom"
             };

             if (sel >= 0)
                 text.valign = valignText[sel];

             text.outline_size = _ttoi(m_PaintManager.FindControl(_T("EOutlineSize"))->GetText());
             text.outline_color = m_PaintManager.FindControl(_T("BOutlineColor"))->GetTag(); ;


#define CHECK_SPEED(name) static_cast<COptionUI*>(m_PaintManager.FindControl(name))->IsSelected()

             if (CHECK_SPEED(_T("OSpeed0")))
                 text.scroll_speed = 0;
             else if(CHECK_SPEED(_T("OSpeed1")))
                 text.scroll_speed = 25;
             else if (CHECK_SPEED(_T("OSpeed2")))
                 text.scroll_speed = 50;
             else if (CHECK_SPEED(_T("OSpeed3")))
                 text.scroll_speed = 100;


             text.outline_opacity = ((CSliderUI*)m_PaintManager.FindControl(_T("SOutlineAlpha")))->GetValue();
             ObsMain::Instance()->AddText(&text);
            Close();
         }
         else if (name == _T("BFontColor") || name==_T("BOutlineColor"))
         {
             COLORREF color;
             if (OpenColorSelectDialog(m_hWnd, color))
             {
                 msg.pSender->SetTag(color);
             }
         }
    }
}

