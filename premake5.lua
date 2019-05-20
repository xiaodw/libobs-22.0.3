require "premake/core"

workspace "obs"
	configurations { "Debug", "Release" }
	location "build"

if is_pfm("android") then
	location "android/jni"
	defines { "POSIX" ,"ANDROID" }
elseif is_pfm("windows") then
	location "build"
	defines { "_WIN32", "WIN32" ,"_WINDOWS"}
	includedirs{
		"3dparty/win_pthreads",
		"depend/win32/include",
	}
	libdirs{
		"depend/win32/bin"
	}
elseif is_pfm("ios") then
	location "build"
	defines { "POSIX" ,"TARGET_OS_IOS","TARGET_OS_IPHONE"}
end

filter "configurations:Debug"
	defines { "DEBUG" , "_DEBUG"}
	flags { "Symbols" }
	optimize "Debug"
	
filter "configurations:Release"
	defines { "NDEBUG" }
	optimize "On"
	optimize "Speed"

group "3dparty"

include "3dparty/jansson"
include "3dparty/mbedtls"
include "3dparty/win_pthreads"
include "3dparty/DuiLib"

group "core"
include "core"
include "core/render/opengl"
include "core/render/d3d11"
include "plugins"

group "project"

include "ui"

project "test"
	kind "WindowedApp"
	language "C++"
	rtti "On"
	exceptionhandling "On"
	includedirs
	{
		"core/"
	}
	files{
		"api/*.h",
		"api/*.cpp",
		"*.cpp",
	}
	defines {

	}
	
	
	links{
		"libobs",
	}
	
	
	