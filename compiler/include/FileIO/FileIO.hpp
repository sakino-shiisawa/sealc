#pragma once

#include "Core/Optional.hpp"
#include "String/Converter.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>


enum class EFileIOError
{
	NoError,
	NoExist,
	EncodingError,
};

class FileIO
{
public:
	static bool WriteFile(std::filesystem::path InPath, const TUtf8String& InBuffer)
	{
		std::ofstream FileStream(InPath, std::ios::binary);
		if (!FileStream.fail())
		{
			FileStream.write(reinterpret_cast<const char*>(InBuffer.Data()), static_cast<std::streamsize>(InBuffer.BufferSize()) - 1);
			return true;
		}
		return false;
	}
	
	static TOptional<TUtf32String, EFileIOError> ReadFile(std::filesystem::path InPath)
	{
		std::ifstream FileStream(InPath, std::ios::binary);
		if (FileStream.fail()) { return EFileIOError::NoExist; }
		std::string RawData(
			(std::istreambuf_iterator<char>(FileStream)),
			(std::istreambuf_iterator<char>())
		);
		TOptional<TUtf32String, EConvertEncodingError> Utf32Str = String::ConvertToUtf32(RawData.data());
		if (!Utf32Str) { return EFileIOError::EncodingError; }
		return *Utf32Str;
	}
};
