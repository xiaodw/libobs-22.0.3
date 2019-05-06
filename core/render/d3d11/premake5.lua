project "obs_d3d11"
	kind "SharedLib"
	language "C"
	defines {
		"GLAD_GLAPI_EXPORT_BUILD"
	}
	files{
		"*.hpp",
		"*.cpp",
	}

	includedirs{
		"../../"
	}

	links{
		"libobs",
		"d3d11",
		"dxgi",
	}
