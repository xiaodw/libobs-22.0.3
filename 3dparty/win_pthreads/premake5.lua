
project "pthread"
	kind "SharedLib"
	language "C"
	defines {
		--"PTW32_STATIC_LIB"
		"PTW32_BUILD"
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
