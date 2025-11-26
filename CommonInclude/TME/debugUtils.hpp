#ifndef TME_DEBUG_UTILS_HPP
#define TME_DEBUG_UTILS_HPP

#include <cassert>
#include <type_traits>
#include <iostream>
#include <mutex>
#include <sstream>

#ifdef NDEBUG
#define TME_ASSERT_REF_PTR_OR_COPIABLE(obj) ((void)0)
#define TME_DEBUG_LOG(fmt, ...) ((void)0)

#else
#define TME_ASSERT_REF_PTR_OR_COPIABLE(obj) \
	static_assert(RefPtrOrTriviallyCopiable<decltype(obj)>().m_value && #obj " is not a reference, a pointer, or copyable")
#define TME_DEBUG_LOG(fmt, ...) \
	{ \
		char buffer[512]; \
		snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__); \
		std::stringstream ss; \
		ss << "\033[34m[DEBUG]\033[0m " << buffer << "\n"; \
		std::cout << ss.str(); \
	}
#endif

#define TME_INFO_LOG(fmt, ...) \
	{ \
		char buffer[512]; \
		snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__); \
		std::stringstream ss; \
		ss << "\033[32m[INFO]\033[0m " << buffer << "\n"; \
		std::cerr << ss.str(); \
	}

#define TME_ERROR_LOG(fmt, ...) \
	{ \
		char buffer[512]; \
		snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__); \
		std::stringstream ss; \
		ss << "\033[31m[ERROR]\033[0m " << buffer << "\n"; \
		std::cerr << ss.str(); \
	}

	template<typename T>
	struct RefPtrOrTriviallyCopiable
	{
		bool m_value = std::is_reference_v<T> || std::is_pointer_v<T> || std::is_trivially_copyable_v<T>;
	};

#endif
