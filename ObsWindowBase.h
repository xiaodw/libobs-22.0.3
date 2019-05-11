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

//����api����ͬϵͳ��ͬʵ��
class ObsWindowBase
{
public:
    virtual ~ObsWindowBase() {}

    //��ȡ���ڳߴ�
    virtual ObsSize GetClientSize() =0;
    virtual void OnResize(const ObsSize& size) =0;
    virtual window_handle_t CreateWnd(window_handle_t parent,const ObsRect& pos) =0;
    virtual void SetWindowPos(const ObsRect& rc) = 0;
    virtual window_handle_t GetWndHandle() = 0;
};

