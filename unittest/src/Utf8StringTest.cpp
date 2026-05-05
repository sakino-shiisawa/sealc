#include "TestFramework.hpp"

// including target api
#include "String/Utf8String.hpp"

IMPLEMENT_TEST_CLASS(Utf8String)
bool Utf8StringTest::RunTest() const
{
    // Test default constructor
    TUtf8String str1;
    AssertTrue(str1.CharCount() == 0 && str1.IsEmpty(), "Default constructor");

    // Test constructor with const char*
    TUtf8String str2("hello");
    AssertTrue(str2.CharCount() == 5 && str2.BufferSize() == 6, "Constructor with const char*");

    // Test constructor with const char8_t*
    const char8_t* utf8Str = u8"world";
    TUtf8String str3(utf8Str);
    AssertTrue(str3.CharCount() == 5 && str3.BufferSize() == 6, "Constructor with const char8_t*");

    // Test constructor with const char8_t* and size
    TUtf8String str4(utf8Str, 3);
    AssertTrue(str4.CharCount() == 3 && str4.BufferSize() == 4, "Constructor with const char8_t* and size");

    // Test Reset
    str2.Reset();
    AssertTrue(str2.CharCount() == 0 && str2.IsEmpty(), "Reset");

    // Test Reserve and ShrinkToFit
    TUtf8String str5("test");
    str5.Reserve(20);
    AssertTrue(str5.BufferSize() <= 20, "Reserve");
    str5.ShrinkToFit();
    AssertTrue(str5.BufferSize() == 5, "ShrinkToFit");

    // Test SubStr
    TUtf8String str6("hello world");
    auto it1 = str6.begin();
    ++it1; // 'e'
    auto it2 = str6.begin();
    ++it2; ++it2; ++it2; ++it2; // 'o'
    TUtf8String substr = str6.SubStr(it1, it2);
    AssertTrue(substr.CharCount() == 3, "SubStr"); // 'ell'

    // Test operator+
    TUtf8String str7 = u8"hello" + str3;
    AssertTrue(str7.CharCount() == 10, "operator+ char8_t* + TUtf8String");

    TUtf8String str8 = str3 + u8"!";
    AssertTrue(str8.CharCount() == 6, "operator+ TUtf8String + char8_t*");

    // Test iterators
    auto it = str3.begin();
    AssertTrue(it.Size() == 1 && *it == TChar(u8'w'), "Iterator begin");
    ++it;
    AssertTrue(*it == TChar(u8'o'), "Iterator increment");
    --it;
    AssertTrue(*it == TChar(u8'w'), "Iterator decrement");
    AssertTrue(it != str3.end(), "Iterator not end");

    // Test Data
    const char8_t* data = str3.Data();
    AssertTrue(data[0] == 'w' && data[5] == '\0', "Data");

    return true;
}