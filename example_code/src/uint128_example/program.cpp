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
#include <cjm/numerics/uint128.hpp>
#include <cjm/numerics/cjm_numeric_concepts.hpp>
#include <array>
#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>
#include <sstream>
#include <sstream>
#include <limits>
#include <type_traits>
#include <climits>
#include <cstdint>
#include <utility>
#include <optional>
#include <exception>
#include <stdexcept>
#include <optional>
#include <concepts>
#include <compare>
#include <cmath>
#include <typeinfo>


//The purpose of this EXAMPLE_CODE is to demonstrate the functionality of the CJM uint128 type,
//show how it works, and talk somewhat about its strategies on various systems. It does NOT attempt
//to prove correctness.  Proof of correctness is available through the uint128_test_app
//which contains extensive unit tests.  THIS application, unlike uint128_test_app
//has NO dependencies other than a C++20 compiler that conforms to a subset of the C++20
//standard and a C++20 standard library implementation that does the same.  Please
// consult this project's repository to learn what subset that is.  Briefly, on Windows tested
// 32 and 64 bit Microsoft 19.28+, on Linux - GCC 10.2.0+ and Linux Clang 11.0.0-2+ are known to work in
// x64 system.  x86 has not been tested.  Hopefully, the dependency free nature of this header-only library
// and example code application will be convenient for learning about the library.

//NOTE: Intel Classic compiler is not currently supported.  Support will be added when/if Intel Classic
//adds sufficient support for C++20.

//By contrast, the UNIT TEST APPLICATION carries with it dependencies including:
	//boost (https://github.com/boostorg)
	//abseil[cxx17] (https://github.com/abseil/abseil-cpp)
	//date (https://github.com/HowardHinnant/date)
//All of the foregoing are available as vcpkg ports and were easily integrated for use in unit tests as follows:
	//x64-windows-static (visual studio, microsoft compiler, 64-bit Intel/Amd (x86-64) static linking of dependencies, including CRT)
	//x86-windows-static (visual studio, microsoft compiler, 32-bit Intel/Amd (x86), static linking of dependencies, including CRT)
	//x64-linux (g++ 10.2.0 and clang 11.0.0.2 using libstdc++ on Kubuntu 20.10, default linking, using CLion as development environment)

//Building and executing the example code project requires no extra dependencies.

namespace cjm::uint128::example_code
{
	using uint128_t = numerics::uint128;
	using divmod_result_t = uint128_t ::divmod_result_t;
	using namespace numerics::uint128_literals;
	using namespace std::string_literals;
	using namespace std::string_view_literals;
	using std::cout;
	constexpr auto newl = '\n';

	template<cjm::numerics::concepts::character Char>
	std::basic_stringstream<Char, std::char_traits<Char>, std::allocator<Char>> make_throwing_sstream();

	template<cjm::numerics::concepts::unsigned_integer Ui, cjm::numerics::concepts::character Char = char>
	std::basic_string<Char> print_max_digits10();


	void demonstrate_subtraction();
	void demonstrate_constexpr_subtraction();
	void demonstrate_addition();
	void demonstrate_constexpr_addition();
	void demonstrate_multiplication();
	void demonstrate_constexpr_multiplication();
	void demonstrate_binary_bitwise_operations();
	void demonstrate_bitshift_operations();
	void demonstrate_unary_operations();
	void say_hello();
	void say_goodbye();
	void demonstrate_runtime_division_and_modulus();
	void demonstrate_nonthrowing_runtime_division_and_modulus();
	void demonstrate_constexpr_division_and_modulus();
	void demonstrate_conversions_to_from_signed_integral();
	void demonstrate_compare_and_hash();
	void demonstrate_conversions_to_from_unsigned_integral();
	void demonstrate_conversions_to_from_floating_points();
	void demonstrate_literals();
	void demonstrate_stream_insertion_and_extraction();

	//utility / helpers
	void print_compare_results(std::string left_operand, std::string right_operand,
		std::strong_ordering ordering, bool greater_than, bool less_than, bool equal, 
		bool not_equal, bool greater_or_equal, bool less_or_equal, 
		size_t left_hash, size_t right_hash, bool runtime);
	constexpr std::pair<uint128_t, uint128_t> demo_constexpr_pre_increment(uint128_t pre_inc_me) noexcept;
	constexpr std::pair<uint128_t, uint128_t> demo_constexpr_pre_decrement(uint128_t pre_dec_me) noexcept;
	constexpr std::pair<uint128_t, uint128_t> demo_constexpr_post_increment(uint128_t post_inc_me) noexcept;
	constexpr std::pair<uint128_t, uint128_t> demo_constexpr_post_decrement(uint128_t post_dec_me) noexcept;
	void print_optional_divmod_result(const std::optional<divmod_result_t>& print_me);
	std::string_view ordering_text(std::strong_ordering ordering);

	//supporting types
	struct alignas(uint128_t) just_like_uint128_t final
	{
		std::uint64_t m_low{};
		std::uint64_t m_high{};

		constexpr just_like_uint128_t() noexcept = default;
		constexpr just_like_uint128_t(const just_like_uint128_t& other) noexcept = default;
		constexpr just_like_uint128_t(just_like_uint128_t&& other) noexcept = default;
		constexpr just_like_uint128_t& operator=(const just_like_uint128_t& other) noexcept = default;
		constexpr just_like_uint128_t& operator=(just_like_uint128_t&& other) noexcept = default;
		constexpr ~just_like_uint128_t() = default;
	};
	static_assert(std::is_default_constructible_v<just_like_uint128_t>&& std::is_trivially_copyable_v<just_like_uint128_t>, "To be bit-castable, gotta be at least this.");

	consteval std::uint64_t increment(std::uint64_t val)
	{
		return val + 1;
	}

	template<cjm::numerics::concepts::character Char>
	std::basic_stringstream<Char, std::char_traits<Char>, std::allocator<Char>> make_throwing_sstream()
	{
		auto ret = std::basic_stringstream<Char>{};
		ret.exceptions(std::ios::failbit | std::ios::badbit);
		return ret;
	}
	template<cjm::numerics::concepts::unsigned_integer Ui, cjm::numerics::concepts::character Char>
	std::basic_string<Char> print_max_digits10()
	{
		auto strm = make_throwing_sstream<Char>();
		constexpr size_t digits_ten = std::numeric_limits<Ui>::digits10;
		strm
			<< std::dec << "Digits10 for type [" << typeid(Ui).name() << "] is [" << digits_ten
			<< "], meaning that its max value, in decimal, will have [" << digits_ten + 1
			<< "] digits." << newl;
		strm << "Maximum value for type [" << typeid(Ui).name() << "], expressed in decimal, is: [";
		for (char c : cjm::numerics::uint128_literals::lit_helper::max_decimal_digits_v<Ui>)
		{
			strm << c;
		}
		strm << "]." << newl;
		return strm.str();
	}
}

int main()
{
	using namespace cjm::uint128::example_code;
	std::ios::sync_with_stdio(false);
	try
	{
		say_hello();
		demonstrate_addition();
		demonstrate_constexpr_addition();
		demonstrate_subtraction();
		demonstrate_constexpr_subtraction();
		demonstrate_multiplication();
		demonstrate_constexpr_multiplication();
		demonstrate_runtime_division_and_modulus();
		demonstrate_nonthrowing_runtime_division_and_modulus();
		demonstrate_constexpr_division_and_modulus();
		demonstrate_binary_bitwise_operations();
		demonstrate_bitshift_operations();
		demonstrate_unary_operations();
		demonstrate_compare_and_hash();
		demonstrate_conversions_to_from_signed_integral();
		demonstrate_conversions_to_from_unsigned_integral();
		demonstrate_conversions_to_from_floating_points();
		demonstrate_literals();
		demonstrate_stream_insertion_and_extraction();
		say_goodbye();		
	}
	catch (const std::exception& ex)
	{
		try
		{
			std::cerr << "example_code program for uint128 threw unexpected exception with message: [" << ex.what() << "]." << newl;
		}
		catch (...)
		{
			//eat it
		}
		return -1;
	}
	catch (...)
	{
		try
		{
			std::cerr << "example_code program for uint128 threw unexpected non-standard exception." << newl;
		}
		catch (...)
		{
			//eat it
		}
		return -1;
	}
	return 0;
}
constexpr std::pair<cjm::uint128::example_code::uint128_t, cjm::uint128::example_code::uint128_t>
cjm::uint128::example_code::demo_constexpr_pre_increment(cjm::uint128::example_code::uint128_t pre_inc_me) noexcept
{
	auto evaluated = ++pre_inc_me;
	return std::make_pair(evaluated, pre_inc_me);
}

constexpr std::pair<cjm::uint128::example_code::uint128_t, cjm::uint128::example_code::uint128_t>
cjm::uint128::example_code::demo_constexpr_pre_decrement(cjm::uint128::example_code::uint128_t pre_dec_me) noexcept
{
	auto evaluated = --pre_dec_me;
	return std::make_pair(evaluated, pre_dec_me);
}

