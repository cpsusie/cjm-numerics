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
#ifndef CJM_UINT128_TEST_PROGRAM_HPP_
#define CJM_UINT128_TEST_PROGRAM_HPP_
#include <concepts>
#include <iostream>
#include <iomanip>
#include <string_view>
#include <limits>
#include <cstdint>
#include <cjm/numerics/numerics.hpp>
#include <cjm/numerics/uint128.hpp>
#include "int128_tests.hpp"
#include <boost/io/ios_state.hpp>
#include <cjm/numerics/cjm_numeric_concepts.hpp>
#include <exception>
#include <stdexcept>
#include <cjm/string/cjm_string.hpp>
#include <fstream>
#include <locale>
#include <tuple>
#include "int128_test_switches.hpp"
#include <cjm/numerics/fixed_uint_container.hpp>

namespace cjm::base_test_program
{
	using cout_saver = cjm::uint128_tests::cout_saver;
	constexpr auto newl = '\n';
	template<typename T>
	concept unsigned_integer = std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed;

	template<unsigned_integer TUInt>
	void print_spec(TUInt val)
	{
		cout_saver{ std::cout };
		std::cout << std::hex << "[0x" << val << "]." << newl;
	}

	template<typename T>
	void print_spec(T val)
	{
		auto saver = cout_saver{ std::cout };
		std::cout << "Thing that isn't unsigned integer: [" << std::dec << val << "]." << newl;
	}

	int execute_test_program(int argc, char* argv[]) noexcept;

	void native_test();

	void log_arg_processing(int argc, char* argv[]);
	
	int print_args(int argc, char* argv[]) noexcept;
	
	int execute_log_arg_processing(int argc, char* argv[]) noexcept;
}
#endif //CJM_UINT128_TEST_PROGRAM_HPP_
