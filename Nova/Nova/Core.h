#pragma once
#ifndef CORE_H
#define CORE_H
#include <memory>
#ifdef WIN32
	#ifdef NOVA_BUILD_DLL
		#define NOVA_API __declspec(dllexport)
	#elseif NOVA_USE_DLL
		#define NOVA_API __declspec(dllimport)
	#else
		#define NOVA_API
	#endif
#else
	#define NOVA_API 
		
#endif

#ifdef NOVA_DEBUG
	#define NOVA_ASSERT(x, ...) {if(!(x)) {NOVA_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();} }
	#define NOVA_CORE_ASSERT(x, ...) {if(!(x)){NOVA_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
#else 
	#define NOVA_ASSERT(x, ...)
	#define NOVA_CORE_ASSERT(x, ...)
#endif
#define BIT(x) (1 << x)
namespace Nova
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ...Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}


	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ...Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}
#endif