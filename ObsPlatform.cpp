#include "ObsPlatform.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <Dwmapi.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <util/platform.h>

std::string GetDefaultVideoSavePath()
{
    wchar_t path_utf16[MAX_PATH];
    char    path_utf8[MAX_PATH] = {};

    SHGetFolderPathW(NULL, CSIDL_MYVIDEO, NULL, SHGFP_TYPE_CURRENT,
        path_utf16);

    os_wcs_to_utf8(path_utf16, wcslen(path_utf16), path_utf8, MAX_PATH);
    return std::string(path_utf8);
}


