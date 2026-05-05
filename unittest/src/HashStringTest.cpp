#include "TestFramework.hpp"

// including target api
#include "String/HashString.hpp"

IMPLEMENT_TEST_CLASS(HashString)
bool HashStringTest::RunTest() const
{
    // Test default constructor
    THashString defaultHash;
    AssertTrue(defaultHash.GetString() == TUtf32String(U"InValidHash"), "Default constructor");

    // Test constructor with TUtf8String
    TUtf8String utf8Str("hello");
    THashString hash1(utf8Str);
    AssertTrue(hash1.GetString() == TUtf32String(U"hello"), "Constructor with TUtf8String");

    // Test constructor with TUtf32String
    TUtf32String utf32Str(U"world");
    THashString hash2(utf32Str);
    AssertTrue(hash2.GetString() == utf32Str, "Constructor with TUtf32String");

    // Test constructor with const char32_t*
    THashString hash3(U"test");
    AssertTrue(hash3.GetString() == TUtf32String(U"test"), "Constructor with const char32_t*");

    // Test constructor with char32_t
    THashString hash4(U'a');
    AssertTrue(hash4.GetString() == TUtf32String(U"a"), "Constructor with char32_t");

    // Test equality
    THashString hash5("hello");
    AssertTrue(hash1 == hash5, "Equality operator");

    // Test inequality
    AssertTrue(hash1 != hash2, "Inequality operator");

    // Test comparison operators
    THashString hash6("apple");
    THashString hash7("banana");
    AssertTrue(hash6 < hash7, "Less than operator");

    // Test same string returns same hash
    THashString hash8("hello");
    AssertTrue(hash1 == hash8, "Same string same hash");

    return true;
}