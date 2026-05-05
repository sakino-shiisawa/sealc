#pragma once

#include "Core/Optional.hpp"
#include "String/BasicString.hpp"

#include "String/Utf32String.hpp"
#include "String/Utf8String.hpp"


enum class EConvertEncodingError
{
	NoError,
	InValidChar,
};

class Char
{
public:
	[[nodiscard]]
	static TOptional<char32_t, EConvertEncodingError> ConvertToUtf32(const TChar& InChar) noexcept
	{
		size_t Length = InChar.GetSize();
		if (Length == 0 || Length > 4) return EConvertEncodingError::InValidChar;
		const char* Data = reinterpret_cast<const char*>(InChar.MyData);
		char32_t CodePoint = 0;
		if (Length == 1)
		{
			if ((Data[0] & 0x80) != 0)
			{
				return EConvertEncodingError::InValidChar;
			}
			CodePoint = static_cast<char32_t>(Data[0]);
		}
		else
		{
			if (Length == 2)
			{
				if ((Data[0] & 0xE0) != 0xC0)
				{
					return EConvertEncodingError::InValidChar;
				}
				CodePoint = (static_cast<char32_t>(Data[0]) & 0x1F) << 6;
			}
			else if (Length == 3)
			{
				if ((Data[0] & 0xF0) != 0xE0)
				{
					return EConvertEncodingError::InValidChar;
				}
				CodePoint = (static_cast<char32_t>(Data[0]) & 0x0F) << 12;
			}
			else if (Length == 4)
			{
				if ((Data[0] & 0xF8) != 0xF0)
				{
					return EConvertEncodingError::InValidChar;
				}
				CodePoint = (static_cast<char32_t>(Data[0]) & 0x07) << 18;
			}
			for (size_t Idx = 1; Idx < Length; ++Idx)
			{
				if ((Data[Idx] & 0xC0) != 0x80)
				{
					return EConvertEncodingError::InValidChar;
				}
				CodePoint |= (static_cast<char32_t>(Data[Idx]) & 0x3F) << (6 * (Length - 1 - Idx));
			}
		}
		return CodePoint;
	}

	[[nodiscard]]
	static constexpr TOptional<TChar, EConvertEncodingError> ConvertToUtf8(char32_t InChar) noexcept
	{
		TChar Char;
		if (InChar <= 0x7F)
		{
			Char.MyData[0] = static_cast<char8_t>(InChar);
			return Char;
		}
		if (InChar <= 0x7FF)
		{
			Char.MyData[0] = 0b11000000 | static_cast<char8_t>(InChar >> 6);
			Char.MyData[1] = 0b10000000 | static_cast<char8_t>(InChar & 0b00111111);
			return Char;
		}

		if (InChar <= 0xFFFF)
		{
			Char.MyData[0] = 0b11100000 | static_cast<char8_t>(InChar >> 12);
			Char.MyData[1] = 0b10000000 | static_cast<char8_t>((InChar >> 6) & 0b00111111);
			Char.MyData[2] = 0b10000000 | static_cast<char8_t>(InChar & 0b00111111);
			return Char;
		}

		Char.MyData[0] = 0b11110000 | static_cast<char8_t>(InChar >> 18);
		Char.MyData[1] = 0b10000000 | static_cast<char8_t>((InChar >> 12) & 0b00111111);
		Char.MyData[2] = 0b10000000 | static_cast<char8_t>((InChar >> 6) & 0b00111111);
		Char.MyData[3] = 0b10000000 | static_cast<char8_t>(InChar & 0b00111111);
		return Char;
	}
};

class String
{
public:
	[[nodiscard]]
	static TOptional<TUtf32String, EConvertEncodingError> ConvertToUtf32(const char* InStr) noexcept
	{
		TUtf32String Str;
		TOptional<char32_t, EConvertEncodingError> Char;
		for (const char* Itr = InStr; *Itr;)
		{
			TChar Utf8Char(reinterpret_cast<const char8_t*>(Itr));
			Char = Char::ConvertToUtf32(Utf8Char);
			if (!Char) { return Char.GetStatus(); }
			Str.PushBack(*Char);
			Itr += Utf8Char.GetSize();
		}
		return Str;
	}
	[[nodiscard]]
	static TOptional<TUtf8String, EConvertEncodingError> ConvertToUtf8(const char32_t* InStr) noexcept
	{
		TArray<char8_t> Str;

		for (const char32_t* Itr = InStr; *Itr; ++Itr)
		{
			TOptional<TChar, EConvertEncodingError> Char = Char::ConvertToUtf8(*Itr);
			if (!Char) { return EConvertEncodingError::InValidChar; }
			for (size_t CIdx = 0; CIdx < (*Char).GetSize(); ++CIdx)
			{
				Str.PushBack((*Char).MyData[CIdx]);
			}
		}
		return TUtf8String(Str.Data(), Str.Size());
	}
};
