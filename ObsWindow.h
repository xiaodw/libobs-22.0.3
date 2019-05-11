#pragma once
#include <tchar.h>
#include <obs.hpp>
#include "ObsWindowBase.h"

class ObsWindow :public ObsWindowBase
{
public:
    ObsWindow();
    ~ObsWindow();

    //创建obs窗口
    static ObsWindow* Create();

    bool CreateDisplay();//创建obsdisplay

    obs_display_t* display() { return m_display; }

    gs_vertbuffer_t *box = nullptr;
    gs_vertbuffer_t *boxLeft = nullptr;
    gs_vertbuffer_t *boxTop = nullptr;
    gs_vertbuffer_t *boxRight = nullptr;
    gs_vertbuffer_t *boxBottom = nullptr;
    gs_vertbuffer_t *circle = nullptr;

protected:
    static void _RenderWindow(void* param,uint32_t cx, uint32_t cy);
    void RenderWindow(uint32_t cx, uint32_t cy);
    void OnResize(const ObsSize& size) override;

    void InitPrimitives();
    void DrawBackdrop(float cx, float cy);
    void DrawSceneEditing();

    //是否锁定
    bool locked = false;
    OBSDisplay m_display;

    float m_previewScale = 1.0;
    int m_previewCX = 0;
    int m_previewCY = 0;
    int m_previewX = 0;
    int m_previewY = 0;
};


