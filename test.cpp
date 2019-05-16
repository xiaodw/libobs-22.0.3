#include <stdio.h>
#include <time.h>
#include <windows.h>
#include "ObsMain.h"
#include "ObsWindow.h"
#include <util/platform.h>

static void do_log(int log_level, const char *msg, va_list args, void *param)
{
	char bla[4096];
	int len = vsnprintf(bla, 4095, msg, args);
    wchar_t* str;
    os_utf8_to_wcs_ptr(bla, len, &str);

	OutputDebugStringW(str);
	OutputDebugStringW(L"\n");
    bfree(str);

	//if (log_level < LOG_WARNING)
	//	__debugbreak();

	UNUSED_PARAMETER(param);
}


int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine,
		int numCmd)
{
    //obs_add_data_path("../data/libobs/");
   // obs_add_module_path("../", "../");

	base_set_log_handler(do_log, nullptr);
    ObsMain* obsMain = ObsMain::Instance();
    obsMain->InitGlobalConfig();

	try {

        ObsWindow * obsWindow = ObsWindow::Create();

        ObsRect rc = {100,200,800,600};
        window_handle_t hwnd = obsWindow->CreateWnd(NULL,rc);

		if (!hwnd)
			throw "Couldn't create main window";

		/* ------------------------------------------------------ */
		/* create OBS */
        obsMain->InitObs();


  //      OBSSource source = obsMain->CreateSource("monitor_capture","screen capture source",NULL);
  //      OBSScene scene = obsMain->AddScene("test scene");
  //      obsMain->AddSource(source);


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
