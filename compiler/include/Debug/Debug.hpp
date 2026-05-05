#pragma once

#if defined(IN_TEST)
#include "TestFramework.hpp"
#endif


#include <format>
#include <stdexcept>

namespace _internal {
	struct constexpr_assert_failed {};
	[[noreturn]] inline void assert_fail_runtime(
		const char* Expr,
		const char* Message,
		const char* Filename,
		unsigned int Line,
		const char* Func
	)
	{
		std::string message = std::format(
			"Assertion failed: ({}), function: {}, filename {}, line: {}.\nmessage: {}",
			Expr, Func, Filename, Line, Message
		);
		throw std::runtime_error(message);
	}
}

#define ENSURE(condition, message) do { \
		if (!(condition)) [[unlikely]] \
		{ \
			if consteval \
			{ \
				throw _internal::constexpr_assert_failed {}; \
			} \
			else \
			{ \
				_internal::assert_fail_runtime(#condition, message, __FILE__, __LINE__, __FUNCTION__); \
			} \
		} \
	} while (false)

#if defined(NDEBUG)
	#if !defined(IN_TEST)
		#define ASSERT(condition, message)
	#endif

	#define DEBUG_BREAK() do {} while (false)
	#define PRIVATE private

#else
	#if !defined(IN_TEST)
		#define ASSERT(condition, message) do { \
			if (!(condition)) [[unlikely]] \
			{ \
				if consteval \
				{ \
					throw _internal::constexpr_assert_failed {}; \
				} \
				else \
				{ \
					_internal::assert_fail_runtime(#condition, message, __FILE__, __LINE__, __FUNCTION__); \
				} \
			} \
		} while (false)
	#endif

	#define PRIVATE public

	#if defined(_MSC_VER)
		#include <Windows.h>
		#define DEBUG_BREAK() __debugbreak()
	#elif defined(__GNUC__) || defined(__clang__)
		#if defined(__APPLE__) || defined(__linux__)
			#include <signal.h>
			#define DEBUG_BREAK() raise(SIGTRAP)
		#else
			#define DEBUG_BREAK() __builtin_trap()
		#endif
	#else
		#define DEBUG_BREAK() do { } while (false)
	#endif

#endif