constexpr std::pair<cjm::uint128::example_code::uint128_t, cjm::uint128::example_code::uint128_t>
cjm::uint128::example_code::demo_constexpr_post_increment(cjm::uint128::example_code::uint128_t post_inc_me) noexcept
{
	auto evaluated = post_inc_me++;
	return std::make_pair(evaluated, post_inc_me);
}

constexpr std::pair<cjm::uint128::example_code::uint128_t, cjm::uint128::example_code::uint128_t>
cjm::uint128::example_code::demo_constexpr_post_decrement(cjm::uint128::example_code::uint128_t post_dec_me) noexcept
{
	auto evaluated = post_dec_me--;
	return std::make_pair(evaluated, post_dec_me);
}
void cjm::uint128::example_code::demonstrate_constexpr_subtraction()
{
	cout << newl << "This is the compile-time subtraction demonstration." << newl;
	constexpr auto minuend = std::numeric_limits<uint128_t>::max();
	constexpr auto subtrahend = 256'368'684'943'268'248'658'307'433'575'740'207'117_u128;
	constexpr auto difference = minuend - subtrahend;

	//subtraction with borrow takes a carry-in argument and returns a pair, the first of which is the difference and the second of which is borrow_out
	//(unsigned char) which if there was overflow will be non-zero.
	//If this were constexpr, you could not use the structured bindings to make it more convenient.
	constexpr auto sbb_res = sub_with_borrow(minuend, subtrahend, 0);
	cout
		<< "Subtracting [" << subtrahend << "] from [" << minuend << "] yields difference: [" << difference << "].  "
		<< "Subtracting with borrow (no borrow-in) yielded [" << sbb_res.first << "] as the difference and indicated that "
		<< (sbb_res.second == 0 ? "there was no borrow-out meaning the operation did not overflow/underflow."
			: "there was borrow due to overflow/underflow.") << newl;  // NOLINT(clang-diagnostic-unreachable-code) -- it's a demonstration!
}


void cjm::uint128::example_code::demonstrate_subtraction()
{
	cout << newl << "This is the runtime subtraction demonstration." << newl;
	const auto minuend = std::numeric_limits<uint128_t>::max();
	const auto subtrahend = 256'368'684'943'268'248'658'307'433'575'740'207'117_u128;
	const auto difference = minuend - subtrahend;

	//subtraction with borrow takes a carry-in argument and returns a pair, the first of which is the difference and the second of which is borrow_out
	//(unsigned char) which if there was overflow will be non-zero.
	//If this were constexpr, you could not use the structured bindings to make it more convenient.
	auto [diff_result, borrow_out] = sub_with_borrow(minuend, subtrahend, 0);
	cout
		<< "Subtracting [" << subtrahend << "] from [" << minuend << "] yields difference: [" << difference << "].  "
		<< "Subtracting with borrow (no borrow-in) yielded [" << diff_result << "] as the difference and indicated that "
		<< (borrow_out == 0 ? "there was no borrow-out meaning the operation did not overflow/underflow."
			: "there was borrow due to overflow/underflow.") << newl;
}

void cjm::uint128::example_code::demonstrate_addition()
{
	cout << newl << "This is the runtime addition demonstration." << newl;
	//123 septillion, 792 sextillion ... etc
	const auto first_addend = 123'792'321'010'356'001'641'481'971_u128;
	//888 octillion, 911 septillion ... etc
	const auto second_addend = 888'911'410'443'901'902'103'000'301'000_u128;
	//addition
	const auto sum = first_addend + second_addend;
	//addition with carry takes a carry-in argument and returns a pair, the first of which is the sum and the second of which is carry_out
	//(unsigned char) which if there was overflow will be non-zero.
	//If this were constexpr, you could not use the structured bindings to make it more convenient.
	auto [sum_result, carry_out] =   add_with_carry(first_addend, second_addend, 0);
	cout
		<< "Adding [" << first_addend << "] to [" << second_addend << "] yields sum: [" << sum << "].  "
		<< "Adding with carry yielded [" << sum_result << "] as the sum and indicated that "
		<< (carry_out == 0 ? "there was no carry-out meaning the operation did not overflow." 
			: "there was carry-out due to overflow.") << newl;
}

void cjm::uint128::example_code::demonstrate_constexpr_addition()
{
	cout << newl << "This is the compile-time addition demonstration." << newl;
	//123 septillion, 792 sextillion ... etc
	constexpr auto first_addend =	123'792'321'010'356'001'641'481'971_u128;
	//888 octillion, 911 septillion ... etc
	constexpr auto second_addend = 888'911'410'443'901'902'103'000'301'000_u128;
	//addition
	constexpr auto sum = first_addend + second_addend;
	//addition with carry takes a carry-in argument and returns a pair, the first of which is the sum and the second of which is carry_out
	//(unsigned char) which if there was overflow will be non-zero.
	//If this were not constexpr, you could, of course, use structured bindings to make it more convenient.
	constexpr auto add_with_carry_res = add_with_carry(first_addend, second_addend, 0);
	static_assert(add_with_carry_res.first == sum && add_with_carry_res.second == 0, "They really ought to match and not overflow!");
	cout
		<< "Adding [" << first_addend << "] to [" << second_addend << "] yields sum: [" << sum << "].  "
		<< "There was" << (add_with_carry_res.second == 0 ? " no overflow." : " overflow.") << newl;  // NOLINT(clang-diagnostic-unreachable-code) it's a demonstration!
}

void cjm::uint128::example_code::demonstrate_multiplication()
{
	cout << newl << "This is the runtime multiplication demonstration." << newl;
	const auto first_factor = 1'693'490'369'045_u128;
	const auto second_factor = 2'309'948'587'417'034'374'169_u128;
	auto product = first_factor * second_factor;
	cout << "Multiplying [" << first_factor << "] by [" << second_factor << "] yields [" << product << "]." << newl;	
}

void cjm::uint128::example_code::demonstrate_constexpr_multiplication()
{
	cout << newl << "This is the compile-time multiplication demonstration." << newl;
	constexpr auto first_factor = 1'693'490'369'045_u128;
	constexpr auto second_factor = 2'309'948'587'417'034'374'169_u128;
	constexpr auto product = first_factor * second_factor;
	cout << "Multiplying [" << first_factor << "] by [" << second_factor << "] yields [" << product << "]." << newl;
}

void cjm::uint128::example_code::demonstrate_binary_bitwise_operations()
{
	cout << newl << "This is the binary bitwise operations demonstration." << newl;
	
	constexpr auto left_operand =	256'368'684'943'268'248'658'307'433'575'740'207'117_u128;
	constexpr auto right_operand =	295'990'755'043'860'794'295'278'260'016'730'001'421_u128;

	auto print_result = [=](uint128_t val, bool runtime, std::string_view op_name) -> void
	{
		cout << "Performing bitwise " << op_name << " on left and right operands yields [0x" << std::hex << std::setw(std::numeric_limits<uint128_t>::digits / 4) << std::setfill('0') << val << "] at " << (runtime ? " run-time." : "compile-time.") << std::dec << newl;
	};
	
	auto runtime_or_res = left_operand | right_operand;
	auto runtime_and_res = left_operand & right_operand;
	auto runtime_xor_res = left_operand ^ right_operand;
	constexpr auto ctime_or_res = left_operand | right_operand;
	constexpr auto ctime_and_res = left_operand & right_operand;
	constexpr auto ctime_xor_res = left_operand ^ right_operand;

	print_result(runtime_or_res, true, "or"sv);
	print_result(runtime_and_res, true, "and"sv);
	print_result(runtime_xor_res, true, "xor"sv);

	print_result(ctime_or_res, false, "or"sv);
	print_result(ctime_and_res, false, "and"sv);
	print_result(ctime_xor_res, false, "xor"sv);
}

