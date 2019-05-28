group "plugins"

project "coreaudio-encoder"
	kind "SharedLib"
	language "C++"
	targetdir "../obs-plugins/"

	defines {

	}
	files{
		"coreaudio-encoder/*.h",
		"coreaudio-encoder/*.cpp",
	}

	includedirs{
		"../core/"
	}

	links{
		"obs",
	}
	
project "win-decklink"
	kind "SharedLib"
	language "C"
	targetdir "../obs-plugins/"
	
	defines {

	}
	files{
		"decklink/*.h",
		"decklink/*.cpp",
		"decklink/*.c",
		"decklink/win/*.cpp",
		"decklink/win/decklink-sdk/DeckLinkAPIVersion.h",
		"decklink/win/decklink-sdk/DeckLinkAPI.idl",
		"../build/DeckLinkAPI_h.h",
		"../build/DeckLinkAPI_i.c",
	}

	includedirs{
		"../core/",
		"../build", --for DeckLinkAPI_h.h
	}

	links{
		"obs",
	}
	
	
project "image-source"
	kind "SharedLib"
	language "C"
	targetdir "../obs-plugins/"
	defines {

	}
	files{
		"image-source/*.h",
		"image-source/*.c",
	}

	includedirs{
		"../core/"
	}

	links{
		"obs",
		"w32-pthreads",
	}	
	
	
	
if is_arch("x86_64") then
project "ffmpeg-mux64"
else
project "ffmpeg-mux32"
end
	kind "ConsoleApp"
	language "C++"
	
	targetdir "../data/obs-plugins/obs-ffmpeg"
		
	defines {

	}
	files{
		"obs-ffmpeg/ffmpeg-mux/*.h",
		"obs-ffmpeg/ffmpeg-mux/*.c",
	}

	includedirs{
		"../core/",
	}

	links{
		"obs",
		"w32-pthreads",
		"avcodec",
		"avformat",
		"avutil",
		"avdevice",
		"swresample",
		"swscale",
	}	
	
project "obs-ffmpeg"
	kind "SharedLib"
	language "C"
	targetdir "../obs-plugins/"
	defines {

	}
	files{
		"obs-ffmpeg/*.h",
		"obs-ffmpeg/*.c",
		"obs-ffmpeg/media-playback/*.h",
		"obs-ffmpeg/media-playback/*.c",
	}

	includedirs{
		"../core/",
		"obs-ffmpeg",
	}

	links{
		"obs",
		"w32-pthreads",
		"avcodec",
		"avformat",
		"avutil",
		"avdevice",
		"swresample",
		"swscale",
	}	


project "obs-filters"
	kind "SharedLib"
	language "C"
	targetdir "../obs-plugins/"
	defines {

	}
	files{
		"obs-filters/*.h",
		"obs-filters/*.c",
	}

	includedirs{
		"../core/"
	}

	links{
		"obs",
		"w32-pthreads",
		"speexdsp",
	}	


project "obs-libfdk"
	kind "SharedLib"
	language "C"
	targetdir "../obs-plugins/"
	defines {

	}
	files{
		"obs-libfdk/*.h",
		"obs-libfdk/*.c",
	}

	includedirs{
		"../core/",
	}

	links{
		"obs",
		"w32-pthreads",
		"fdk-aac_dll",
	}
	
project "obs-outputs"
	kind "SharedLib"
	language "C"
	targetdir "../obs-plugins/"
	defines {
		"USE_MBEDTLS",
		"FTL_STATIC_COMPILE",
	}
	files{
		"obs-outputs/*.h",
		"obs-outputs/*.c",
		"obs-outputs/librtmp/*.h",
		"obs-outputs/librtmp/*.c",
		"obs-outputs/ftl-sdk/libftl/**.h",
		"obs-outputs/ftl-sdk/libftl/**.c",
	}

	removefiles{
		"obs-outputs/ftl-sdk/libftl/posix/*.h",
		"obs-outputs/ftl-sdk/libftl/posix/*.c",	
	}
	includedirs{
		"../3dparty/mbedtls/include",
		"../3dparty/jansson",
		"../core/",
		"obs-outputs/ftl-sdk/libftl",
		"obs-outputs/ftl-sdk/libftl/win32",
	}

	links{
		"obs",
		"w32-pthreads",
		"jansson",
		"mbedtls",
		"libcurl",
		"zlib",
		"Crypt32",
		"Iphlpapi",
		"winmm",
	}
	
	linkoptions{
		"/SAFESEH:NO",
	}
	
