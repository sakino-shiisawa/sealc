#include "TestFramework.hpp"
#include "Core/Function.hpp"

IMPLEMENT_TEST_CLASS(Function)
bool FunctionTest::RunTest() const
{
    // Test default constructor
    TFunction<void()> func1;
    AssertTrue(!func1.IsValid(), "Default constructed function should not be valid");

    // Test nullptr assignment
    TFunction<int()> func2 = nullptr;
    AssertTrue(!func2.IsValid(), "Nullptr assigned function should not be valid");

    // Test lambda with void return
    auto lambdaVoid = []() { /* do nothing */ };
    TFunction<void()> func3 = lambdaVoid;
    AssertTrue(func3.IsValid(), "Lambda assigned function should be valid");
    func3(); // Call it
    AssertTrue(true, "Void lambda call succeeded");

    // Test lambda with int return
    auto lambdaInt = []() { return 42; };
    TFunction<int()> func4 = lambdaInt;
    AssertTrue(func4.IsValid(), "Int lambda assigned function should be valid");
    int result = func4();
    AssertTrue(result == 42, "Int lambda returned correct value");

    // Test copy constructor
    TFunction<int()> func5 = func4;
    AssertTrue(func5.IsValid(), "Copied function should be valid");
    int result2 = func5();
    AssertTrue(result2 == 42, "Copied function returned correct value");

    // Test move constructor
    TFunction<int()> func6 = std::move(func5);
    AssertTrue(func6.IsValid(), "Moved function should be valid");
    AssertTrue(!func5.IsValid(), "Moved from function should not be valid");
    int result3 = func6();
    AssertTrue(result3 == 42, "Moved function returned correct value");

    // Test reset
    func6.Reset();
    AssertTrue(!func6.IsValid(), "Reset function should not be valid");

    // Test function with parameters
    auto lambdaParam = [](int x) { return x * 2; };
    TFunction<int(int)> func7 = lambdaParam;
    AssertTrue(func7.IsValid(), "Parameterized lambda assigned function should be valid");
    int result4 = func7(5);
    AssertTrue(result4 == 10, "Parameterized lambda returned correct value");

    // Test large lambda (should not inline)
    auto largeLambda = [a = std::string(100, 'x')]() { return a.size(); };
    TFunction<size_t()> func8 = largeLambda;
    AssertTrue(func8.IsValid(), "Large lambda assigned function should be valid");
    size_t result5 = func8();
    AssertTrue(result5 == 100, "Large lambda returned correct value");

    return true;
}