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
	
	[[nodiscard]]
	static TOptional<TUtf32String, EFileIOError> ReadFile(std::filesystem::path InPath)
	{
		std::ifstream FileStream(InPath, std::ios::binary);
		if (FileStream.fail()) { return EFileIOError::NoExist; }
		std::string RawData(
			(std::istreambuf_iterator<char>(FileStream)),
			(std::istreambuf_iterator<char>())
		);
		std::string NormalizedData;
		NormalizedData.reserve(RawData.size());
		for (size_t Idx = 0; Idx < RawData.size(); ++Idx)
		{
			if (RawData[Idx] == '\r')
			{
				if (Idx + 1 < RawData.size() && RawData[Idx + 1] == '\n')
				{
					// CRLF → LF
					NormalizedData += '\n';
					++Idx;
				}
				else
				{
					// CR → LF
					NormalizedData += '\n';
				}
			}
			else
			{
				NormalizedData += RawData[Idx];
			}
		}
		
		TOptional<TUtf32String, EConvertEncodingError> Utf32Str = String::ConvertToUtf32(NormalizedData.data());
		if (!Utf32Str) { return EFileIOError::EncodingError; }
		return *Utf32Str;
	}
};
