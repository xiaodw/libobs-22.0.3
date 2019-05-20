project "duilib"
	kind "SharedLib"
	language "C++"
	defines{
		"_WINDOWS",
		"UILIB_EXPORTS",
	}
	includedirs{
		"./"
	}
	files{
		"Control/*.h",
		"Control/*.cpp",
		"Core/*.h",
		"Core/*.cpp",
		"Layout/*.h",
		"Layout/*.cpp",
		"Utils/*.h",
		"Utils/*.c",
		"Utils/*.cpp",
		"*.h",
		"*.cpp",
	}
	
	links{
		"comctl32"
	}