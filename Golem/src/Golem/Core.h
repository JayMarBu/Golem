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

#define CLEANUP(x) if(x != NULL) { delete x; x = NULL;}

#define REMOVE_COPY_CONSTRUCTOR(_class_) _class_(const _class_&) = delete; _class_ &operator=(const _class_&) = delete;

#define SAFE_RUN_VULKAN_FUNC(_create_method_, _message_) if(_create_method_ != VK_SUCCESS) {throw std::runtime_error(_message_);}

#define  MAP_CONTAINS(_map_,_val_) (_map_.find(_val_) != _map_.end())
#define  LOOP_MAP_ITEMS(_map_, _it_) auto _it_ = _map_.begin(); _it_ != _map_.end(); _it_++