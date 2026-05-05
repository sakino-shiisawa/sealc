#pragma once

#include "Debug/Debug.hpp"
#include <utility>


enum class EDefaultErrorType
{
	NoError,
	Error,
};

template <
	class Type,
	class ErrorType = EDefaultErrorType
>
class TOptional
{
public:
	constexpr TOptional() noexcept
		: MyValue()
		, MyError(ErrorType(0))
	{}
	constexpr ~TOptional() noexcept = default;
	
	template <class... Args>
	constexpr TOptional(Args&&... InArgs) noexcept
		: MyValue(std::forward<Args...>(InArgs)...)
		, MyError(ErrorType(0))
	{}
	constexpr TOptional(const Type& InValue) noexcept
		: MyValue(InValue)
		, MyError(ErrorType(0))
	{}
	constexpr TOptional(Type&& InValue) noexcept
		: MyValue(std::move(InValue))
		, MyError(ErrorType(0))
	{}
	constexpr TOptional(const ErrorType& InError) noexcept
		: MyValue()
		, MyError(InError)
	{}
	constexpr TOptional(ErrorType&& InError) noexcept
		: MyValue()
		, MyError(std::move(InError))
	{}

	[[nodiscard]]
	explicit operator bool() const noexcept { return HasValue(); }

	[[nodiscard]]
	constexpr bool HasValue() const noexcept { return MyError == ErrorType(0); }
	[[nodiscard]]
	constexpr const Type& Value() const noexcept { return MyValue; }
	[[nodiscard]]
	constexpr Type& Value() noexcept { return MyValue; }

	[[nodiscard]]
	constexpr Type& operator*() noexcept { return MyValue; }
	[[nodiscard]]
	constexpr const Type& operator*() const noexcept { return MyValue; }

	[[nodiscard]]
	constexpr Type* operator->() noexcept
	{
		if (MyError == ErrorType(0)) { return &MyValue; }
		return nullptr;
	}
	[[nodiscard]]
	constexpr const Type* operator->() const noexcept
	{
		if (MyError == ErrorType(0)) { return &MyValue; }
		return nullptr;
	}
	[[nodiscard]]
	constexpr ErrorType GetStatus() const noexcept { return MyError; }
	constexpr void Reset() noexcept
	{
		MyValue.~Type();
		MyError = ErrorType(0);
	}

PRIVATE:
	Type MyValue;
	ErrorType MyError;
};
