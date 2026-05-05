#pragma once

#include "Debug/Debug.hpp"
#include "Core/Span.hpp"


class TChar
{
public:
	[[nodiscard]]
	static constexpr size_t GetUtf8CharSize(unsigned char InCodepoint) noexcept
	{
		if (InCodepoint < 128) { return 1; }
		if (InCodepoint >= 0b11110000) { return 4; }
		if (InCodepoint >= 0b11100000) { return 3; }
		if (InCodepoint >= 0b11000000) { return 2; }
		return 0;
	}

private:
	constexpr void SetUtf8Char(const char8_t* InChar) noexcept
	{
		size_t Size = GetUtf8CharSize(*InChar);
		if (Size == 4)
		{
			MyData[4] = u8'\0';
			MyData[3] = InChar[3];
			MyData[2] = InChar[2];
			MyData[1] = InChar[1];
			MyData[0] = InChar[0];
		}
		else if (Size == 3)
		{
			MyData[4] =MyData[3] = u8'\0';
			MyData[2] = InChar[2];
			MyData[1] = InChar[1];
			MyData[0] = InChar[0];
		}
		else if (Size == 2)
		{
			MyData[4] =MyData[3] = MyData[2] = u8'\0';
			MyData[1] = InChar[1];
			MyData[0] = InChar[0];
		}
		else if (Size == 1)
		{
			MyData[4] =MyData[3] = MyData[2] = MyData[1] = u8'\0';
			MyData[0] = InChar[0];
		}
		else
		{
			MyData[4] = MyData[3] = MyData[2] = MyData[1] = MyData[0] = u8'\0';
		}
	}
public:
	constexpr TChar() noexcept
	{
		MyData[4] =MyData[3] = MyData[2] = MyData[1] = MyData[0] = u8'\0';
	}
	constexpr TChar(const char8_t InChar) noexcept
	{
		MyData[0] = InChar;
		MyData[4] =MyData[3] = MyData[2] = MyData[1] = u8'\0';
	}
	constexpr TChar(const char* InChar) noexcept
		: TChar(reinterpret_cast<const char8_t*>(InChar))
	{}
	constexpr TChar(const char8_t* InChar) noexcept
	{
		SetUtf8Char(InChar);
	}
	constexpr TChar(const char8_t* InChar, size_t InLength)
	{
		MyData[4] =MyData[3] = MyData[2] = MyData[1] = MyData[0] = u8'\0';
		for (size_t Idx = 0; Idx < InLength; ++Idx)
		{
			MyData[Idx] = InChar[Idx];
		}
	}
	constexpr TChar(const TChar& InOther)
		: MyData()
	{
		SetUtf8Char(InOther.MyData);
	}
	constexpr TChar(TChar&& InOther)
		: MyData()
	{
		SetUtf8Char(InOther.MyData);
		InOther.SetUtf8Char(u8""); /* fill '\0' */
	}

	[[nodiscard]]
	const char* operator&() const noexcept { return reinterpret_cast<const char*>(MyData); }
	[[nodiscard]]
	constexpr operator bool() const noexcept { return MyData[0] != u8'\0'; }

	constexpr TChar& operator=(const TChar& InRhs)
	{
		SetUtf8Char(InRhs.MyData);
		return *this;
	}
	constexpr TChar& operator=(TChar&& InRhs)
	{
		SetUtf8Char(InRhs.MyData);
		InRhs.SetUtf8Char(u8""); /* fill '\0' */
		return *this;
	}

	[[nodiscard]]
	constexpr bool operator==(const TChar& InRhs) const noexcept
	{
		return MyData[4] == InRhs.MyData[4] &&
			MyData[3] == InRhs.MyData[3] &&
			MyData[2] == InRhs.MyData[2] &&
			MyData[1] == InRhs.MyData[1] &&
			MyData[0] == InRhs.MyData[0];
	}
	[[nodiscard]]
	constexpr bool operator!=(const TChar& InRhs) const noexcept { return !(*this == InRhs); }
	[[nodiscard]]
	constexpr bool operator>(const TChar& InRhs) const noexcept
	{
		return *(unsigned int*)MyData > *(unsigned int*)InRhs.MyData;
	}
	[[nodiscard]]
	constexpr bool operator<(const TChar& InRhs) const noexcept
	{
		return *(unsigned int*)MyData < *(unsigned int*)InRhs.MyData;
	}

	[[nodiscard]]
	constexpr bool operator>=(const TChar& InRhs) const noexcept
	{
		return *(unsigned int*)MyData >= *(unsigned int*)InRhs.MyData;
	}
	[[nodiscard]]
	constexpr bool operator<=(const TChar& InRhs) const noexcept
	{
		return *(unsigned int*)MyData <= *(unsigned int*)InRhs.MyData;
	}

	[[nodiscard]]
	constexpr size_t GetSize() const noexcept { return GetUtf8CharSize(MyData[0]); }

PRIVATE:
	char8_t MyData[5];

private:
	friend class String;
	friend class Char;
	friend class TUtf8String;
};

typedef TSpan<char32_t> TUtf32StringView;
