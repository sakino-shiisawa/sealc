#include "TestFramework.hpp"

// including target api
#include "String/Converter.hpp"

IMPLEMENT_TEST_CLASS(EncodingConvert)
bool EncodingConvertTest::RunTest() const
{
    // Test Char::ConvertToUtf32
    // ASCII character 'a'
    TChar asciiChar("a");
    auto result = Char::ConvertToUtf32(asciiChar);
    AssertTrue(result.HasValue() && result.Value() == U'a', "ASCII character conversion");

    // Test Char::ConvertToUtf8
    // ASCII
    auto utf8Result = Char::ConvertToUtf8(U'a');
    AssertTrue(utf8Result.HasValue() && utf8Result.Value().MyData[0] == 'a' && utf8Result.Value().GetSize() == 1, "UTF-32 to ASCII conversion");

    // 2-byte character (e.g., 'é' U+00E9)
    utf8Result = Char::ConvertToUtf8(U'\u00E9');
    AssertTrue(utf8Result.HasValue() && utf8Result.Value().GetSize() == 2, "2-byte UTF-8 conversion");

    // 3-byte character (e.g., '中' U+4E2D)
    utf8Result = Char::ConvertToUtf8(U'\u4E2D');
    AssertTrue(utf8Result.HasValue() && utf8Result.Value().GetSize() == 3, "3-byte UTF-8 conversion");

    // 4-byte character (e.g., '😀' U+1F600)
    utf8Result = Char::ConvertToUtf8(U'\U0001F600');
    AssertTrue(utf8Result.HasValue() && utf8Result.Value().GetSize() == 4, "4-byte UTF-8 conversion");

    // Test String::ConvertToUtf32
    // Valid UTF-8 string "hello"
    auto strResult = String::ConvertToUtf32("hello");
    AssertTrue(strResult.HasValue() && strResult.Value().CharCount() == 5, "UTF-8 string to UTF-32 conversion");

    // Valid UTF-8 string "こんにちは"
    strResult = String::ConvertToUtf32(reinterpret_cast<const char*>(u8"こんにちは"));
    AssertTrue(strResult.HasValue() && strResult.Value().CharCount() == 5, "Japanese UTF-8 string to UTF-32 conversion");

    // Invalid UTF-8 string (incomplete sequence)
    strResult = String::ConvertToUtf32("\xC0"); // Invalid start byte
    AssertTrue(!strResult.HasValue() && strResult.GetStatus() == EConvertEncodingError::InValidChar, "Invalid UTF-8 string detection");

    // Test String::ConvertToUtf8
    // Valid UTF-32 string U"hello"
    const char32_t* utf32Str = U"hello";
    auto utf8StrResult = String::ConvertToUtf8(utf32Str);
    AssertTrue(utf8StrResult.HasValue() && utf8StrResult.Value().CharCount() == 5, "UTF-32 string to UTF-8 conversion");

    return true;
}
