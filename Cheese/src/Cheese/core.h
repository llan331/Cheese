#pragma once

#include <memory>

#ifdef CS_PLATFORM_WINDOWS
	#if CS_DYNAMIC_LINK
		#ifdef CS_BUILD_DLL
			#define CS_API __declspec(dllexport)
		#else		
			#define CS_API __declspec(dllimport)
		#endif
	#else
		#define CS_API
	#endif
#else
	#error Cheese only supports Windows!
#endif

#ifdef CS_DEBUG
	#define CS_ENABLE_ASSERTS
#endif

#ifdef CS_ENABLE_ASSERTS
	#define CS_ASSERT(x, ...) { if(!(x)) { CS_ERROR("Assertion Failed: {0}", __VA_ARGS__);  __debugbreak(); } }
	#define CS_CORE_ASSERT(x, ...) { if(!(x)) { CS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define CS_ASSERT(x, ...)
	#define CS_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define CS_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Cheese {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

}