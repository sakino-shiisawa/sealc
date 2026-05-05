#include "TestFramework.hpp"

// including target api
#include "FileIO/FileIO.hpp"
#include <cstring>
#include <filesystem>

IMPLEMENT_TEST_CLASS(FileIO)
bool FileIOTest::RunTest() const
{
    // Test WriteFile and ReadFile with UTF-8 Japanese text
    std::filesystem::path testPath = "test_utf8.txt";
    TUtf8String testContent(u8"こんにちは、世界！"); // Hello, world! in Japanese

    // Write the file
    bool writeResult = FileIO::WriteFile(testPath, testContent);
    AssertTrue(writeResult, "WriteFile should succeed");

    // Read the file
    auto readResult = FileIO::ReadFile(testPath);
    AssertTrue(readResult.HasValue(), "ReadFile should succeed");
    if (readResult.HasValue())
    {
        // Convert back to UTF-8 for comparison
        auto utf8Result = String::ConvertToUtf8(readResult.Value().Data());
        AssertTrue(utf8Result.HasValue(), "Conversion to UTF-8 should succeed");
        if (utf8Result.HasValue())
        {
            AssertTrue(utf8Result.Value().CharCount() == testContent.CharCount(), "Char count should match");
            AssertTrue(std::strcmp(reinterpret_cast<const char*>(utf8Result.Value().Data()), reinterpret_cast<const char*>(testContent.Data())) == 0, "Read content should match written content");
        }
    }

    // Clean up
    std::filesystem::remove(testPath);

    return true;
}
