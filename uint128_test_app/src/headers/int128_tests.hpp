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
#ifndef INT128_TESTS_HPP_
#define INT128_TESTS_HPP_
#include <iostream>
#include <cjm/numerics/numerics.hpp>
#include <cjm/numerics/uint128.hpp>
#include <cjm/numerics/cjm_numeric_concepts.hpp>
#include <cjm/string/cjm_string.hpp>
#include "testing.hpp"
#include <iostream>
#include <iomanip>
#include <string_view>
#include <array>
#include <string>
#include <sstream>
#include <limits>
#include <type_traits>
#include <cstdint>
#include <boost/io/ios_state.hpp>
#include <exception>
#include <stdexcept>
#include <utility>
#include <boost/multiprecision/cpp_int.hpp>
#include <concepts>
#include <cstdint>
#include <random>
#include <algorithm>
#include <vector>
#include<functional>
#include <memory>
#include <optional>
#include <chrono>
#include <date/date.h>
#include <filesystem>
#include <tuple>
#include <span>
#include <unordered_set>
#include <cjm/string/istream_utils.hpp>
#include "int128_test_switches.hpp"
#include <absl/numeric/int128.h>
#include <cjm/numerics/fixed_uint_container_math.hpp>
#include "umult.hpp"
#include "output_guard.hpp"
#include<functional>
#include <fstream>
#include <map>
#include <thread>
namespace cjm::uint128_tests::generator
{
	class rgen;
}



namespace cjm::uint128_tests
{
	template<typename Invocable>
	concept invocable = requires (Invocable i)
	{
		{i()} -> std::convertible_to<void>;
	};

	template<typename TFunctionLike>
	concept test_invocable = std::convertible_to<TFunctionLike, std::function<void(std::basic_ostream<char>&, std::basic_ostream<char>&)>> &&
		requires (TFunctionLike & func_like, std::basic_ostream<char>&cout, std::basic_ostream<char>&cerr)
	{
		{func_like(cout, cerr)} -> std::convertible_to<void>;
	};
	
	
	using ctrl_uint128_t = boost::multiprecision::uint128_t;
	using alt_ctrl_uint128_t = absl::uint128;
	using uint128_t = numerics::uint128;
	using namespace numerics::uint128_literals;
	using namespace numerics::literals;
	using namespace std::string_literals;
	using namespace std::string_view_literals;
	using testing::cjm_assert;
	using testing::cjm_deny;
	constexpr auto newl = '\n';
	using cout_saver = boost::io::ios_flags_saver;
	using switches::test_switch;
	using switches::test_mode;
	enum class unary_op : unsigned int;
	enum class binary_op : unsigned int;
	constexpr size_t binary_op_count = 11;
	constexpr size_t unary_op_count = 9;

	class bad_binary_op;
	class bad_unary_op;
	class divmod_fail_match;

	constexpr auto multithread_test_demo_tests = std::array{ "addition_tests"sv, "hash_dx"sv, "issue_10_strm_insrt_test"sv,"binary_operation_rt_ser_tests"sv, "unary_operation_vec_rt_serialization_tests"sv, "division_modulus_tests"sv };


	template<typename TestTypeUi, typename ControlTypeUi>
	concept test_uint_and_control_set =
		cjm::numerics::concepts::cjm_unsigned_integer<TestTypeUi> &&
		cjm::numerics::concepts::unsigned_integer<ControlTypeUi> &&
		std::numeric_limits<TestTypeUi>::digits ==
		std::numeric_limits<ControlTypeUi>::digits;

	template<typename TestType = uint128_t, typename ControlType = ctrl_uint128_t>
	requires (test_uint_and_control_set<TestType, ControlType>)
		struct binary_operation;
	template<typename TestType, typename ControlType>
	requires (test_uint_and_control_set<TestType, ControlType>)
		struct unary_operation;
	using binary_op_u128_t = binary_operation<uint128_t, ctrl_uint128_t>;
	using binary_op_u128_vect_t = std::vector<binary_op_u128_t>;
	using unary_op_u128_t = unary_operation<uint128_t, ctrl_uint128_t>;
	using unary_op_u128_vect_t = std::vector<unary_op_u128_t>;
	/// <summary>
	/// hascorrectresult() must be true or throws
	/// must be divide or modulus or throws
	/// </summary>
	/// <param name="op">the result</param>
	void validate_divmod_op(const binary_op_u128_t& op);

	template<test_invocable Invocable>
	void execute_test(Invocable test, std::string_view test_name);

	alt_ctrl_uint128_t to_alt_ctrl(uint128_t convert) noexcept;
	uint128_t to_test(alt_ctrl_uint128_t convert) noexcept;
	ctrl_uint128_t to_ctrl(uint128_t convert);
	uint128_t to_test(const ctrl_uint128_t& convert);

	std::shared_ptr<cjm::testing::output_guard::dedicated_output> get_dedicated();

	void execute_specified_tests(std::span<std::string_view> tests, unsigned max_threads = 0);
	
