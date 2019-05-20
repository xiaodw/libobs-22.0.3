#include <stdio.h>
#include <time.h>
#include <windows.h>
#include "ObsMain.h"
#include "ObsWindow.h"
#include "ObsUtils.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine,
		int numCmd)
{
    ObsMain* obsMain = ObsMain::Instance();
    obsMain->InitGlobalConfig();
    obsMain->InitObs();

    EnumGameProcess(NULL,NULL);

	try {
        ObsWindow * obsWindow = ObsWindow::Create();

        ObsRect rc = {100,200,800,600};
        window_handle_t hwnd = obsWindow->CreateWnd(NULL,rc);

		if (!hwnd)
			throw "Couldn't create main window";

        obsWindow->CreateDisplay();

        //开始推流
        RtmpServer addr;
        addr.server = "rtmp://192.168.1.10/live";
        addr.key = "livestream";
        obsMain->SetRtmpServer(addr);
        //obsMain->StartStreaming();

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

        //obsMain->StopStreaming();
        obsMain->SaveProject();

        delete obsWindow;
	} catch (char *error) {
		MessageBoxA(NULL, error, NULL, 0);
	}

    delete obsMain;

	blog(LOG_INFO, "Number of memory leaks: %ld", bnum_allocs());

	UNUSED_PARAMETER(prevInstance);
	UNUSED_PARAMETER(cmdLine);
	UNUSED_PARAMETER(numCmd);
	return 0;
}