void cjm::uint128::example_code::demonstrate_bitshift_operations()
{
	cout << newl << "This is the bitshifting operations demonstration." << newl;
	
	constexpr auto shift_me = 0x5aa5'a55a'5aa5'a55a'a55a'5aa5'a55a'5aa5_u128;

	auto print_result = [=](uint128_t val, bool runtime, unsigned shift_amount, std::string_view op_name) -> void
	{
		std::cout
			<< "Shifting [0x" << std::hex << std::setw(std::numeric_limits<uint128_t>::digits / 4)
			<< std::setfill('0') << shift_me << "] " << op_name << " by " << std::dec
			<< shift_amount << " at " << (runtime ? "run-time" : "compile-time")
			<< " yields: [0x" << std::hex << std::setw(std::numeric_limits<uint128_t>::digits / 4)
			<< std::setfill('0') << val << "]." << std::dec << newl;
	};
	
	auto runtime_lshift_4 = shift_me << 4;
	constexpr auto ctime_lshift_4 = shift_me << 4;
	auto runtime_rshift_4 = shift_me >> 4;
	constexpr auto ctime_rshift_4 = shift_me >> 4;

	auto runtime_lshift_65 = shift_me << 65;
	constexpr auto ctime_lshift_65 = shift_me << 65;
	auto runtime_rshift_65 = shift_me >> 65;
	constexpr auto ctime_rshift_65 = shift_me >> 65;

	//the max shift without undefined behavior is 127 (see std::numeric_limits<uint128_t>::digits, subtract one to get max shift (here 127)
	//an attempt to shift by more than 127 or less than 0 (with a signed right hand operand) results in undefined behavior
	//for built-ins and it does for uint128_t too!
	auto runtime_lshift_127 = shift_me << 127;
	constexpr auto ctime_lshift_127 = shift_me << 127;
	auto runtime_rshift_127 = shift_me >> 127;
	constexpr auto ctime_rshift_127 = shift_me >> 127;

	//UB ALERT!!!!! (though this isn't any different from the case with built-in arithmetic types)
	//any of the following four lines would cause undefined behavior if uncommented --
	//there may be a precondition assertion failure in debug builds
	//auto illegal_lshift_1 = shift_me << -1;
	//auto illegal_rshift_1 = shift_me >> -1;
	//auto illegal_lshift_2 = shift_me << 128;
	//auto illegal_rshift_2 = shift_me >> 128;

	print_result(runtime_lshift_4, true, 4, "LEFT"sv);
	print_result(ctime_lshift_4, false, 4, "LEFT"sv);
	print_result(runtime_rshift_4, true, 4, "RIGHT"sv);
	print_result(ctime_rshift_4, false, 4, "RIGHT"sv);

	print_result(runtime_lshift_65, true, 65, "LEFT"sv);
	print_result(ctime_lshift_65, false, 65, "LEFT"sv);
	print_result(runtime_rshift_65, true, 65, "RIGHT"sv);
	print_result(ctime_rshift_65, false, 65, "RIGHT"sv);

	print_result(runtime_lshift_127, true, 127, "LEFT"sv);
	print_result(ctime_lshift_127, false, 127, "LEFT"sv);
	print_result(runtime_rshift_127, true, 127, "RIGHT"sv);
	print_result(ctime_rshift_127, false, 127, "RIGHT"sv);
	
}

void cjm::uint128::example_code::demonstrate_unary_operations()
{
	constexpr auto original_operand = 123'890'567'234'901'678'346'012'789'456'123_u128;
	//note use of numeric limits to get max value (all 1's in binary, all f's in hexadecimal)
	constexpr auto max_value_operand = std::numeric_limits<uint128_t>::max();
	//if this were a signed (2's complement) representation, the following would be the min value
	//that is in hex: 0x8000.....0000; 0r binary (a 1 followed by 127 zeroes)
	constexpr auto min_value_if_were_signed = 0x01_u128 << 127;
	cout << newl << "This is the unary operations demonstration." << newl;
	
	//BITWISE NEGATION
	//bitwise negation works at both compile time and runtime as expected ... all 0 bits flipped to 1
	//all 1 bits flipped to zero
	constexpr auto ctime_not_result = ~original_operand;
	auto rtime_not_result = ~original_operand;

	auto  print_result = [](uint128_t operand, uint128_t result, bool runtime, std::string_view op_name) -> void
	{
		cout
			<< "Applying [" << op_name << "] to [0x" << std::hex
			<< std::setw(std::numeric_limits<uint128_t>::digits / 4) << std::setfill('0')
			<< operand << "] yields [0x" << std::hex
			<< std::setw(std::numeric_limits<uint128_t>::digits / 4)
			<< std::setfill('0') << result << "] at " << std::dec
			<< (runtime ? " run-time."sv : " compile-time."sv);
		cout << newl;
	};

	//UNARY MINUS SUPPLIES 2's complement
	//typical example
	constexpr auto ctime_unmin_result_normal = -original_operand;
	auto rtime_unmin_result_normal = -original_operand;
	//example using min 2's complement value
	//note this value is itself in two's complement
	constexpr auto ctime_unmin_result_2cmin = -min_value_if_were_signed;
	auto rtime_unmin_result_2cmin = -min_value_if_were_signed;
	//example using zero
	//two's complement of zero is also zero
	constexpr auto ctime_result_2czero = -(0_u128);
	auto rtime_result_2czero = -(0_u128);
	//example using max value
	//result should be 1 
	constexpr auto ctime_result_max = -max_value_operand;
	auto rtime_result_max = -max_value_operand;
	
	
	print_result(original_operand, ctime_not_result, false, "BITWISE-NOT"sv);
	print_result(original_operand, rtime_not_result, true, "BITWISE-NOT"sv);

	print_result(original_operand, ctime_unmin_result_normal, false, "UNARY-MINUS"sv);
	print_result(original_operand, rtime_unmin_result_normal, true, "UNARY-MINUS"sv);
	print_result(min_value_if_were_signed, ctime_unmin_result_2cmin, false, "UNARY-MINUS"sv);
	print_result(min_value_if_were_signed, rtime_unmin_result_2cmin, true, "UNARY-MINUS"sv);
	print_result(0_u128, ctime_result_2czero, false, "UNARY-MINUS"sv);
	print_result(0_u128, rtime_result_2czero, true, "UNARY-MINUS"sv);
	print_result(max_value_operand, ctime_result_max, false, "UNARY-MINUS"sv);
	print_result(max_value_operand, rtime_result_max, true, "UNARY-MINUS"sv);

	//UNARY PLUS:
	//compile time or runtime, it's a no-op for uint128_t
	constexpr auto ctime_plus_zero = +0_u128;
	constexpr auto ctime_plus_max = +(std::numeric_limits<uint128_t>::max());
	uint128_t zero = 0;
	uint128_t max = std::numeric_limits<uint128_t>::max();
	auto rtime_plus_zero = +zero;
	auto rtime_plus_max = +max;
	cout << "Compile-time plus zero: [" << std::dec << ctime_plus_zero << "]." << newl;
	cout << "Compile-time plus max: [" << std::dec << ctime_plus_max << "]." << newl;
	cout << "Run-time plus zero: [" << std::dec << rtime_plus_zero << "]." << newl;
	cout << "Run-time plus max: [" << std::dec << rtime_plus_max << "]." << newl;

	//BOOLEAN CONVERSION operations
	//zero is false, anything else is true
	if constexpr (original_operand) //constexpr or runtime if ok
	{
		cout << "Conditional context implicit bool conversion of [" << std::dec <<  original_operand << "]: [" << std::boolalpha << true << "]." << newl;
	}
	else
	{
		cout << "Conditional context implicit bool conversion of [" << std::dec <<  original_operand << "]: [" << std::boolalpha
			<< false << "]." << newl;  // NOLINT(clang-diagnostic-unreachable-code) it's a demonstration!
	}
	if constexpr (std::numeric_limits<uint128_t>::min()) //constexpr or runtime if ok
	{
		cout << "Conditional context implicit bool conversion of [" << std::dec <<  std::numeric_limits<uint128_t>::min() <<  // NOLINT(clang-diagnostic-unreachable-code) demonstration!
			"]: [" << std::boolalpha << true << "]." << newl;
	}
	else
	{
		cout << "Conditional context implicit bool conversion of [" << std::dec <<  std::numeric_limits<uint128_t>::min() << "]: [" << std::boolalpha << false << "]." << newl;
	}
	//a conversion to a boolean is narrowing so must be explicit outside a boolean/conditional context
	//neither of the following two lines will compile for that reason
	//bool help_runtime = original_operand;
	//constexpr bool help_compiletime = original_operand;
	//to do the foregoing, a static cast is required:
	bool help_runtime = static_cast<bool>(original_operand);
	constexpr bool help_compiletime = static_cast<bool>(original_operand);
	cout << "Explicit cast of [" << std::dec << original_operand << "] to boolean at runtime: [" << std::boolalpha << help_runtime << "]." << newl;
	cout << "Explicit cast of [" << std::dec << original_operand << "] to boolean at compile-time: [" << std::boolalpha << help_compiletime << "]." << newl;
	//conversion TO uint128_t is possible implicitly but there is no guarantee that casting uint128_t -> bool then bool -> uint128_t
	//will preserve original value
	uint128_t runtime_roundtrip_attempt = help_runtime;
	constexpr uint128_t compiletime_roundtrip_attempt = help_compiletime;
	cout << "Result of casting [" << std::dec << original_operand << "] to boolean, then casting back to uint128_t at runtime: [" << runtime_roundtrip_attempt << "]." << newl;
	cout << "Result of casting [" << std::dec << original_operand << "] to boolean, then casting back to uint128_t at compile-time: [" << compiletime_roundtrip_attempt << "]." << newl;

	//operator ! converts to boolean then negates
	//you do not need to static_cast because the operator ! removes implicit nature of cast
	bool runtime_neg_result = !original_operand;
	constexpr bool ctime_neg_result = !original_operand;
	uint128_t runtine_neg_rtattempt = !runtime_neg_result;
	constexpr uint128_t ctime_neg_rtattempt = !ctime_neg_result;
	cout << "Result of logical negating [" << std::dec << original_operand << "] at runtime: [" << std::boolalpha << runtime_neg_result << "]." << newl;
	cout << "Result of logical negating [" << std::dec << original_operand << "] at compile-time: [" << std::boolalpha << ctime_neg_result << "]." << newl;
	cout << "Attempted roundtrip of negated value back to uint128_t at runtime: [" << std::dec << runtine_neg_rtattempt << "]." << newl;
	cout << "Attempted roundtrip of negated value back to uint128_t at compile-time: [" << std::dec << ctime_neg_rtattempt << "]." << newl;
	uint128_t help = !original_operand;
	uint128_t and_back = !help;
	cout << "Don't be fooled into thinking any numeric value (besides 0 or 1) can survive a cast to bool (via cast or with negation via !): [" << help << "] or any kind of roundtrip: [" << and_back << "]." << newl;

	//PRE AND POST INCREMENT / DECREMENT (runtime)
	//cannot apply an operator with side effect directly to constexpr value
	auto orig_operand_copy_0 = original_operand;
	auto orig_operand_copy_1 = original_operand;
	auto orig_operand_copy_2 = original_operand;
	auto orig_operand_copy_3 = original_operand;
	//works just as you'd expect (well, I hope you'd expect these work the way they do anyways....)
	std::cout << "Pre-incremented original operand: [" << std::dec << ++orig_operand_copy_0 << "]." << newl;
	std::cout << "After application of pre-increment: [" << std::dec << orig_operand_copy_0 << "]." << newl;
	std::cout << "Post-incremented original operand: [" << std::dec << orig_operand_copy_1++ << "]." << newl;
	std::cout << "After application of post-increment: [" << std::dec << orig_operand_copy_1 << "]." << newl;
	std::cout << "Pre-decremented original operand: [" << std::dec << --orig_operand_copy_2 << "]." << newl;
	std::cout << "After application of pre-decrement: [" << std::dec << orig_operand_copy_2 << "]." << newl;
	std::cout << "Post-decremented original operand: [" << std::dec << orig_operand_copy_3-- << "]." << newl;
	std::cout << "After application of post-decrement: [" << std::dec << orig_operand_copy_3 << "]." << newl;

	//PRE AND POST INCREMENT / DECREMENT (compile-time)
	//Although you cannot apply these operations to a constexpr VARIABLE
	//because they cause mutation to the original, pre and post increment can
	//be used in constexpr FUNCTIONS AND OPERATORS without voiding their constexprness.
	//This same state of affairs holds with compound assignment operators ... the
	//mutating effect means they cannot be used with constexpr variables, but they can certainly
	//be used in constexpr functions without voiding the constexprness thereof.
	//Note that the return value pair's first value is an evaluation of the operator applied to the variable and the second is the version after that application

	constexpr auto ctime_pre_inc_res = demo_constexpr_pre_increment(original_operand);
	constexpr auto ctime_post_inc_res = demo_constexpr_post_increment(original_operand);
	constexpr auto ctime_pre_dec_res = demo_constexpr_pre_decrement(original_operand);
	constexpr auto ctime_post_dec_res = demo_constexpr_post_decrement(original_operand);
	std::cout << "COMPILE TIME Pre-incremented original operand: [" << std::dec << ctime_pre_inc_res.first << "]." << newl;
	std::cout << "COMPILE TIME After application of pre-increment: [" << std::dec << ctime_pre_inc_res.second << "]." << newl;
	std::cout << "COMPILE TIME Post-incremented original operand: [" << std::dec << ctime_post_inc_res.first << "]." << newl;
	std::cout << "COMPILE TIME After application of post-increment: [" << std::dec << ctime_post_inc_res.second << "]." << newl;
	std::cout << "COMPILE TIME Pre-decremented original operand: [" << std::dec << ctime_pre_dec_res.first << "]." << newl;
	std::cout << "COMPILE TIME After application of pre-decrement: [" << std::dec << ctime_pre_dec_res.second << "]." << newl;
	std::cout << "COMPILE TIME Post-decremented original operand: [" << std::dec << ctime_post_dec_res.first << "]." << newl;
	std::cout << "COMPILE TIME After application of post-decrement: [" << std::dec << ctime_post_dec_res.second << "]." << newl;


}

