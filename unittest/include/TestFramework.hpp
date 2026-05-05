#pragma once


#include <source_location>
#include <iostream>
#include <list>
#include <unordered_map>
#include <mutex>

#if defined(NDEBUG)
#undef NDEBUG
#endif

#if defined(ASSERT)
#undef ASSERT
#endif

#define IN_TEST
#define ASSERT(COND, MESSAGE) TTestFramework::Get().TargetTest->AssertTrue(bool(COND), MESSAGE)


class TTestBaseClass
{
public:
	TTestBaseClass() : MyIsSuccess(true) {}
	virtual ~TTestBaseClass() = default;

	virtual std::string GetTestName() const noexcept = 0;
	virtual bool RunTest() const = 0;


	void AssertTrue(
		bool InCondition,
		std::string_view InMessage,
		std::source_location InCodeLocation = std::source_location::current()
	) const
	{
		if (!InCondition)
		{
			std::cout
				<< "[Line: " << InCodeLocation.line() << "] "
				<< InMessage << " "
				<< "[File: " << InCodeLocation.file_name() << "]"
				<< std::endl;
			MyIsSuccess = false;
		}
	}
	void AssertFalse(
		bool InCondition,
		std::string_view InMessage,
		std::source_location InCodeLocation = std::source_location::current()
	) const
	{
		if (InCondition)
		{
			std::cout
				<< "[Line: " << InCodeLocation.line() << "] "
				<< InMessage << " "
				<< "[File: " << InCodeLocation.file_name() << "]"
				<< std::endl;
			MyIsSuccess = false;
		}
	}

protected:
	mutable bool MyIsSuccess;

	friend class TTestFramework;
};

class TTestFramework
{
public:
	TTestFramework() = default;
	~TTestFramework() = default;

	static TTestFramework& Get()
	{
		return Instance;
	}
	bool RunTest(const char* InTestName) const
	{
		auto Itr = TestList.find(InTestName);
		if (Itr == TestList.end())
		{
			std::cout << InTestName << " is not found" << std::endl;
			return false;
		}
		std::cout << ">>> " << InTestName << " <<<" << std::endl;
		TargetTest = Itr->second;
		if (!Itr->second->RunTest() || !Itr->second->MyIsSuccess)
		{
			std::cout << "Test " << InTestName << " is failed" << std::endl;
			TargetTest = nullptr;
			return false;
		}
		TargetTest = nullptr;
		return true;
	}

	std::list<std::string> GatherTest() const
	{
		std::list<std::string> TestNameList;
		for (const auto& Test : TestList)
		{
			TestNameList.push_back(Test.first);
		}
		return TestNameList;
	}
	void AddTestClass(TTestBaseClass* InClassPtr)
	{
		if (InClassPtr)
		{
			std::lock_guard<std::mutex> LockScope(TestListLocker);
			TestList.insert(std::pair<std::string, TTestBaseClass*>(InClassPtr->GetTestName(), InClassPtr));
		}
	}

	static TTestFramework Instance;
	std::unordered_map<std::string, TTestBaseClass*> TestList;
	std::mutex TestListLocker;
	mutable TTestBaseClass* TargetTest;
};

#define IMPLEMENT_TEST_CLASS(TestClassName) class TestClassName##Test : public TTestBaseClass { \
public: \
	~TestClassName##Test() = default; \
	TestClassName##Test() = default; \
	TestClassName##Test(nullptr_t) { TTestFramework::Get().AddTestClass(new TestClassName##Test()); } \
	std::string GetTestName() const noexcept override { return #TestClassName; } \
	bool RunTest() const override; \
}; TestClassName##Test TestClassName##Test##Instance(nullptr);

