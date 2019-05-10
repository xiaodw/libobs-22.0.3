#include "ObsWindow.h"
#include "ObsMain.h"

#include <graphics/vec4.h>
#include <graphics/matrix4.h>
#include <algorithm>
#define HANDLE_RADIUS     4.0f
#define HANDLE_SEL_RADIUS (HANDLE_RADIUS * 1.5f)
#define PREVIEW_EDGE_SIZE 10

void ObsWindow::CreateDisplay()
{
    if (m_display || !m_hWnd)
        return;

    RECT rc;
    GetClientRect(m_hWnd,&rc);
    gs_init_data info = {};
    info.cx = rc.right - rc.left;
    info.cy = rc.bottom - rc.top;
    info.format = GS_RGBA;
    info.zsformat = GS_ZS_NONE;
    info.window.hwnd = m_hWnd;

    m_display = obs_display_create(&info, 0xff000000);


    obs_display_add_draw_callback(m_display, [](void*data, uint32_t cx, uint32_t cy) {
        ObsWindow* window = (ObsWindow*)data;
        window->RenderWindow(cx, cy);
    }, this);

    InitPrimitives();

    ResizePreview(info.cx, info.cy);
}

ObsWindow::ObsWindow()
{


}

ObsWindow::~ObsWindow()
{


}

LRESULT ObsWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
        ResizePreview((uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
    default:
        break;
    }

    return CMyWindow::HandleMessage(uMsg, wParam, lParam);
}



void ObsWindow::InitPrimitives()
{
    obs_enter_graphics();

    gs_render_start(true);
    gs_vertex2f(0.0f, 0.0f);
    gs_vertex2f(0.0f, 1.0f);
    gs_vertex2f(1.0f, 1.0f);
    gs_vertex2f(1.0f, 0.0f);
    gs_vertex2f(0.0f, 0.0f);
    box = gs_render_save();

    gs_render_start(true);
    gs_vertex2f(0.0f, 0.0f);
    gs_vertex2f(0.0f, 1.0f);
    boxLeft = gs_render_save();

    gs_render_start(true);
    gs_vertex2f(0.0f, 0.0f);
    gs_vertex2f(1.0f, 0.0f);
    boxTop = gs_render_save();

    gs_render_start(true);
    gs_vertex2f(1.0f, 0.0f);
    gs_vertex2f(1.0f, 1.0f);
    boxRight = gs_render_save();

    gs_render_start(true);
    gs_vertex2f(0.0f, 1.0f);
    gs_vertex2f(1.0f, 1.0f);
    boxBottom = gs_render_save();

    gs_render_start(true);
    for (int i = 0; i <= 360; i += (360 / 20)) {
        float pos = RAD(float(i));
        gs_vertex2f(cosf(pos), sinf(pos));
    }
    circle = gs_render_save();

    obs_leave_graphics();
}

void ObsWindow::DrawBackdrop(float cx, float cy)
{
    if (!box)
        return;

    gs_effect_t    *solid = obs_get_base_effect(OBS_EFFECT_SOLID);
    gs_eparam_t    *color = gs_effect_get_param_by_name(solid, "color");
    gs_technique_t *tech = gs_effect_get_technique(solid, "Solid");

    vec4 colorVal;
    vec4_set(&colorVal, 0.0f, 0.0f, 0.0f, 1.0f);
    gs_effect_set_vec4(color, &colorVal);

    gs_technique_begin(tech);
    gs_technique_begin_pass(tech, 0);
    gs_matrix_push();
    gs_matrix_identity();
    gs_matrix_scale3f(float(cx), float(cy), 1.0f);

    gs_load_vertexbuffer(box);
    gs_draw(GS_TRISTRIP, 0, 0);

    gs_matrix_pop();
    gs_technique_end_pass(tech);
    gs_technique_end(tech);

    gs_load_vertexbuffer(nullptr);
}



static void DrawCircleAtPos(float x, float y)
{
    struct vec3 pos;
    vec3_set(&pos, x, y, 0.0f);

    struct matrix4 matrix;
    gs_matrix_get(&matrix);
    vec3_transform(&pos, &pos, &matrix);

    gs_matrix_push();
    gs_matrix_identity();
    gs_matrix_translate(&pos);
    gs_matrix_scale3f(HANDLE_RADIUS, HANDLE_RADIUS, 1.0f);
    gs_draw(GS_LINESTRIP, 0, 0);
    gs_matrix_pop();
}

