#include "stdafx.h"
#include "ObsMainFrame.h"
#include "api/ObsMain.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);
    CWndShadow::Initialize(hInstance);
    CPaintManagerUI::SetResourcePath( _T("..\\data\\skin\\"));

    HRESULT Hr = ::CoInitialize(NULL);
    if (FAILED(Hr)) return 0;

    ObsMain* obsMain = ObsMain::Instance();
    obsMain->InitGlobalConfig();
    obsMain->InitObs();


    EnumMonitor(NULL, NULL);

    CObsMainFrame* pFrame = new CObsMainFrame();
    if (pFrame == NULL) return 0;
    pFrame->Create(NULL, _T("ObsClient"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 572);
    pFrame->CenterWindow();
    ::ShowWindow(*pFrame, SW_SHOW);

    CPaintManagerUI::MessageLoop();

    obsMain->SaveProject();
    delete obsMain;

    blog(LOG_INFO, "Number of memory leaks: %ld", bnum_allocs());

    ::CoUninitialize();
    return 0;
}

