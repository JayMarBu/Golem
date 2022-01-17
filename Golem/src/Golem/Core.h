#pragma once

#ifdef GOL_PLATFORM_WINDOWS
	#ifdef GOL_BUILD_DLL
		#define GOLEM_API __declspec(dllexport)
	#else
		#define GOLEM_API __declspec(dllimport)
	#endif
#else
	#error Golem Only Supports Windows
#endif

#define BIT(n) (1 << n)