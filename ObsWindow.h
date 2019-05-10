#pragma once
#include <tchar.h>
#include "MyWindow.h"
#include <obs.hpp>

class ObsWindow :public CMyWindow
{
public:
    ObsWindow();
    ~ObsWindow();

    void CreateDisplay();//´´½¨obsdisplay

    obs_display_t* display() { return m_display; }

    gs_vertbuffer_t *box = nullptr;
    gs_vertbuffer_t *boxLeft = nullptr;
    gs_vertbuffer_t *boxTop = nullptr;
    gs_vertbuffer_t *boxRight = nullptr;
    gs_vertbuffer_t *boxBottom = nullptr;
    gs_vertbuffer_t *circle = nullptr;

private:
    void RenderWindow(uint32_t cx, uint32_t cy);
    void ResizePreview(uint32_t cx, uint32_t cy);

    void InitPrimitives();
    void DrawBackdrop(float cx, float cy);
    void DrawSceneEditing();

    bool locked = false;

    virtual LPCTSTR GetWindowClassName() const { return _T("render_window"); }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    OBSDisplay m_display;

    float m_previewScale = 1.0;
    int m_previewCX = 0;
    int m_previewCY = 0;
    int m_previewX = 0;
    int m_previewY = 0;
};