static inline bool crop_enabled(const obs_sceneitem_crop *crop)
{
    return crop->left > 0 ||
        crop->top > 0 ||
        crop->right > 0 ||
        crop->bottom > 0;
}

static bool SceneItemHasVideo(obs_sceneitem_t *item)
{
    obs_source_t *source = obs_sceneitem_get_source(item);
    uint32_t flags = obs_source_get_output_flags(source);
    return (flags & OBS_SOURCE_VIDEO) != 0;
}

static bool CloseFloat(float a, float b, float epsilon = 0.01)
{
    using std::abs;
    return abs(a - b) <= epsilon;
}

bool DrawSelectedItem(obs_scene_t *scene,
    obs_sceneitem_t *item, void *param)
{
    if (obs_sceneitem_locked(item))
        return true;

    if (!SceneItemHasVideo(item))
        return true;

    if (obs_sceneitem_is_group(item)) {
        matrix4 mat;
        obs_sceneitem_get_draw_transform(item, &mat);

        gs_matrix_push();
        gs_matrix_mul(&mat);
        obs_sceneitem_group_enum_items(item, DrawSelectedItem, param);
        gs_matrix_pop();
    }

    if (!obs_sceneitem_selected(item))
        return true;

    matrix4 boxTransform;
    matrix4 invBoxTransform;
    obs_sceneitem_get_box_transform(item, &boxTransform);
    matrix4_inv(&invBoxTransform, &boxTransform);

    vec3 bounds[] = {
        { { { 0.f, 0.f, 0.f } } },
        { { { 1.f, 0.f, 0.f } } },
        { { { 0.f, 1.f, 0.f } } },
        { { { 1.f, 1.f, 0.f } } },
    };

    bool visible = std::all_of(std::begin(bounds), std::end(bounds),
        [&](const vec3 &b)
    {
        vec3 pos;
        vec3_transform(&pos, &b, &boxTransform);
        vec3_transform(&pos, &pos, &invBoxTransform);
        return CloseFloat(pos.x, b.x) && CloseFloat(pos.y, b.y);
    });

    if (!visible)
        return true;

    obs_transform_info info;
    obs_sceneitem_get_info(item, &info);

    ObsWindow* window = (ObsWindow*)param;
    gs_load_vertexbuffer(window->circle);

    gs_matrix_push();
    gs_matrix_mul(&boxTransform);

    DrawCircleAtPos(0.0f, 0.0f);
    DrawCircleAtPos(0.0f, 1.0f);
    DrawCircleAtPos(1.0f, 0.0f);
    DrawCircleAtPos(1.0f, 1.0f);
    DrawCircleAtPos(0.5f, 0.0f);
    DrawCircleAtPos(0.0f, 0.5f);
    DrawCircleAtPos(0.5f, 1.0f);
    DrawCircleAtPos(1.0f, 0.5f);

    obs_sceneitem_crop crop;
    obs_sceneitem_get_crop(item, &crop);

    if (info.bounds_type == OBS_BOUNDS_NONE && crop_enabled(&crop)) {
        vec4 color;
        gs_effect_t *eff = gs_get_effect();
        gs_eparam_t *param = gs_effect_get_param_by_name(eff, "color");

#define DRAW_SIDE(side, vb) \
		if (crop.side > 0) \
			vec4_set(&color, 0.0f, 1.0f, 0.0f, 1.0f); \
		else \
			vec4_set(&color, 1.0f, 0.0f, 0.0f, 1.0f); \
		gs_effect_set_vec4(param, &color); \
		gs_load_vertexbuffer(window->vb); \
		gs_draw(GS_LINESTRIP, 0, 0);

        DRAW_SIDE(left, boxLeft);
        DRAW_SIDE(top, boxTop);
        DRAW_SIDE(right, boxRight);
        DRAW_SIDE(bottom, boxBottom);
#undef DRAW_SIDE
    }
    else {
        gs_load_vertexbuffer(window->box);
        gs_draw(GS_LINESTRIP, 0, 0);
    }

    gs_matrix_pop();

    UNUSED_PARAMETER(scene);
    UNUSED_PARAMETER(param);
    return true;
}