void cjm::uint128::example_code::say_hello()
{
	using namespace numerics;
	cout << "Hello, welcome to the demonstration of CJM's uint128 type!" << newl << newl;
	cout << "Detected compiler: [" << get_text_narrow(compiler) << "]." << newl;
	cout << "Calculation mode: [";
	if constexpr (calculation_mode == uint128_calc_mode::intrinsic_u128)
	{
		cout << "IntrinsicU128]." << newl;
	}
	else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64_clang_or_intel_llvm)
	{
		cout << "msvc_x64_clang_or_llvm_intel]." << newl;
	}
	else if constexpr (calculation_mode == uint128_calc_mode::msvc_x64)
	{
		cout << "msvc_x64]." << newl;
	}
	else
	{
		cout << "fallback]." << newl;
	}
	cout << "Intel Adx Available: [" << std::boolalpha << intel_adx_available << "]." << newl;
	cout << "Intel BMI2 Available: [" << std::boolalpha << intel_bmi2_available << "]." << newl;
	cout << "Sse3 available: [" << std::boolalpha << sse3_available << "]." << newl;
	cout << "Consteval available: [" << std::boolalpha << cjm::numerics::internal::has_consteval << "]." << newl;
}

void cjm::uint128::example_code::say_goodbye()
{
	cout << newl << newl << "The demonstration of CJM's uint128 type has completed.  We hope you found it useful." << std::endl;
}

void cjm::uint128::example_code::demonstrate_runtime_division_and_modulus()
{
	cout << newl << "This is the simple runtime division demonstration." << newl;
	constexpr auto dividend = 256'368'684'943'268'121'395'391'016'720'575'361'037_u128;
	constexpr auto divisor = 3'135'107'085_u128;
	//note that numeric limits is specialized appropriately.
	constexpr auto zero = std::numeric_limits<uint128_t>::min();
	uint128_t quotient = dividend / divisor;
	uint128_t remainder = dividend % divisor;

	//division and modulus can also be done together.
	auto&& [quotient_2, remainder_2] = uint128_t::div_mod(dividend, divisor);

	cout << "[" << dividend << "] / [" << divisor << "] == [" << quotient << "] with a remainder of [" << remainder << "]." << newl;
	cout << "Obtained via divmod function: == [" << quotient_2 << "] with a remainder of [" << remainder_2 << "]." << newl;

	//for the characteristics of divmod_result_t, consult demonstrate_constexpr_division_and_modulus, or the header
	//at <cjm/numerics/cjm_numeric_concepts.hpp>

	//Division and modulus are somewhat unique because they will throw std::domain_error on a divisor of 0
	//rather than exhibiting undefined behavior.  Most other operations (such as shift by >= number of binary digits)
	//exhibit undefined behavior to avoid the expense of input validation.  Operations like shift etc however
	//are very fast operations and I did not want to introduce an additional branch.  Division, however,
	//is a slow operation already and one doubts that a precondition check is going to add much if anything to it.

	//Example -- just division:
	std::optional<uint128_t> res;
	std::optional<uint128_t::divmod_result_t> divmod_res;
	try
	{
		res = dividend / zero;
		auto strm = std::stringstream{};
		strm << "The foregoing was supposed to throw an exception but instead yielded a quotient of [" << res.value() << "]." << newl;
		throw std::logic_error{strm.str()};
	}
	catch (const std::domain_error& ex)
	{
		std::cout << "This is correct behavior. Division and modulus operators and the divmod function will throw on divisor == 0.  Message: [" << ex.what() << "]." << newl;
	}
	catch (const std::logic_error&)
	{
		throw;
	}
	catch (...)
	{
		throw std::logic_error{"The exception was supposed to be a domain error."};
	}

	//Example: divmod
	try
	{
		divmod_res = uint128_t::div_mod(dividend, zero);
		auto strm = std::stringstream{};
		strm << "The foregoing was supposed to throw an exception but instead yielded a quotient of [" << divmod_res.value().quotient << "] and a remainder of [" << divmod_res.value().remainder << "]." << newl;
		throw std::logic_error{strm.str()};
	}
	catch (const std::domain_error& ex)
	{
		std::cout << "This is correct behavior. Division and modulus operators and the divmod function will throw on divisor == 0.  Message: [" << ex.what() << "]." << newl;
	}
	catch (const std::logic_error&)
	{
		throw;
	}
	catch (...)
	{
		throw std::logic_error{"The exception was supposed to be a domain error."};
	}
}

