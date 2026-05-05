#include "TestFramework.hpp"


int main(int Argc, const char** Argv)
{
	if (Argc == 1)
	{
		int FailedCount = 0;
		for (const std::string& TestName : TTestFramework::Get().GatherTest())
		{
			bool result = TTestFramework::Get().RunTest(TestName.c_str());
			if (!result) { ++FailedCount; }
		}
		return FailedCount;
	}

	return !TTestFramework::Get().RunTest(Argv[1]) ? 1 : 0;
}