void ObsWindow::DrawSceneEditing()
{
    if (locked)
        return;

    gs_effect_t    *solid = obs_get_base_effect(OBS_EFFECT_SOLID);
    gs_technique_t *tech = gs_effect_get_technique(solid, "Solid");

    vec4 color;
    vec4_set(&color, 1.0f, 0.0f, 0.0f, 1.0f);
    gs_effect_set_vec4(gs_effect_get_param_by_name(solid, "color"), &color);

    gs_technique_begin(tech);
    gs_technique_begin_pass(tech, 0);

    OBSScene scene = ObsMain::Instance()->GetCurrentScene();

    if (scene) {
        gs_matrix_push();
        gs_matrix_scale3f(m_previewScale, m_previewScale, 1.0f);
        obs_scene_enum_items(scene, DrawSelectedItem, this);
        gs_matrix_pop();
    }

    gs_load_vertexbuffer(nullptr);

    gs_technique_end_pass(tech);
    gs_technique_end(tech);
}


void ObsWindow::RenderWindow(uint32_t cx, uint32_t cy)
{
    obs_video_info ovi;

    obs_get_video_info(&ovi);

    m_previewCX = int(m_previewScale * float(ovi.base_width));
    m_previewCY = int(m_previewScale * float(ovi.base_height));

    gs_viewport_push();
    gs_projection_push();

    /* --------------------------------------- */

    gs_ortho(0.0f, float(ovi.base_width), 0.0f, float(ovi.base_height),
        -100.0f, 100.0f);
    gs_set_viewport(m_previewX, m_previewY,
        m_previewCX, m_previewCY);

    DrawBackdrop(float(ovi.base_width), float(ovi.base_height));

    //if (window->IsPreviewProgramMode()) {
    //    OBSScene scene = window->GetCurrentScene();
    //    obs_source_t *source = obs_scene_get_source(scene);
    //    if (source)
    //        obs_source_video_render(source);
    //}
    //else 
    {
        obs_render_main_texture();
    }
    gs_load_vertexbuffer(nullptr);

    /* --------------------------------------- */
    RECT rc;
    GetClientRect(m_hWnd, &rc);

    float right = float(rc.right - rc.left) - m_previewX;
    float bottom = float(rc.bottom - rc.top) - m_previewY;

    gs_ortho(-m_previewX, right,
        -m_previewY, bottom,
        -100.0f, 100.0f);
    gs_reset_viewport();

    DrawSceneEditing();

    /* --------------------------------------- */

    gs_projection_pop();
    gs_viewport_pop();
}



static inline void GetScaleAndCenterPos(
    int baseCX, int baseCY, int windowCX, int windowCY,
    int &x, int &y, float &scale)
{
    double windowAspect, baseAspect;
    int newCX, newCY;

    windowAspect = double(windowCX) / double(windowCY);
    baseAspect = double(baseCX) / double(baseCY);

    if (windowAspect > baseAspect) {
        scale = float(windowCY) / float(baseCY);
        newCX = int(double(windowCY) * baseAspect);
        newCY = windowCY;
    }
    else {
        scale = float(windowCX) / float(baseCX);
        newCX = windowCX;
        newCY = int(float(windowCX) / baseAspect);
    }

    x = windowCX / 2 - newCX / 2;
    y = windowCY / 2 - newCY / 2;
}


void ObsWindow::ResizePreview(uint32_t tcx, uint32_t tcy)
{
    if (m_display)
    {
        obs_display_resize(m_display, tcx, tcy);
    }

    obs_video_info ovi;
    if (obs_get_video_info(&ovi))
    {
        GetScaleAndCenterPos(int(ovi.base_width), int(ovi.base_height),
            tcx - PREVIEW_EDGE_SIZE * 2,
            tcy - PREVIEW_EDGE_SIZE * 2,
            m_previewX, m_previewY, m_previewScale);


        m_previewX += float(PREVIEW_EDGE_SIZE);
        m_previewY += float(PREVIEW_EDGE_SIZE);
    }
}

