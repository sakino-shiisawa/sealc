#include "TestFramework.hpp"

// including target api
#include "String/Utf32String.hpp"

IMPLEMENT_TEST_CLASS(Utf32String)
bool Utf32StringTest::RunTest() const
{
    // Test default constructor
    TUtf32String str1;
    AssertTrue(str1.Size() == 0 && str1.CharCount() == 0, "Default constructor");

    // Test constructor with char32_t
    TUtf32String str2(U'a');
    AssertTrue(str2.CharCount() == 1 && str2[0] == U'a' && str2[1] == U'\0', "Constructor with char32_t");

    // Test constructor with const char32_t*
    TUtf32String str3(U"hello");
    AssertTrue(str3.CharCount() == 5 && str3[0] == U'h' && str3[4] == U'o' && str3[5] == U'\0', "Constructor with const char32_t*");

    // Test constructor with const char32_t* and size
    const char32_t* testStr = U"world";
    TUtf32String str4(testStr, 3);
    AssertTrue(str4.CharCount() == 3 && str4[0] == U'w' && str4[2] == U'r' && str4[3] == U'\0', "Constructor with const char32_t* and size");

    // Test PushBack
    str1.PushBack(U'x');
    str1.PushBack(U'y');
    AssertTrue(str1.CharCount() == 2 && str1[0] == U'x' && str1[1] == U'y' && str1[2] == U'\0', "PushBack");

    // Test Size and Capacity
    AssertTrue(str3.Size() == 6 && str3.Capacity() >= 6, "Size and Capacity");

    // Test Data
    const char32_t* data = str3.Data();
    AssertTrue(data[0] == U'h' && data[5] == U'\0', "Data");

    // Test iterators
    auto it = str3.begin();
    AssertTrue(*it == U'h', "Iterator begin");
    ++it;
    AssertTrue(*it == U'e', "Iterator increment");
    --it;
    AssertTrue(*it == U'h', "Iterator decrement");
    AssertTrue(it != str3.end(), "Iterator not end");

    // Test equality
    TUtf32String str5(U"hello");
    AssertTrue(str3 == str5, "Equality operator");

    // Test inequality
    TUtf32String str6(U"world");
    AssertTrue(str3 != str6, "Inequality operator");

    return true;
}