project "obs-qsv11"
	kind "SharedLib"
	language "C++"
	targetdir "../obs-plugins/"
	defines {
		"DX11_D3D"
	}
	files{
		"obs-qsv11/*.h",
		"obs-qsv11/*.cpp",
		"obs-qsv11/*.c",
		"obs-qsv11/libmfx/**.h",
		"obs-qsv11/libmfx/**.cpp",
	}

	includedirs{
		"../core/",
		"obs-qsv11/",
		"obs-qsv11/libmfx/include",
		"obs-qsv11/libmfx/include/msdk/include",
	}

	links{
		"obs",
		"w32-pthreads",
		"d3d11",
		"dxgi",
		"D3d9",
		"Dxva2",
	}	
	
project "obs-text"
	kind "SharedLib"
	language "C++"
	targetdir "../obs-plugins/"
	defines {

	}
	files{
		"obs-text/gdiplus/*.h",
		"obs-text/gdiplus/*.cpp",
	}

	includedirs{
		"../core/",
	}

	links{
		"obs",
		"gdiplus",
	}	
		
project "obs-transitions"
	kind "SharedLib"
	language "C"
	targetdir "../obs-plugins/"
	defines {

	}
	files{
		"obs-transitions/*.h",
		"obs-transitions/*.c",
	}

	includedirs{
		"../core/",
	}

	links{
		"obs",
	}		
	
project "obs-x264"
	kind "SharedLib"
	language "C"
	targetdir "../obs-plugins/"
	defines {

	}
	files{
		"obs-x264/*.h",
		"obs-x264/*.c",
	}

	includedirs{
		"../core/",
	}

	links{
		"obs",
		"x264",
	}		
	
project "vlc-video"
	kind "SharedLib"
	language "C"
	targetdir "../obs-plugins/"
	defines {

	}
	files{
		"vlc-video/*.h",
		"vlc-video/*.c",
	}

	includedirs{
		"../core/",
		"vlc-video/vlc",
	}

	links{
		"obs",
		"w32-pthreads",
	}		
		
project "win-capture"
	kind "SharedLib"
	language "C++"
	targetdir "../obs-plugins/"
	defines {

	}
	files{
		"win-capture/*.h",
		"win-capture/*.c",
		"win-capture/ipc-util/*.h",
		"win-capture/ipc-util/*.c",
	}

	includedirs{
		"../core/",
	}

	links{
		"obs",
		"psapi"
	}		
	
if is_arch("x86_64") then
project "get-graphics-offsets64"
else
project "get-graphics-offsets32"
end

	kind "ConsoleApp"
	language "C++"
	targetdir "../data/obs-plugins/win-capture/"
		
	defines {

	}
	files{
		"win-capture/get-graphics-offsets/*.h",
		"win-capture/get-graphics-offsets/*.c",
		"win-capture/get-graphics-offsets/*.cpp",
		"win-capture/ipc-util/*.h",
		"win-capture/ipc-util/*.c",
	}

	includedirs{
		"../core/",
	}

	links{
		"obs",
		"d3d9",
		"dxgi",
		"d3d11",
	}
	
if is_arch("x86_64") then
project "graphics-hook64"
else
project "graphics-hook32"
end

	kind "SharedLib"
	language "C++"
	targetdir "../data/obs-plugins/win-capture/"
	
	defines {

	}
	files{
		"win-capture/graphics-hook/*.h",
		"win-capture/graphics-hook/*.c",
		"win-capture/graphics-hook/*.cpp",
		"win-capture/obfuscate.c",
		"win-capture/funchook.c",
		"win-capture/ipc-util/*.h",
		"win-capture/ipc-util/*.c",
	}

	includedirs{
		"../core/",
		"win-capture/",
	}

	links{
		"obs",
		"d3d9",
		"dxgi",
		"d3d11",
		"psapi",
	}	