void cjm::uint128::example_code::demonstrate_nonthrowing_runtime_division_and_modulus()
{

	cout << newl << "This is the nonthrowing runtime division demonstration." << newl;
	//Im sure some people will worry about the performance for checking zero so I have provided non-throwing
	//alternatives.

	constexpr auto dividend = 256'368'684'943'268'121'395'391'016'720'575'361'037_u128;
	constexpr auto divisor = 3'135'107'085_u128;
	//note that numeric limits is specialized appropriately.
	constexpr auto zero = std::numeric_limits<uint128_t>::min();

	//the following if supplied a zero divisor will exhibit undefined behavior:
	//no precondition check made
	auto&& [quotient, remainder] = uint128_t::unsafe_div_mod(dividend, divisor);
	cout << "[" << dividend << "] / [" << divisor << "] == [" << quotient << "] with a remainder of [" << remainder << "]." << newl;

	//if you would like a way to have precondition-checked division but do not like exceptions for some fool reason,
	//the following works too.
	std::optional<divmod_result_t> safe_divmod_result = uint128_t::try_div_mod(dividend, divisor);
	cout << "Here is the result -- success case try_divmod:\t";
	print_optional_divmod_result(safe_divmod_result);
	cout << newl;

	//zero divisor will yield error signaled by nullopt return value:
	safe_divmod_result = uint128_t::try_div_mod(dividend, zero);
	cout << "Here is the result -- failure case try_divmod:\t";
	print_optional_divmod_result(safe_divmod_result);
	cout << newl;
}

void cjm::uint128::example_code::print_optional_divmod_result(const std::optional<divmod_result_t>& print_me)
{
	if (print_me.has_value())
	{
		cout << "Quotient: [" << print_me->quotient << "]; Remainder: [" << print_me->remainder << "]";
	}
	else
	{
		cout << "NullOpt/ErrorResult";
	}
}

void cjm::uint128::example_code::demonstrate_constexpr_division_and_modulus()
{
	cout << newl << "This is the compile-time division demonstration." << newl;
	constexpr auto dividend = 256'368'684'943'268'121'395'391'016'720'575'361'037_u128;
	constexpr auto divisor = 3'135'107'085_u128;

	//You can use division or modulus or the compound function uint128_t::div_mod in a constexpr context.
	constexpr auto quotient = dividend / divisor;
	constexpr auto remainder = dividend % divisor;
	constexpr divmod_result_t quo_rem_2 = uint128_t::div_mod(dividend, divisor);
	constexpr divmod_result_t quo_rem_3 = uint128_t::div_mod(4_u128, 2_u128);

	cout << "[" << dividend << "] / [" << divisor << "] == [" << quotient << "] with a remainder of [" << remainder << "]." << newl;
	cout << "Obtained via divmod function: == [" << quo_rem_2.quotient << "] with a remainder of [" << quo_rem_2.remainder << "]." << newl;
	static_assert(quotient == quo_rem_2.quotient && remainder == quo_rem_2.remainder, "In a constexpr context calculation results can be used in static assertion.");
	static_assert(quo_rem_3.quotient == 2_u128 && quo_rem_3.remainder == 0_u128, "4/2 == 2, no remainder");

	//unlike with a non-constexpr context, attempting to divide by zero in a constexpr context will generate a compilation error
	//because of a lack of constant expression
	//if it were bad_quotient/bad_remainder were not constexpr, computation would be deferred to runtime and then throw an exception
	//(following two lines will not compile):
	//constexpr auto bad_quotient = dividend / 0_u128;
	//constexpr auto bad_remainder = dividend % 0_u128;

	//another constexpr division / modulus option is try_div_mod
	constexpr std::optional<divmod_result_t> divide_by_zero = uint128_t::try_div_mod(dividend, 0);
	static_assert(divide_by_zero == std::nullopt, "Divide by zero yields std::nullopt.");
	print_optional_divmod_result(divide_by_zero);
	cout << newl;

	//note that div_mod_t is constexpr hashable and constexpr totally ordered
	constexpr size_t hash_1 = std::hash<divmod_result_t>{}(quo_rem_2);
	constexpr size_t hash_2 = std::hash<divmod_result_t>{}(quo_rem_3);
	constexpr std::strong_ordering comp_res = quo_rem_2 <=> quo_rem_3;
	cout << "Constexpr hash of first divmod_result_t: [0x" << std::hex << std::setw(std::numeric_limits<size_t>::digits / 4) << std::setfill('0') << hash_1 << "]." << newl;
	cout << "Constexpr hash of second divmod_result_t: [0x" << std::hex << std::setw(std::numeric_limits<size_t>::digits / 4) << std::setfill('0') << hash_2 << "]." << newl;
	cout << "Their ordering: [" << ordering_text(comp_res) << "]." << newl;
	static_assert(cjm::numerics::concepts::nothrow_hashable<divmod_result_t> && std::totally_ordered<divmod_result_t>, "Meet nothrow hashable and totally ordered concepts.");
}

void cjm::uint128::example_code::demonstrate_compare_and_hash()
{
	cout << newl << "This is the hasher and comparison demonstration." << newl;

	auto decimal_stringify = [](uint128_t v) -> std::string
	{
		auto strm = std::stringstream{};
		strm << std::dec << v;
		return strm.str();
	};
	
	
	//default hasher for uint128_t
	constexpr auto hasher = std::hash<uint128_t>{};
	
	constexpr auto smallest = std::numeric_limits<uint128_t>::min(); //i.e. zero
	constexpr auto middle = 256'368'684'943'268'248'658'307'433'575'740'207'117_u128;
	constexpr auto biggest = std::numeric_limits<uint128_t>::max(); //i.e. a big-ass number, all f's in hex
	constexpr auto biggest_plus_one = biggest + 1_u128; //also zero

	//hashes computable at compile time
	constexpr size_t ctime_small_hash = hasher(smallest);
	constexpr size_t ctime_middle_hash = hasher(middle);
	constexpr size_t ctime_big_hash = hasher(biggest);
	constexpr size_t ctime_biggest_plus_one_hash = hasher(biggest_plus_one);

	//or runtime
	const size_t rtime_small_hash = hasher(smallest);
	const size_t rtime_middle_hash = hasher(middle);
	const size_t rtime_big_hash = hasher(biggest);
	const size_t rtime_biggest_plus_one_hash = hasher(biggest_plus_one);
	
	//compile time compare smallest to biggest:
	constexpr std::strong_ordering ctime_small_big_ordering = smallest <=> biggest;
	constexpr bool ctime_small_big_equal = smallest == biggest;
	constexpr bool ctime_small_big_not_equal = smallest != biggest;
	constexpr bool ctime_small_big_greater = smallest > biggest;
	constexpr bool ctime_small_big_greater_or_equal = smallest >= biggest;
	constexpr bool ctime_small_big_less = smallest < biggest;
	constexpr bool ctime_small_big_less_or_equal = smallest <= biggest;

	//compile time compare biggest to middle
	constexpr std::strong_ordering ctime_big_middle_ordering = biggest <=> middle;
	constexpr bool ctime_big_middle_equal = biggest == middle;
	constexpr bool ctime_big_middle_not_equal = biggest != middle;
	constexpr bool ctime_big_middle_greater = biggest > middle;
	constexpr bool ctime_big_middle_greater_or_equal = biggest >= middle;
	constexpr bool ctime_big_middle_less = biggest < middle;
	constexpr bool ctime_big_middle_less_or_equal = biggest <= middle;

	//compile time compare biggest_plus_one to smallest
	constexpr std::strong_ordering ctime_biggest_plus_one_smallest_ordering = biggest_plus_one <=> smallest;
	constexpr bool ctime_biggest_plus_one_smallest_equal = biggest_plus_one == smallest;
	constexpr bool ctime_biggest_plus_one_smallest_not_equal = biggest_plus_one != smallest;
	constexpr bool ctime_biggest_plus_one_smallest_greater = biggest_plus_one > smallest;
	constexpr bool ctime_biggest_plus_one_smallest_greater_or_equal = biggest_plus_one >= smallest;
	constexpr bool ctime_biggest_plus_one_smallest_less = biggest_plus_one < smallest;
	constexpr bool ctime_biggest_plus_one_smallest_less_or_equal = biggest_plus_one <= smallest;

	//(potentially) run time compare smallest to biggest:
	const std::strong_ordering rtime_small_big_ordering = smallest <=> biggest;
	const bool rtime_small_big_equal = smallest == biggest;
	const bool rtime_small_big_not_equal = smallest != biggest;
	const bool rtime_small_big_greater = smallest > biggest;
	const bool rtime_small_big_greater_or_equal = smallest >= biggest;
	const bool rtime_small_big_less = smallest < biggest;
	const bool rtime_small_big_less_or_equal = smallest <= biggest;

	//(potentially) run time compare biggest to middle
	const std::strong_ordering rtime_big_middle_ordering = biggest <=> middle;
	const bool rtime_big_middle_equal = biggest == middle;
	const bool rtime_big_middle_not_equal = biggest != middle;
	const bool rtime_big_middle_greater = biggest > middle;
	const bool rtime_big_middle_greater_or_equal = biggest >= middle;
	const bool rtime_big_middle_less = biggest < middle;
	const bool rtime_big_middle_less_or_equal = biggest <= middle;

	//(potentially) run time compare biggest_plus_one to smallest
	const std::strong_ordering rtime_biggest_plus_one_smallest_ordering = biggest_plus_one <=> smallest;
	const bool rtime_biggest_plus_one_smallest_equal = biggest_plus_one == smallest;
	const bool rtime_biggest_plus_one_smallest_not_equal = biggest_plus_one != smallest;
	const bool rtime_biggest_plus_one_smallest_greater = biggest_plus_one > smallest;
	const bool rtime_biggest_plus_one_smallest_greater_or_equal = biggest_plus_one >= smallest;
	const bool rtime_biggest_plus_one_smallest_less = biggest_plus_one < smallest;
	const bool rtime_biggest_plus_one_smallest_less_or_equal = biggest_plus_one <= smallest;

	//print compile time results
	print_compare_results(decimal_stringify(smallest), decimal_stringify(biggest),
		ctime_small_big_ordering, ctime_small_big_greater, ctime_small_big_less, 
		ctime_small_big_equal, ctime_small_big_not_equal, ctime_small_big_greater_or_equal, 
		ctime_small_big_less_or_equal, ctime_small_hash, ctime_big_hash, false);
	print_compare_results(decimal_stringify(biggest), decimal_stringify(middle),
		ctime_big_middle_ordering, ctime_big_middle_greater, ctime_big_middle_less,
		ctime_big_middle_equal, ctime_big_middle_not_equal, ctime_big_middle_greater_or_equal,
		ctime_big_middle_less_or_equal, ctime_big_hash, ctime_middle_hash, false);
	print_compare_results(decimal_stringify(biggest_plus_one), decimal_stringify(smallest),
		ctime_biggest_plus_one_smallest_ordering, ctime_biggest_plus_one_smallest_greater, ctime_biggest_plus_one_smallest_less,
		ctime_biggest_plus_one_smallest_equal, ctime_biggest_plus_one_smallest_not_equal, ctime_biggest_plus_one_smallest_greater_or_equal,
		ctime_biggest_plus_one_smallest_less_or_equal, ctime_biggest_plus_one_hash, ctime_small_hash, false); 

	//print runtime results
	print_compare_results(decimal_stringify(smallest), decimal_stringify(biggest),
		rtime_small_big_ordering, rtime_small_big_greater, rtime_small_big_less,
		rtime_small_big_equal, rtime_small_big_not_equal, rtime_small_big_greater_or_equal,
		rtime_small_big_less_or_equal, rtime_small_hash, rtime_big_hash, true);
	print_compare_results(decimal_stringify(biggest), decimal_stringify(middle),
		rtime_big_middle_ordering, rtime_big_middle_greater, rtime_big_middle_less,
		rtime_big_middle_equal, rtime_big_middle_not_equal, rtime_big_middle_greater_or_equal,
		rtime_big_middle_less_or_equal, rtime_big_hash, rtime_middle_hash, true);
	print_compare_results(decimal_stringify(biggest_plus_one), decimal_stringify(smallest),
		rtime_biggest_plus_one_smallest_ordering, rtime_biggest_plus_one_smallest_greater, rtime_biggest_plus_one_smallest_less,
		rtime_biggest_plus_one_smallest_equal, rtime_biggest_plus_one_smallest_not_equal, rtime_biggest_plus_one_smallest_greater_or_equal,
		rtime_biggest_plus_one_smallest_less_or_equal, rtime_biggest_plus_one_hash, rtime_small_hash, true); 
	
}

