#pragma once
#include <util/c99defs.h>

#include <string>
#include <vector>


std::string GetDefaultVideoSavePath();

std::string GetFileName(const  std::string &file);
std::string GetFilePostfix(const  std::string &file);

//»ñÈ¡ÆÁÄ»³ß´ç
bool GetScreenSize(int *cx,int *cy);