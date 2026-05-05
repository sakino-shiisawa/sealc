#pragma once

#include "Debug/Debug.hpp"
#include "Core/Array.hpp"
#include <cstring>


class TUtf32String
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
			ENSURE(MyCurrent < MyEnd, "Out of Range");
			return *this;
		}
		TIterator operator++(int)
		{
			TIterator Temp(*this);
			++MyCurrent;
			ENSURE(MyCurrent < MyEnd, "Out of Range");
			return Temp;
		}
		TIterator& operator--()
		{
			--MyCurrent;
			ENSURE(MyCurrent >= MyHead, "Out of Range");
			return *this;
		}
		TIterator operator--(int)
		{
			TIterator Temp(*this);
			--MyCurrent;
			ENSURE(MyCurrent >= MyHead, "Out of Range");
			return Temp;
		}

		friend TIterator operator+(const TIterator& InLhs, std::ptrdiff_t InRhs)
		{
			char32_t* NewCurrent = InLhs.MyCurrent + InRhs;
			ENSURE(InLhs.MyHead <= NewCurrent && NewCurrent < InLhs.MyEnd, "Out of Range");
			return TIterator(InLhs.MyHead, NewCurrent, InLhs.MyEnd);
		}
		friend TIterator operator+(std::ptrdiff_t InLhs, const TIterator& InRhs)
		{
			char32_t* NewCurrent = InRhs.MyCurrent + InLhs;
			ENSURE(InRhs.MyHead <= NewCurrent && NewCurrent < InRhs.MyEnd, "Out of Range");
			return TIterator(InRhs.MyHead, NewCurrent, InRhs.MyEnd);
		}
		friend TIterator operator-(const TIterator& InLhs, std::ptrdiff_t InRhs)
		{
			char32_t* NewCurrent = InLhs.MyCurrent - InRhs;
			ENSURE(InLhs.MyHead <= NewCurrent && NewCurrent < InLhs.MyEnd, "Out of Range");
			return TIterator(InLhs.MyHead, NewCurrent, InLhs.MyEnd);
		}
		friend TIterator operator-(std::ptrdiff_t InLhs, const TIterator& InRhs)
		{
			char32_t* NewCurrent = InRhs.MyCurrent - InLhs;
			ENSURE(InRhs.MyHead <= NewCurrent && NewCurrent < InRhs.MyEnd, "Out of Range");
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

		char32_t& operator*() noexcept { return *MyCurrent; }
		const char32_t& operator*() const noexcept { return *MyCurrent; }

	private:
		TIterator(char32_t* InHead, char32_t* InCurrent, char32_t* InEnd)
			: MyHead(InHead)
			, MyCurrent(InCurrent)
			, MyEnd(InEnd)
		{}

	PRIVATE:
		char32_t* MyHead;
		char32_t* MyCurrent;
		char32_t* MyEnd;
	private:
		friend TUtf32String;
	};
public:
	static size_t Strlen(const char32_t* InStr) noexcept
	{
		size_t Idx = 0;
		while (*InStr) { InStr++; Idx++; }
		return Idx;
	}

public:
	TUtf32String() = default;
	~TUtf32String() = default;

	TUtf32String(const char32_t InChar)
	{
		MyData.PushBack(InChar);
		MyData.PushBack(U'\0');
	}

	TUtf32String(const char32_t* InStr)
		: TUtf32String(InStr, Strlen(InStr))
	{}
	TUtf32String(const char32_t* InStr, size_t InSize)
		: MyData()
	{
		MyData.Resize(InSize + 1);
		std::memcpy(MyData.Data(), InStr, InSize * sizeof(char32_t));
	}

	size_t Size() const noexcept { return MyData.Size(); }
	size_t CharCount() const noexcept { return MyData.IsEmpty() ? 0 : MyData.Size() - 1; }
	size_t Capacity() const noexcept { return MyData.Capacity(); }

	const char32_t* Data() const noexcept { return MyData.Data(); }
	char32_t* Data() noexcept { return MyData.Data(); }

	char32_t& operator[](size_t InIdx) noexcept
	{
		return MyData[InIdx];
	}
	const char32_t& operator[](size_t InIdx) const noexcept
	{
		return MyData[InIdx];
	}

	void PushBack(char32_t InChar) noexcept
	{
		if (!MyData.IsEmpty() && MyData.Data()[MyData.Size() - 1] == '\0')
		{
			MyData.PopBack();
		}
		MyData.PushBack(InChar);
		MyData.PushBack(U'\0');
	}

	TIterator begin() noexcept { return TIterator(MyData.Data(), MyData.Data(), MyData.Data() + MyData.Size()); }
	TIterator end() noexcept { return TIterator(MyData.Data(), MyData.Data() + MyData.Size(), MyData.Data() + MyData.Size()); }
	TIterator begin() const noexcept
	{
		char32_t* Ptr = const_cast<char32_t*>(MyData.Data());
		return TIterator(
			Ptr,
			Ptr,
			Ptr + MyData.Size()
		);
	}
	TIterator end() const noexcept
	{
		char32_t* Ptr = const_cast<char32_t*>(MyData.Data());
		return TIterator(
			Ptr,
			Ptr + MyData.Size(),
			Ptr + MyData.Size()
		);
	}

	bool operator==(const TUtf32String& InRhs) const noexcept
	{
		if (MyData.Size() != InRhs.Size()) { return false; }
		for (size_t Idx = 0; Idx < Size(); ++Idx)
		{
			if (MyData[Idx] != InRhs.MyData[Idx]) { return false; }
		}
		return true;
	}
	bool operator!=(const TUtf32String& InRhs) const noexcept
	{
		return !(*this == InRhs);
	}

PRIVATE:
	TArray<char32_t> MyData;
};
