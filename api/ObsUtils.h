#pragma once
#include <string>
#include <vector>

typedef void(*enum_window_callback)(void* ptr,const char* name,const char* window);

//ö����Ϸ����
void EnumGameWindow(enum_window_callback callback,void * ptr);

//ö�ٴ���
void EnumWindow(enum_window_callback callback, void * ptr);

typedef void(*enum_device_callback)(void* ptr, bool isVideo,const char* name, const char* id);

//ö������Ƶ�豸
void EnumDevice(enum_device_callback callback, void * ptr);


struct WindowInfo
{
    WindowInfo(const char* _name,const char* _window)
        :name(_name),window(_window)
    {
    }
    std::string name;
    std::string window;
};

std::vector<WindowInfo> GetWindowInfo(bool isGame);




