-- _OPTIONS

-- _ACTION

-- pfm=windows
newoption {
	trigger = "pfm",
	value   = "dest platform",
	description = "Set the dest platform",
	allowed = {
	  { "ios", "ios" },
	  { "android",  "android" },
	  { "windows",  "windows" },
	}
}

local pfm=_OPTIONS["pfm"] or "windows"


--判断编译的目标平台 is_pfm("windows|ios")
function is_pfm(arg)
	if string.find(arg, pfm) then
		return true
	else
		return false
	end	
end


-- is_pfm("android")
-- is_pfm("ios")
-- is_pfm("windows")


if is_pfm("android") then
	require "premake/android/androidmk"
	
    ndkabi "armeabi-v7a"
    ndkplatform "android-14"
	ndkstl "gnustl_static"
	ndktoolchainversion "4.9"
end

-- premake5 vs2015
-- premake5 androidmk --pfm=android
-- premake5 xcode4 --os=ios --pfm=ios

-- ndk-build PM5_CONFIG=debug -j