if is_arch("x86_64") then
project "inject-helper64"
else
project "inject-helper32"
end

	kind "ConsoleApp"
	language "C++"
	targetdir "../data/obs-plugins/win-capture/"
	
	defines {

	}
	files{
		"win-capture/inject-helper/*.c",
		"win-capture/inject-library.c",
		"win-capture/obfuscate.c",
	}

	includedirs{
		"../core/",
	}

	links{
		"obs",
	}	
	
	
project "win-dshow"
	kind "SharedLib"
	language "C++"
	targetdir "../obs-plugins/"
	defines {

	}
	files{
		"win-dshow/*.h",
		"win-dshow/*.hpp",
		"win-dshow/*.c",
		"win-dshow/*.cpp",
		"win-dshow/libdshowcapture/*.hpp",		
		"win-dshow/libdshowcapture/source/*.h",
		"win-dshow/libdshowcapture/source/*.hpp",
		"win-dshow/libdshowcapture/source/*.cpp",
	}

	includedirs{
		"../core/",
		"win-dshow/libdshowcapture",
	}

	links{
		"obs",
		"strmiids",
		"ksuser",
		"wmcodecdspuuid",
		"avcodec",
		"avformat",
		"avutil",
		"avdevice",
		"swresample",
		"swscale",
	}
	
--[[	
project "win-ivcam"
	kind "SharedLib"
	language "C++"
	defines {

	}
	files{
		"win-ivcam/*.cpp",
		"win-ivcam/seg_library/*.h",
		"win-ivcam/seg_library/*.cpp",
	}

	includedirs{
		"../core/",
	}

	links{
		"obs",
	}		
	

project "seg_service"
	kind "ConsoleApp"
	language "C++"
	defines {

	}
	files{
		"win-ivcam/seg_service/*.h",
		"win-ivcam/seg_service/*.cpp",
		"win-ivcam/seg_service/*.c",
		"win-ivcam/seg_service/seg_service.idl",
		"win-ivcam/seg_service/seg_service.rc",
		"../build/seg_service_p.c",
		"../build/seg_service_i.c",
	}

	includedirs{
		"../core/",
		"../build/",
	}

	links{
		"obs",
	}	
	]]
	
	
project "win-mf"
	kind "SharedLib"
	language "C++"
	targetdir "../obs-plugins/"
	defines {

	}
	files{
		"win-mf/*.hpp",
		"win-mf/*.cpp",
	}

	includedirs{
		"../core/",
	}

	links{
		"obs",
		"d3d9",
		"dxva2",
		"uuid",
		"mfplat",
		"mfuuid",
		"mf",
		"wmcodecdspuuid",
	}	
	
project "win-wasapi"
	kind "SharedLib"
	language "C++"
	targetdir "../obs-plugins/"
	defines {

	}
	files{
		"win-wasapi/*.hpp",
		"win-wasapi/*.cpp",
	}

	includedirs{
		"../core/",
	}

	links{
		"obs",
		"d3d9",
		"dxva2",
		"uuid",
		"mfplat",
		"mfuuid",
		"mf",
		"wmcodecdspuuid",
	}	
	
project "rtmp-services"
	kind "SharedLib"
	language "C++"
	targetdir "../obs-plugins/"
	defines {

	}
	files{
		"rtmp-services/*.h",
		"rtmp-services/*.c",
	}

	includedirs{
		"../core/",
		"../3dparty/jansson",
	}

	links{
		"obs",
		"jansson",
	}
	
project "obslua"
	kind "SharedLib"
	language "C++"
	defines {

	}
	files{
		"obs-scripting/cstrcache.h",
		"obs-scripting/cstrcache.cpp",
		"obs-scripting/obslua/*.c",
	}

	includedirs{
		"../core/",
		"obs-scripting/",
	}

	links{
		"obs",
		"luajit",
		"w32-pthreads",
	}		
		
	
project "obs-scripting"
	kind "SharedLib"
	language "C++"
	defines {

	}
	files{
		"obs-scripting/*.h",
		"obs-scripting/*.c",
		"obs-scripting/*.cpp",
	}

	includedirs{
		"../core/",
	}

	links{
		"obs",
		"luajit",
		"w32-pthreads",
	}		
	
	