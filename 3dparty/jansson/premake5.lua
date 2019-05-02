
project "jansson"
	kind "StaticLib"
	language "C"
	defines {
		"HAVE_STDINT_H"
	}
	files{
		"*.h",
		"*.c",
	}

	includedirs{
		"./",
	}