	void print_environ_data(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void print_sizes(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void print_alignments(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void print_floating_point_info(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);

	void print_available_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_uint128_tests_threaded();
	void execute_uint128_tests();
	constexpr size_t pow_2_arr_size = 63;
	constexpr std::array<std::uint64_t, pow_2_arr_size> get_pow2_arr();
	constexpr std::array<int, pow_2_arr_size> get_pow2_res_arr();
	[[maybe_unused]] void save_random_unary_ops_to_file(std::filesystem::path target,
		std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	[[maybe_unused]] void save_random_binary_ops_to_file(std::filesystem::path target,
		std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_help(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_specified_test(std::string_view test_name, std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_unary_test_file(const std::filesystem::path& file, std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_binary_test_file(const std::filesystem::path& file, std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void run_test_application(std::span<test_switch> switches);
	void execute_binary_operation_rt_ser_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_trim_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_ascii_char_interconversions(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_div_mod_zero_dividend_nonzero_divisor_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_div_mod_by_zero_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_basic_test_one(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_string_parse_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_basic_multiplication_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void test_fls(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void print_uint128_eval_mode(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void print_constexpr_bitcast_available(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void print_cpp20_bitops_available(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void print_builtin_uint128_data_if_present(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void print_whether_has_consteval(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void test_interconversion(const ctrl_uint128_t& control, uint128_t test, std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_builtin_u128fls_test_if_avail(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_first_bin_op_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_gen_comp_ops_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_stream_insert_bin_op_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_print_generated_filename_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_generate_addition_ops_rt_ser_deser_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_addition_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_subtraction_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_shift_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_bw_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_comparison_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_multiplication_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_failing_division_test_1(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_failing_division_test_2(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_failing_modulus_test_1(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_division_modulus_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_unary_op_code_rt_serialization_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_unary_operation_rt_serialization_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_unary_operation_vec_rt_serialization_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_unary_op_basic_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_parse_file_test(std::string_view path, size_t expected_ops, std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_unary_op_post_stat_assert_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_test_convert_to_float(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_test_convert_to_double(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_test_convert_to_long_double(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_throwing_float_conversion_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_safe_float_conversions_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_controlled_from_float_conversion_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_controlled_float_rt_conversion_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);

	void execute_hash_dx(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_issue_10_strm_insrt_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_issue_10_showbase_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);

	void execute_unary_op_pre_inc_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_unary_op_post_inc_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_unary_op_pre_dec_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_unary_op_post_dec_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_unary_op_unary_plus_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);

	void execute_unary_op_unary_minus_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_unary_op_bitwise_not_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_unary_op_bool_cast_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_unary_op_logical_negation_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);

	void execute_builtin_add_with_carry_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_basic_u128_adc_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_basic_u128_sbb_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_builtin_sub_with_borrow_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);

	void execute_umult_spec_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_uintcontainer_adc_tests(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);

	void execute_issue27_bug_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void execute_literal_test(std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);

	using test_func_t = std::add_const_t<decltype(&execute_literal_test)>;
	//static_assert(test_invocable<test_func_t>);
	using test_func_lookup_t = std::map<std::string_view, test_func_t, cjm::string::case_ignoring_trimmed_ordinal_less<char>>;

	const test_func_lookup_t& get_test_func_lookup() noexcept;

	
	std::basic_ostream<char>& operator<<(std::basic_ostream<char>&, lit_type v);

	std::pair<ctrl_uint128_t, std::string> create_random_dec_n_digits_long(size_t decimal_digits, generator::rgen& gen);
	std::pair<ctrl_uint128_t, std::string> create_random_hex_n_digits_long(size_t hex_digits, generator::rgen& gen);
	std::string generate_literal_test(lit_type literal_type, size_t num_digits, generator::rgen& gen);
	std::vector<std::string> generate_literal_tests();
	void generate_then_print_literal_tests(std::basic_ostream<char>& cout);

	[[maybe_unused]] void print_n_static_assertions(const binary_op_u128_vect_t& op_vec, size_t n,
		std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	[[maybe_unused]] void print_n_static_assertions(const unary_op_u128_vect_t& op_vec, size_t n, 
		std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	constexpr auto base_test_battery_res_filename = "base_test_battery_results"sv;
	constexpr auto base_un_op_filename = "unary_ops"sv;
	constexpr auto base_bin_op_filename = "binary_ops"sv;
	constexpr auto op_failed_test_tag = "failed_test"sv;
	constexpr auto op_generated_tag = "generated"sv;
	constexpr auto op_extension = "txt"sv;

	
	
	std::filesystem::path create_test_battery_results_filename();
	std::filesystem::path create_generated_op_filename(binary_op op);
	std::filesystem::path create_failing_op_pathname(binary_op op);
	std::filesystem::path create_generated_op_filename(unary_op op);
	std::filesystem::path create_failing_op_pathname(unary_op op);

	template<numerics::concepts::character Char>
	std::basic_ostream<Char, std::char_traits<Char>>& operator<<(std::basic_ostream<Char,
		std::char_traits<Char>>&os, const binary_op_u128_t& op);

	template<numerics::concepts::character Char>
	std::basic_istream<Char, std::char_traits<Char>>& operator>>(std::basic_istream<Char,
		std::char_traits<Char>>&is, binary_op_u128_t& op);

	template<numerics::concepts::character Char>
	std::basic_ostream<Char, std::char_traits<Char>>& operator<<(std::basic_ostream<Char,
		std::char_traits<Char>>&os, const binary_op_u128_vect_t& op);

	template<numerics::concepts::character Char>
	std::basic_istream<Char, std::char_traits<Char>>& operator>>(std::basic_istream<Char,
		std::char_traits<Char>>&is, binary_op_u128_vect_t& op);

	template<numerics::concepts::character Char>
	std::basic_ostream<Char, std::char_traits<Char>>& operator<<(std::basic_ostream<Char,
		std::char_traits<Char>>&os, const unary_op_u128_t& op);

	template<numerics::concepts::character Char>
	std::basic_istream<Char, std::char_traits<Char>>& operator>>(std::basic_istream<Char,
		std::char_traits<Char>>&is, unary_op_u128_t& op);

	template<numerics::concepts::character Char>
	std::basic_ostream<Char, std::char_traits<Char>>& operator<<(std::basic_ostream<Char,
		std::char_traits<Char>>&os, const unary_op_u128_vect_t& op);

	template<numerics::concepts::character Char>
	std::basic_istream<Char, std::char_traits<Char>>& operator>>(std::basic_istream<Char,
		std::char_traits<Char>>&is, unary_op_u128_vect_t& op);

	template<numerics::concepts::character Char>
	binary_op_u128_t parse(std::basic_string_view<Char> sv);

	template<numerics::concepts::character Char>
	unary_op_u128_t parse_unary(std::basic_string_view<Char> sv);

	unary_op_u128_vect_t generate_random_standard_test_ops();
	unary_op_u128_vect_t generate_post_inc_dec_ops(size_t num_ops_each_type, bool include_standard_tests);
	unary_op_u128_vect_t generate_specified_un_ops(unary_op op_code, size_t num_rnd_ops,
		bool include_standard_tests);

	binary_op_u128_vect_t generate_easy_ops(size_t num_ops, binary_op op, bool include_standard_tests, std::basic_ostream<char>& cerr);
	binary_op_u128_vect_t generate_shift_ops(size_t num_ops, bool include_standard_tests, std::basic_ostream<char>& cerr);
	binary_op_u128_vect_t generate_bw_ops(size_t num_ops, bool include_standard_tests, std::basic_ostream<char>& cerr);
	binary_op_u128_vect_t generate_mult_ops(size_t num_each_range, bool include_standard_tests, std::basic_ostream<char>& cerr);
	binary_op_u128_vect_t generate_divmod_ops(size_t num_each_range, bool include_standard_tests, std::basic_ostream<char>& cerr);
	void insert_standard_divmod_ops(binary_op_u128_vect_t& op_vec);


	void test_binary_operation(binary_op_u128_t& op, std::string_view test_name, std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	void test_unary_operation(unary_op_u128_t& op, std::string_view test_name, std::basic_ostream<char>& cout, std::basic_ostream<char>& cerr);
	namespace u128_testing_constant_providers
	{
		namespace concepts
		{
			//Must be a cjm_unsigned_integer or a built-in, unsigned integer that is no larger than 64 bits.
			template<typename T>
			concept supports_testing_constant_provider =
				//EITHER: (integer AND unsigned AND size <= 64)
				(std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed && std::is_fundamental_v<T> &&
					sizeof(T) <= sizeof(std::uint64_t)) || cjm::numerics::concepts::cjm_unsigned_integer<T>;
		}

		template<concepts::supports_testing_constant_provider T>
		struct testing_constant_provider;

		template<>
		struct testing_constant_provider<std::uint8_t>
		{
			using full_uint_t = std::uint8_t;
			using half_uint_t = std::uint8_t;

			static constexpr full_uint_t maximum = std::numeric_limits<full_uint_t>::max();
			static constexpr full_uint_t max_less_one = maximum - full_uint_t{ 1 };
			static constexpr full_uint_t zero = std::numeric_limits<full_uint_t>::min();
			static constexpr full_uint_t one = zero + full_uint_t{ 1 };
			static constexpr full_uint_t maximum_half = std::numeric_limits<half_uint_t>::max() >> (std::numeric_limits<half_uint_t>::digits / 2);
			static constexpr full_uint_t maximum_half_less_one = maximum_half - 1;
			static constexpr full_uint_t maximum_half_plus_one = maximum_half + 1;

			static constexpr std::array<full_uint_t, 7> all_values =
			{ maximum, max_less_one, zero,
				one, maximum_half, max_less_one,
				maximum_half_plus_one };

			using half_provider_t = testing_constant_provider<half_uint_t>;
		};

		template<>
		struct testing_constant_provider<std::uint16_t>
		{
			using full_uint_t = std::uint16_t;
			using half_uint_t = std::uint8_t;

			static constexpr full_uint_t maximum = std::numeric_limits<full_uint_t>::max();
			static constexpr full_uint_t max_less_one = maximum - full_uint_t{ 1 };
			static constexpr full_uint_t zero = std::numeric_limits<full_uint_t>::min();
			static constexpr full_uint_t one = zero + full_uint_t{ 1 };
			static constexpr full_uint_t maximum_half = std::numeric_limits<half_uint_t>::max();
			static constexpr full_uint_t maximum_half_less_one = maximum_half - 1;
			static constexpr full_uint_t maximum_half_plus_one = maximum_half + 1;

			static constexpr std::array<full_uint_t, 7> all_values =
			{ maximum, max_less_one, zero,
				one, maximum_half, max_less_one,
				maximum_half_plus_one };
			using half_provider_t = testing_constant_provider<half_uint_t>;
		};

		template<>
		struct testing_constant_provider<std::uint32_t>
		{
			using full_uint_t = std::uint32_t;
			using half_uint_t = std::uint16_t;

			static constexpr full_uint_t maximum = std::numeric_limits<full_uint_t>::max();
			static constexpr full_uint_t max_less_one = maximum - full_uint_t{ 1 };
			static constexpr full_uint_t zero = std::numeric_limits<full_uint_t>::min();
			static constexpr full_uint_t one = zero + full_uint_t{ 1 };
			static constexpr full_uint_t maximum_half = std::numeric_limits<half_uint_t>::max();
			static constexpr full_uint_t maximum_half_less_one = maximum_half - 1;
			static constexpr full_uint_t maximum_half_plus_one = maximum_half + 1;

			static constexpr std::array<full_uint_t, 7> all_values =
			{ maximum, max_less_one, zero,
				one, maximum_half, max_less_one,
				maximum_half_plus_one };
			using half_provider_t = testing_constant_provider<half_uint_t>;
		};

		template<>
		struct testing_constant_provider<std::uint64_t> final
		{
			using full_uint_t = std::uint64_t;
			using half_uint_t = std::uint32_t;

			static constexpr full_uint_t maximum = std::numeric_limits<full_uint_t>::max();
			static constexpr full_uint_t max_less_one = maximum - full_uint_t{ 1 };
			static constexpr full_uint_t zero = std::numeric_limits<full_uint_t>::min();
			static constexpr full_uint_t one = zero + full_uint_t{ 1 };
			static constexpr full_uint_t maximum_half = std::numeric_limits<half_uint_t>::max();
			static constexpr full_uint_t maximum_half_less_one = maximum_half - 1;
			static constexpr full_uint_t maximum_half_plus_one = maximum_half + 1;

			static constexpr std::array<full_uint_t, 7> all_values =
			{ maximum, max_less_one, zero,
				one, maximum_half, max_less_one,
				maximum_half_plus_one };
			using half_provider_t = testing_constant_provider<half_uint_t>;
		};

		template<concepts::supports_testing_constant_provider T>
		struct testing_constant_provider final
		{
			using full_uint_t = std::remove_const_t<T>;
			using half_uint_t = typename T::int_part;
			static constexpr size_t full_digits = std::numeric_limits<full_uint_t>::digits;
			static constexpr size_t half_digits = std::numeric_limits<half_uint_t>::digits;

			static constexpr full_uint_t maximum = std::numeric_limits<full_uint_t>::max();
			static constexpr full_uint_t max_less_one = maximum - full_uint_t{ 1 };
			static constexpr full_uint_t zero = std::numeric_limits<full_uint_t>::min();
			static constexpr full_uint_t one = zero + full_uint_t{ 1 };
			static constexpr full_uint_t signed_version_min_bit_pattern_full = 1_u128 << (full_digits - 1);
			static constexpr full_uint_t signed_version_min_bit_pattern_half = half_uint_t{ 1 } << (half_digits - 1);
			static constexpr full_uint_t maximum_half = std::numeric_limits<half_uint_t>::max();
			static constexpr full_uint_t maximum_half_less_one = maximum_half - 1;
			static constexpr full_uint_t maximum_half_plus_one = maximum_half + 1;

			static constexpr std::array<full_uint_t, 9> all_values =
			{
				maximum, max_less_one, zero,
				one, signed_version_min_bit_pattern_full,
				signed_version_min_bit_pattern_half, maximum_half,
				max_less_one, maximum_half_plus_one
			};

			using half_provider_t = testing_constant_provider<half_uint_t>;
		};
	}

	template<numerics::concepts::unsigned_integer UnsignedInt>
	constexpr std::pair<UnsignedInt, UnsignedInt> test_post_increment(UnsignedInt inc_me) noexcept
	{
		auto first_res = inc_me++;
		return std::make_pair(first_res, inc_me);
	}

	template<numerics::concepts::unsigned_integer UnsignedInt>
	constexpr std::pair<UnsignedInt, UnsignedInt> test_post_decrement(UnsignedInt dec_me) noexcept
	{
		auto first_res = dec_me--;
		return std::make_pair(first_res, dec_me);
	}

	template<test_invocable Invocable>
	void execute_test(Invocable test, std::string_view test_name)
	{
		auto ptr = get_dedicated();

		auto cout = ptr->cout_proxy();
		auto cerr = ptr->cerr_proxy();

		cout_saver o_saver{ cout };
		cout_saver e_saver{ cerr };

		cout << "Beginning test: [" << test_name << "]:" << newl;
		try
		{
			test(cout, cerr);
			cout << "Test [" << test_name << "] PASSED" << newl << newl;
		}
		catch (const std::exception& ex)
		{
			std::stringstream ss;
			ss << "Test [" << test_name << "] failed with exception message: [" << ex.what() << "]." << newl;
			cerr << ss.str();
			throw;
		}
	}

	
	[[maybe_unused]] void compile_time_addition_test() noexcept;
	[[maybe_unused]] void compile_time_shift_test() noexcept;
	[[maybe_unused]] void compile_time_bw_test() noexcept;
	[[maybe_unused]] void compile_time_subtraction_test() noexcept;
	[[maybe_unused]] void compile_time_comparison_test() noexcept;
	[[maybe_unused]] void compile_time_multiplication_test() noexcept;
	[[maybe_unused]] void compile_time_divmod_test() noexcept;
	[[maybe_unused]] void compile_time_postfix_un_op_test() noexcept;
	 
	[[maybe_unused]] void compile_time_unary_op_pre_inc_test() noexcept;
	[[maybe_unused]] void compile_time_unary_op_pre_dec_test() noexcept;
	[[maybe_unused]] void compile_time_unary_op_unary_plus_test() noexcept;
	[[maybe_unused]] void compile_time_unary_op_unary_minus_test() noexcept;
	[[maybe_unused]] void compile_time_unary_op_bitwise_not_test() noexcept;
	[[maybe_unused]] void compile_time_unary_op_bool_cast_test() noexcept;
	[[maybe_unused]] void compile_time_unary_op_logical_negation_test() noexcept;

	constexpr auto all_the_tests = std::array
	{
		std::make_pair<test_func_t, std::string_view>(&execute_literal_test, "literal_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_basic_test_one, "basic_test_one"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_binary_operation_rt_ser_tests, "binary_operation_rt_ser_tests"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_print_generated_filename_test, "print_generated_filename_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_string_parse_test, "string_parse_text"sv),

		std::make_pair<test_func_t, std::string_view>(&execute_basic_multiplication_test, "basic_multiplication_test"sv),
		std::make_pair<test_func_t, std::string_view>(&test_fls, "test_fls"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_builtin_u128fls_test_if_avail, "builtin_u128fls_test_if_avail"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_first_bin_op_test, "first_bin_op_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_div_mod_by_zero_tests, "div_mod_zero_tests"sv),

		std::make_pair<test_func_t, std::string_view>(&execute_div_mod_zero_dividend_nonzero_divisor_tests, "div_mod_zero_dividend_nonzero_divisor_tests"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_gen_comp_ops_test, "gen_comp_ops_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_ascii_char_interconversions, "ascii_char_interconversions"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_trim_tests, "trim_tests"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_stream_insert_bin_op_test, "stream_insert_bin_op_test"sv),

		std::make_pair<test_func_t, std::string_view>(&execute_addition_tests, "addition_tests"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_shift_tests, "shift_tests"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_bw_tests, "bw_tests"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_subtraction_tests, "subtraction_tests"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_comparison_tests, "comparison_tests"sv),

		std::make_pair<test_func_t, std::string_view>(&execute_multiplication_tests, "multiplication_tests"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_failing_division_test_1, "failing_division_test_1"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_failing_division_test_2, "failing_division_test_2"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_failing_modulus_test_1, "failing_modulus_test_1"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_division_modulus_tests, "division_modulus_tests"sv),

		std::make_pair<test_func_t, std::string_view>(&execute_unary_op_code_rt_serialization_tests, "unary_op_code_rt_serialization_tests"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_unary_op_basic_test, "unary_op_basic_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_unary_op_post_stat_assert_test, "unary_op_post_stat_assert_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_unary_operation_rt_serialization_tests, "unary_operation_rt_serialization_tests"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_unary_operation_vec_rt_serialization_tests, "unary_operation_vec_rt_serialization_tests"sv),

		std::make_pair<test_func_t, std::string_view>(&execute_unary_op_post_inc_test, "unary_op_post_inc_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_unary_op_pre_inc_test, "unary_op_pre_inc_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_unary_op_pre_dec_test, "unary_op_pre_dec_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_unary_op_post_dec_test, "unary_op_post_dec_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_unary_op_unary_plus_test, "unary_op_unary_plus_test"sv),

		std::make_pair<test_func_t, std::string_view>(&execute_unary_op_unary_minus_test, "unary_op_unary_minus_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_unary_op_bitwise_not_test, "unary_op_bitwise_not_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_unary_op_bool_cast_test, "unary_op_bool_cast_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_unary_op_logical_negation_test, "unary_op_logical_negation_test"sv),


		std::make_pair<test_func_t, std::string_view>(&execute_test_convert_to_float, "test_convert_to_float"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_test_convert_to_double, "test_convert_to_double"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_test_convert_to_long_double, "test_convert_to_long_double"sv),


		std::make_pair<test_func_t, std::string_view>(&execute_throwing_float_conversion_test, "throwing_float_conversion_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_safe_float_conversions_test, "safe_float_conversions_test"sv),

		std::make_pair<test_func_t, std::string_view>(&execute_controlled_from_float_conversion_test, "controlled_from_float_conversion_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_controlled_float_rt_conversion_test, "controlled_float_rt_conversion_test"sv),

		std::make_pair<test_func_t, std::string_view>(&execute_issue_10_strm_insrt_test, "issue_10_strm_insrt_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_issue_10_showbase_test, "issue_10_showbase_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_hash_dx, "hash_dx"sv),

		std::make_pair<test_func_t, std::string_view>(&execute_builtin_add_with_carry_test, "builtin_add_with_carry_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_basic_u128_adc_test, "basic_u128_adc_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_basic_u128_sbb_test, "basic_u128_sbb_test"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_builtin_sub_with_borrow_test, "builtin_sub_with_borrow_test"sv),

		std::make_pair<test_func_t, std::string_view>(&execute_umult_spec_tests, "umult_spec_tests"sv),
		std::make_pair<test_func_t, std::string_view>(&execute_uintcontainer_adc_tests, "uintcontainer_adc_tests"sv),
	};
	
}

namespace std
{
	template<typename TestType, typename ControlType>
		requires (cjm::uint128_tests::test_uint_and_control_set<TestType, ControlType>)
	struct hash<cjm::uint128_tests::binary_operation<TestType, ControlType>>
	{
		std::size_t operator()(const cjm::uint128_tests::binary_operation<TestType, ControlType>& hash_me) noexcept;
	};

	template<typename TestType, typename ControlType>
		requires (cjm::uint128_tests::test_uint_and_control_set<TestType, ControlType>)
	struct hash<cjm::uint128_tests::unary_operation<TestType, ControlType>>
	{
		std::size_t operator()(const cjm::uint128_tests::unary_operation<TestType, ControlType>& hash_me) noexcept;
	};
}

namespace cjm::uint128_tests
{

	enum class binary_op : unsigned int
	{   //IF YOU EDIT THIS, MAKE SURE YOU EDIT THE CONSTANTS BELOW AND KEEP RELATED GROUPS CONSECUTIVE!
		left_shift = 0,
		right_shift = 1,

		bw_and = 2,
		bw_or = 3,
		bw_xor = 4,

		divide = 5,
		modulus = 6,

		add = 7,
		subtract = 8,
		multiply = 9,

		compare = 10
	};
	
	enum class unary_op : unsigned int
	{
		pre_increment = 0,
		pre_decrement = 1,
		post_increment = 2,
		post_decrement = 3,
		unary_plus = 4,
		unary_minus=5,
		bitwise_not = 6,
		bool_cast = 7,
		logical_negation = 8,
	};	
	
	static_assert(!cjm::numerics::is_windows || std::is_unsigned_v<wchar_t>);
	constexpr binary_op first_op = binary_op::left_shift;
	constexpr binary_op last_op = binary_op::compare;

	constexpr binary_op first_shift_op = binary_op::left_shift;
	constexpr binary_op last_shift_op = binary_op::right_shift;

	constexpr binary_op first_bw_op = binary_op::bw_and;
	constexpr binary_op last_bw_op = binary_op::bw_xor;

	constexpr binary_op first_divmod_op = binary_op::divide;
	constexpr binary_op last_divmod_op = binary_op::modulus;

	constexpr binary_op first_add_sub_mul_op = binary_op::add;
	constexpr binary_op last_add_sub_mul_op = binary_op::multiply;
	
	constexpr unary_op first_unary_op = unary_op::pre_increment;
	constexpr unary_op last_unary_op = unary_op::logical_negation;
	
	constexpr unary_op first_inc_dec_op = unary_op::pre_increment;
	constexpr unary_op last_inc_dec_op = unary_op::post_decrement;
	
	constexpr unary_op first_plus_minus_op = unary_op::unary_plus;
	constexpr unary_op last_plus_minus_op = unary_op::unary_minus;
	
	constexpr unary_op first_boolean_op = unary_op::bool_cast;
	constexpr unary_op last_boolean_op = unary_op::logical_negation;

	constexpr auto un_op_plus_plus = "++"sv;
	constexpr auto un_op_minus_minus = "--"sv;
	constexpr auto un_op_plus = "+"sv;
	constexpr auto un_op_minus = "-"sv;
	constexpr auto un_op_bw_not = "~"sv;
	constexpr auto un_op_static_cast_bool = "static_cast<bool>"sv;
	constexpr auto un_op_open_parens = "("sv;
	constexpr auto un_op_close_parens = ")"sv;
	constexpr auto un_op_logical_negate = "!"sv;
	constexpr auto un_op_semicolon = ";"sv;
	constexpr auto un_op_equals = "=="sv;
	constexpr auto un_on_not_equal = "!="sv;
	constexpr auto un_op_u128_literal_suffix = "_u128"sv;
	

	constexpr auto un_op_name_lookup = std::array<sv_t, unary_op_count>
	{
"PreIncrement"sv, "PreDecrement"sv, "PostIncrement"sv, "PostDecrement"sv,
"UnaryPlus"sv, "UnaryMinus"sv, "BitwiseNot"sv, "BoolCast"sv, "LogicalNegation"sv
	};
	constexpr auto un_op_name_lookup_wide = std::array<wsv_t, unary_op_count>
	{
L"PreIncrement"sv, L"PreDecrement"sv, L"PostIncrement"sv, L"PostDecrement"sv,
L"UnaryPlus"sv, L"UnaryMinus"sv, L"BitwiseNot"sv, L"BoolCast"sv, L"LogicalNegation"sv
	};
	constexpr auto un_op_name_lookup_u8 = std::array<u8sv_t, unary_op_count>
	{
u8"PreIncrement"sv, u8"PreDecrement"sv, u8"PostIncrement"sv, u8"PostDecrement"sv,
u8"UnaryPlus"sv, u8"UnaryMinus"sv, u8"BitwiseNot"sv, u8"BoolCast"sv, u8"LogicalNegation"sv
	};
	constexpr auto un_op_name_lookup_u16 = std::array<u16sv_t, unary_op_count>
	{
u"PreIncrement"sv, u"PreDecrement"sv, u"PostIncrement"sv, u"PostDecrement"sv,
u"UnaryPlus"sv, u"UnaryMinus"sv, u"BitwiseNot"sv, u"BoolCast"sv, u"LogicalNegation"sv
	};
	constexpr auto un_op_name_lookup_u32 = std::array<u32sv_t, unary_op_count>
	{
U"PreIncrement"sv, U"PreDecrement"sv, U"PostIncrement"sv, U"PostDecrement"sv,
U"UnaryPlus"sv, U"UnaryMinus"sv, U"BitwiseNot"sv, U"BoolCast"sv, U"LogicalNegation"sv
	};
	constexpr auto un_op_symbol_lookup = std::array<sv_t, unary_op_count>
	{
		"++x"sv, "--x"sv, "x++"sv, "x--"sv,
		"+x"sv, "-x"sv, "~x"sv, "static_cast<bool>(x)"sv, "!x"sv
	};
	constexpr auto un_op_symbol_lookup_wide = std::array<wsv_t, unary_op_count>
	{
	L"++x"sv, L"--x"sv, L"x++"sv, L"x--"sv,
	L"+x"sv, L"-x"sv, L"~x"sv, L"static_cast<bool>(x)"sv, L"!x"sv
	};
	constexpr auto un_op_symbol_lookup_u8 = std::array<u8sv_t, unary_op_count>
	{
	u8"++x"sv, u8"--x"sv, u8"x++"sv, u8"x--"sv,
	u8"+x"sv, u8"-x"sv, u8"~x"sv, u8"static_cast<bool>(x)"sv, u8"!x"sv
	};
	constexpr auto un_op_symbol_lookup_u16 = std::array<u16sv_t, unary_op_count>
	{
	u"++x"sv, u"--x"sv, u"x++"sv, u"x--"sv,
	u"+x"sv, u"-x"sv, u"~x"sv, u"static_cast<bool>(x)"sv, u"!x"sv
	};
	constexpr auto un_op_symbol_lookup_u32 = std::array<u32sv_t, unary_op_count>
	{
	U"++x"sv, U"--x"sv, U"x++"sv, U"x--"sv,
	U"+x"sv, U"-x"sv, U"~x"sv, U"static_cast<bool>(x)"sv, U"!x"sv
	};
	constexpr auto op_name_lookup = std::array<sv_t, binary_op_count>{
		"LeftShift"sv, "RightShift"sv, "And"sv, "Or"sv,
			"Xor"sv, "Divide"sv, "Modulus"sv, "Add"sv,
			"Subtract"sv, "Multiply"sv, "Compare"sv};
	constexpr auto op_symbol_lookup = std::array<sv_t, binary_op_count>{
		"<<"sv, ">>"sv, "&"sv, "|"sv,
			"^"sv, "/"sv, "%"sv, "+"sv,
			"-"sv, "*"sv, "<=>"sv};
	constexpr auto op_name_lookup_wide = std::array<wsv_t, binary_op_count>{
		L"LeftShift"sv, L"RightShift"sv, L"And"sv, L"Or"sv,
			L"Xor"sv, L"Divide"sv, L"Modulus"sv, L"Add"sv,
			L"Subtract"sv, L"Multiply"sv, L"Compare"sv};
	constexpr auto op_symbol_lookup_wide = std::array<wsv_t, binary_op_count>{
		L"<<"sv, L">>"sv, L"&"sv, L"|"sv,
			L"^"sv, L"/"sv, L"%"sv, L"+"sv,
			L"-"sv, L"*"sv, L"<=>"sv};
	constexpr auto op_name_lookup_u8 = std::array<u8sv_t, binary_op_count>{
		u8"LeftShift"sv, u8"RightShift"sv, u8"And"sv, u8"Or"sv,
			u8"Xor"sv, u8"Divide"sv, u8"Modulus"sv, u8"Add"sv,
			u8"Subtract"sv, u8"Multiply"sv, u8"Compare"sv};
	constexpr auto op_symbol_lookup_u8 = std::array<u8sv_t, binary_op_count>{
		u8"<<"sv, u8">>"sv, u8"&"sv, u8"|"sv,
			u8"^"sv, u8"/"sv, u8"%"sv, u8"+"sv,
			u8"-"sv, u8"*"sv, u8"<=>"sv};
	constexpr auto op_name_lookup_u16 = std::array<u16sv_t, binary_op_count>{
		u"LeftShift"sv, u"RightShift"sv, u"And"sv, u"Or"sv,
			u"Xor"sv, u"Divide"sv, u"Modulus"sv, u"Add"sv,
			u"Subtract"sv, u"Multiply"sv, u"Compare"sv};
	constexpr auto op_symbol_lookup_u16 = std::array<u16sv_t, binary_op_count>{
		u"<<"sv, u">>"sv, u"&"sv, u"|"sv,
			u"^"sv, u"/"sv, u"%"sv, u"+"sv,
			u"-"sv, u"*"sv, u"<=>"sv};
	constexpr auto op_name_lookup_u32 = std::array<u32sv_t, binary_op_count>{
		U"LeftShift"sv, U"RightShift"sv, U"And"sv, U"Or"sv,
			U"Xor"sv, U"Divide"sv, U"Modulus"sv, U"Add"sv,
			U"Subtract"sv, U"Multiply"sv, U"Compare"sv};
	constexpr auto op_symbol_lookup_u32 = std::array<u32sv_t, binary_op_count>{
		U"<<"sv, U">>"sv, U"&"sv, U"|"sv,
			U"^"sv, U"/"sv, U"%"sv, U"+"sv,
			U"-"sv, U"*"sv, U"<=>"sv};

	
	constexpr sv_t get_op_text(binary_op op);
	constexpr wsv_t get_op_wtext(binary_op op);
	constexpr u8sv_t get_op_u8text(binary_op op);
	constexpr u16sv_t get_op_u16text(binary_op op);
	constexpr u32sv_t get_op_u32text(binary_op op);

	constexpr sv_t get_un_op_text(unary_op op);
	constexpr wsv_t get_un_op_wtext(unary_op op);
	constexpr u8sv_t get_un_op_u8text(unary_op op);
	constexpr u16sv_t get_un_op_u16text(unary_op op);
	constexpr u32sv_t get_un_op_u32text(unary_op op);

	constexpr sv_t get_un_op_symbol_n(unary_op op);
	constexpr wsv_t get_un_op_symbol_w(unary_op op);
	constexpr u8sv_t get_un_op_symbol_u8(unary_op op);
	constexpr u16sv_t get_un_op_symbol_u16(unary_op op);
	constexpr u32sv_t get_un_op_symbol_u32(unary_op op);

	constexpr sv_t get_op_symbol_n(binary_op op);
	constexpr wsv_t get_op_symbol_w(binary_op op);
	constexpr u8sv_t get_op_symbol_u8(binary_op op);
	constexpr u16sv_t get_op_symbol_u16(binary_op op);
	constexpr u32sv_t get_op_symbol_u32(binary_op op);

	template<numerics::concepts::character Char>
	constexpr auto get_op_symbol(binary_op op) -> std::basic_string_view<Char>;

	template<>
	constexpr auto get_op_symbol<char>(binary_op op) -> std::basic_string_view<char>
	{
		return get_op_symbol_n(op);
	}
	template<>
	constexpr auto get_op_symbol<wchar_t>(binary_op op) -> std::basic_string_view<wchar_t>
	{
		return get_op_symbol_w(op);
	}

	template<>
	constexpr auto get_op_symbol<char8_t>(binary_op op) -> std::basic_string_view<char8_t>
	{
		return get_op_symbol_u8(op);
	}

	template<>
	constexpr auto get_op_symbol<char16_t>(binary_op op) -> std::basic_string_view<char16_t>
	{
		return get_op_symbol_u16(op);
	}

	template<>
	constexpr auto get_op_symbol<char32_t>(binary_op op) -> std::basic_string_view<char32_t>
	{
		return get_op_symbol_u32(op);
	}

	template<numerics::concepts::character Char>
	constexpr auto get_un_op_symbol(unary_op op) -> std::basic_string_view<Char>;

	template<>
	constexpr auto get_un_op_symbol<char>(unary_op op) -> std::basic_string_view<char>
	{
		return get_un_op_symbol_n(op);
	}
	template<>
	constexpr auto get_un_op_symbol<wchar_t>(unary_op op) -> std::basic_string_view<wchar_t>
	{
		return get_un_op_symbol_w(op);
	}

	template<>
	constexpr auto get_un_op_symbol<char8_t>(unary_op op) -> std::basic_string_view<char8_t>
	{
		return get_un_op_symbol_u8(op);
	}

	template<>
	constexpr auto get_un_op_symbol<char16_t>(unary_op op) -> std::basic_string_view<char16_t>
	{
		return get_un_op_symbol_u16(op);
	}

	template<>
	constexpr auto get_un_op_symbol<char32_t>(unary_op op) -> std::basic_string_view<char32_t>
	{
		return get_un_op_symbol_u32(op);
	}

	unary_op parse_unary_op_symbol(sv_t text);
	unary_op parse_unary_op_symbol(wsv_t text);
	unary_op parse_unary_op_symbol(u8sv_t text);
	unary_op parse_unary_op_symbol(u16sv_t text);
	unary_op parse_unary_op_symbol(u32sv_t text);

	binary_op parse_binary_op_symbol(sv_t text);
	binary_op parse_binary_op_symbol(wsv_t text);
	binary_op parse_binary_op_symbol(u8sv_t text);
	binary_op parse_binary_op_symbol(u16sv_t text);
	binary_op parse_binary_op_symbol(u32sv_t text);

	template<numerics::concepts::character Char>
	std::basic_ostream<Char, std::char_traits<Char>>& operator<<(std::basic_ostream<Char, std::char_traits<Char>>& os, binary_op op)
	{
		std::basic_string_view<Char> sv = get_op_symbol<std::remove_cvref_t<std::remove_const_t<Char>>>(op);
		os << sv;
		return os;
	}

	template<numerics::concepts::character Char>
	std::basic_istream<Char, std::char_traits<Char>>& operator>>(std::basic_istream<Char,
			std::char_traits<Char>>& is, binary_op& op)
	{
		using string_t = std::basic_string<Char>;
		using lsv_t = std::basic_string_view<Char>;
		string_t temp;
		if constexpr (cjm::numerics::is_windows || !cjm::numerics::concepts::utf_character<Char>)
		{
			is >> temp;
		}
		else
		{

			Char c{};
			do
			{
				if (is.good() && !is.eof() && is.peek() != std::char_traits<Char>::eof())
				{
					is.get(c);
					if (!is.bad() && !is.fail())
					{
						if (!std::isspace<char>(convert_char<Char, char>(c), std::locale("")))
							temp.push_back(c);
						else
							break;
					}
				}
				else
					break;
			} while (!is.bad() && !is.fail() && !is.eof());
			if (is.bad() || is.fail())
			{
				return is;
			}
			if (temp.empty())
			{
				is.setstate(std::ios_base::failbit);
				return is;
			}
		}
		op = parse_binary_op_symbol(lsv_t{temp});
		return is;
	}

	template<numerics::concepts::character Char>
	std::basic_ostream<Char, std::char_traits<Char>>& operator<<(std::basic_ostream<Char, std::char_traits<Char>>& os, unary_op op)
	{
		std::basic_string_view<Char> sv = get_un_op_symbol<std::remove_cvref_t<std::remove_const_t<Char>>>(op);
		os << sv;
		return os;
	}

	template<numerics::concepts::character Char>
	std::basic_istream<Char, std::char_traits<Char>>& operator>>(std::basic_istream<Char,
			std::char_traits<Char>>& is, unary_op& op)
	{
		using string_t = std::basic_string<Char>;
		using lsv_t = std::basic_string_view<Char>;
		string_t temp;
		if constexpr (cjm::numerics::is_windows || !cjm::numerics::concepts::utf_character<Char>)
		{
			is >> temp;
		}
		else
		{

			Char c{};
			do
			{
				if (is.good() && !is.eof() && is.peek() != std::char_traits<Char>::eof())
				{
					is.get(c);
					if (!is.bad() && !is.fail())
					{
						if (!std::isspace<char>(convert_char<Char, char>(c), std::locale("")))
							temp.push_back(c);
						else
							break;
					}
				}
				else
					break;
			} while (!is.bad() && !is.fail() && !is.eof());
			if (is.bad() || is.fail())
			{
				return is;
			}
			if (temp.empty())
			{
				is.setstate(std::ios_base::failbit);
				return is;
			}
		}
		op = parse_unary_op_symbol(lsv_t{temp});
		return is;
	}

	
	
 

	class bad_binary_op final : public std::invalid_argument
	{
	public:
		using stream_t = std::stringstream;
		bad_binary_op(binary_op op)
			: invalid_argument(create_message(op)) {}

		using enum_type_t = std::underlying_type_t<binary_op>;
	private:
		static std::string create_message(binary_op op)
		{
			stream_t ss;
			ss  << "The value underlying the binary_op provided ("
				<< static_cast<enum_type_t>(op)
				<< "), is not a defined member of the binary_op enum class.";
			return ss.str();
		}
	};

	class bad_unary_op final : public std::invalid_argument
	{
	public:
		using stream_t = std::stringstream;
		bad_unary_op(unary_op op)
			: invalid_argument(create_message(op)) {}

		using enum_type_t = std::underlying_type_t<unary_op>;
	private:
		static std::string create_message(unary_op op)
		{
			stream_t ss;
			ss  << "The value underlying the unary_op provided ("
				<< static_cast<enum_type_t>(op)
				<< "), is not a defined member of the unary_op enum class.";
			return ss.str();
		}
	};

	
	template<typename TestType, typename ControlType>
		requires (test_uint_and_control_set<TestType, ControlType>)
	std::basic_ostream<char>& append_static_assertion(std::basic_ostream<char>& strm,
		const binary_operation<TestType, ControlType>& bin_op);

	template<typename TestType, typename ControlType>
		requires (test_uint_and_control_set<TestType, ControlType>)
	struct binary_operation final
	{
		using uint_test_t = std::remove_const_t<TestType>;
		using uint_ctrl_t = std::remove_const_t<ControlType>;
		using result_t = std::optional<std::pair<uint_test_t, uint_test_t>>;
		using compound_result_t = std::optional<uint_test_t>;
		[[nodiscard]] std::size_t hash_value() const noexcept
		{
			std::size_t seed = 0x1FBB0493;
			boost::hash_combine(seed, static_cast<size_t>(m_op));
			boost::hash_combine(seed, std::hash<uint_test_t>{}(m_lhs));
			boost::hash_combine(seed, std::hash<uint_test_t>{}(m_rhs));
			return seed;
		}

		friend bool operator<(const binary_operation& lhs, const binary_operation& rhs)
		{
			if (lhs.m_op < rhs.m_op)
				return true;
			if (rhs.m_op < lhs.m_op)
				return false;
			if (lhs.m_rhs < rhs.m_rhs)
				return true;
			if (rhs.m_rhs < lhs.m_rhs)
				return false;
			return lhs.m_lhs < rhs.m_lhs;
		}

		friend bool operator<=(const binary_operation& lhs, const binary_operation& rhs) { return !(rhs < lhs); }

		friend bool operator>(const binary_operation& lhs, const binary_operation& rhs) { return rhs < lhs; }

		friend bool operator>=(const binary_operation& lhs, const binary_operation& rhs) { return !(lhs < rhs); }

		friend bool operator==(const binary_operation& lhs, const binary_operation& rhs)
		{
			return lhs.m_op == rhs.m_op
				&& lhs.m_rhs == rhs.m_rhs
				&& lhs.m_lhs == rhs.m_lhs;
		}

		friend bool operator!=(const binary_operation& lhs, const binary_operation& rhs) { return !(lhs == rhs); }
		friend std::weak_ordering operator <=>(const binary_operation& lhs, const binary_operation& rhs) = default;

		[[nodiscard]] const compound_result_t& compound_result() const noexcept {return m_compound_result;}
		[[nodiscard]] binary_op op_code() const noexcept { return m_op; }
		[[nodiscard]] const uint_test_t& left_operand() const noexcept { return m_lhs; }
		[[nodiscard]] const uint_test_t& right_operand() const noexcept { return m_rhs; }
		[[nodiscard]] const result_t& result() const noexcept { return m_result; }
		[[nodiscard]] bool has_result() const noexcept { return m_result.has_value(); }
		[[nodiscard]] bool has_correct_result() const
		{
			if (m_result.has_value())
			{
				if (m_result->first == m_result->second)
				{
					return m_op == binary_op::compare ||
						(m_compound_result.has_value() &&
						*m_compound_result == m_result->first);
				}
				return false;
			}
			return false;
		}

		binary_operation() noexcept : m_op(), m_lhs{ 0 }, m_rhs{ 0 }, m_result{}, m_compound_result{} {}

		binary_operation(binary_op op, const uint_test_t& first_operand, const uint_test_t& second_operand, bool calculate_now) 
			: m_op(op), m_lhs(first_operand), m_rhs(second_operand), m_result{}, m_compound_result{}
		{
			validate(op, first_operand, second_operand);
			if (calculate_now)
			{
				auto [result, comp_res] = perform_calculate_result(first_operand, second_operand, m_op);
				m_result = result;
				m_compound_result = comp_res;
			}
		}
		
		binary_operation(binary_op op, const uint_test_t& first_operand, 
			const uint_test_t& second_operand) : m_op{ op }, m_lhs{ first_operand },
			m_rhs{ second_operand }, m_result{}, m_compound_result{}
		{
		   validate(op, first_operand, second_operand);
		}
		binary_operation(binary_op op, const uint_test_t& first_operand, 
			const uint_test_t& second_operand, const uint_test_t& test_result, 
				const uint_test_t& ctrl_result, std::optional<uint_test_t> compound_res) 
					: m_op(op), m_lhs(first_operand), m_rhs(second_operand),
						m_result(std::make_pair(test_result, ctrl_result)), 
							m_compound_result{compound_res}
		{
			validate(op, first_operand, second_operand);
		}
		binary_operation(const binary_operation& other) noexcept = default;
		binary_operation(binary_operation&& other) noexcept = default;
		binary_operation& operator=(const binary_operation& other) noexcept = default;
		binary_operation& operator=(binary_operation&& other) noexcept = default;
		~binary_operation() = default;

		void calculate_result() { do_calculate_result(); }

	private:

		static auto to_control(const uint_test_t& test)-> uint_ctrl_t
		{
			uint_ctrl_t ctrl = test.high_part();
			ctrl <<= std::numeric_limits<typename uint_test_t::int_part>::digits;
			ctrl |= test.low_part();
			return ctrl;
		}

		static uint_test_t to_test(const uint_ctrl_t& ctrl)
		{
			uint_test_t high = static_cast<typename uint_test_t::int_part>(ctrl >> std::numeric_limits<typename uint_test_t::int_part>::digits);
			high <<= std::numeric_limits<typename uint_test_t::int_part>::digits;
			uint_test_t low = static_cast<typename uint_test_t::int_part>(ctrl);
			return high | low;
		}

		bool do_calculate_result()
		{
			auto left_copy = m_lhs;
			auto right_copy = m_rhs;
			auto [test_res, ctrl_res, comp_res] = perform_calculate_result(left_copy, right_copy, m_op);
			auto result = std::make_pair(test_res, ctrl_res);
			const bool changed_value = m_result != result || comp_res != m_compound_result;
			m_result = result;
			m_compound_result = comp_res;
			return changed_value;
		}
		static std::tuple<uint_test_t, uint_test_t, std::optional<uint_test_t>> perform_calculate_result(const uint_test_t& lhs, const uint_test_t& rhs, binary_op op)
		{
			assert(static_cast<size_t>(op) < op_name_lookup.size());
			uint_test_t ret_tst = 0;
			uint_ctrl_t ret_ctrl = 0;
			uint_ctrl_t lhs_ctrl = to_control(lhs);
			uint_ctrl_t rhs_ctrl = to_control(rhs);
			uint_test_t lhs_copy = lhs;
			std::optional<uint_test_t> compound_res = std::nullopt;
			switch (op)
			{
			case binary_op::left_shift:
				ret_tst = lhs << static_cast<int>(rhs);
				lhs_copy <<= static_cast<int>(rhs);
				compound_res = lhs_copy;
				ret_ctrl = lhs_ctrl << static_cast<int>(rhs);
				break;
			case binary_op::right_shift:
				ret_tst = lhs >> static_cast<int>(rhs_ctrl);
				lhs_copy >>= static_cast<int>(rhs);
				compound_res = lhs_copy;
				ret_ctrl = lhs_ctrl >> static_cast<int>(rhs_ctrl);
				break;
			case binary_op::bw_and:
				ret_tst = lhs & rhs;
				lhs_copy &= rhs;
				compound_res = lhs_copy;
				ret_ctrl = lhs_ctrl & rhs_ctrl;
				break;
			case binary_op::bw_or:
				ret_tst = lhs | rhs;
				lhs_copy |= rhs;
				compound_res = lhs_copy;
				ret_ctrl = lhs_ctrl | rhs_ctrl;
				break;
			case binary_op::bw_xor:
				ret_tst = lhs ^ rhs;
				lhs_copy ^= rhs;
				compound_res = lhs_copy;
				ret_ctrl = lhs_ctrl ^ rhs_ctrl;
				break;
			case binary_op::divide:
				ret_tst = lhs / rhs;
				lhs_copy /= rhs;
				compound_res = lhs_copy;
				ret_ctrl = lhs_ctrl / rhs_ctrl;
				break;
			case binary_op::modulus:
				ret_tst = lhs % rhs;
				lhs_copy %= rhs;
				compound_res = lhs_copy;
				ret_ctrl = lhs_ctrl % rhs_ctrl;
				break;
			case binary_op::add:
				ret_tst = lhs + rhs;
				lhs_copy += rhs;
				compound_res = lhs_copy;
				ret_ctrl = lhs_ctrl + rhs_ctrl;
				break;
			case binary_op::subtract:
				ret_tst = lhs - rhs;
				lhs_copy -= rhs;
				compound_res = lhs_copy;
				ret_ctrl = lhs_ctrl - rhs_ctrl;
				break;
			case binary_op::multiply:
				ret_tst = lhs * rhs;
				lhs_copy *= rhs;
				compound_res = lhs_copy;
				ret_ctrl = lhs_ctrl * rhs_ctrl;
				break;
			case binary_op::compare:
				if (lhs_ctrl == rhs_ctrl)
				{
					ret_ctrl = 0;
				}
				else
				{
					ret_ctrl = lhs_ctrl > rhs_ctrl ? 1 : std::numeric_limits<uint_ctrl_t>::max();
				}
				if (lhs == rhs)
				{
					ret_tst = 0;
				}
				else
				{
					ret_tst = lhs > rhs ? 1 : u128_testing_constant_providers::testing_constant_provider<uint_test_t>::maximum;
				}
				break;
			}
			return std::make_tuple(ret_tst, to_test(ret_ctrl), compound_res);
		}

		static void validate(binary_op op, [[maybe_unused]] const uint_test_t& lhs, const uint_test_t& rhs)
		{
			if (op == binary_op::divide || op == binary_op::modulus)
			{
				if (rhs == 0) throw std::invalid_argument{"Zero is not a permissible divisor for a division or modulus operation and may cause undefined behavior with the control type."};
			}
			if (op == binary_op::left_shift || op == binary_op::right_shift)
			{
				size_t test_digits = std::numeric_limits<uint_test_t >::digits;
				size_t ctrl_digits = std::numeric_limits<uint_ctrl_t>::digits;
				uint_ctrl_t rhs_ctrl = to_control(rhs);
				if (rhs_ctrl >= ctrl_digits || rhs >= test_digits)
				{
					auto ss = cjm::string::make_throwing_sstream<char>();
					ss  << "The second operand: [" << rhs << "] is not a valid right hand "
						<< "operand for a shift operation: shifting by an amount greater than or "
						<< "equal to [" << test_digits << "] for test value or ["
						<< ctrl_digits << "] for the control value may cause undefined behavior." << newl;
					throw std::invalid_argument{ss.str()};
				}

			}
		}

		binary_op m_op;
		uint_test_t m_lhs;
		uint_test_t m_rhs;
		result_t m_result;   
		compound_result_t m_compound_result;
	};

	template<typename TestType, typename ControlType>
		requires (test_uint_and_control_set<TestType, ControlType>)
	std::basic_ostream<char>& append_static_assertion(std::basic_ostream<char>& strm,
		const unary_operation<TestType, ControlType>& bin_op);

	template<typename TestType, typename ControlType>
		requires (test_uint_and_control_set<TestType, ControlType>)
	struct unary_operation final
	{
	   using uint_test_t = std::remove_const_t<TestType>;
	   using uint_ctrl_t = std::remove_const_t<ControlType>;
	   using result_t = std::optional<std::pair<uint_test_t, uint_test_t>>;

		[[nodiscard]] std::size_t hash_value() const noexcept
		{
			std::size_t seed = 0x1FBB0493;
			boost::hash_combine(seed, static_cast<size_t>(m_op));
			boost::hash_combine(seed, std::hash<uint_test_t>{}(m_operand));
			return seed;
		}

		friend bool operator==(const unary_operation& lhs, const unary_operation& rhs)
		{
			return lhs.m_op == rhs.m_op && lhs.m_operand == rhs.m_operand;
		}

		friend bool operator<(const unary_operation &lhs, const unary_operation &rhs)
		{
			if (lhs.m_op < rhs.m_op)
				return true;
			if (rhs.m_op < lhs.m_op)
				return false;
			return lhs.m_operand < rhs.m_operand;
		}

		friend bool operator>(const unary_operation& lhs,
			const unary_operation& rhs) { return rhs < lhs; }
		friend bool operator<=(const unary_operation& lhs,
			const unary_operation& rhs) {return !(rhs < lhs);}
		friend bool operator>=(const unary_operation& lhs,
			const unary_operation& rhs)  {return !(lhs < rhs);}
		friend bool operator!=(const unary_operation& lhs,
			const unary_operation& rhs) { return !(rhs == lhs); }
		friend std::weak_ordering operator <=>(const unary_operation& lhs,
			const unary_operation& rhs) = default;

		[[nodiscard]] unary_op op_code() const noexcept { return m_op; }
		[[nodiscard]] const uint_test_t& operand() const noexcept { return m_operand; }
		[[nodiscard]] const result_t& result() const noexcept { return m_result; }
		[[nodiscard]] const result_t& post_result() const noexcept {return m_post_result;}
		[[nodiscard]] bool has_result() const noexcept { return m_result.has_value(); }
		[[nodiscard]] bool has_post_result() const noexcept {return m_post_result.has_value();}
		[[nodiscard]] bool has_correct_result() const
		{
			if (m_result.has_value())
			{
				if (m_result->first == m_result->second)
				{
					const bool should_have_post_res = m_op == unary_op::post_increment
							|| m_op == unary_op::post_decrement;
					if (should_have_post_res == m_post_result.has_value())
					{
						return !should_have_post_res ||
							m_post_result.value().first == m_post_result->second;
					}
					return false;
				}
				return false;
			}
			return false;
		}

		unary_operation(unary_op op_code, const uint_test_t& operand) noexcept
			: m_op{op_code}, m_operand{operand}, m_result{}, m_post_result{} {}
		unary_operation() : unary_operation(unary_op::pre_increment, 0_u128) {}
		unary_operation(const unary_operation& other) noexcept = default;
		unary_operation(unary_operation&& other) noexcept = default;
		unary_operation& operator=(const unary_operation& other) noexcept = default;
		unary_operation& operator=(unary_operation&& other) noexcept = default;
		~unary_operation() = default;

		bool calculate_result()
		{
			if (has_correct_result())
			{
				return true;
			}
			if (do_calculation())
			{
				const bool should_have_post_res = m_op == unary_op::post_increment
												  || m_op == unary_op::post_decrement;
				return	  m_result.has_value()
						&&  m_result->first == m_result->second
						&&  should_have_post_res == m_post_result.has_value()
						&& (	!m_post_result.has_value() ||
								m_post_result.value().first == m_post_result->second);
			}
			return false;
		}

	private:

		bool do_calculation()
		{
			auto operand = m_operand;
			auto op = m_op;
			auto [res, post_res] = perform_calculate_result(op, operand);
			const bool changed = m_result != res || m_post_result != post_res;
			m_result = res;
			m_post_result = post_res;
			return changed;
		}

		static auto to_control(const uint_test_t& test)-> uint_ctrl_t
		{
			uint_ctrl_t ctrl = test.high_part();
			ctrl <<= std::numeric_limits<typename uint_test_t::int_part>::digits;
			ctrl |= test.low_part();
			return ctrl;
		}


		static uint_test_t to_test(const uint_ctrl_t& ctrl)
		{
			uint_test_t high = static_cast<typename uint_test_t::int_part>(ctrl >> std::numeric_limits<typename uint_test_t::int_part>::digits);
			high <<= std::numeric_limits<typename uint_test_t::int_part>::digits;
			uint_test_t low = static_cast<typename uint_test_t::int_part>(ctrl);
			return high | low;
		}

		static std::pair<result_t, result_t> perform_calculate_result(unary_op op, uint_test_t test_operand)
		{
			uint_ctrl_t ctrl_operand = to_control(test_operand);
			uint_test_t test_result;
			uint_ctrl_t ctrl_result;
			std::optional<uint_test_t> test_post_result;
			std::optional<uint_ctrl_t> ctrl_post_result;
			switch (op)
			{

				case unary_op::pre_increment:
					ctrl_result = ++ctrl_operand;
					test_result = ++test_operand;
					test_post_result = std::nullopt;
					ctrl_post_result = std::nullopt;
					break;
				case unary_op::pre_decrement:
					ctrl_result = --ctrl_operand;
					test_result = --test_operand;
					test_post_result = std::nullopt;
					ctrl_post_result = std::nullopt;
					break;
				case unary_op::post_increment:
					ctrl_result = ctrl_operand++;
					ctrl_post_result = ctrl_operand;
					test_result = test_operand++;
					test_post_result = test_operand;
					break;
				case unary_op::post_decrement:
					ctrl_result = ctrl_operand--;
					ctrl_post_result = ctrl_operand;
					test_result = test_operand--;
					test_post_result = test_operand;
					break;
				case unary_op::unary_plus:
					ctrl_result = +ctrl_operand;
					test_result = +test_operand;
					test_post_result = std::nullopt;
					ctrl_post_result = std::nullopt;
					break;
				case unary_op::unary_minus:
					ctrl_result = ((~ctrl_operand)+1);
					test_result = -test_operand;
					test_post_result = std::nullopt;
					ctrl_post_result = std::nullopt;
					break;
				case unary_op::bitwise_not:
					ctrl_result = ~ctrl_operand;
					test_result = ~test_operand;
					test_post_result = std::nullopt;
					ctrl_post_result = std::nullopt;
					break;
				case unary_op::bool_cast:
					ctrl_result = static_cast<uint_ctrl_t>(static_cast<bool>(ctrl_operand) ? 1 : 0);
					test_result = static_cast<uint_test_t>(static_cast<bool>(test_operand) ? 1 : 0);
					test_post_result = std::nullopt;
					ctrl_post_result = std::nullopt;
					break;
				case unary_op::logical_negation:
					ctrl_result = static_cast<uint_ctrl_t>((!ctrl_operand) ? 1 : 0);
					test_result = static_cast<uint_test_t>((!test_operand) ? 1 : 0);
					test_post_result = std::nullopt;
					ctrl_post_result = std::nullopt;
					break;
			}
			assert(test_post_result.has_value() == ctrl_post_result.has_value());
			if (test_post_result.has_value())
			{
				auto post_res = std::make_pair(test_post_result.value(), to_test(ctrl_post_result.value()));
				auto res = std::make_pair(test_result, to_test(ctrl_result));
				return std::make_pair(res, post_res);
			}
			return std::make_pair(std::make_pair(test_result, to_test(ctrl_result)), std::nullopt);
		}

		unary_op m_op;
		uint_test_t m_operand;
		result_t m_result;
		result_t m_post_result;
	};

	template<numerics::concepts::character Char>
	std::basic_ostream<Char, std::char_traits<Char>>& operator<<(std::basic_ostream<Char,
		std::char_traits<Char>>&os, const unary_op_u128_t& op)
	{
		using char_t = std::remove_const_t<Char>;

		char_t item_separator;
		if constexpr (std::is_same_v<char_t, char>)
		{
			item_separator = ';';
		}
		else
		{
			item_separator = convert_char<char, char_t>(';');
		}

		os << op.op_code() << item_separator;
		os << op.operand() << item_separator;
		return os;
	}
	
	template<numerics::concepts::character Char>
	std::basic_ostream<Char, std::char_traits<Char>>& operator<<(std::basic_ostream<Char,
		std::char_traits<Char>>&os, const binary_op_u128_t& op)
	{
		using char_t = std::remove_const_t<Char>;
			
		char_t item_separator;
		if constexpr (std::is_same_v<char_t, char>)
		{
			item_separator = ';';
		}
		else
		{
			item_separator = convert_char<char, char_t>(';');
		}

		os << op.op_code() << item_separator;
		os << op.left_operand() << item_separator;
		os << op.right_operand() << item_separator;
		return os;			
	}

	template<numerics::concepts::character Char>
	std::basic_ostream<Char, std::char_traits<Char>>& operator<<(std::basic_ostream<Char,
		std::char_traits<Char>>&os, const binary_op_u128_vect_t& op)
	{
		if (op.empty())
			return os;
		using char_t = std::remove_const_t<Char>;
		char_t item_separator;
		if constexpr (std::is_same_v<char_t, char>)
		{
			item_separator = '\n';
		}
		else
		{
			item_separator = convert_char<char, char_t>('\n');
		}

		for (const auto& itm : op)
		{
			os << itm << item_separator;
		}
		return os;
	}

	template<numerics::concepts::character Char>
	std::basic_ostream<Char, std::char_traits<Char>>& operator<<(std::basic_ostream<Char,
		std::char_traits<Char>>&os, const unary_op_u128_vect_t& op)
	{
		if (op.empty())
			return os;
		using char_t = std::remove_const_t<Char>;
		char_t item_separator;
		if constexpr (std::is_same_v<char_t, char>)
		{
			item_separator = '\n';
		}
		else
		{
			item_separator = convert_char<char, char_t>('\n');
		}

		for (const auto& itm : op)
		{
			os << itm << item_separator;
		}
		return os;
	}
	

	template<numerics::concepts::character Char>
	std::basic_istream<Char, std::char_traits<Char>>& operator>>(std::basic_istream<Char,
		std::char_traits<Char>>&is, binary_op_u128_vect_t& op)
	{

		using char_t = std::remove_const_t<Char>;
		using string_t = std::basic_string<char_t, std::char_traits<char_t>>;
		using lsv_t = std::basic_string_view<char_t, std::char_traits<char_t>>;

		char_t item_separator;
		if constexpr (std::is_same_v<char_t, char>)
		{
			item_separator = '\n';
		}
		else
		{
			item_separator = convert_char<char, char_t>('\n');
		}
		string_t temp;

		while (is.good() && !is.bad() && !is.fail() && !is.eof() && is.peek() != std::char_traits<char_t>::eof())
		{
			temp.clear();
			std::getline(is, temp, item_separator);
			if (is.eof() || is.bad() || is.fail())
			{
				return is;
			}
			lsv_t temp_view = temp;
			temp_view = cjm::string::trim_as_sv(temp_view);
			if (temp_view.empty())
			{
				is.setstate(std::ios_base::failbit);
				return is;
			}

			try
			{
				op.emplace_back(parse<char_t>(temp_view));
			}
			catch (const std::exception& ex)
			{
				std::cerr << "Failure to parse string.  Msg: [" << ex.what() << "].";
				is.setstate(std::ios_base::failbit);
				throw;
			}
		}
		return is;
	}

	template<numerics::concepts::character Char>
	std::basic_istream<Char, std::char_traits<Char>>& operator>>(std::basic_istream<Char,
		std::char_traits<Char>>&is, unary_op_u128_vect_t& op)
	{

		using char_t = std::remove_const_t<Char>;
		using string_t = std::basic_string<char_t, std::char_traits<char_t>>;
		using lsv_t = std::basic_string_view<char_t, std::char_traits<char_t>>;

		char_t item_separator;
		if constexpr (std::is_same_v<char_t, char>)
		{
			item_separator = '\n';
		}
		else
		{
			item_separator = convert_char<char, char_t>('\n');
		}
		string_t temp;

		while (is.good() && !is.bad() && !is.fail() && !is.eof() && is.peek() != std::char_traits<char_t>::eof())
		{
			temp.clear();
			std::getline(is, temp, item_separator);
			if (is.eof() || is.bad() || is.fail())
			{
				return is;
			}
			lsv_t temp_view = temp;
			temp_view = cjm::string::trim_as_sv(temp_view);
			if (temp_view.empty())
			{
				is.setstate(std::ios_base::failbit);
				return is;
			}

			try
			{
				op.emplace_back(parse_unary<char_t>(temp_view));
			}
			catch (const std::exception& ex)
			{
				std::cerr << "Failure to parse string.  Msg: [" << ex.what() << "].";
				is.setstate(std::ios_base::failbit);
				throw;
			}
		}
		return is;
	} 
	
	template<numerics::concepts::character Char>
	std::basic_istream<Char, std::char_traits<Char>>& operator>>(std::basic_istream<Char,
	std::char_traits<Char>>&is, binary_op_u128_t& op)
	{
		using char_t = std::remove_const_t<Char>;
		using lstr_t = std::basic_string<Char>;
		using lsv_t = std::basic_string<Char>;
		op = binary_op_u128_t{};
		char_t item_separator;
		char_t line_separator;
		if constexpr (std::is_same_v<char_t, char>)
		{
			item_separator = ';';
			line_separator = '\n';
		}
		else
		{
			item_separator = convert_char<char, char_t>(';');
			line_separator = convert_char<char, char_t>('\n');
		}

		if (!is.good() || is.bad() || is.fail() || is.eof() || is.peek() == std::char_traits<char_t>::eof())
		{
			if (is.fail() && !is.bad())
				is.setstate(std::ios_base::failbit | std::ios_base::badbit);
			else
				is.setstate(std::ios_base::failbit);
			return is;
		}

		auto str = lstr_t{};
		std::getline(is, str, line_separator);
		if (str.empty() || is.bad() || is.fail())
		{
			is.setstate(std::ios_base::failbit);
			return is;
		}

		try
		{
			lsv_t txt = str;
			op = parse<char_t>(txt);
		}
		catch (const std::exception& ex)
		{
			std::cerr << "Failure to parse string.  Msg: [" << ex.what() << "].";
			is.setstate(std::ios_base::failbit);
			throw;
		}
		/*is.setstate(std::ios_base::failbit);*/
		return is;		
	}

	template<numerics::concepts::character Char>
	std::basic_istream<Char, std::char_traits<Char>>& operator>>(std::basic_istream<Char,
		std::char_traits<Char>>&is, unary_op_u128_t& op)
	{
		using char_t = std::remove_const_t<Char>;
		using lstr_t = std::basic_string<Char>;
		using lsv_t = std::basic_string<Char>;
		op = unary_op_u128_t{};
		char_t item_separator;
		char_t line_separator;
		if constexpr (std::is_same_v<char_t, char>)
		{
			item_separator = ';';
			line_separator = '\n';
		}
		else
		{
			item_separator = convert_char<char, char_t>(';');
			line_separator = convert_char<char, char_t>('\n');
		}

		if (!is.good() || is.bad() || is.fail() || is.eof() || is.peek() == std::char_traits<char_t>::eof())
		{
			if (is.fail() && !is.bad())
				is.setstate(std::ios_base::failbit | std::ios_base::badbit);
			else
				is.setstate(std::ios_base::failbit);
			return is;
		}

		auto str = lstr_t{};
		std::getline(is, str, line_separator);
		if (str.empty() || is.bad() || is.fail())
		{
			is.setstate(std::ios_base::failbit);
			return is;
		}

		try
		{
			lsv_t txt = str;
			op = parse_unary<char_t>(txt);
		}
		catch (const std::exception& ex)
		{
			std::cerr << "Failure to parse string.  Msg: [" << ex.what() << "].";
			is.setstate(std::ios_base::failbit);
			throw;
		}
		/*is.setstate(std::ios_base::failbit);*/
		return is;
	}

	template<numerics::concepts::character Char>
	unary_op_u128_t parse_unary(std::basic_string_view<Char> sv)
	{
		if (sv.empty()) throw std::invalid_argument{ "The string is empty." };
		using char_t = std::remove_const_t<Char>;
		using lsv_t = std::basic_string_view<char_t>;
		char_t item_separator;
		if constexpr (std::is_same_v<char_t, char>)
		{
			item_separator = ';';
		}
		else
		{
			item_separator = convert_char<char, char_t>(';');
		}
		auto get_up_to_delim = [](lsv_t txt, char_t delim) -> std::pair<lsv_t, lsv_t>
		{
			if (txt.empty())
				return std::make_pair(lsv_t{}, lsv_t{});

			std::size_t first_delim_idx = 0;
			for (char_t c : txt)
			{
				if (c == delim)
				{
					break;
				}
				++first_delim_idx;
			}
			if (first_delim_idx >= txt.size())
			{
				return std::make_pair(lsv_t{}, lsv_t{});
			}
			else if (first_delim_idx == 0)
			{
				return std::make_pair(lsv_t{}, txt.substr(1));
			}
			else
			{   //"2,4"  
				//"<<;1;2"
				lsv_t first = txt.substr(0, first_delim_idx);
				lsv_t second = first_delim_idx + 1 < txt.size() ? txt.substr(first_delim_idx + 1) : lsv_t{};
				return std::make_pair(first, second);
			}
		};
		constexpr auto elements = 2_szt;
		auto temp = std::array<lsv_t, elements>{};
		lsv_t line = sv;
		lsv_t remainder = line;
		size_t added = 0;
		while (added < elements && !remainder.empty())
		{
			auto [symbol, remainder_temp] = get_up_to_delim(remainder, item_separator);
			if (symbol.empty())
			{
				throw std::runtime_error{ "Unable to parse supplied text into a binary operation." };
			}
			temp[added++] = symbol;
			remainder = remainder_temp;
		}
		if (added == elements &&
			std::all_of(temp.cbegin(), temp.cend(),
				[](const lsv_t& t) -> bool {return !t.empty(); }))
		{
			auto op_text = string::trim_as_sv<char_t, std::char_traits<char_t>>(temp[0]);
			auto operand_text = string::trim_as_sv<char_t, std::char_traits<char_t>>(temp[1]);
			
			unary_op op_code = parse_unary_op_symbol(op_text);
			uint128_t operand = uint128_t::make_from_string(operand_text);
			return unary_op_u128_t{ op_code, operand};
		}
		throw std::runtime_error{ "Unable to parse supplied text into a binary operation." };
	}
	
	template<numerics::concepts::character Char>
	binary_op_u128_t parse(std::basic_string_view<Char> sv)
	{
		if (sv.empty()) throw std::invalid_argument{ "The string is empty." };
		using char_t = std::remove_const_t<Char>;
		using lsv_t = std::basic_string_view<char_t>;
		char_t item_separator;
		if constexpr (std::is_same_v<char_t, char>)
		{
			item_separator = ';';			
		}
		else
		{
			item_separator = convert_char<char, char_t>(';');			
		}
				
		auto get_up_to_delim = [](lsv_t txt, char_t delim) -> std::pair<lsv_t, lsv_t>
		{
			if (txt.empty())
				return std::make_pair(lsv_t{}, lsv_t{});

			std::size_t first_delim_idx = 0;
			for (char_t c : txt)
			{
				if (c == delim)
				{
					break;
				}
				++first_delim_idx;
			}
			if (first_delim_idx >= txt.size())
			{
				return std::make_pair(lsv_t{}, lsv_t{});
			}
			else if (first_delim_idx == 0)
			{
				return std::make_pair(lsv_t{}, txt.substr(1));
			}
			else
			{   //"2,4"  
				//"<<;1;2"
				lsv_t first = txt.substr(0, first_delim_idx);
				lsv_t second = first_delim_idx + 1 < txt.size() ? txt.substr(first_delim_idx + 1) : lsv_t{};
				return std::make_pair(first, second);
			}
		};

		constexpr auto elements = 3_szt;
		auto temp = std::array<lsv_t, elements>{};
		lsv_t line = sv;
		lsv_t remainder = line;
		unsigned added = 0;
		while (added < elements && !remainder.empty())
		{
			auto [symbol, remainder_temp] = get_up_to_delim(remainder, item_separator);
			if (symbol.empty())
			{
				throw std::runtime_error{ "Unable to parse supplied text into a binary operation." };
			}
			temp[added++] = symbol;
			remainder = remainder_temp;
		}
		if (added == elements && 
			std::all_of(temp.cbegin(), temp.cend(), 
				[](const lsv_t& t) -> bool {return !t.empty(); }))
		{
			auto op_text = string::trim_as_sv<char_t, std::char_traits<char_t>>(temp[0]);
			auto lhs_text = string::trim_as_sv<char_t, std::char_traits<char_t>>(temp[1]);
			auto rhs_text = string::trim_as_sv<char_t, std::char_traits<char_t>>(temp[2]);
			binary_op op_code = parse_binary_op_symbol(op_text);
			uint128_t lhs = uint128_t::make_from_string(lhs_text);
			uint128_t rhs = uint128_t::make_from_string(rhs_text);
			return binary_op_u128_t{ op_code, lhs, rhs };
		}
		throw std::runtime_error{ "Unable to parse supplied text into a binary operation." };
	}
	
	template <typename TestType, typename ControlType>
		requires (test_uint_and_control_set<TestType, ControlType>)
	std::basic_ostream<char>& append_static_assertion(
		std::basic_ostream<char>& strm, const binary_operation<TestType, ControlType>& bin_op)
	{
		using uint_test_t = typename binary_operation<TestType, ControlType>::uint_test_t;
		auto saver = cout_saver{ strm };
		strm << "static_assert(("
			<< std::dec << bin_op.left_operand() << "_u" << std::dec
			<< std::numeric_limits<uint_test_t>::digits
			<< " " << op_symbol_lookup[static_cast<unsigned>(bin_op.op_code())]
			<< " " << std::dec << bin_op.right_operand() << "_u" << std::dec
			<< std::numeric_limits<uint_test_t>::digits << ") == ";
		if (!bin_op.has_correct_result())
		{
			strm << "UNKNOWN OR INCORRECT VALUE"  << newl;
		}
		else
		{
			auto result = bin_op.result();
			auto value = result.value().first;
			if (bin_op.op_code() == binary_op::compare)
			{
				if (value == 0_u128)
					strm << "std::strong_ordering::equal";
				else if (value == 1_u128)
					strm << "std::strong_ordering::greater";
				else
					strm << "std::strong_ordering::less";
			}
			else
			{
				strm << std::dec << value << "_u" << std::dec
					<< std::numeric_limits<uint_test_t>::digits;
			}
		}
		strm << ");";
		return strm;
	}

	template<typename TestType, typename ControlType>
		requires (test_uint_and_control_set<TestType, ControlType>)
	std::basic_ostream<char>& append_static_assertion(std::basic_ostream<char>& strm, 
		const unary_operation<TestType, ControlType>& un_op)
	{
		auto saver = cout_saver{ strm };
		strm << std::dec;
		using uint_test_t = typename unary_operation<TestType, ControlType>::uint_test_t;
		auto op = un_op.op_code();

		std::string operand;
		{
			auto temp = string::make_throwing_sstream<char>();
			temp << un_op.operand();
			operand = string::trim(temp.str());			
		}

		std::string result;
		auto res = un_op.has_correct_result();
		if (res)
		{
			auto temp = string::make_throwing_sstream<char>();
			temp << un_op.result().value().first;
			result = string::trim(temp.str());
		}
		else
		{
			result = "UNKNOWN OR INCORRECT RESULT";
		}
		

		constexpr size_t digits = std::numeric_limits<uint_test_t>::digits;
		switch (op)
		{
		case unary_op::pre_decrement:
			strm	<< "static_assert((--" << operand << "_u" << std::dec << digits
					<< ") == " << result << "_u" << digits << ");";			
			break;
			//test_post_decrement(operand);
		case unary_op::post_decrement:
			//static_assert((test_post_decrement(0xc0de'd00d_u128)) == std::make_pair(0xc0de'd00d_u128, 0xc0de'd00d_u128 - 1));
			strm	<< "static_assert((test_post_decrement(" << std::dec << operand
					<< "_u" << digits << ")) == std::make_pair(" << operand << "_u"
					<< digits << ", (" << operand << "_u" << digits << " - 1)));";
			break;
		case unary_op::pre_increment:
			strm << "static_assert((++" << operand << "_u" << std::dec << digits
				<< ") == " << result << "_u" << digits << ");";
			break;
		case unary_op::post_increment:			
			strm << "static_assert((test_post_increment(" << std::dec << operand
				<< "_u" << digits << ")) == std::make_pair(" << operand << "_u"
				<< digits << ", (" << operand << "_u" << digits << " + 1)));";
			break;
		case unary_op::unary_plus:
			strm << "static_assert((+" << std::dec << operand << "_u" << digits << ") == " << operand << "_u" << digits << ");";
			break;
		case unary_op::unary_minus:
			strm << "static_assert((-" << std::dec << operand << "_u" << digits << ") == " << result << "_u" << digits << ");";
			break;
		case unary_op::bitwise_not:
			strm << "static_assert((~" << std::dec << operand << "_u" << digits << ") == " << result << "_u" << digits << ");";
			break;
		case unary_op::bool_cast:
			strm << "static_assert(static_cast<bool>(" << std::dec << operand << "_u" << digits << ") == static_cast<bool>(" << result << "_u" << digits << "));";
			break;
		case unary_op::logical_negation:
			strm << "static_assert(static_cast<bool>(!" << std::dec << operand << "_u" << digits << ") == static_cast<bool>(" << result << "_u" << digits << "));";
			break;
		default:
			throw bad_unary_op{ op };
			break;
		}
		return strm;
	}

	class divmod_fail_match final : public testing::testing_failure
	{
		using enum_type_t = std::underlying_type_t<binary_op>;
		using stream_t = std::stringstream;

	public:
		divmod_fail_match(const binary_op_u128_t& failed) : testing_failure(create_message(failed)) {}

	private:
		static std::string create_message(const binary_op_u128_t& bin_op)
		{
			stream_t ss;
			ss << "The result of the " << get_op_text(bin_op.op_code()) << " operation is different when using divmod."
			   << "Dividend: [" << bin_op.left_operand() << "]; Divisor: [" << bin_op.right_operand() << "].";
			return ss.str();
		}
	};
	
}

constexpr cjm::uint128_tests::sv_t cjm::uint128_tests::get_un_op_text(unary_op op)
{
	if (op < first_unary_op || op > last_unary_op) throw bad_unary_op{ op };
	return un_op_name_lookup[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::wsv_t cjm::uint128_tests::get_un_op_wtext(unary_op op)
{
	if (op < first_unary_op || op > last_unary_op) throw bad_unary_op{ op };
	return un_op_name_lookup_wide[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::u8sv_t cjm::uint128_tests::get_un_op_u8text(unary_op op)
{
	if (op < first_unary_op || op > last_unary_op) throw bad_unary_op{ op };
	return un_op_name_lookup_u8[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::u16sv_t cjm::uint128_tests::get_un_op_u16text(unary_op op)
{
	if (op < first_unary_op || op > last_unary_op) throw bad_unary_op{ op };
	return un_op_name_lookup_u16[static_cast<unsigned>(op)];;
}

constexpr cjm::uint128_tests::u32sv_t cjm::uint128_tests::get_un_op_u32text(unary_op op)
{
	if (op < first_unary_op || op > last_unary_op) throw bad_unary_op{ op };
	return un_op_name_lookup_u32[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::sv_t cjm::uint128_tests::get_op_text(binary_op op)
{
	if (op < first_op || op > last_op) throw bad_binary_op{ op };
	return op_name_lookup[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::wsv_t cjm::uint128_tests::get_op_wtext(binary_op op)
{
	if (op < first_op || op > last_op) throw bad_binary_op{ op };
	return op_name_lookup_wide[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::u8sv_t cjm::uint128_tests::get_op_u8text(binary_op op)
{
	if (op < first_op || op > last_op) throw bad_binary_op{ op };
	return op_name_lookup_u8[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::u16sv_t cjm::uint128_tests::get_op_u16text(binary_op op)
{
	if (op < first_op || op > last_op) throw bad_binary_op{ op };
	return op_name_lookup_u16[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::u32sv_t cjm::uint128_tests::get_op_u32text(binary_op op)
{
	if (op < first_op || op > last_op) throw bad_binary_op{ op };
	return op_name_lookup_u32[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::sv_t cjm::uint128_tests::get_op_symbol_n(binary_op op)
{
	if (op < first_op || op > last_op) throw bad_binary_op{ op };
	return op_symbol_lookup[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::wsv_t cjm::uint128_tests::get_op_symbol_w(binary_op op)
{
	if (op < first_op || op > last_op) throw bad_binary_op{ op };
	return op_symbol_lookup_wide[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::u8sv_t cjm::uint128_tests::get_op_symbol_u8(binary_op op)
{
	if (op < first_op || op > last_op) throw bad_binary_op{ op };
	return op_symbol_lookup_u8[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::u16sv_t cjm::uint128_tests::get_op_symbol_u16(binary_op op)
{
	if (op < first_op || op > last_op) throw bad_binary_op{ op };
	return op_symbol_lookup_u16[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::u32sv_t cjm::uint128_tests::get_op_symbol_u32(binary_op op)
{
	if (op < first_op || op > last_op) throw bad_binary_op{ op };
	return op_symbol_lookup_u32[static_cast<unsigned>(op)];
}

///////////
constexpr cjm::uint128_tests::sv_t cjm::uint128_tests::get_un_op_symbol_n(unary_op op)
{
	if (op < first_unary_op || op > last_unary_op) throw bad_unary_op{ op };
	return un_op_symbol_lookup[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::wsv_t cjm::uint128_tests::get_un_op_symbol_w(unary_op op)
{
	if (op < first_unary_op || op > last_unary_op) throw bad_unary_op{ op };
	return un_op_symbol_lookup_wide[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::u8sv_t cjm::uint128_tests::get_un_op_symbol_u8(unary_op op)
{
	if (op < first_unary_op || op > last_unary_op) throw bad_unary_op{ op };
	return un_op_symbol_lookup_u8[static_cast<unsigned>(op)];
}

constexpr cjm::uint128_tests::u16sv_t cjm::uint128_tests::get_un_op_symbol_u16(unary_op op)
{
	if (op < first_unary_op || op > last_unary_op) throw bad_unary_op{ op };
	return un_op_symbol_lookup_u16[static_cast<unsigned>(op)];;
}

constexpr cjm::uint128_tests::u32sv_t cjm::uint128_tests::get_un_op_symbol_u32(unary_op op)
{
	if (op < first_unary_op || op > last_unary_op) throw bad_unary_op{ op };
	return un_op_symbol_lookup_u32[static_cast<unsigned>(op)];
}
constexpr std::array<std::uint64_t, cjm::uint128_tests::pow_2_arr_size> cjm::uint128_tests::get_pow2_arr()
{
	std::uint64_t current = 1;
	std::array<std::uint64_t, pow_2_arr_size> ret{};
	for (size_t idx = 0; idx < ret.size(); ++idx)
	{
		ret[idx] = current;
		current <<= 1;
	}
	return ret;
}

constexpr std::array<int, cjm::uint128_tests::pow_2_arr_size> cjm::uint128_tests::get_pow2_res_arr()
{
	using namespace cjm::numerics::literals;
	constexpr std::array<std::uint64_t, pow_2_arr_size> work_on_me = get_pow2_arr();
	std::array<int, pow_2_arr_size> ret{};
	for (auto i = 0_szt; i < work_on_me.size(); ++i)
	{
		ret[i] = cjm::numerics::internal::fls_default(work_on_me[i]);
	}
	return ret;
}




namespace cjm::uint128_tests::generator
{
	static_assert(sizeof(std::uintptr_t) >= 8 || sizeof(std::uintptr_t) == 4,
		"Only 32 and 64 bit systems are supported.");
	namespace internal
	{

		class rgen_impl;
	}
	class rgen;
}

namespace std
{
	template<>
	struct hash<cjm::uint128_tests::generator::rgen>
	{
		[[nodiscard]] size_t operator()(const cjm::uint128_tests::generator::rgen& key) const noexcept;

	};
}
namespace cjm::uint128_tests
{
	template<typename TestType, typename ControlType>
	requires (test_uint_and_control_set<TestType, ControlType>)
		struct binary_operation;
	using uint128_t = cjm::numerics::uint128;

}
namespace cjm::uint128_tests::generator
{
	using uint128_t = numerics::uint128;
	using ctrl_uint128_t = boost::multiprecision::uint128_t;
	enum class binary_op_category
	{
		any,
		bitwise,
		bitshift,
		add_subtract_multiply,
		div_mod,
		compare,
	};

	namespace concepts
	{
		template<typename UnsignedInteger>
		concept up_to_ui128 = std::numeric_limits<UnsignedInteger>::is_integer &&
			std::numeric_limits<UnsignedInteger>::digits <= 128 &&
			(!std::numeric_limits<UnsignedInteger>::is_signed);

		template<typename Invocable>
		concept binary_op_producer = requires
		(Invocable func, const rgen & gen, binary_operation<uint128_t, ctrl_uint128_t> op)
		{
			{op = func(gen) };

		};

		template<typename Invocable>
		concept unary_op_producer = requires (Invocable func, const rgen & gen, unary_op_u128_t op)
		{
			{op = func(gen)};
		};
	}


	
	binary_operation<cjm::uint128_tests::uint128_t,
		cjm::uint128_tests::ctrl_uint128_t> create_compare_op(const rgen& rgen);
	binary_operation<uint128_t, ctrl_uint128_t> create_shift_op(const rgen& rgen);
	binary_operation<uint128_t, ctrl_uint128_t> create_bw_op(const rgen& rgen);
	binary_operation<uint128_t, ctrl_uint128_t>
		create_division_op(const rgen& rgen, size_t max_dividend_size, size_t max_divisor_size);

	std::vector<binary_operation<uint128_t, ctrl_uint128_t>>
		create_division_ops(const rgen& rgen, size_t num_ops, size_t max_dividend_size, size_t max_divisor_size);

	binary_operation<uint128_t, ctrl_uint128_t>
		create_modulus_op(const rgen& rgen, size_t max_dividend_size, size_t max_divisor_size);

	std::vector<binary_operation<uint128_t, ctrl_uint128_t>>
		create_modulus_ops(const rgen& rgen, size_t num_ops, size_t max_dividend_size, size_t max_divisor_size);

	unary_op create_random_unary_opcode(const rgen& rgen);
	
	template<concepts::up_to_ui128 UnsignedInteger>
	uint128_t create_random_in_range(const rgen& rgen);

	template<numerics::concepts::builtin_floating_point TFloat>
	auto generate_u128_float(const rgen& rgen) -> std::remove_cvref_t<std::remove_const_t<TFloat>>;
	
	template<concepts::up_to_ui128 UnsignedInteger>
	int create_shift_param_for_type(const rgen& rgen);


	
	template<concepts::binary_op_producer OpFactory>
	std::vector<binary_operation<uint128_t, ctrl_uint128_t>> create_many_ops(OpFactory factory, const rgen& gen, size_t num_ops);

	binary_op_u128_t create_multiplication_op(rgen& rgen, size_t max_l_factor_size, size_t max_r_factor_size);
	
	bool operator==(const rgen& lhs, const rgen& rhs) noexcept;
	bool operator!=(const rgen& lhs, const rgen& rhs) noexcept;
	uint128_t create_random_in_range(const rgen& rgen, size_t byte_limit);

	class rgen final
	{
	public:
		[[nodiscard]] bool good() const { return static_cast<bool>(m_gen); }
		rgen();
		rgen(const rgen& other) = delete;
		rgen(rgen&& other) noexcept;
		rgen& operator=(const rgen& other) = delete;
		rgen& operator=(rgen&& other) noexcept;
		~rgen() = default;

		void shuffle(binary_op_u128_vect_t& vec, size_t shuffle_depth);

		char random_decimal_digit(char max = '9');
		char random_hex_digit();
		
		friend bool operator==(const rgen& lhs, const rgen& rhs) noexcept;
		friend bool operator!=(const rgen& lhs, const rgen& rhs) noexcept;
		friend struct std::hash<rgen>;
		explicit operator bool() const { return good(); }
		bool operator!() const { return !good(); }

		[[nodiscard]] binary_op random_op(binary_op_category category) const;
		[[nodiscard]] std::vector<std::uint8_t> generate(size_t bytes) const;
		[[nodiscard]] int generate_shift_param(int binary_digits) const;
	private:
		explicit rgen(bool) noexcept;
		std::unique_ptr<internal::rgen_impl> m_gen;
	};


	template<concepts::up_to_ui128 UnsignedInteger>
	uint128_t create_random_in_range(const rgen& rgen)
	{
		assert(rgen.good());
		using byte_array = typename uint128_t::byte_array;
		auto vec = rgen.generate(sizeof(UnsignedInteger));
		auto arr = byte_array{};
		assert(vec.size() <= arr.size());
		std::copy(vec.cbegin(), vec.cend(), arr.begin());
		return uint128_t::make_from_bytes_little_endian(arr);
	}

	template <concepts::up_to_ui128 UnsignedInteger>
	int create_shift_param_for_type(const rgen& rgen)
	{
		return rgen.generate_shift_param(std::numeric_limits<UnsignedInteger>::digits);
	}

	template<concepts::binary_op_producer OpFactory>
	std::vector<binary_operation<uint128_t, ctrl_uint128_t>> create_many_ops(OpFactory factory, const rgen& gen, size_t num_ops)
	{
		auto vec = std::vector<binary_operation<uint128_t, ctrl_uint128_t>>{};
		if (num_ops > 0)
		{
			vec.reserve(num_ops);
		}
		while (vec.size() < num_ops)
		{
			vec.emplace_back(factory(gen));
		}
		return vec;
	}

	template<numerics::concepts::builtin_floating_point TFloat>
	auto generate_u128_float(const rgen& rgen)->std::remove_cvref_t<std::remove_const_t<TFloat>>
	{
		using flt_t = std::remove_cvref_t<std::remove_const_t<TFloat>>;
		constexpr size_t float_size = sizeof(flt_t);
		constexpr int uint128_digits = std::numeric_limits<uint128_t>::digits;
		constexpr int uint16_digits = std::numeric_limits<std::uint16_t>::digits;
		constexpr int uint32_digits = std::numeric_limits<std::uint32_t>::digits;
		constexpr int uint16_lshift_modulo = uint128_digits - uint16_digits;
		constexpr int uint32_lshift_modulo = uint128_digits - uint32_digits;
		static_assert( uint16_lshift_modulo > 0);
		static_assert(uint32_lshift_modulo > 0);
		
		if constexpr (float_size == sizeof(float))
		{

			uint128_t source = generator::create_random_in_range<std::uint16_t>(rgen);
			const auto l_shift_amnt = rgen.generate_shift_param(uint128_digits) % uint16_lshift_modulo;
			source <<= l_shift_amnt;
			const int decimal = static_cast<int>(generator::create_random_in_range<std::uint8_t>(rgen));
			flt_t dec_part = static_cast<flt_t>(decimal) / 100.0f;
			auto integer_part = static_cast<flt_t>(source);
			return integer_part + dec_part;
		}
		else
		{
			uint128_t source = generator::create_random_in_range<std::uint32_t>(rgen);
			const auto l_shift_amnt = rgen.generate_shift_param(std::numeric_limits<uint128_t>::digits);
			source <<= l_shift_amnt;
			const int decimal = static_cast<int>(generator::create_random_in_range<std::uint8_t>(rgen));
			const flt_t dividend = 100;
			flt_t dec_part = static_cast<flt_t>(decimal) / dividend;
			auto integer_part = static_cast<flt_t>(source);
			return integer_part + dec_part;
		}
	}

}
template<typename TestType, typename ControlType>
	requires (cjm::uint128_tests::test_uint_and_control_set<TestType, ControlType>)
std::size_t std::hash<cjm::uint128_tests::binary_operation<TestType, ControlType>>
	::operator()(const cjm::uint128_tests::binary_operation<TestType, ControlType>& 
		hash_me) noexcept
{
	return hash_me.hash_value();
}

template<typename TestType, typename ControlType>
	requires (cjm::uint128_tests::test_uint_and_control_set<TestType, ControlType>)
std::size_t std::hash<cjm::uint128_tests::unary_operation<TestType, ControlType>>::operator()
	(const cjm::uint128_tests::unary_operation < TestType, ControlType>& hash_me) noexcept
{
	return hash_me.hash_value();
}

#endif //INT128_INT128_TESTS_HPP_
