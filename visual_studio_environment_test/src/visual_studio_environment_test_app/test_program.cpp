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
#include "test_program.hpp"

int cjm::clang::test_program::execute_test_program() noexcept
{
	int ok = execute_subroutine_log_exception(test_program, "test_program"sv, -1);
	
	if (ok == 0) 
		ok = execute_subroutine_log_exception(intrinsics_test::print_intrinsic_availability_info, "print_intrinsic_availability_info"sv);
	return ok;	
}

void cjm::clang::test_program::test_program()
{
	auto bool_to_sign = [](bool s) -> std::string_view {return s ? "signed"sv : "unsigned"sv; };
	using std::cout;
	
	cout << "Is microsoft?: [" << std::boolalpha << environment::is_microsoft << "]." << newl;
	cout << "Is x64?: [" << environment::is_x64 << "]." << newl;
	cout << "Is x64 windows?: [" << environment::is_windows_x64 << "]." << newl;
	cout << "Is clang?: [" << environment::is_clang << "]." << newl;
	cout << "Is intel classic?: [" << environment::is_intel_classic << "]." << newl;
	cout << "Is intel?: [" << environment::is_intel_llvm_compiler << "]." << newl;
	cout << "Is GCC?: [" << environment::is_gcc << "]." << newl;
	cout << newl;
	
	cout << "CHAR_BIT: [" << CHAR_BIT << "]." << newl;
	cout << "Sizeof(bool): [" << sizeof(bool) << "]." << newl;
	cout << "Sizeof(wchar_t): [" << sizeof(wchar_t) << "]." << newl;
	cout << "Sizeof(unsigned short): [" << sizeof(unsigned short) << "]." << newl;
	cout << "Sizeof(unsigned int): [" << sizeof(unsigned int) << "]." << newl;
	cout << "Sizeof(unsigned long): [" << sizeof(unsigned long) << "]." << newl;
	cout << "Sizeof(unsigned long long): [" << sizeof(unsigned long long) << "]." << newl;
	cout << "Sizeof(std::uintmax_t): [" << sizeof(std::uintmax_t) << "]." << newl;
	cout << "Sizeof(std::size_t): [" << sizeof(std::size_t) << "]." << newl;
	cout << "Sizeof(std::uintptr_t): [" << sizeof(std::uintptr_t) << "]." << newl;

	cout << newl;
	
	cout << "Sign of char: [" << bool_to_sign(std::numeric_limits<char>::is_signed) << "]." << newl;
	cout << "Sign of wchar_t: [" << bool_to_sign(std::numeric_limits<wchar_t>::is_signed) << "]." << newl;
		
	cout << newl;

	cout << "Has concepts?: [" << environment::has_concepts << "]." << newl;
	cout << "Has char8_t?: [" << environment::has_char8_t << "]." << newl;
	cout << "Has consteval?: [" << environment::has_consteval << "]." << newl;
	cout << "Has library bit-ops?: [" << environment::has_lib_bitops << "]." << newl;
	cout << "Has library char8_t?: [" << environment::has_lib_char8_t << "]." << newl;
	cout << "Has library concepts?: [" << environment::has_lib_concepts << "]." << newl;

	cout << newl;
	
	cout << "Has library endian?: [" << environment::has_lib_endian << "]." << newl;
	cout << "Has library constant evaluated?: [" << environment::has_lib_constant_evaluated << "]." << newl;
	cout << "Has library nothrow convertible?: [" << environment::has_lib_nothrow_convertible << "]." << newl;
	cout << "Has library remove_cvref?: [" << environment::has_lib_remove_cvref << "]." << newl;
	cout << "Has library span?: [" << environment::has_lib_span << "]." << newl;

	cout << newl;
	
	cout << "Has three-way comparison?: [" << environment::has_three_way_comparison << "]." << newl;
	cout << "Has library bit_cast?: [" << environment::has_lib_bit_cast << "]." << newl;
	cout << "Has intrinsic int128?: [" << environment::has_intrinsic_u128 << "]." << newl;

	if constexpr (environment::has_intrinsic_u128)
	{
		cout << "Is uint128_t an arithmetic type?: [" << environment::is_intrin_u128_arithmetic << "]." << newl;
		cout << "uint128_t size: [" << sizeof(environment::uint128_t) << "]." << newl;
		cout << "uint128_t align: [" << alignof(environment::uint128_t) << "]." << newl;
		constexpr bool is_uint128_t_specialized = std::numeric_limits<environment::uint128_t>::is_specialized;
		if constexpr (is_uint128_t_specialized)
		{
			cout << "Is numeric_limits specialized for uint128_t?: [YES]." << newl;
			cout << "uint128_t digit count: [" << std::numeric_limits<environment::uint128_t>::digits << "]." << newl;
		}
		else
		{
			cout << "Is numeric_limits specialized for uint128_t?: [NO]." << newl;
		}
		using environment::uint128_t;
#ifdef __SIZEOF_INT128__
		constexpr std::uint64_t high = 0xc0de'd00d'fea2'cafe;
		constexpr std::uint64_t low = 0xb00b'600d'f00d'bad0;
		constexpr uint128_t combined = (uint128_t{ high } << 64) | (uint128_t{low});
		constexpr uint128_t result_1 = combined - static_cast<uint128_t>(low);
		constexpr uint128_t result_2 = result_1 >> 64;
		static_assert(static_cast<std::uint64_t>(result_2) == high);
		auto division_result = result_1 / result_2;
		std::uint64_t low_division_result = static_cast<std::uint64_t>(division_result);
		std::uint64_t high_division_result = static_cast<std::uint64_t>(division_result >> 64);
		cout << "Division result: [0x" << std::hex << high_division_result << std::setw(16) << std::setfill('0') << low_division_result << "]." << newl;
		cout << std::dec;
#endif
	}
	
}

