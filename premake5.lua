require "premake/core"

workspace "obs"
	configurations { "Debug", "Release" }
	location "build"

if is_pfm("android") then
	location "android/jni"
	defines { "POSIX" ,"ANDROID" }
elseif is_pfm("windows") then
	location "build"
	defines { "_WIN32", "WIN32" ,"_WINDOWS", "NOMINMAX","PTW32_STATIC_LIB"}
	includedirs{
		"3dparty/win_pthreads",
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

include "3dparty/zlib"	
include "3dparty/jansson"
include "3dparty/win_pthreads"
include "core"
include "core/render/opengl"
	

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
		"*.cpp",
	}
	defines {

	}
	
	
	links{
		"libobs",
	}
	
	
	