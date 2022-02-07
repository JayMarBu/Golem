#pragma once

#ifdef GOL_DEBUG
	#define GOL_ENABLE_ASSERTS
#endif

#ifdef GOL_ENABLE_ASSERTS
	#define GOL_ASSERT(x, ...) {if(!(x)) { GOL_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
	#define GOL_CORE_ASSERT(x, ...) {if(!(x)) { GOL_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else
	#define GOL_ASSERT(x, ...)
	#define GOL_CORE_ASSERT(x, ...)
#endif

#define BIT(n) (1 << n)

#define BIND_EVENT_FUNC(func) std::bind(&func, this, std::placeholders::_1)

#define CLEANUP(x) if(x != NULL) { delete x; x = NULL;}

#define REMOVE_COPY_CONSTRUCTOR(_class_) _class_(const _class_&) = delete; _class_ &operator=(const _class_&) = delete;

#define SAFE_RUN_VULKAN_FUNC(_create_method_, _message_) {auto result = _create_method_; GOL_CORE_ASSERT(result == VK_SUCCESS, _message_);}

#define  MAP_CONTAINS(_map_,_val_) (_map_.find(_val_) != _map_.end())
#define  LOOP_MAP_ITEMS(_map_, _it_) auto _it_ = _map_.begin(); _it_ != _map_.end(); _it_++


#include <memory>
namespace golem
{
	template <typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T>
	using Ref = std::shared_ptr<T>;
}