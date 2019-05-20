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
		"menu/*.h",
		"menu/*.cpp",
		"*.h",
		"*.cpp",
	}
	defines {

	}
	
	links{
		"libobs",
		"duilib",
	}