#include <stdio.h>
#include <time.h>
#include <windows.h>

#include <util/base.h>
#include <graphics/vec2.h>
#include <media-io/audio-resampler.h>
#include <obs.h>
#include "ObsWindow.h"
#include "ObsMain.h"
#include <intrin.h>

static const int cx = 800;
static const int cy = 600;

#define DL_OPENGL "obs_opengl"
#define DL_D3D11 "obs_d3d11"

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


static void CreateOBS(HWND hwnd)
{
	RECT rc;
	GetClientRect(hwnd, &rc);

	struct obs_video_info ovi;
	ovi.adapter         = 0;

    //obs»­²¼³ß´ç
	ovi.base_width      = rc.right;
	ovi.base_height     = rc.bottom;

	ovi.fps_num         = 30000;
	ovi.fps_den         = 1001;
	ovi.graphics_module = DL_D3D11;
	ovi.output_format   = VIDEO_FORMAT_RGBA;

    //obsÊä³ö»­Ãæ³ß´ç
	ovi.output_width    = rc.right;
	ovi.output_height   = rc.bottom;

	if (obs_reset_video(&ovi) != 0)
		throw "Couldn't initialize video";
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

        ObsWindow obsWindow;
        HWND hwnd = obsWindow.Create(NULL, _T("obs window"), WS_OVERLAPPEDWINDOW | WS_VISIBLE,0,0,0, 1920 / 2 - cx / 2, 1080 / 2);
        obsWindow.CenterWindow();

		if (!hwnd)
			throw "Couldn't create main window";

		/* ------------------------------------------------------ */
		/* create OBS */
		CreateOBS(hwnd);

		/* ------------------------------------------------------ */
		/* load modules */
		obs_load_all_modules();

		/* ------------------------------------------------------ */
        OBSSource source = obsMain->CreateSource("monitor_capture","screen capture source",NULL);

		/* ------------------------------------------------------ */
		/* create filter */
		//SourceContext filter = obs_source_create("test_filter",
		//		"a nice green filter", NULL, nullptr);
		//if (!filter)
		//	throw "Couldn't create test filter";
		//obs_source_filter_add(source, filter);

		/* ------------------------------------------------------ */
		/* create scene and add source to scene (twice) */

        OBSScene scene = ObsMain::Instance()->AddScene("test scene");

		AddTestItems(scene, source);

		/* ------------------------------------------------------ */
		/* set the scene as the primary draw source and go */
		obs_set_output_source(0, obs_scene_get_source(scene));

        obsWindow.CreateDisplay();

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	} catch (char *error) {
		MessageBoxA(NULL, error, NULL, 0);
	}

    delete ObsMain::Instance();

	blog(LOG_INFO, "Number of memory leaks: %ld", bnum_allocs());
	//DestroyWindow(hwnd);

	UNUSED_PARAMETER(prevInstance);
	UNUSED_PARAMETER(cmdLine);
	UNUSED_PARAMETER(numCmd);
	return 0;
}
