#include <stdio.h>
#include <time.h>
#include <windows.h>

#include <util/base.h>
#include <graphics/vec2.h>
#include <media-io/audio-resampler.h>
#include <obs.h>


#include <intrin.h>

#include "ObsMain.h"
#include "ObsWindow.h"

static const int cx = 800;
static const int cy = 600;


static void do_log(int log_level, const char *msg, va_list args, void *param)
{
	char bla[4096];
	vsnprintf(bla, 4095, msg, args);

	OutputDebugStringA(bla);
	OutputDebugStringA("\n");

	//if (log_level < LOG_WARNING)
	//	__debugbreak();

	UNUSED_PARAMETER(param);
}


static void AddTestItems(obs_scene_t *scene, obs_source_t *source)
{
    OBSSceneItem item = obs_scene_add(scene, source);
    obs_sceneitem_select(item, true);
}



/* --------------------------------------------------- */

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine,
		int numCmd)
{
    //obs_add_data_path("../data/libobs/");
   // obs_add_module_path("../", "../");

	base_set_log_handler(do_log, nullptr);
    ObsMain* obsMain = ObsMain::Instance();
	try {

        ObsWindow * obsWindow = ObsWindow::Create();

        ObsRect rc = {100,200,800,600};
        window_handle_t hwnd = obsWindow->CreateWnd(NULL,rc);

		if (!hwnd)
			throw "Couldn't create main window";

		/* ------------------------------------------------------ */
		/* create OBS */
        obsMain->ResetVideo();

		/* ------------------------------------------------------ */
        OBSSource source = obsMain->CreateSource("monitor_capture","screen capture source",NULL);

		/* ------------------------------------------------------ */
		/* create scene and add source to scene (twice) */

        OBSScene scene = ObsMain::Instance()->AddScene("test scene");

		AddTestItems(scene, source);

		/* ------------------------------------------------------ */
		/* set the scene as the primary draw source and go */
		obs_set_output_source(0, obs_scene_get_source(scene));

        obsWindow->CreateDisplay();

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

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
