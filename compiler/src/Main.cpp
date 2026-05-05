#include "FileIO/FileIO.hpp"

#include <iostream>

/* 日本語のコメント（エンコードチェック用） */

int main(int Argc, const char** Argv)
{
	if (Argc == 1)
	{
		std::cerr << "no input" << std::endl;
		return 1;
	}
	TOptional<TUtf32String, EFileIOError> RawData = FileIO::ReadFile(Argv[1]);
	if (!RawData)
	{
		std::cerr << "Invalid Encoding" << std::endl;
		return 2;
	}
	const char32_t* RawPtr = RawData->Data();
	std::cout
		<< reinterpret_cast<const char*>(String::ConvertToUtf8(RawPtr)->Data())
		<< std::endl;
	return 0;
}
