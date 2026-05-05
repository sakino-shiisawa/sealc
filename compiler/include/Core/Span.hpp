#pragma once

#include "Debug/Debug.hpp"


template <class Type>
class TSpan
{
public:
	class TIterator
	{
	public:
		TIterator() = default;
		TIterator(const TIterator&) = default;
		TIterator(TIterator&&) = default;
		~TIterator() = default;

		TIterator& operator=(const TIterator&) = default;
		TIterator& operator=(TIterator&&) = default;

		TIterator& operator++()
		{
			++MyCurrent;
			ASSERT(MyCurrent < MyEnd, "Out of Range");
			return *this;
		}
		TIterator operator++(int)
		{
			TIterator Temp(*this);
			++MyCurrent;
			ASSERT(MyCurrent < MyEnd, "Out of Range");
			return Temp;
		}
		TIterator& operator--()
		{
			--MyCurrent;
			ASSERT(MyCurrent >= MyHead, "Out of Range");
			return *this;
		}
		TIterator operator--(int)
		{
			TIterator Temp(*this);
			--MyCurrent;
			ASSERT(MyCurrent >= MyHead, "Out of Range");
			return Temp;
		}

		friend TIterator operator+(const TIterator& InLhs, std::ptrdiff_t InRhs) noexcept
		{
			Type* NewCurrent = InLhs.MyCurrent + InRhs;
			ASSERT(InLhs.MyHead <= NewCurrent && NewCurrent < InLhs.MyEnd, "Out of Range");
			return TIterator(InLhs.MyHead, NewCurrent, InLhs.MyEnd);
		}
		friend TIterator operator+(std::ptrdiff_t InLhs, const TIterator& InRhs) noexcept
		{
			Type* NewCurrent = InRhs.MyCurrent + InLhs;
			ASSERT(InRhs.MyHead <= NewCurrent && NewCurrent < InRhs.MyEnd, "Out of Range");
			return TIterator(InRhs.MyHead, NewCurrent, InRhs.MyEnd);
		}
		friend TIterator operator-(const TIterator& InLhs, std::ptrdiff_t InRhs) noexcept
		{
			Type* NewCurrent = InLhs.MyCurrent - InRhs;
			ASSERT(InLhs.MyHead <= NewCurrent && NewCurrent < InLhs.MyEnd, "Out of Range");
			return TIterator(InLhs.MyHead, NewCurrent, InLhs.MyEnd);
		}
		friend TIterator operator-(std::ptrdiff_t InLhs, const TIterator& InRhs) noexcept
		{
			Type* NewCurrent = InRhs.MyCurrent - InLhs;
			ASSERT(InRhs.MyHead <= NewCurrent && NewCurrent < InRhs.MyEnd, "Out of Range");
			return TIterator(InRhs.MyHead, NewCurrent, InRhs.MyEnd);
		}

		friend std::ptrdiff_t operator-(const TIterator& InLhs, const TIterator& InRhs) noexcept
		{
			return InLhs.MyCurrent - InRhs.MyCurrent;
		}

		bool operator==(const TIterator& InRhs) const noexcept
		{
			return MyCurrent == InRhs.MyCurrent;
		}
		bool operator!=(const TIterator& InRhs) const noexcept
		{
			return !(*this == InRhs);
		}

		Type& operator*() noexcept { return *MyCurrent; }
		const Type& operator*() const noexcept { return *MyCurrent; }
		Type* operator->() noexcept { return MyCurrent; }
		const Type* operator->() const noexcept { return MyCurrent; }

	private:
		TIterator(Type* InHead, Type* InCurrent, Type* InEnd)
			: MyHead(InHead)
			, MyCurrent(InCurrent)
			, MyEnd(InEnd)
		{}

	PRIVATE:
		Type* MyHead;
		Type* MyCurrent;
		Type* MyEnd;
	};

public:
	constexpr TSpan() = default;
	constexpr ~TSpan() = default;

	constexpr TSpan(Type* InPtr, size_t InSize)
		: MyPtr(InPtr)
		, MySize(InSize)
	{}

	constexpr size_t Size() const noexcept { return MySize; }
	constexpr Type* Data() noexcept { return MyPtr; }
	constexpr const Type* Data() const noexcept { return MyPtr; }

	constexpr Type& operator[](size_t Idx) noexcept
	{
		ASSERT(MySize > Idx, "Out of range");
		return MyPtr[Idx];
	}
	constexpr const Type& operator[](size_t Idx) const noexcept
	{
		ASSERT(MySize > Idx, "Out of range");
		return MyPtr[Idx];
	}

	constexpr TSpan<Type> SubSpan(const TIterator& InBegin, size_t InCount) const noexcept
	{
		return TSpan<Type>(InBegin.MyCurrent, InCount);
	}
	constexpr TSpan<Type> SubSpan(const TIterator& InBegin, const TIterator& InEnd) const noexcept
	{
		return TSpan<Type>(InBegin.MyCurrent, InEnd.MyCurrent - InBegin.MyCurrent);
	}

PRIVATE:
	Type* MyPtr;
	size_t MySize;
};
