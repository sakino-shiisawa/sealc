#include "TestFramework.hpp"

// including target api
#include "Core/Array.hpp"

IMPLEMENT_TEST_CLASS(Array)
bool ArrayTest::RunTest() const
{
    // Test default constructor
    TArray<int> arr1;
    AssertTrue(arr1.Size() == 0 && arr1.IsEmpty(), "Default constructor");

    // Test initializer_list constructor
    TArray<int> arr2 = {1, 2, 3};
    AssertTrue(arr2.Size() == 3 && arr2[0] == 1 && arr2[1] == 2 && arr2[2] == 3, "Initializer list constructor");

    // Test PushBack
    arr1.PushBack(10);
    arr1.PushBack(20);
    AssertTrue(arr1.Size() == 2 && arr1[0] == 10 && arr1[1] == 20, "PushBack");

    // Test EmplaceBack
    arr1.EmplaceBack(30);
    AssertTrue(arr1.Size() == 3 && arr1[2] == 30, "EmplaceBack");

    // Test Reserve and Capacity
    arr1.Reserve(10);
    AssertTrue(arr1.Capacity() >= 10, "Reserve");

    // Test Resize
    arr1.Resize(5);
    AssertTrue(arr1.Size() == 5, "Resize");

    // Test ShrinkToFit
    arr1.ShrinkToFit();
    AssertTrue(arr1.Capacity() == arr1.Size(), "ShrinkToFit");

    // Test Clear
    arr1.Clear();
    AssertTrue(arr1.Size() == 0 && arr1.IsEmpty(), "Clear");

    // Test iterators
    TArray<int> arr3 = {4, 5, 6};
    auto it = arr3.begin();
    AssertTrue(*it == 4, "Iterator begin");
    ++it;
    AssertTrue(*it == 5, "Iterator increment");
    --it;
    AssertTrue(*it == 4, "Iterator decrement");
    AssertTrue(it != arr3.end(), "Iterator not end");

    // Test copy constructor
    TArray<int> arr4 = arr3;
    AssertTrue(arr4.Size() == arr3.Size() && arr4[0] == arr3[0], "Copy constructor");

    return true;
}