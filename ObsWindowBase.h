#pragma once


struct ObsSize {
    ObsSize() 
        :width(0),height(0)
    {
    }
    ObsSize(int w,int h)
        :width(w), height(h)
    {
    }
    int width;
    int height;
};

struct ObsRect {
    int x;
    int y;
    int width;
    int height;
};

typedef void* window_handle_t;

//窗口api，不同系统不同实现
class ObsWindowBase
{
public:
    virtual ~ObsWindowBase() {}

    //获取窗口尺寸
    virtual ObsSize GetClientSize() =0;
    virtual void OnResize(const ObsSize& size) =0;
    virtual window_handle_t CreateWnd(window_handle_t parent,const ObsRect& pos) =0;
    virtual void SetWindowPos(const ObsRect& rc) = 0;
    virtual window_handle_t GetWndHandle() = 0;
};

