
project "pthread"
	kind "StaticLib"
	language "C++"
	rtti "On"
	exceptionhandling "On"
	defines {
		"PTW32_STATIC_LIB"
	}
	files{
		"implement.h",
		"pthread.h",
		"sched.h",
		"semaphore.h",
		"pthread.c",
	}

	includedirs{
		"./",
	}
