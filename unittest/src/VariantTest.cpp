#include "TestFramework.hpp"

// Include necessary headers for Variant
#include "Core/Variant.hpp"
#include <string>


struct TestVisitor {
	int operator()(std::string str) { return static_cast<int>(str.size()); }
	int operator()(auto val) { return static_cast<int>(val) * 2; }
};

IMPLEMENT_TEST_CLASS(Variant)
bool VariantTest::RunTest() const
{
    // Define a variant type for testing
    using TestVariant = TVariant<int, float, std::string>;

    // Test default constructor (requires default initializable first type)
    TestVariant v1;
    AssertTrue(!v1.HasValue(), "Default constructed variant should have value");
    AssertTrue(!v1.Holds<int>(), "Default constructed variant should hold int");
    AssertTrue(v1.Index() == decltype(v1)::NPos, "Default index should be 0");

    // Test value constructor
    TestVariant v2(42);
    AssertTrue(v2.Holds<int>(), "Variant constructed with int should hold int");
    AssertTrue(*v2.GetIf<int>() == 42, "Value should be 42");

    TestVariant v3(3.14f);
    AssertTrue(v3.Holds<float>(), "Variant constructed with float should hold float");
    AssertTrue(*v3.GetIf<float>() == 3.14f, "Value should be 3.14f");

    TestVariant v4(std::string("hello"));
    AssertTrue(v4.Holds<std::string>(), "Variant constructed with string should hold string");
    AssertTrue(*v4.GetIf<std::string>() == "hello", "Value should be 'hello'");

    // Test copy assignment
    TestVariant v5;
    v5 = v2;
    AssertTrue(v5.Holds<int>(), "Copy assigned variant should hold int");
    AssertTrue(*v5.GetIf<int>() == 42, "Copied value should be 42");

    // Test move assignment
    TestVariant v6;
    v6 = std::move(v4);
    AssertTrue(v6.Holds<std::string>(), "Move assigned variant should hold string");
    AssertTrue(*v6.GetIf<std::string>() == "hello", "Moved value should be 'hello'");

    // Test assignment with different type
    v5 = 2.71f;
    AssertTrue(v5.Holds<float>(), "Assigned float should hold float");
    AssertTrue(*v5.GetIf<float>() == 2.71f, "Assigned value should be 2.71f");

    // Test Emplace
    TestVariant v7;
    std::string& str = v7.Emplace<std::string>("world");
    AssertTrue(v7.Holds<std::string>(), "Emplaced variant should hold string");
    AssertTrue(str == "world", "Emplaced value should be 'world'");

    // Test Reset
    v7.Reset();
    AssertFalse(v7.HasValue(), "Reset variant should not have value");

    // Test Visit
    TestVariant v8(100);
    int visitResult = v8.Visit(
		TestVisitor {},
        []() { return 0; }
    );
    AssertTrue(visitResult == 200, "Visit should execute lambda with int value");

    TestVariant v9;
    v9.Reset(); // No value
    int fallbackResult = v9.Visit(
        [](auto) { return 0; },
        []() { return 42; }
    );
    AssertTrue(fallbackResult == 42, "Visit on empty variant should call fallback");

    // Test GetIf on wrong type
    AssertTrue(v2.GetIf<float>() == nullptr, "GetIf on wrong type should return nullptr");

    return true;
}
