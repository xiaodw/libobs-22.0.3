project "ObsClient"
	kind "WindowedApp"
	language "C++"
	rtti "On"
	exceptionhandling "On"
	includedirs
	{
		"../",
		"../core/",
		"../3dparty/DuiLib",
	}
	files{
		"../api/*.h",
		"../api/*.cpp",
		"controls/*.h",
		"controls/*.cpp",
		"*.h",
		"*.cpp",
		"rc/client.rc",
	}
	defines {

	}
	
	links{
		"obs",
		"duilib",
	}