#pragma once
#include <obs.hpp>
#include <string>
#include <vector>

typedef void(*enum_game_callback)(void* ptr,const char* name,const char* window);

//Ã¶¾Ù½ø³Ì
void EnumGameProcess(enum_game_callback callback,void * ptr);


struct GameWindowInfo
{
    GameWindowInfo(const char* _name,const char* _window)
        :name(_name),window(_window)
    {
    }
    std::string name;
    std::string window;
};

std::vector<GameWindowInfo> GetGameWindowInfo();