void cjm::uint128::example_code::print_compare_results(std::string left_operand, std::string right_operand,
	std::strong_ordering ordering, bool greater_than, bool less_than, bool equal, bool not_equal, bool greater_or_equal,
		bool less_or_equal, size_t left_hash, size_t right_hash, bool runtime)
{
	auto get_ordering_text = [](std::strong_ordering v) -> std::string_view
	{
		if (v == std::strong_ordering::equal || v == std::strong_ordering::equivalent)
			return "EQUAL"sv;
		if (v == std::strong_ordering::greater)
			return "GREATER"sv;
		return "LESSER"sv;
	};
	std::string_view compute_time = runtime ? "run-time"sv : "compile-time"sv;
	auto strm = std::stringstream{};
	strm << "Comparison (computed at " << compute_time << ") results for [" << left_operand << "] compared to [" << right_operand << "]:" << newl;
	strm << "\t<=>: [" << get_ordering_text(ordering) << "]." << newl;
	strm << "\t: Equal?: [" << std::boolalpha << equal << "]." << newl;
	strm << "\t: Not Equal?: [" << std::boolalpha << not_equal << "]." << newl;
	strm << "\t: Greater?: [" << std::boolalpha << greater_than << "]." << newl;
	strm << "\t: Greater or Equal?: [" << std::boolalpha << greater_or_equal << "]." << newl;
	strm << "\t: Lesser?: [" << std::boolalpha << less_than << "]." << newl;
	strm << "\t: Less than or equal?: [" << std::boolalpha << less_or_equal << "]." << newl;
	strm << "\t: LHS Hash: [0x" << std::hex << std::setw(std::numeric_limits<size_t>::digits / 4) << std::setfill('0') << left_hash << "]." << newl << std::dec;
	strm << "\t: RHS Hash: [0x" << std::hex << std::setw(std::numeric_limits<size_t>::digits / 4) << std::setfill('0') << right_hash << "]." << newl << std::dec;
	cout << strm.str() << std::endl;
}

std::string_view cjm::uint128::example_code::ordering_text(std::strong_ordering ordering)
{
	using namespace std::string_view_literals;
	if (ordering == std::strong_ordering::equal || ordering == std::strong_ordering::equivalent)
	{
		return "EQUAL"sv;
	}
	else if (ordering == std::strong_ordering::greater)
	{
		return "GREATER"sv;
	}
	else
	{
		return "LESS"sv;
	}
}

void cjm::uint128::example_code::demonstrate_conversions_to_from_signed_integral()
{
	constexpr auto u128_max = std::numeric_limits<uint128_t>::max();

	cout
		<< newl << "This is the demonstration of conversion to and from built-in SIGNED integral types.  "
		<< "All demonstrated with compile-time computation, but runtime is exactly same." << newl;

	auto print = []<typename SourceType>
			(SourceType orig, uint128_t converted,
		SourceType round_tripped, std::string_view source_type_name) -> void
	{
		cout
			<< std::dec << "Original value (of type " << source_type_name
			<< "): [" << +orig << "], converted to uint128_t: [" << converted
			<< "]; converted back: [" << +round_tripped << "]." << newl;
	};

	auto print_start_too_big = [=]<typename TargetType>
			(TargetType converted, uint128_t round_tripped,
		std::string_view target_type_name) -> void
	{
		cout
			<< std::dec << "Original uint128_t maximum value [" << u128_max
			<< "], converted to type " << target_type_name << " yielded ["
			<< +converted << "]; Converted back yielded: ["
			<< round_tripped << "]." << newl;
	};

	constexpr signed char a_char = 0x61;
	constexpr signed char neg_a_char{ -97 };
	constexpr std::int16_t neg_short_9 = -9;
	constexpr std::int16_t short_9 = 9;
	constexpr std::int32_t int_12 = 12;
	constexpr std::int32_t int_neg_12 = -12;
	constexpr std::int64_t i64_million = 1'000'000;
	constexpr std::int64_t i64_neg_million = -i64_million;

	//all the basic signed integer types implicitly nothrow convert to uint128_t
	//if they are negative, they will end up being positive unsigned value with same bit pattern
	constexpr uint128_t  from_signed_char = a_char;
	constexpr uint128_t  from_neg_a_char = neg_a_char;
	constexpr uint128_t from_short = short_9;
	constexpr uint128_t from_neg_short = neg_short_9;
	constexpr uint128_t from_int_12 = int_12;
	constexpr uint128_t from_neg_int_12 = int_neg_12;
	constexpr uint128_t from_i64_mill = i64_million;
	constexpr uint128_t from_i64_neg_mill = i64_neg_million;

	//narrowing implicit conversions from uint128_t will not work for the typical signed built-ins
	//none of the following four lines will compile:
	//constexpr signed char back_char = from_signed_char;
	//constexpr std::int16_t back_short = from_short;
	//constexpr std::int32_t back_int = from_int_12;
	//constexpr std::int64_t back_int64 = from_i64_mill;

	//instead, you need to use static_cast for narrowing conversions
	constexpr signed char back_char = static_cast<signed char>(from_signed_char);
	constexpr std::int16_t back_short = static_cast<std::int16_t>(from_short);
	constexpr std::int32_t back_int = static_cast<std::int32_t>(from_int_12);
	constexpr std::int64_t back_int64 =static_cast<std::int64_t>(from_i64_mill);

	//the negatives
	constexpr signed char neg_back_char = static_cast<signed char>(from_neg_a_char);
	constexpr std::int16_t neg_back_short = static_cast<std::int16_t>(from_neg_short);
	constexpr std::int32_t neg_back_int = static_cast<std::int32_t>(from_neg_int_12);
	constexpr std::int64_t neg_back_int64 =static_cast<std::int64_t>(from_i64_neg_mill);

	print(a_char, from_signed_char, back_char, "signed char"sv);
	print(neg_a_char, from_neg_a_char, neg_back_char, "signed char"sv);
	print(short_9, from_short, back_short, "std::int16_t"sv);
	print(neg_short_9, from_neg_short, neg_back_short, "std::int16_t"sv);
	print(int_12, from_int_12, back_int, "std::int32_t"sv);
	print(int_neg_12, from_neg_int_12, neg_back_int, "std::int32_t"sv);
	print(i64_million, from_i64_mill, back_int64, "std::int64_t"sv);
	print(i64_neg_million, from_i64_neg_mill, neg_back_int64, "std::int64_t"sv);

	//working with values too big to fit in the built-in type
	constexpr signed char char_biggest_u128 = static_cast<signed char>(u128_max);
	constexpr std::int16_t short_biggest_u128 = static_cast<std::int16_t>(u128_max);
	constexpr std::int32_t int_biggest_u128 = static_cast<std::int32_t>(u128_max);
	constexpr std::int64_t long_biggest_u128 = static_cast<std::int64_t>(u128_max);

	constexpr uint128_t back_biggest_char = char_biggest_u128;
	constexpr uint128_t back_biggest_short = short_biggest_u128;
	constexpr uint128_t back_biggest_int = int_biggest_u128;
	constexpr uint128_t back_biggest_long = long_biggest_u128;

	//show conversion back WITH DATA LOSS (no sign extension available as with signed example)
	// .. bit pattern left filled with zeroes:
	print_start_too_big(char_biggest_u128, back_biggest_char, "signed char"sv);
	print_start_too_big(short_biggest_u128, back_biggest_short, "std::int16_t"sv);
	print_start_too_big(int_biggest_u128, back_biggest_int, "std::int32_t"sv);
	print_start_too_big(long_biggest_u128, back_biggest_long, "std::int64_t"sv);

}

