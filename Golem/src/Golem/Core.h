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

#ifdef GOL_ENABLE_ASSERTS
	#define GOL_ASSERT(x, ...) {if(!x) { GOL_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
	#define GOL_CORE_ASSERT(x, ...) {if(!x) { GOL_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
	#define GOL_ASSERT(x, ...)
	#define GOL_CORE_ASSERT(x, ...)
#endif

#define BIT(n) (1 << n)