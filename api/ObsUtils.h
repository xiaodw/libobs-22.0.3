#pragma once
#include <string>
#include <vector>

typedef void(*enum_window_callback)(void* ptr,const char* name,const char* window);

//枚举游戏进程
void EnumGameWindow(enum_window_callback callback,void * ptr);

//枚举窗口
void EnumWindow(enum_window_callback callback, void * ptr);

typedef void(*enum_device_callback)(void* ptr, bool isVideo,const char* name, const char* id);

//枚举音视频设备
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