void cjm::uint128::example_code::demonstrate_conversions_to_from_unsigned_integral()
{
	constexpr auto u128_max = std::numeric_limits<uint128_t>::max();

	cout
			<< newl << "This is the demonstration of conversion to and from built-in UNSIGNED integral types.  "
			<< "All demonstrated with compile-time computation, but runtime is exactly same." << newl;

	auto print = []<typename SourceType>
			(SourceType orig, uint128_t converted,
			 SourceType round_tripped, std::string_view source_type_name) -> void
	{
		cout
				<< std::dec << "Original value (of type " << source_type_name
				<< "): [" << +orig << "], converted to uint128_t: [" << converted
				<< "]; converted back: [" << +round_tripped << "]." << newl;
	};

	auto print_start_too_big = [=]<typename TargetType>
			(TargetType converted, uint128_t round_tripped,
			 std::string_view target_type_name) -> void
	{
		cout
				<< std::dec << "Original uint128_t maximum value [" << u128_max
				<< "], converted to type " << target_type_name << " yielded ["
				<< +converted << "]; Converted back yielded: ["
				<< round_tripped << "]." << newl;
	};


	constexpr unsigned char a_char = 0x61;
	constexpr std::uint16_t ushort_9 = 9;
	constexpr std::uint32_t uint_12 = 12;
	constexpr std::uint64_t ui64_million = 1'000'000;


	//all the basic signed integer types implicitly nothrow convert to uint128_t
	//if they are negative, they will end up being positive unsigned value with same bit pattern
	constexpr uint128_t from_unsigned_char = a_char;
	constexpr uint128_t from_ushort_9 = ushort_9;
	constexpr uint128_t from_uint_12 = uint_12;
	constexpr uint128_t from_ui64_mill = ui64_million;


	//narrowing implicit conversions from uint128_t will not work for the typical unsigned built-ins
	//none of the following four lines will compile:
//	constexpr unsigned char back_char = from_unsigned_char;
//	constexpr std::uint16_t back_short = from_ushort_9;
//	constexpr std::uint32_t back_int = from_uint_12;
//	constexpr std::uint64_t back_int64 = from_ui64_mill;

	//instead, you need to use static_cast for narrowing conversions
	constexpr auto back_uchar = static_cast<unsigned char>(from_unsigned_char);
	constexpr auto back_ushort = static_cast<std::uint16_t>(from_ushort_9);
	constexpr auto back_uint = static_cast<std::uint32_t>(from_uint_12);
	constexpr auto back_uint64 =static_cast<std::uint64_t>(from_ui64_mill);

	print(a_char, from_unsigned_char, back_uchar, "unsigned char"sv);
	print(ushort_9, from_ushort_9, back_ushort, "std::uint16_t"sv);
	print(uint_12, from_uint_12, back_uint, "std::uint32_t"sv);
	print(ui64_million, from_ui64_mill, back_uint64, "std::uint64_t"sv);


	//working with values too big to fit in the built-in type
	constexpr auto uchar_biggest_u128 = static_cast<unsigned char>(u128_max);
	constexpr auto ushort_biggest_u128 = static_cast<std::uint16_t>(u128_max);
	constexpr auto uint_biggest_u128 = static_cast<std::uint32_t>(u128_max);
	constexpr auto ulong_biggest_u128 = static_cast<std::uint64_t>(u128_max);

	constexpr uint128_t back_biggest_uchar = uchar_biggest_u128;
	constexpr uint128_t back_biggest_ushort = ushort_biggest_u128;
	constexpr uint128_t back_biggest_uint = uint_biggest_u128;
	constexpr uint128_t back_biggest_ulong = ulong_biggest_u128;

	//show conversion back (works because sign extended during conversion):
	print_start_too_big(uchar_biggest_u128, back_biggest_uchar, "unsigned char"sv);
	print_start_too_big(ushort_biggest_u128, back_biggest_ushort, "std::uint16_t"sv);
	print_start_too_big(uint_biggest_u128, back_biggest_uint, "std::uint32_t"sv);
	print_start_too_big(ulong_biggest_u128, back_biggest_ulong, "std::uint64_t"sv);

	//each above should equal max value of smaller type with zeroes filled in when converting to uint128_t:
	static_assert(back_biggest_uchar == std::numeric_limits<unsigned char>::max());
	static_assert(back_biggest_ushort == std::numeric_limits<std::uint16_t>::max());
	static_assert(back_biggest_uint == std::numeric_limits<std::uint32_t>::max());
	static_assert(back_biggest_ulong == std::numeric_limits<std::uint64_t>::max());
}

