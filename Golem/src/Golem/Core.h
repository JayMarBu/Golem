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

#define BIND_EVENT_FUNC(func) [this](auto&&... args) -> decltype(auto) {return this->func(std::forward<decltype(args)>(args)...); }
// std::bind(&func, this, std::placeholders::_1)

#define CLEANUP(x) if(x != NULL) { delete x; x = NULL;}

#define REMOVE_COPY_CONSTRUCTOR(_class_) _class_(const _class_&) = delete; _class_ &operator=(const _class_&) = delete;

#define SAFE_RUN_VULKAN_FUNC(_create_method_, _message_) {auto result = _create_method_; GOL_CORE_ASSERT(result == VK_SUCCESS, _message_);}

#define  MAP_CONTAINS(_map_,_val_) (_map_.find(_val_) != _map_.end())
#define  LOOP_MAP_ITEMS(_map_, _it_) auto _it_ = _map_.begin(); _it_ != _map_.end(); _it_++

#define COMBINE1(X,Y) X##Y  // helper macro
#define COMBINE(X,Y) COMBINE1(X,Y)

#include <memory>
namespace golem
{
	// will eventually replace these with custom types
	// with intrusive ref counting. Using aliases for now
	// to make it less of a headache to implement later

	template <typename T>
	using Scope = std::unique_ptr<T>;
	template <typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Ref = std::shared_ptr<T>;
	template <typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}