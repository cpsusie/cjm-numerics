#include "testing.hpp"
#include <sstream>

void cjm::testing::cjm_deny(bool predicateExpression)
{
	if (predicateExpression)
	{
		throw testing_failure("The specified condition is true.");
	}
}

void cjm::testing::cjm_assert(bool predicateExpression)
{
	if (!predicateExpression)
	{
		throw testing_failure("The specified condition was not met.");
	}
}

void cjm::testing::cjm_assert_throws(const std::function<void()>& func)
{
	if (!func)
	{
		throw testing_failure("The specified delegate is no good.");
	}
	bool itThrew;
	try
	{
		func();
		itThrew = false;
	}
	catch (const std::exception&)
	{
		itThrew = true;
	}
	catch (...)
	{
		itThrew = true;
	}
	if (!itThrew)
		throw testing_failure("The specified routine did not throw.");
}

void cjm::testing::cjm_assert_nothrow(const std::function<void()>& func)
{
	if (!func)
	{
		throw testing_failure("The specified delegate is no good.");
	}
	try
	{
		func();
	}
	catch (const std::exception& ex)
	{
		std::stringstream ss;
		ss << "The specified delegate threw an exception with message: \"" << ex.what() << "\".";
		throw testing_failure(ss.str());
	}
	catch (...)
	{
		throw testing_failure("The delegate threw a non-standard exception.");
	}
	
}