void cjm::uint128::example_code::demonstrate_conversions_to_from_floating_points()
{
	using namespace cjm::numerics;
	cout << newl << "This is the to-from floating point example." << newl;

	//cjm::concepts::builtin_floating_point means float, double and long double
	auto print_three_way_convert = []<cjm::numerics::concepts::builtin_floating_point Float>
		(Float orig, uint128_t converted, Float back,
			std::string_view float_type_name) -> void
	{
		cout
			<< "Value [" << std::dec << std::setw(8) << std::fixed << orig << "] of type "
			<< float_type_name << " as converted to uint128 yields: [" << std::dec
			<< converted << "] and converted back to " << float_type_name << " yields: ["
			<< std::dec << std::setw(8) << std::fixed << back << "]." << newl;
	};

	auto optional_to_str = [](std::optional<uint128_t> v) -> std::string
	{
		auto strm = std::stringstream{};
		if (v.has_value())
			strm << std::dec << *v;
		else
			strm << "std::nullopt"sv;
		return strm.str();
	};
	
	constexpr float tiny_positive_float = 0.0001f;
	constexpr float one_plus_a_tad_float = 1.0001f;
	constexpr float big_ole_number_float = 1'000'000'000'000.1f;

	constexpr double tiny_positive_dbl = 0.0001;
	constexpr double one_plus_a_tad_dbl = 1.0001;
	constexpr double big_ole_number_dbl = 1'000'000'000'000.1;

	constexpr long double tiny_positive_ln_dbl = 0.0001L;
	constexpr long double one_plus_a_tad_ln_dbl = 1.0001L;
	constexpr long double big_ole_number_ln_dbl = 1'000'000'000'000.1L;

	//unlike built-in integer <-> uint128_t conversions,
	//conversions to and from floating point numbers are
	//runtime-only and always require explicit static_cast.
	//It causes undefined behavior (perhaps caught
	//in debug mode by an assertion) to try to convert a negative
	//float
	
	const auto from_tp_fl = static_cast<uint128_t>(tiny_positive_float);
	const auto from_opt_fl = static_cast<uint128_t>(one_plus_a_tad_float);
	const auto from_big_ole_fl = static_cast<uint128_t>(big_ole_number_float);
	
	const auto fl_tp_rt = static_cast<float>(from_tp_fl);
	const auto fl_opt_rt = static_cast<float>(from_opt_fl);
	const auto fl_big_rt = static_cast<float>(from_big_ole_fl);


	const auto from_tp_db = static_cast<uint128_t>(tiny_positive_dbl);
	const auto from_opt_db = static_cast<uint128_t>(one_plus_a_tad_dbl);
	const auto from_big_ole_db = static_cast<uint128_t>(big_ole_number_dbl);

	const auto db_tp_rt = static_cast<double>(from_tp_db);
	const auto db_opt_rt = static_cast<double>(from_opt_db);
	const auto db_big_rt = static_cast<double>(from_big_ole_db);

	const auto from_tp_ln_db = static_cast<uint128_t>(tiny_positive_ln_dbl);
	const auto from_opt_ln_db = static_cast<uint128_t>(one_plus_a_tad_ln_dbl);
	const auto from_big_ole_ln_db = static_cast<uint128_t>(big_ole_number_ln_dbl);

	const auto ln_db_tp_rt = static_cast<long double>(from_tp_ln_db);
	const auto ln_db_opt_rt = static_cast<long double>(from_opt_ln_db);
	const auto ln_db_big_rt = static_cast<long double>(from_big_ole_ln_db);
	
	print_three_way_convert(tiny_positive_float, from_tp_fl, fl_tp_rt, "float"sv);
	print_three_way_convert(one_plus_a_tad_float, from_opt_fl, fl_opt_rt, "float"sv);
	print_three_way_convert(big_ole_number_float, from_big_ole_fl, fl_big_rt, "float"sv);
		
	print_three_way_convert(tiny_positive_dbl, from_tp_db, db_tp_rt, "double"sv);
	print_three_way_convert(one_plus_a_tad_dbl, from_opt_db, db_opt_rt, "double"sv);
	print_three_way_convert(big_ole_number_dbl, from_big_ole_db, db_big_rt, "double"sv);
			
	print_three_way_convert(tiny_positive_ln_dbl, from_tp_ln_db, ln_db_tp_rt, "long double"sv);
	print_three_way_convert(one_plus_a_tad_ln_dbl, from_opt_ln_db, ln_db_opt_rt, "long double"sv);
	print_three_way_convert(big_ole_number_ln_dbl, from_big_ole_ln_db, ln_db_big_rt, "long double"sv);

	//remember there will usually be loss of information due to rounding errors.

	//If you don't want undefined behavior for failed conversions, the library provides other options
	//from floating point, use the safe_from_floating_or_throw function or the safe_from_floating function (returns std::
	//nullopt on failure).

	//for example, if you tried a static cast with this, it would be UB. but safe_from_floating_or_throw
	try
	{
		uint128_t res = safe_from_floating_or_throw(-2.1f);
		std::cerr << "You should never see this: [" << res << "]." << newl;
		throw std::logic_error{ "Last statement should have thrown." };
	}
	catch (const std::invalid_argument& ex)
	{
		std::cout
			<< "Correct behavior: invalid_argument thrown for float "
			<< "that would cause UB if static_cast to uint128_t.  Msg: ["
			<< ex.what() << "]." << newl;
	}
	catch (...)
	{
		std::cerr << "Did not throw or threw the wrong thing!" << newl;
	}

	std::optional<uint128_t> result = safe_from_floating(std::numeric_limits<long double>::quiet_NaN());
	cout << "Converting nan to uint128_t: [" << optional_to_str(result) << "]." << newl;
	cout
		<< "Converting long double max to uint128_t: ["
		<< optional_to_str(safe_from_floating(std::numeric_limits<long double>::max()))
		<< "]." << newl;
	cout
		<< "Converting float max to uint128_t: ["
		<< optional_to_str(safe_from_floating(std::numeric_limits<float>::max()))
		<< "]." << newl;
}

void cjm::uint128::example_code::demonstrate_literals()
{
	//if we hadn't already imported the namespace, you would need to import
	//the uint128_literals such as by doing
	//using namespace cjm::numerics::uint128_literals; // OR
	//using cjm::numerics::uint128_literals::operator ""_u128;

	//Decimal and hex literals are supported.
	//Full support for separator char "'" in literals
	//Octal and binary literals not supported.

	//Do not use the literal operator with runtime data:
	//when msvc, clang, intel and gcc all support consteval,
	//the literal operators will all be changed.  In
	//short use the literals for literals.

	cout << newl << "This is the literals demonstration." << newl;
		
	{
		cout << "Here are some hexadecimal literals: " << newl;
		//hex examples
		constexpr auto a = 0x123456789ABCDEF0123456789ABCDEF0_u128;
		auto b = 0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u128;
		auto c = 0x1234'5678'9abc'def0'1234'5678'9abc'def0_u128;
		auto d = 0X1234'5678'9abc'def0'1234'5678'9abc'def0_u128;
		auto e = 0X1234'5678'9abc'def0'1234'5678'9abc'def0_u128;
		auto f = 0x1'23'456'789ABCDEF012345678'9AB'C'DEF'0_u128; //stupid but legal
		auto g = 0x0_u128;
		auto h = 0xaBcD_u128; //mix up capital lowercase

		cout
			<< std::hex
			<< a << " " << b << newl
			<< c << " " << d << newl
			<< e << " " << f << newl
			<< g << " " << h << newl;
	}

	{
		constexpr uint128_t x = 340282366920938463463374607431768211455_u128; //(i.e.max value)
		static_assert(x == std::numeric_limits<uint128_t>::max());
		auto a = 340'282'366'920'938'463'463'374'607'431'768'211'455_u128; //thousands separators
		auto b = 0_u128; //0 is ok too
		auto c = 00_u128; //even multiple leading zeros if all are 0

		cout << newl << "Here are some decimal literals: " << newl;
		cout
			<< std::dec 
			<< a << " " << b << newl
			<< c << newl;
	}
	
	//ILLEGAL LITERAL VALUES ARE CAUGHT AT COMPILE TIME
	//illegal literals should be caught at compile time even if not assigned to constexpr.
	//examples ... none of following declaration/assignments should compile:
	//auto illegal_octal = 01_u128; //illegal -- octal literals not supported.
	//auto illegal_too_long = 0xabcd'ef01'2345'6789'abdc'ef01'6789'abcd'e_u128;
	//auto illegal_dec_too_big_by_one = 340'282'366'920'938'463'463'374'607'431'768'211'456_u128;
	//auto illegal_one_too_many_dec_digits = 1'000'000'000'000'000'000'000'000'000'000'000'000'000_u128;
	//cout << "illegal_octal: " << illegal_octal << newl;
	//cout << "illegal_too_long: " << illegal_too_long << newl;
	//cout << "illegal_dec_too_big_by_one: " << illegal_dec_too_big_by_one << newl;
	//cout << "illegal_one_too_many_dec_digits: " << illegal_one_too_many_dec_digits << newl;
}

void cjm::uint128::example_code::demonstrate_stream_insertion_and_extraction()
{

	std::cout << newl << "This is the stream insertion and extraction demonstration." << newl;
	{
		std::cout << "First, we demonstrate decimal format: " << newl;
		
		constexpr auto expected_value = 256'368'684'943'268'121'395'391'016'720'575'361'037_u128;
		//commas and underscores ignored
		constexpr auto narrow_text = "256_368_684_943_268_121_395_391_016_720_575_361_037"sv;
		constexpr auto wide_text = L"256,368,684,943,268,121,395,391,016,720,575,361,037"sv; 

		std::cout << "Going to stream insert the following narrow text then extract it into a uint128_t: \"" << narrow_text << "\"." << newl;

		auto narrow_stream = make_throwing_sstream<char>();
		auto wide_stream = make_throwing_sstream<wchar_t>();

		narrow_stream << narrow_text;
		wide_stream << wide_text;

		uint128_t narrow, wide;

		narrow_stream >> narrow;
		wide_stream >> wide;

		bool narrow_equal = narrow == expected_value;
		bool wide_equal = wide == expected_value;

		if (!narrow_equal)
		{
			throw std::logic_error{ "Narrow isn't equal!" };
		}
		if (!wide_equal)
		{
			throw std::logic_error{ "Wide isn't equal!" };
		}

		cout << "Narrow uint128_t: [" << std::dec << narrow << "]." << newl;
		//don't want to use wcout in same program with cout
		cout << "Wide also worked." << newl;
				 
	}
	{
		std::cout << "Next, we demonstrate hexadecimal format: " << newl;
		constexpr auto expected_value = 0xc0de'd00d'ea75'dead'beef'600d'f00d_u128;
		//commas and underscores ignored
		constexpr auto narrow_text = "0xc0de_d00d_ea75_dead_beef_600d_f00d"sv;
		constexpr auto wide_text =  L"0xc0de,d00d,ea75,dead,beef,600d,f00d"sv;

		std::cout << "Going to stream insert the following narrow text then extract it into a uint128_t: \"" << narrow_text << "\"." << newl;

		auto narrow_stream = make_throwing_sstream<char>();
		auto wide_stream = make_throwing_sstream<wchar_t>();

		narrow_stream << narrow_text;
		wide_stream << wide_text;

		uint128_t narrow, wide;

		narrow_stream >> narrow;
		wide_stream >> wide;

		bool narrow_equal = narrow == expected_value;
		bool wide_equal = wide == expected_value;

		if (!narrow_equal)
		{
			throw std::logic_error{ "Narrow isn't equal!" };
		}
		if (!wide_equal)
		{
			throw std::logic_error{ "Wide isn't equal!" };
		}

		cout << "Narrow uint128_t: [" << std::hex << narrow << "]." << newl;
		//don't want to use wcout in same program with cout
		cout << "Wide also worked." << newl;

	}
}