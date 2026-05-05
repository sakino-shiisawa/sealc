#pragma once

#include "Core/Array.hpp"
#include "String/BasicString.hpp"
#include <cstring>


class TUtf8String
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
			size_t Count = TChar::GetUtf8CharSize(*MyCurrent);
			ENSURE(Count > 0, "Invalid Codepoint");
			MyCurrent += Count;
			ENSURE(MyEnd >= MyCurrent, "Out of range");
			return *this;
		}
		TIterator operator++(int)
		{
			TIterator Temp(*this);
			++(*this);
			return Temp;
		}
		TIterator& operator--()
		{
			do {
				--MyCurrent;
				ENSURE(MyCurrent >= MyHead, "Out of Range");
			}
			while (TChar::GetUtf8CharSize(*MyCurrent) == 0);
			return *this;
		}
		TIterator operator--(int)
		{
			TIterator Temp(*this);
			--(*this);
			return Temp;
		}

		[[nodiscard]]
		size_t Size() const noexcept { return TChar::GetUtf8CharSize(*MyCurrent); }

		[[nodiscard]]
		TChar operator*() const { return TChar(MyCurrent); }

		[[nodiscard]]
		bool operator==(const TIterator& InRhs) const noexcept { return MyCurrent == InRhs.MyCurrent; }
		[[nodiscard]]
		bool operator!=(const TIterator& InRhs) const noexcept { return MyCurrent != InRhs.MyCurrent; }
		[[nodiscard]]
		bool operator>(const TIterator& InRhs) const noexcept { return MyCurrent > InRhs.MyCurrent; }
		[[nodiscard]]
		bool operator>=(const TIterator& InRhs) const noexcept { return MyCurrent >= InRhs.MyCurrent; }
		[[nodiscard]]
		bool operator<(const TIterator& InRhs) const noexcept { return MyCurrent < InRhs.MyCurrent; }
		[[nodiscard]]
		bool operator<=(const TIterator& InRhs) const noexcept { return MyCurrent <= InRhs.MyCurrent; }

	private:
		TIterator(char8_t* InHead, char8_t* InCurrent, char8_t* InEnd)
			: MyHead(InHead)
			, MyCurrent(InCurrent)
			, MyEnd(InEnd)
		{}

	PRIVATE:
		char8_t* MyHead;
		char8_t* MyCurrent;
		char8_t* MyEnd;

	private:
		friend TUtf8String;
	};
public:
	TUtf8String() = default;
	~TUtf8String() = default;

	TUtf8String(const char* InStr)
		: TUtf8String(reinterpret_cast<const char8_t*>(InStr))
	{}
	TUtf8String(const char8_t* InStr)
		: TUtf8String(InStr, std::strlen(reinterpret_cast<const char*>(InStr)))
	{}
	TUtf8String(const char8_t* InStr, size_t InSize)
	{
		ASSERT(InSize > 0, "InValid String Length");
		MyData.Resize(InSize + 1);
		std::memcpy(MyData.Data(), InStr, InSize);
		MyCharCount = 0;
		for (size_t Idx = 0; Idx < InSize;)
		{
			size_t Size = TChar::GetUtf8CharSize(InStr[Idx]);
			ASSERT(Size > 0, "Invalid Char Encoding");
			Idx += Size;
			++MyCharCount;
		}
	}

	/* excluding null-char */
	[[nodiscard]]
	size_t CharCount() const noexcept
	{
		return MyCharCount;
	}
	[[nodiscard]]
	size_t BufferSize() const noexcept
	{
		return MyData.Size();
	}

	[[nodiscard]]
	bool IsEmpty() const noexcept { return MyData.IsEmpty(); }
	void Reset() noexcept
	{
		MyCharCount = 0;
		MyData.Resize(0);
	}
	void ShrinkToFit() noexcept
	{
		MyData.ShrinkToFit();
	}
	void Reserve(size_t InNewSize) noexcept
	{
		MyData.Reserve(InNewSize);
	}

	[[nodiscard]]
	TUtf8String SubStr(const TIterator& InBegin, const TIterator& InEnd) const
	{
		ASSERT(InEnd.MyCurrent - InBegin.MyCurrent > 0, "reverse address");
		return TUtf8String(InBegin.MyCurrent, static_cast<size_t>(InEnd.MyCurrent - InBegin.MyCurrent));
	}

	[[nodiscard]]
	friend TUtf8String operator+(const char8_t* InLhs, const TUtf8String& InRhs) noexcept
	{
		TUtf8String LhsStr = InLhs;
		TUtf8String Ret;
		Ret.MyData.Resize(LhsStr.BufferSize() + InRhs.BufferSize() + 1);
		for (const auto& Char : LhsStr)
		{
			for (size_t CIdx = 0; CIdx < Char.GetSize(); ++CIdx)
			{
				Ret.MyData.PushBack(Char.MyData[CIdx]);
			}
			++Ret.MyCharCount;
		}
		for (const auto& Char : InRhs)
		{
			for (size_t CIdx = 0; CIdx < Char.GetSize(); ++CIdx)
			{
				Ret.MyData.PushBack(Char.MyData[CIdx]);
			}
			++Ret.MyCharCount;
		}
		return Ret;
	}
	[[nodiscard]]
	friend TUtf8String operator+(const TUtf8String& InLhs, const char8_t* InRhs) noexcept
	{
		TUtf8String RhsStr = InRhs;
		TUtf8String Ret;
		Ret.MyData.Resize(RhsStr.BufferSize() + InLhs.BufferSize() + 1);
		for (const auto& Char : InLhs)
		{
			for (size_t CIdx = 0; CIdx < Char.GetSize(); ++CIdx)
			{
				Ret.MyData.PushBack(Char.MyData[CIdx]);
			}
			++Ret.MyCharCount;
		}
		for (const auto& Char : RhsStr)
		{
			for (size_t CIdx = 0; CIdx < Char.GetSize(); ++CIdx)
			{
				Ret.MyData.PushBack(Char.MyData[CIdx]);
			}
			++Ret.MyCharCount;
		}
		return Ret;
	}

	[[nodiscard]]
	TIterator begin() noexcept { return TIterator(MyData.Data(), MyData.Data(), MyData.Data() + MyData.Size() - 1); }
	[[nodiscard]]
	TIterator end() noexcept { return TIterator(MyData.Data(), MyData.Data() + MyData.Size() - 1, MyData.Data() + MyData.Size() - 1); }
	[[nodiscard]]
	TIterator begin() const noexcept
	{
		char8_t* Ptr = const_cast<char8_t*>(MyData.Data());
		return TIterator(
			Ptr,
			Ptr,
			Ptr + MyData.Size() - 1
		);
	}
	[[nodiscard]]
	TIterator end() const noexcept
	{
		char8_t* Ptr = const_cast<char8_t*>(MyData.Data());
		return TIterator(
			Ptr,
			Ptr + MyData.Size() - 1,
			Ptr + MyData.Size() - 1
		);
	}

	[[nodiscard]]
	const char8_t* Data() const noexcept { return MyData.Data(); }
	[[nodiscard]]
	char8_t* Data() noexcept { return MyData.Data(); }

PRIVATE:
	TArray<char8_t> MyData;
	size_t MyCharCount { 0 };
};
