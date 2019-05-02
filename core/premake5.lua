
project "libobs"
	kind "StaticLib"
	language "C++"
	rtti "On"
	exceptionhandling "On"
	
	files{
		"*.h",
		"*.c",
		"graphics/*.h",
		"graphics/*.c",
		"graphics/libnsgif/*.h",
		"graphics/libnsgif/*.c",
	}
	defines {
		"NOMINMAX",
	}
	includedirs{
		"./",
		"../3dparty/ffmpeg/include",
		"../3dparty/zlib",
		"../3dparty/win_pthreads",
		"../3dparty/jansson",
	}

if is_pfm("windows") then

	files{
		"audio-monitoring/win32/*.h",
		"audio-monitoring/win32/*.c",
		"callback/*.h",
		"callback/*.c",
		"media-io/*.h",
		"media-io/*.c",
		"util/*.h",
		"util/*.c",
		"util/windows/*.h",
		"util/windows/*.c",
		"util/vc/*.h",
	}
	
	removefiles{
		"obs-nix.c",
		"obs-cocoa.c",
		"util/pipe-posix.c",
		"util/platform-nix.c",
		"util/platform-nix-dbus.c",
		"util/threading-posix.*",
	}
	
end