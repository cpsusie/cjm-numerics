// Copyright © 2020-2021 CJM Screws, LLC
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// CJM Screws, LLC is a Maryland Limited Liability Company.
// No copyright claimed to unmodified original work of others.
// The original, unmodified work of others, to the extent included in this library,
// is licensed to you under the same terms under which it was licensed to CJM Screws, LLC.
// For information about copyright and licensing of the original work of others,
// see Notices file in cjm/ folder.
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
