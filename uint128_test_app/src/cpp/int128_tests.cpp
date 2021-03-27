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
#include "int128_tests.hpp"
#include <ios>
namespace
{
	using uint128_t = cjm::numerics::uint128;
	using uint128_ctrl = boost::multiprecision::uint128_t;
	using namespace cjm::testing;
	using cjm::testing::newl;

	constexpr auto hex_char_arr = std::array<char, 16> {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

	template<concepts::unsigned_integer Ui, bool HexAndDecimal>
	constexpr std::array<size_t, (std::numeric_limits<Ui>::digits10 + 2)> init_num_tests_arr() noexcept
	{
		auto temp = std::array<size_t, std::numeric_limits<Ui>::digits10 + 2>{};
		constexpr size_t num_zero_digit_tests = 0;
		constexpr size_t num_one_digit_tests = HexAndDecimal ? 26u : 10u;
		constexpr size_t num_tests_per_other_digit_per_base = 5u;
		temp[0] = num_zero_digit_tests;
		temp[1] = num_one_digit_tests;
		for (size_t i = 2; i < temp.size(); ++i)
		{
			temp[i] = num_tests_per_other_digit_per_base * (HexAndDecimal ? 2u : 1u);
		}
		return temp;
	}
	constexpr std::array<int, 128> get_shifting_arr() noexcept
	{
		std::array<int, 128> ret{};
		for (int i = 0; i < 128; ++i)
		{
			ret[i] = i;
		}
		return ret;
	}

	constexpr std::string_view bool_to_yes_no(bool value) noexcept
	{
		using namespace std::string_view_literals;
		return value ? "YES"sv : "NO"sv;
	}

	std::string to_lower(std::string_view sv)
	{
		std::string ret;
		if (!sv.empty())
		{
			ret.reserve(sv.size());
			std::transform(sv.cbegin(), sv.cend(),
				std::back_inserter(ret),
				[](unsigned char c) {return std::tolower(c); });
		}
		return ret;
	}

	std::filesystem::path construct_op_filename(cjm::uint128_tests::binary_op op,  std::string_view op_tag)
	{
		using namespace date;
		auto stamp = std::chrono::system_clock::now();
		auto date_part = std::chrono::floor<days>(stamp);
		auto time_part = make_time(stamp - date_part);
		auto ymd = year_month_day{ date_part };
		auto hms = hh_mm_ss{ time_part };
		auto op_name = to_lower(get_op_text(op));

		auto ss = cjm::string::make_throwing_sstream<char>();
		ss
			<< cjm::uint128_tests::base_bin_op_filename << "_" << op_name << "_"
			<< op_tag << "_" << ymd.year() << "_" << std::setw(2)
			<< std::setfill('0') << ymd.month() << "_"
			<< ymd.day() << "_" << std::setw(2) << std::setfill('0')
			<< hms.hours().count() << "_" << std::setw(2) << std::setfill('0')
			<< hms.minutes().count() << "_" << std::setw(2) << std::setfill('0')
			<< hms.seconds().count() << "Z." << cjm::uint128_tests::op_extension;
		return std::filesystem::path{ ss.str() };
	}

	std::filesystem::path construct_op_filename(cjm::uint128_tests::unary_op op, std::string_view op_tag)
	{
		using namespace date;
		auto stamp = std::chrono::system_clock::now();
		auto date_part = std::chrono::floor<days>(stamp);
		auto time_part = make_time(stamp - date_part);
		auto ymd = year_month_day{ date_part };
		auto hms = hh_mm_ss{ time_part };
		auto op_name = to_lower(get_un_op_text(op));

		auto ss = cjm::string::make_throwing_sstream<char>();
		ss 
			<< cjm::uint128_tests::base_un_op_filename << "_" << op_name << "_"
			<< op_tag << "_" << ymd.year() << "_" << std::setw(2)
			<< std::setfill('0') << ymd.month() << "_"
			<< ymd.day() << "_" << std::setw(2) << std::setfill('0')
			<< hms.hours().count() << "_" << std::setw(2) << std::setfill('0')
			<< hms.minutes().count() << "_" << std::setw(2) << std::setfill('0')
			<< hms.seconds().count() << "Z." << cjm::uint128_tests::op_extension;
		return std::filesystem::path{ ss.str() };
	}

	template<typename VectorOfOperations>
	void execute_test_file(std::string_view op_type, const std::filesystem::path& file)
	{
		using namespace cjm::uint128_tests;
		
		using vec_t = std::remove_const_t<VectorOfOperations>;
		auto op_vec = vec_t{};
		using op_t = std::remove_const_t<std::remove_reference_t<decltype(op_vec[0])>>;
		op_t* ptr_current = nullptr;
		bool opened = false;
		try
		{
			{
				auto istream = cjm::string::make_throwing_ifstream<char>(file);
				opened = true;
				istream >> op_vec;
			}
			const size_t num_ops = op_vec.size();
			std::cout 
				<< "Successfully read " << num_ops << " " << op_type << " operations from file [" 
				<< file << "]." << newl;

			for (auto& op : op_vec)
			{
				ptr_current = &op;
				op.calculate_result();
				cjm_assert(op.has_result() && op.has_correct_result());
			}
			std::cout << "All " << num_ops << " tests from file [" << file << "] PASSED." << newl;
		}
		catch (const std::ios::failure& ex)
		{
			if (opened)
			{
				std::cerr 
					<< "Unable to parse contents of file [" << file 
					<< "] as " << op_type << " operations.  Message: [" 
					<< ex.what() << "]." << newl;
			}
			else
			{
				std::cerr << "Unable to open " << op_type << " operations file [" << file << "] -- Message: [" << ex.what() << "]." << newl;
			}
			throw;
		}
		catch (const cjm::testing::testing_failure&)
		{
			std::cerr << "A test from: [" << file << "] FAILED!" << newl;
			if (ptr_current)
			{
				std::cerr << "A " << op_type << " operation failed.  As a static_assertion it has the form: \"";
				append_static_assertion(std::cerr, *ptr_current) << "\"." << newl;
				auto dump_file = create_failing_op_pathname(ptr_current->op_code());
				std::cerr << "Will attempt to serialize the failing operation to [" << dump_file << "]";
				try
				{
					auto ostream = cjm::string::make_throwing_ofstream<char>(dump_file);
					ostream << *ptr_current;
				}
				catch (const std::exception& ex)
				{
					std::cerr << "Unable to serialize failing item to file because of exception.  "
						<< "Msg: [" << ex.what() << "]." << newl;
					std::cerr << "Serializing to stderr instead (between brackets): ["
						<< *ptr_current << "]." << newl;
				}
			}
			else
			{
				std::cerr << "Unable to determine which test from the file failed." << newl;
			}
			throw;
		}
		catch (const std::exception& ex)
		{
			std::cerr << "An unexpected error occurred. Msg: [" << ex.what() << "]." << newl;
			throw;
		}
	}

	template<cjm::numerics::concepts::builtin_floating_point TFloat>
	void test_u128_rt_float_conversion(uint128_t test_me)
	{
		using flt_t = std::remove_cvref_t<std::remove_const_t<TFloat>>;
		using cjm::uint128_tests::ctrl_uint128_t; //boost uint128
		using cjm::uint128_tests::alt_ctrl_uint128_t; //abseil uint128
		using cjm::uint128_tests::to_ctrl; //to boost uint128
		using cjm::uint128_tests::to_alt_ctrl; //to abseil uint128
		using cjm::uint128_tests::to_test; //from boost or abseil to uint128_t

		const auto ctrl_u128 = to_ctrl(test_me);
		const auto alt_ctrl_u128 = to_alt_ctrl(test_me);
		
		try
		{
			const auto from_ctrl = static_cast<flt_t>(ctrl_u128);
			const auto from_alt_ctrl = static_cast<flt_t>(alt_ctrl_u128);
			const auto from_test = static_cast<flt_t>(test_me);

			uint128_t rt_from_ctrl_flt = cjm::numerics::safe_from_floating_or_throw(from_ctrl);
			uint128_t rt_from_alt_ctrl_flt = cjm::numerics::safe_from_floating_or_throw(from_alt_ctrl);
			uint128_t rt_from_test_flt = cjm::numerics::safe_from_floating_or_throw(from_test);

			cjm_assert_equal(rt_from_alt_ctrl_flt, rt_from_ctrl_flt);
			cjm_assert_equal(rt_from_alt_ctrl_flt, rt_from_test_flt);
			cjm_assert_equal(rt_from_test_flt, rt_from_ctrl_flt);
		}
		catch (const std::exception& ex)
		{
			
			auto strm = cjm::string::make_throwing_sstream<char>();
			strm	<< "An unexpected exception occured when attempting to "
					<< "test value [0x" << std::hex
					<< std::setw(std::numeric_limits<uint128_t>::digits / 4)
					<< std::setfill('0') << test_me << "]"
					<< " in round trip floating point conversion test."
					<< "  Exception message: \"" << ex.what() << "\"." << newl;
			throw testing_failure{ strm.str() };
		}
		
	}

	template<cjm::numerics::concepts::builtin_floating_point TFloat>
	void test_convert_float_value(TFloat v)
	{
		using cjm::uint128_tests::ctrl_uint128_t; //boost uint128
		using cjm::uint128_tests::alt_ctrl_uint128_t; //abseil uint128
				
		try
		{
			const uint128_t from_float = cjm::numerics::safe_from_floating_or_throw(v);
			const auto from_float_ctrl = static_cast<ctrl_uint128_t>(v);
			const auto from_float_alt_ctrl = static_cast<alt_ctrl_uint128_t>(v);
			const auto from_ctrl_u128 = cjm::uint128_tests::to_test(from_float_ctrl);
			const auto from_alt_ctrl_u128 = cjm::uint128_tests::to_test(from_float_alt_ctrl);

			cjm_assert_equal(from_float, from_ctrl_u128);
			cjm_assert_equal(from_float, from_alt_ctrl_u128);
		}
		catch (const std::invalid_argument& ex)
		{
			auto strm = cjm::string::make_throwing_sstream<char>();
			strm	<< "The test failed because the specified floating point value [" << v
					<< "] cannot safely be converted to uint128_t without causing undefined behavior." 
					<< "Exception message: \"" << ex.what() << "\"." << newl;
			throw testing_failure{ strm.str() };
		}
	}
	
	
}

void cjm::uint128_tests::validate_divmod_op(const binary_op_u128_t& op)
{
	const auto& result = op.result().value();
	if (!op.has_correct_result()) throw std::invalid_argument{ "Does not have correct result to begin with." };
	if (op.op_code() != binary_op::divide && op.op_code() != binary_op::modulus)
	{
		throw std::invalid_argument{ "Operation is not division or modulus." };
	}

	if (op.op_code() != binary_op::divide)
	{
		auto res = uint128_t::div_mod(op.left_operand(), op.right_operand());
		if (res.remainder != result.first)
			throw divmod_fail_match{ op };
	}
	else
	{
		auto res = uint128_t::div_mod(op.left_operand(), op.right_operand());
		if(res.quotient != result.first)
			throw divmod_fail_match{ op };
	}	
}

cjm::uint128_tests::alt_ctrl_uint128_t cjm::uint128_tests::to_alt_ctrl(uint128_t convert) noexcept
{
	alt_ctrl_uint128_t ret = static_cast<std::uint64_t>(convert >> 64);
	ret <<= 64;
	ret |= static_cast<std::uint64_t>(convert);
	return ret;
}

uint128_t cjm::uint128_tests::to_test(alt_ctrl_uint128_t convert) noexcept
{
	uint128_t ret = static_cast<std::uint64_t>(convert >> 64);
	ret <<= 64;
	ret |= static_cast<std::uint64_t>(convert);
	return ret;
}

cjm::uint128_tests::ctrl_uint128_t cjm::uint128_tests::to_ctrl(uint128_t convert) 
{
	ctrl_uint128_t ret = convert.high_part();
	ret <<= 64;
	ret |= convert.low_part();
	return ret;
}

cjm::uint128_tests::uint128_t cjm::uint128_tests::to_test(const ctrl_uint128_t& convert) 
{
	const std::uint64_t low_part = static_cast<std::uint64_t>(convert);
	const std::uint64_t high_part = static_cast<std::uint64_t>(convert >> 64);
	return uint128_t::make_uint128(high_part, low_part);
}
void cjm::uint128_tests::print_floating_point_info()
{
	constexpr bool is_float_iec = std::numeric_limits<float>::is_iec559;
	constexpr bool is_double_iec = std::numeric_limits<double>::is_iec559;
	constexpr bool is_long_double_iec = std::numeric_limits<long double>::is_iec559;
	auto saver = cout_saver {cout};
	
	cout << newl << "Printing data for type float: " << newl;
	if constexpr (is_float_iec)
	{
		cout << "\tfloat (size: " << std::dec << sizeof(float ) << ", align: " << alignof(float) << ") is an iec559 type." << newl;
		cout << "\tfloat uses " << std::numeric_limits<float>::digits -1 << " of its " << (sizeof(float) * CHAR_BIT) << " bits for mantissa." << newl;
		cout << "\tfloat's min exponent: " << std::numeric_limits<float>::min_exponent << "; max exponent: " << std::numeric_limits<float>::max_exponent << "." << newl;
		cout << "\tfloat's max value: " << std::numeric_limits<float>::max() << "." << newl;
		cout << "Done printing data for type float." << newl << newl;
	}
	else
	{
		cout << "\tfloat (size: " << std::dec << sizeof(float ) << ", align: " << alignof(float) << ") is an NOT iec559 type." << newl;
		cout << "\tfloat uses " << std::numeric_limits<float>::digits << " of its " << (sizeof(float) * CHAR_BIT) << " bits for mantissa." << newl;
		cout << "\tfloat's min exponent: " << std::numeric_limits<float>::min_exponent << "; max exponent: " << std::numeric_limits<float>::max_exponent << "." << newl;
		cout << "\tfloat's max value: " << std::numeric_limits<float>::max() << "." << newl;
		cout << "Done printing data for type float." << newl << newl;
	}

	cout << "Printing data for type double: " << newl;
	if constexpr (is_double_iec)
	{
		cout << "\tdouble (size: " << std::dec << sizeof(double ) << ", align: " << alignof(double) << ") is an iec559 type." << newl;
		cout << "\tdouble uses " << std::numeric_limits<double>::digits -1 << " of its " << (sizeof(double) * CHAR_BIT) << " bits for mantissa." << newl;
		cout << "\tdouble's min exponent: " << std::numeric_limits<double>::min_exponent << "; max exponent: " << std::numeric_limits<double>::max_exponent << "." << newl;
		cout << "\tdouble's max value: " << std::numeric_limits<double>::max() << "." << newl;
		cout << "Done printing data for type double." << newl << newl;
	}
	else
	{
		cout << "\tdouble (size: " << std::dec << sizeof(double ) << ", align: " << alignof(double) << ") is an NOT iec559 type." << newl;
		cout << "\tdouble uses " << std::numeric_limits<double>::digits << " of its " << (sizeof(double) * CHAR_BIT) << " bits for mantissa." << newl;
		cout << "\tdouble's min exponent: " << std::numeric_limits<double>::min_exponent << "; max exponent: " << std::numeric_limits<double>::max_exponent << "." << newl;
		cout << "\tdouble's max value: " << std::numeric_limits<double>::max() << "." << newl;
		cout << "Done printing data for type double." << newl << newl;
	}

	cout << "Printing data for type long double: " << newl;
	if constexpr (is_long_double_iec)
	{
		cout << "\tlong double (size: " << std::dec << sizeof(long double ) << ", align: " << alignof(long double) << ") is an iec559 type." << newl;
		cout << "\tlong double uses " << std::numeric_limits<long double>::digits -1 << " of its " << (sizeof(long double) * CHAR_BIT) << " bits for mantissa." << newl;
		cout << "\tlong double's min exponent: " << std::numeric_limits<long double>::min_exponent << "; max exponent: " << std::numeric_limits<long double>::max_exponent << "." << newl;
		cout << "\tlong double's max value: " << std::numeric_limits<long double>::max() << "." << newl;
		cout << "Done printing data for type long double." << newl << newl;
	}
	else
	{
		cout << "\tlong double (size: " << std::dec << sizeof(long double ) << ", align: " << alignof(long double) << ") is an NOT iec559 type." << newl;
		cout << "\tlong double uses " << std::numeric_limits<long double>::digits << " of its " << (sizeof(long double) * CHAR_BIT) << " bits for mantissa." << newl;
		cout << "\tlong double's min exponent: " << std::numeric_limits<long double>::min_exponent << "; max exponent: " << std::numeric_limits<long double>::min_exponent << "." << newl;
		cout << "\tlong double's max value: " << std::numeric_limits<long double>::max() << "." << newl;
		cout << "Done printing data for type long double." << newl;
	}
	cout << "Done printing floating point information." << newl << newl;
}
void cjm::uint128_tests::print_alignments()
{
	std::cout << newl << "PRINTING ALIGNMENTS:" << newl;
	std::cout << "\tAlignof unsigned short: [" << alignof(unsigned short) << "]." << newl;
	std::cout << "\tAlignof unsigned int: [" << alignof(unsigned int) << "]." << newl;
	std::cout << "\tAlignof unsigned long: [" << alignof(unsigned long) << "]." << newl;
	std::cout << "\tAlignof unsigned long long: [" << alignof(unsigned long long) << "]." << newl;
	std::cout << "\tAlignof size_t: [" << alignof(size_t) << "]." << newl;
	std::cout << "\tAlignof uintmax_t: [" << alignof(std::uintmax_t) << "]." << newl;
	if constexpr (numerics::has_intrinsic_u128)
	{
		std::cout << "\tAlignof built-in unsigned 128 bit integer: [" << alignof(numerics::natuint128_t) << "]." << newl;
	}
	else
	{
		std::cout << "\tunsigned 128 bit integer unavailable in this environment." << newl;
	}
	
	std::cout << "\tAlignof char: [" << alignof(char) << "]." << newl;
	std::cout << "\tAlignof wide character: [" << alignof(wchar_t) << "]." << newl;
	std::cout << "\tAlignof cjm::numerics::uint128: [" << alignof(numerics::uint128) << "]." << newl;
	std::cout << "\tAlignof float: [" << alignof(float) << "]." << newl;
	std::cout << "\tAlignof double: [" << alignof(double) << "]." << newl;
	std::cout << "\tAlignof long double: [" << alignof(long double) << "]." << newl;
	std::cout << "\tAlignof pointer to object: [" << alignof(std::uintptr_t) << "]." << newl;
	std::cout << "\tAlignof most aligned built-in: [" << alignof(std::max_align_t) << "]." << newl;
	std::cout << "DONE PRINTING ALIGNMENTS" << newl;
}

void cjm::uint128_tests::save_random_unary_ops_to_file(std::filesystem::path target)
{
	auto random = generate_random_standard_test_ops();
	{
		auto fstrm = string::make_throwing_ofstream<char>(target);
		fstrm << random;
	}
	cjm_assert(std::filesystem::exists(target));
	std::cout << "Successfully wrote [" << random.size() << "] unary operations to file [" << target << "]" << newl;
}

void cjm::uint128_tests::save_random_binary_ops_to_file(std::filesystem::path target)
{
	auto random = generate_divmod_ops(10, true);
	{
		auto fstrm = string::make_throwing_ofstream<char>(target);
		fstrm << random;
	}
	cjm_assert(std::filesystem::exists(target));
	std::cout << "Successfully wrote [" << random.size() << "] binary operations to file [" << target << "]" << newl;
}

void cjm::uint128_tests::execute_test_switch(const test_switch& test_switch)
{
	switch (test_switch.mode())  // NOLINT(clang-diagnostic-switch-enum)
	{
	case test_mode::print_environ_info:
		print_environ_data();
		break;
	case test_mode::run_default_tests:
		execute_uint128_tests();
		break;
	case test_mode::execute_unary_from_file: 
		execute_unary_test_file(test_switch.file_path().value());
		break;
	case test_mode::execute_binary_from_file:
		execute_binary_test_file(test_switch.file_path().value());
		break;
	default:
		{
			auto strm = string::make_throwing_sstream<char>();
			strm << "Switch [" << test_switch.mode() << "] has not yet been implemented." << newl;
			throw std::logic_error{ strm.str() };
		}
		break;
	}
}

void cjm::uint128_tests::execute_unary_test_file(const std::filesystem::path& file)
{
	execute_test_file<unary_op_u128_vect_t>("unary"sv, file);
}

void cjm::uint128_tests::execute_binary_test_file(const std::filesystem::path& file)
{
	execute_test_file<binary_op_u128_vect_t>("binary"sv, file);
}

void cjm::uint128_tests::run_test_application(std::span<test_switch> switches)
{
	//uncomment to generate random binary and unary ops file for demo purposes
	/*save_random_binary_ops_to_file(std::filesystem::path{ "random_binary_ops.txt" });
	save_random_unary_ops_to_file(std::filesystem::path{ "random_unary_ops.txt" });*/
	//using boost::multiprecision::literals::operator ""_cppui128;
	//cjm_assert_equal(to_test(0x98908bf90a64f3d8da5beb787031b0f6_cppui128), 0x98908bf90a64f3d8da5beb787031b0f6_u128, "0x98908bf90a64f3d8da5beb787031b0f6"sv);
	//cjm_assert_equal(to_test(0x98908bf90a64f3d8da5beb787031b0f3_cppui128), 0x98908bf90a64f3d8da5beb787031b0f6_u128, "0x98908bf90a64f3d8da5beb787031b0f6"sv);
	//generate_then_print_literal_tests();
	
	if (switches.empty())
	{
		print_environ_data();
		execute_uint128_tests();
	}
	else
	{
		const auto has_print_environ = std::find_if(
			switches.begin(), switches.end(),
		[](const test_switch& ts) -> bool
			{
					return ts.mode() == test_mode::print_environ_info;
			}) != switches.end();
		const auto has_execute_battery = std::find_if(switches.begin(),
			switches.end(), [](const test_switch& ts) -> bool
			{
					return ts.mode() == test_mode::run_default_tests;
			}) != switches.end();
		if (has_print_environ)
		{
			std::cout << "Executing switch [" << test_mode::print_environ_info << "]: " << newl;
			print_environ_data();
			std::cout << "Done executing switch [" << test_mode::print_environ_info << "] " << newl << newl;
		}
		if (has_execute_battery)
		{
			std::cout << "Executing switch [" << test_mode::run_default_tests << "]: " << newl;
			execute_uint128_tests();
			std::cout << "Done executing switch [" << test_mode::run_default_tests << "] " << newl << newl;
		}
		for (const auto& test_switch : switches)
		{
			const auto mode = test_switch.mode();
			if (mode == test_mode::execute_unary_from_file)
			{
				execute_unary_test_file(test_switch.file_path().value());
			}
			else if (mode == test_mode::execute_binary_from_file)
			{
				execute_binary_test_file(test_switch.file_path().value());
			}
		}
	}
}

void cjm::uint128_tests::print_sizes()
{
	std::cout << newl << "PRINTING SIZES:" << newl;
	std::cout << "\tCHAR_BIT: " << CHAR_BIT << " bits per byte on this system." << newl;
	std::cout << "\tSizeof unsigned short: [" << sizeof(unsigned short) << "]." << newl;
	std::cout << "\tSizeof unsigned int: [" << sizeof(unsigned int) << "]." << newl;
	std::cout << "\tSizeof unsigned long: [" << sizeof(unsigned long) << "]." << newl;
	std::cout << "\tSizeof unsigned long long: [" << sizeof(unsigned long long) << "]." << newl;
	std::cout << "\tSizeof size_t: [" << sizeof(size_t) << "]." << newl;
	std::cout << "\tSizeof uintmax_t: [" << sizeof(std::uintmax_t) << "]." << newl;
	if constexpr (numerics::has_intrinsic_u128)
	{
		std::cout << "\tSizeof built-in unsigned 128 bit integer: [" << sizeof(numerics::natuint128_t) << "]." << newl;
	}
	else
	{
		std::cout << "\tunsigned 128 bit integer unavailable in this environment." << newl;
	}
	if constexpr (std::is_signed_v<char>)
	{
		std::cout << "\tchar is signed in this implementation." << newl;
	}
	else
	{
		std::cout << "\tchar is unsigned in this implementation." << newl;
	}

	std::cout << "\tSizeof wide character: [" << sizeof(wchar_t) << "]." << newl;
	if constexpr (std::is_signed_v<wchar_t>)
	{
		std::cout << "\tWide character is signed in this implementation." << newl;
	}
	else
	{
		std::cout << "\tWide character is unsigned in this implementation." << newl;
	}

	std::cout << "\tSizeof cjm::numerics::uint128: [" << sizeof(numerics::uint128) << "]." << newl;
	std::cout << "\tSizeof float: [" << sizeof(float) << "]." << newl;
	std::cout << "\tSizeof double: [" << sizeof(double) << "]." << newl;
	std::cout << "\tSizeof long double: [" << sizeof(long double) << "]." << newl;
	std::cout << "\tSizeof pointer to object: [" << sizeof(std::uintptr_t) << "]." << newl;
	std::cout << "\tSizeof most aligned built-in: [" << sizeof(std::max_align_t) << "]." << newl;
	std::cout << "DONE PRINTING SIZES" << newl;
}
void cjm::uint128_tests::print_environ_data()
{
	cout << "ENVIRONMENT DATA: " << newl;
	cout << "Detected compiler: [" << numerics::compiler << "]." << newl;
	if constexpr (numerics::sse3_available)
	{
		cout << "SSE3: [AVAILABLE]." << newl;
	}
	else
	{
		cout << "SSE3: [NOT AVAILABLE]." << newl;
	}
	if constexpr (numerics::intel_adx_available)
	{
		cout << "Intel ADX: [AVAILABLE]." << newl;
	}
	else
	{
		cout << "Intel ADX: [NOT AVAILABLE]." << newl;
	}
	if constexpr (numerics::intel_bmi2_available)
	{
		cout << "Intel BMI2: [AVAILABLE]." << newl;
	}
	else
	{
		cout << "Intel BMI2: [NOT AVAILABLE]." << newl;
	}
	
	print_sizes();
	print_alignments();
	print_floating_point_info();
	print_constexpr_bitcast_available();
	print_uint128_eval_mode();
	print_cpp20_bitops_available();
	print_builtin_uint128_data_if_present();
	print_whether_has_consteval();
	cout << "END ENVIRONMENT DATA" << newl << newl;
}

void cjm::uint128_tests::execute_ascii_char_interconversions()
{
	for (int i = 0; i <= std::numeric_limits<char>::max(); ++i)
	{
		char c = static_cast<char>(i);
		wchar_t wc = convert_char<char, wchar_t>(c);
		char8_t u8c = convert_char<char, char8_t>(c);
		char16_t u16c = convert_char<char, char16_t>(c);
		char32_t u32c = convert_char<char, char32_t>(c);

		cjm_assert(convert_char<wchar_t, char>(wc) == c);
		cjm_assert(convert_char<char8_t, char>(u8c) == c);
		cjm_assert(convert_char<char16_t, char>(u16c) == c);
		cjm_assert(convert_char<char32_t, char>(u32c) == c);

	}
}

void cjm::uint128_tests::execute_div_mod_zero_dividend_nonzero_divisor_tests()
{
	constexpr auto dividend = 0_u128;
	constexpr auto divisor = 0xd00d_u128;
	static_assert(0_u128 <=> 1_u128 == std::strong_ordering::less);
	cjm_assert(dividend / divisor == 0_u128);
	cjm_assert(dividend % divisor == 0_u128);
	auto divmod_res = uint128_t::div_mod(dividend, divisor);
	cjm_assert(divmod_res.quotient == 0 && divmod_res.remainder == 0);
	auto res = uint128_t::try_div_mod(dividend, divisor);
	cjm_assert(res.has_value() && res->quotient == 0 && res->remainder == 0);
}

void cjm::uint128_tests::execute_div_mod_by_zero_tests()
{
	using namespace testing;
	
	cjm_assert_throws<std::domain_error>([]() -> void
	{
		const auto dividend = 0xd00d_u128;
		const auto divisor = 0_u128;
		const auto quotient = dividend / divisor;
	   std::cerr << "You should never see this value: [" << quotient << "]." << newl;
	});

	cjm_assert_throws<std::domain_error>([]() -> void
	{
		const auto dividend = 0xd00d_u128;
		const auto divisor = 0_u128;
		const auto remainder = dividend % divisor;
		std::cerr << "You should never see this value: [" << remainder << "]." << newl;
	});

	cjm_assert_throws<std::domain_error>([]() -> void
	{
		const auto dividend = 0xd00d_u128;
		const auto divisor = 0_u128;
		const auto result = uint128_t::div_mod(dividend, divisor);
		std::cerr << "You should never see this value: [Quotient: " << result.quotient << "; remainder: " << result.remainder << "]." << newl;
	});
	cjm_assert(uint128_t::try_div_mod(0xd00d_u128, 0_u128) == std::nullopt);
	
}

void cjm::uint128_tests::execute_uint128_tests()
{
	using tconst_prov_t [[maybe_unused]] = u128_testing_constant_providers::testing_constant_provider<uint128_t>;
	static_assert(tconst_prov_t::maximum + tconst_prov_t::one == tconst_prov_t::zero);
	cout_saver saver{ cout };
	constexpr auto two_fifty_five = 0xff_u128;
	constexpr auto all_at_ends = 0xff00'0000'0000'0000'0000'0000'0000'0000_u128;
	static_assert(((two_fifty_five << (15 * 8)) == all_at_ends) && ((all_at_ends >> (15 * 8)) == two_fifty_five));
	cout << "BEGINNING STANDARD TEST BATTERY." << newl;
	execute_test(execute_generate_addition_ops_rt_ser_deser_test, "generate_addition_ops_rt_ser_deser_test"sv);

	execute_test(execute_issue27_bug_test, "issue27_bug_test");

	//execute_test(parse_file_test, "parse_file_test"sv);
	execute_test(execute_literal_test, "literal_test"sv);
	execute_test(execute_basic_test_one, "basic_test_one"sv);
	execute_test(execute_binary_operation_rt_ser_tests, "binary_operation_rt_ser_tests"sv);
	execute_test(execute_print_generated_filename_test, "print_generated_filename_test"sv);
	execute_test(execute_string_parse_test, "string_parse_text"sv);
	execute_test(execute_basic_multiplication_test, "basic_multiplication_test"sv);
	execute_test(test_fls, "test_fls"sv);
	execute_test(execute_builtin_u128fls_test_if_avail, "builtin_u128fls_test_if_avail"sv);
	execute_test(execute_first_bin_op_test, "first_bin_op_test"sv);
	execute_test(execute_div_mod_by_zero_tests, "div_mod_zero_tests"sv);
	execute_test(execute_div_mod_zero_dividend_nonzero_divisor_tests, "div_mod_zero_dividend_nonzero_divisor_tests"sv);
	execute_test(execute_gen_comp_ops_test, "gen_comp_ops_test"sv);
	execute_test(execute_ascii_char_interconversions, "ascii_char_interconversions"sv);
	execute_test(execute_trim_tests, "trim_tests"sv);
	execute_test(execute_stream_insert_bin_op_test, "stream_insert_bin_op_test"sv);
	execute_test(execute_addition_tests, "addition_tests"sv);
	execute_test(execute_shift_tests, "shift_tests"sv);
	execute_test(execute_bw_tests, "bw_tests"sv);
	execute_test(execute_subtraction_tests, "subtraction_tests"sv);
	execute_test(execute_comparison_tests, "comparison_tests"sv);
	execute_test(execute_multiplication_tests, "multiplication_tests"sv);
	execute_test(execute_failing_division_test_1, "failing_division_test_1"sv);
	execute_test(execute_failing_division_test_2, "failing_division_test_2"sv);
	execute_test(execute_failing_modulus_test_1, "failing_modulus_test_1"sv);
	execute_test(execute_division_modulus_tests, "division_modulus_tests"sv);
	execute_test(execute_unary_op_code_rt_serialization_tests, "unary_op_code_rt_serialization_tests"sv);
	execute_test(execute_unary_op_basic_test, "unary_op_basic_test"sv);
	execute_test(execute_unary_op_post_stat_assert_test, "unary_op_post_stat_assert_test"sv);
	execute_test(execute_unary_operation_rt_serialization_tests, "unary_operation_rt_serialization_tests"sv);
	execute_test(execute_unary_operation_vec_rt_serialization_tests, "unary_operation_vec_rt_serialization_tests"sv);
	
	execute_test(execute_unary_op_post_inc_test, "unary_op_post_inc_test"sv);
	execute_test(execute_unary_op_pre_inc_test, "unary_op_pre_inc_test"sv);
	execute_test(execute_unary_op_pre_dec_test, "unary_op_pre_dec_test"sv);
	execute_test(execute_unary_op_post_dec_test, "unary_op_post_dec_test"sv);
	execute_test(execute_unary_op_unary_plus_test, "unary_op_unary_plus_test"sv);

	execute_test(execute_unary_op_unary_minus_test, "unary_op_unary_minus_test"sv);
	execute_test(execute_unary_op_bitwise_not_test, "unary_op_bitwise_not_test"sv);
	execute_test(execute_unary_op_bool_cast_test, "unary_op_bool_cast_test"sv);
	execute_test(execute_unary_op_logical_negation_test, "unary_op_logical_negation_test"sv);

	execute_test(execute_test_convert_to_float, "test_convert_to_float"sv);
	execute_test(execute_test_convert_to_double, "test_convert_to_double"sv);
	execute_test(execute_test_convert_to_long_double, "test_convert_to_long_double"sv);

	execute_test(execute_throwing_float_conversion_test, "throwing_float_conversion_test"sv);
	execute_test(execute_safe_float_conversions_test, "safe_float_conversions_test"sv);

	execute_test(execute_controlled_from_float_conversion_test, "controlled_from_float_conversion_test"sv);
	execute_test(execute_controlled_float_rt_conversion_test, "controlled_float_rt_conversion_test"sv);
	
	execute_test(execute_issue_10_strm_insrt_test, "issue_10_strm_insrt_test"sv);
	execute_test(execute_issue_10_showbase_test, "issue_10_showbase_test"sv);
	execute_test(execute_hash_dx, "hash_dx"sv);

	execute_test(execute_builtin_add_with_carry_test, "builtin_add_with_carry_test"sv);
	execute_test(execute_basic_u128_adc_test, "basic_u128_adc_test"sv);
	execute_test(execute_basic_u128_sbb_test, "basic_u128_sbb_test"sv);
	execute_test(execute_builtin_sub_with_borrow_test, "builtin_sub_with_borrow_test"sv);

	execute_test(execute_umult_spec_tests, "umult_spec_tests"sv);
	execute_test(execute_uintcontainer_adc_tests, "uintcontainer_adc_tests"sv);

	cout << "STANDARD TEST BATTERY: All tests PASSED." << newl;
	static_assert(most_sign_set_bit(2_u128) == 1);
}

void cjm::uint128_tests::execute_parse_file_test(std::string_view path, size_t expected_ops)
{
	auto op_vec = binary_op_u128_vect_t{};
	try
	{
		auto stream = string::make_throwing_ifstream<char>(path);
		stream >> op_vec;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error opening reading or parsing file: [" << path << "]. Msg: [" << ex.what() << "]." << newl;
		throw;
	}
	std::cout << "Read: " << op_vec.size() << " operations from [" << path << "]." << newl;
	cjm_assert(expected_ops == op_vec.size());
}
void cjm::uint128_tests::execute_unary_op_post_stat_assert_test()
{
	auto post_inc = unary_op_u128_t{unary_op::post_increment, 1_u128};
	auto post_dec = unary_op_u128_t{unary_op::post_decrement, 1_u128};
	append_static_assertion(std::cout, post_inc) << newl;
	append_static_assertion(std::cout, post_dec) << newl;
}
void cjm::uint128_tests::execute_test_convert_to_double()
{
	constexpr auto oh_d00d_big_src = 0xc0de'd00d'dad0'0000'0000'0000'0000'0000_u128;
	constexpr auto oh_d00d_half_src = 0xc0de'd00d'dad0'0000_u128;
	const auto oh_d00d_big_ctrl = to_ctrl(oh_d00d_big_src);
	const auto oh_d00d_half_ctrl = to_ctrl(oh_d00d_half_src);
	const auto oh_d00d_big_alt_ctrl = to_alt_ctrl(oh_d00d_big_src);
	const auto oh_dood_half_alt_ctrl = to_alt_ctrl(oh_d00d_half_src);
	cjm_assert(to_test(oh_d00d_big_alt_ctrl) == oh_d00d_big_src);
	cjm_assert(to_test(oh_dood_half_alt_ctrl) == oh_d00d_half_src);
	using flt_t = double;
	const flt_t zero = 0.0;
	const flt_t one = 1.0;
	const flt_t zero_point_zero_one = 0.01;
	const flt_t big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
	const flt_t not_quite_as_big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000_u128);
	const flt_t oh_d00d_big = static_cast<flt_t>(oh_d00d_big_src);
	const flt_t oh_d00d_half = static_cast<flt_t>(oh_d00d_half_src);
	const flt_t ctrl_oh_d00d_big = static_cast<flt_t>(oh_d00d_big_ctrl);
	const flt_t ctrl_oh_d00d_half = static_cast<flt_t>(oh_d00d_half_ctrl);
	const flt_t alt_ctrl_oh_d00d_big = static_cast<flt_t>(oh_d00d_big_alt_ctrl);
	cjm_assert_close_enough(static_cast<uint128_t>(zero), 0_u128);
	cjm_assert_close_enough(static_cast<uint128_t>(one), 1_u128);
	cjm_assert_close_enough(static_cast<uint128_t>(zero_point_zero_one), 0_u128);
	cjm_assert_close_enough(static_cast<uint128_t>(big_ass_num), 0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
	cjm_assert_close_enough(static_cast<uint128_t>(not_quite_as_big_ass_num), 0x8000'0000'0000'0000_u128);
	const auto rt_oh_d00d_big = static_cast<uint128_t>(oh_d00d_big);
	const auto rt_oh_d00d_half = static_cast<uint128_t>(oh_d00d_half);
	cjm_assert_close_enough(rt_oh_d00d_big, oh_d00d_big_src);
	cjm_assert_close_enough(rt_oh_d00d_half, oh_d00d_half_src);
	cjm_assert_equal(static_cast<uint128_t>(ctrl_oh_d00d_big), oh_d00d_big_src);
	cjm_assert_equal(static_cast<uint128_t>(alt_ctrl_oh_d00d_big), oh_d00d_big_src);
	cjm_assert_equal(static_cast<uint128_t>(ctrl_oh_d00d_half), oh_d00d_half_src);
}
void cjm::uint128_tests::execute_test_convert_to_long_double()
{
	using flt_t = long double;
	constexpr auto oh_d00d_big_src = 0xc0de'd00d'dad0'0000'0000'0000'0000'0000_u128;
	constexpr auto oh_d00d_half_src = 0xc0de'd00d'dad0'0000_u128;
	const auto oh_d00d_big_ctrl = to_ctrl(oh_d00d_big_src);
	const auto oh_d00d_half_ctrl = to_ctrl(oh_d00d_half_src);
	const auto oh_d00d_big_alt_ctrl = to_alt_ctrl(oh_d00d_big_src);
	const auto oh_dood_half_alt_ctrl = to_alt_ctrl(oh_d00d_half_src);
	cjm_assert(to_test(oh_d00d_big_alt_ctrl) == oh_d00d_big_src);
	cjm_assert(to_test(oh_dood_half_alt_ctrl) == oh_d00d_half_src);
	const flt_t zero = 0.0;
	const flt_t one = 1.0;
	const flt_t zero_point_zero_one = 0.01;
	const flt_t big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
	const flt_t not_quite_as_big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000_u128);
	const flt_t oh_d00d_big = static_cast<flt_t>(oh_d00d_big_src);
	const flt_t oh_d00d_half = static_cast<flt_t>(oh_d00d_half_src);
	const flt_t ctrl_oh_d00d_big = static_cast<flt_t>(oh_d00d_big_ctrl);
	const flt_t ctrl_oh_d00d_half = static_cast<flt_t>(oh_d00d_half_ctrl);
	const flt_t alt_ctrl_oh_d00d_big = static_cast<flt_t>(oh_d00d_big_alt_ctrl);
	cjm_assert_close_enough(static_cast<uint128_t>(zero), 0_u128);
	cjm_assert_close_enough(static_cast<uint128_t>(one), 1_u128);
	cjm_assert_close_enough(static_cast<uint128_t>(zero_point_zero_one), 0_u128);
	cjm_assert_close_enough(static_cast<uint128_t>(big_ass_num), 0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
	cjm_assert_close_enough(static_cast<uint128_t>(not_quite_as_big_ass_num), 0x8000'0000'0000'0000_u128);
	const auto rt_oh_d00d_big = static_cast<uint128_t>(oh_d00d_big);
	const auto rt_oh_d00d_half = static_cast<uint128_t>(oh_d00d_half);
	cjm_assert_close_enough(rt_oh_d00d_big, oh_d00d_big_src);
	cjm_assert_close_enough(rt_oh_d00d_half, oh_d00d_half_src);
	cjm_assert_equal(static_cast<uint128_t>(ctrl_oh_d00d_big), rt_oh_d00d_big);
	cjm_assert_equal(static_cast<uint128_t>(alt_ctrl_oh_d00d_big), rt_oh_d00d_big);
	cjm_assert_equal(static_cast<uint128_t>(ctrl_oh_d00d_half), rt_oh_d00d_half);
	
   
}

void cjm::uint128_tests::execute_throwing_float_conversion_test()
{
	constexpr float max_float = std::numeric_limits<float>::max();
	constexpr double max_double = std::numeric_limits<double>::max();
	constexpr long double max_l_d = std::numeric_limits<long double>::max();

	using arb_int = boost::multiprecision::cpp_int;
	constexpr auto max_uint128_t = std::numeric_limits<uint128_t>::max();
	const ctrl_uint128_t ctrl_max = to_ctrl(max_uint128_t);
	const arb_int arb_max = static_cast<arb_int>(ctrl_max);

	const bool max_float_exceeds = static_cast<arb_int>(max_float) > arb_max;
	const bool max_double_exceeds = static_cast<arb_int>(max_double) > arb_max;
	const bool max_ld_exceeds = static_cast<arb_int>(max_l_d) > arb_max;

	if (max_float_exceeds)
	{
		cjm_assert_throws<std::invalid_argument>([]()-> void
		{
				const auto converted = numerics::safe_from_floating_or_throw(max_float);
				std::cerr << "You should never see this: " << converted << newl;
		});
	}
	if (max_double_exceeds)
	{
		cjm_assert_throws<std::invalid_argument>([]()-> void
		{
				const auto converted = numerics::safe_from_floating_or_throw(max_double);
				std::cerr << "You should never see this: " << converted << newl;
		});
	}
	if (max_ld_exceeds)
	{
		cjm_assert_throws<std::invalid_argument>([]()-> void
		{
				const auto converted = numerics::safe_from_floating_or_throw(max_l_d);
				std::cerr << "You should never see this: " << converted << newl;
		});
	}

	cjm_assert_throws<std::invalid_argument>([]()-> void
	{
		const auto converted = numerics::safe_from_floating_or_throw(-1.01f);
		std::cerr << "You should never see this: " << converted << newl;
	});
	cjm_assert_throws<std::invalid_argument>([]()-> void
	{
		const auto converted = numerics::safe_from_floating_or_throw(-1.01);
		std::cerr << "You should never see this: " << converted << newl;
	});
	cjm_assert_throws<std::invalid_argument>([]()-> void
	{
		const auto converted = numerics::safe_from_floating_or_throw(-1.01L);
		std::cerr << "You should never see this: " << converted << newl;
	});
	{
		using flt_t = float;
		const flt_t zero = 0.0f;
		const flt_t one = 1.0f;
		const flt_t zero_point_zero_one = 0.01f;
		const flt_t big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
		const flt_t not_quite_as_big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000_u128);

		cjm_assert_close_enough(numerics::safe_from_floating_or_throw(zero), 0_u128);
		cjm_assert_close_enough(numerics::safe_from_floating_or_throw(one), 1_u128);
		cjm_assert_close_enough(numerics::safe_from_floating_or_throw(zero_point_zero_one), 0_u128);
		cjm_assert_close_enough(numerics::safe_from_floating_or_throw(big_ass_num), 0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
		cjm_assert_close_enough(numerics::safe_from_floating_or_throw(not_quite_as_big_ass_num), 0x8000'0000'0000'0000_u128);
	}
	{
		using flt_t = double;
		const flt_t zero = 0.0;
		const flt_t one = 1.0;
		const flt_t zero_point_zero_one = 0.01;
		const flt_t big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
		const flt_t not_quite_as_big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000_u128);

		cjm_assert_close_enough(numerics::safe_from_floating_or_throw(zero), 0_u128);
		cjm_assert_close_enough(numerics::safe_from_floating_or_throw(one), 1_u128);
		cjm_assert_close_enough(numerics::safe_from_floating_or_throw(zero_point_zero_one), 0_u128);
		cjm_assert_close_enough(numerics::safe_from_floating_or_throw(big_ass_num), 0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
		cjm_assert_close_enough(numerics::safe_from_floating_or_throw(not_quite_as_big_ass_num), 0x8000'0000'0000'0000_u128);
	}
	{
		using flt_t = long double;
		const flt_t zero = 0.0L;
		const flt_t one = 1.0L;
		const flt_t zero_point_zero_one = 0.01L;
		const flt_t big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
		const flt_t not_quite_as_big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000_u128);

		cjm_assert_close_enough(numerics::safe_from_floating_or_throw(zero), 0_u128);
		cjm_assert_close_enough(numerics::safe_from_floating_or_throw(one), 1_u128);
		cjm_assert_close_enough(numerics::safe_from_floating_or_throw(zero_point_zero_one), 0_u128);
		cjm_assert_close_enough(numerics::safe_from_floating_or_throw(big_ass_num), 0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
		cjm_assert_close_enough(numerics::safe_from_floating_or_throw(not_quite_as_big_ass_num), 0x8000'0000'0000'0000_u128);
	}
}

void cjm::uint128_tests::execute_safe_float_conversions_test()
{
	constexpr float max_float = std::numeric_limits<float>::max();
	constexpr double max_double = std::numeric_limits<double>::max();
	constexpr long double max_l_d = std::numeric_limits<long double>::max();

	using arb_int = boost::multiprecision::cpp_int;
	constexpr auto max_uint128_t = std::numeric_limits<uint128_t>::max();
	const ctrl_uint128_t ctrl_max = to_ctrl(max_uint128_t);
	const arb_int arb_max = static_cast<arb_int>(ctrl_max);

	const bool max_float_exceeds = static_cast<arb_int>(max_float) > arb_max;
	const bool max_double_exceeds = static_cast<arb_int>(max_double) > arb_max;
	const bool max_ld_exceeds = static_cast<arb_int>(max_l_d) > arb_max;

	if (max_float_exceeds)
	{
		const auto converted = numerics::safe_from_floating(max_float);
		cjm_assert_nullopt(converted);
	}
	if (max_double_exceeds)
	{
		const auto converted = numerics::safe_from_floating(max_double);
		cjm_assert_nullopt(converted);
	}
	if (max_ld_exceeds)
	{
		const auto converted = numerics::safe_from_floating(max_l_d);
		cjm_assert_nullopt(converted);
	}

	{
		const auto converted = numerics::safe_from_floating(-1.01f);
		cjm_assert_nullopt(converted);
	}
	{
		const auto converted = numerics::safe_from_floating(-1.01);
		cjm_assert_nullopt(converted);
	}
	{
		const auto converted = numerics::safe_from_floating(-1.01L);
		cjm_assert_nullopt(converted);
	}
	
	{
		using flt_t = float;
		const flt_t zero = 0.0f;
		const flt_t one = 1.0f;
		const flt_t zero_point_zero_one = 0.01f;
		const flt_t big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
		const flt_t not_quite_as_big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000_u128);

		cjm_assert_close_enough(numerics::safe_from_floating(zero), 0_u128);
		cjm_assert_close_enough(numerics::safe_from_floating(one), 1_u128);
		cjm_assert_close_enough(numerics::safe_from_floating(zero_point_zero_one), 0_u128);
		cjm_assert_close_enough(numerics::safe_from_floating(big_ass_num), 0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
		cjm_assert_close_enough(numerics::safe_from_floating(not_quite_as_big_ass_num), 0x8000'0000'0000'0000_u128);
	}
	{
		using flt_t = double;
		const flt_t zero = 0.0;
		const flt_t one = 1.0;
		const flt_t zero_point_zero_one = 0.01;
		const flt_t big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
		const flt_t not_quite_as_big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000_u128);

		cjm_assert_close_enough(numerics::safe_from_floating(zero), 0_u128);
		cjm_assert_close_enough(numerics::safe_from_floating(one), 1_u128);
		cjm_assert_close_enough(numerics::safe_from_floating(zero_point_zero_one), 0_u128);
		cjm_assert_close_enough(numerics::safe_from_floating(big_ass_num), 0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
		cjm_assert_close_enough(numerics::safe_from_floating(not_quite_as_big_ass_num), 0x8000'0000'0000'0000_u128);
	}
	{
		using flt_t = long double;
		const flt_t zero = 0.0L;
		const flt_t one = 1.0L;
		const flt_t zero_point_zero_one = 0.01L;
		const flt_t big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
		const flt_t not_quite_as_big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000_u128);

		cjm_assert_close_enough(numerics::safe_from_floating(zero), 0_u128);
		cjm_assert_close_enough(numerics::safe_from_floating(one), 1_u128);
		cjm_assert_close_enough(numerics::safe_from_floating(zero_point_zero_one), 0_u128);
		cjm_assert_close_enough(numerics::safe_from_floating(big_ass_num), 0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
		cjm_assert_close_enough(numerics::safe_from_floating(not_quite_as_big_ass_num), 0x8000'0000'0000'0000_u128);
	}
}

void cjm::uint128_tests::execute_controlled_from_float_conversion_test()
{
	constexpr size_t num_tests = 10'000;
	auto rgen = generator::rgen{};
	std::cout << newl << "Executing " << num_tests << " from float conversion tests:" << newl;
	{
		size_t executed = 0;
		do 
		{
			const auto test_val = generator::generate_u128_float<float>(rgen);
			test_convert_float_value(test_val);
		} while (++executed < num_tests);
	}
	std::cout << "All " << num_tests << " tests passed for float." << newl;
	std::cout << "Executing " << num_tests << " from double conversion tests:" << newl;
	{
		size_t executed = 0;
		do
		{
			const auto test_val = generator::generate_u128_float<double>(rgen);
			test_convert_float_value(test_val);
		} while (++executed < num_tests);
	}
	std::cout << "All " << num_tests << " tests passed for double." << newl;
	std::cout << "Executing " << num_tests << " from long double conversion tests:" << newl;
	{
		size_t executed = 0;
		do
		{
			const auto test_val = generator::generate_u128_float<long double>(rgen);
			test_convert_float_value(test_val);
		} while (++executed < num_tests);
	}
	std::cout << "All " << num_tests << " tests passed for long double." << newl;
	
}

void cjm::uint128_tests::execute_controlled_float_rt_conversion_test()
{
	constexpr size_t num_values = 7;
	auto values = std::array<uint128_t, num_values>
	{
		0xc0de'd00d'dad0'0000'0000'0000'0000'0000_u128,
		0xc0de'd00d'0000'0000'0000'0000'0000'0000_u128,
		0xc0de'd00d'dad0'0000_u128,

		0xc0de'd00d'0000'0000_u128,
		0xcafe'0000'0000'0000'0000'0000'0000'0000_u128,
		0xcafe'0000'0000'0000'0000'0000'0000'0000_u128,
		0xc0de'd00d'fea2'cafe'babe'b00b'600d'f001_u128
	};

	cout << newl << "Going to execute tests for float: " << newl;
	std::for_each(values.cbegin(), values.cend(), [](const uint128_t& v) -> void
	{
		test_u128_rt_float_conversion<float>(v);
	});
	cout << "Done executing test for float ... proceeding to double:" << newl;
	std::for_each(values.cbegin(), values.cend(), [](const uint128_t& v) -> void
	{
		test_u128_rt_float_conversion<double>(v);
	});
	cout << "Done executing test for double ... proceeding to long double:" << newl;
	std::for_each(values.cbegin(), values.cend(), [](const uint128_t& v) -> void
	{
		test_u128_rt_float_conversion<long double>(v);
	});
	cout << "long double passed." << newl;
}

void cjm::uint128_tests::execute_hash_dx()
{
	constexpr size_t num_hashes = 1'000'000;
	auto gen = generator::rgen{};
	size_t unique_values, unique_hashes, difference;
	try
	{
		auto hashes = std::unordered_set<size_t>{};
		hashes.reserve(num_hashes);
								
		auto vec = std::vector<uint128_t>{};
		vec.reserve(num_hashes);

		while (vec.size() < num_hashes)
		{
			vec.emplace_back(generator::create_random_in_range<uint128_t>(gen));
		}

		auto values = std::unordered_set<uint128_t>(vec.cbegin(), vec.cend());
				

		std::transform(values.cbegin(), values.cend(), std::inserter(hashes, hashes.begin()),
		[](const uint128_t& v) -> size_t
		{
			return std::hash<uint128_t>{}(v);
		});
		unique_values = values.size();
		unique_hashes = hashes.size();
		difference = unique_values - unique_hashes;		
	}
	catch (const std::bad_alloc& ex)
	{
		std::cerr
			<< "HASH_DX ran out of memory and could not be completed.  "
			<<"This will not count as a failed test, but no diagnostic "
			<<"info can be provided. Consider reducing num_hashes (currently: "
			<< num_hashes << "), changing to x64 or getting more memory if "
			<< "you want to see the results of this dx.  "
			<< "Exception message: [" << ex.what() << "]." << newl;
		return;
	}
	
	std::cout << "Of " << num_hashes << " uint128_t's, " << unique_values
		<< " unique values were generated.  Those values were reduced to "
		<< unique_hashes << " unique hashes." << " There were " << difference
		<< " colliding hash values." << newl;
}

void cjm::uint128_tests::execute_issue_10_strm_insrt_test()
{
	using pair_t = std::pair<uint128_t, size_t>;
	using tuple_t = std::tuple<uint128_t, size_t, std::string>;
	constexpr std::array<pair_t,7> x = 
	{
		std::make_pair(0x00_u128, 1_szt),
		std::make_pair(0x01_u128, 1_szt),
		std::make_pair(0x10_u128, 2_szt),
		std::make_pair(0x100_u128, 3_szt),
		std::make_pair(0x1000, 4_szt),
		std::make_pair(0xfea2'dead'beef'f00d_u128, 16_szt),
		std::make_pair(0xc0de'd00d'fea2'cafe'babe'b00b'600d'f00d_u128, 32_szt)
	};

	auto make_string = [](const pair_t& p, bool setw) -> std::string
	{
		auto strm = string::make_throwing_sstream<char>();
		strm << std::hex;
		if (setw)
		{
			strm << std::setw(std::numeric_limits<uint128_t>::digits / 4)
				<< std::setfill('0') << p.first;
		}
		else
		{
			strm << p.first;
		}
		return strm.str();
	};
	constexpr auto setw_expect_width = static_cast<size_t>(std::numeric_limits<uint128_t>::digits / 4);
	auto setw_res_vec = std::vector<tuple_t>{};
	auto nosetw_res_vec = std::vector<tuple_t>{};
	for (const auto& p : x)
	{
		std::string setw_str = make_string(p, true);
		std::string nosetw_str = make_string(p, false);
		if (setw_str.size() != setw_expect_width)
			setw_res_vec.emplace_back(std::make_tuple(p.first, setw_expect_width, std::move(setw_str)));
		if (nosetw_str.size() != p.second)
			setw_res_vec.emplace_back(std::make_tuple(p.first, p.second, std::move(nosetw_str)));		
	}

	try
	{
		cjm_assert(setw_res_vec.empty() && nosetw_res_vec.empty());
	}
	catch (const testing_failure& ex)
	{
		auto saver = cout_saver{ std::cerr };
		std::cerr << std::dec;
		std::cerr << newl << "\tTest failed.  One or more string's were not the expected length." << newl;
		if (!setw_res_vec.empty())
		{
			std::cerr   << "\tThe following " << setw_res_vec.size()
						<< " results were made with setw and have unexpected lengths:" << newl;
			for (const auto& [value, length,
				text] : setw_res_vec)
			{
				std::cerr
					<< "\t\tFor value: [0x" << std::hex << value
					<< "], expected width: [" << std::dec
					<< length << "]; text: [" << text << "]; actual length: ["
					<< text.length() << "]." << newl;
			}
		}
		if (!nosetw_res_vec.empty())
		{
			std::cerr << "\tThe following " << nosetw_res_vec.size()
				<< " results were made WITHOUT setw and have unexpected lengths:" << newl;
			for (const auto& [value, length,
				text] : setw_res_vec)
			{
				std::cerr
					<< "\t\tFor value: [0x" << std::hex << value
					<< "], expected width: [" << std::dec
					<< length << "]; text: [" << text << "]; actual length: ["
					<< text.length() << "]." << newl;
			}
		}
		std::cerr
			<< "Done printing failing results.  Original exception msg: ["
			<< ex.what() << "]." << newl;
		throw;
	}	
}

void cjm::uint128_tests::execute_issue_10_showbase_test()
{
	using namespace std::string_view_literals;
	constexpr auto one = 0x01_u128;
	constexpr auto beef = 0xdead'beef_u128;
	constexpr auto beef_prepend_1 = 0x1'dead'beef_u128;
	constexpr auto cafe_babe = 0xc0de'd00d'fea2'cafe'babe'b00b'600d'f00d_u128;
	auto get_string = [](const uint128_t& v, bool setw) -> std::string
	{
		auto stream = string::make_throwing_sstream<char>();
		stream << std::hex << std::showbase;
		if (setw)
			stream << std::internal << std::setw(32) << std::setfill('0') << v;
		else
			stream << v;
		return stream.str();
	};
	
	
	cjm_assert(get_string(one, false) == "0x1"sv);
	cjm_assert(get_string(beef, false) == "0xdeadbeef"sv);
	cjm_assert(get_string(beef_prepend_1, false) == "0x1deadbeef"sv);
	cjm_assert(get_string(cafe_babe, false) == "0xc0ded00dfea2cafebabeb00b600df00d"sv);

	constexpr auto expected_one = "0x00000000000000000000000000000001"sv;
	cjm_assert(34_szt == expected_one.size());
	const auto actual_one = get_string(one, true);

	constexpr auto expected_beef = "0x000000000000000000000000deadbeef"sv;
	cjm_assert(34_szt == expected_beef.size());
	const auto actual_beef = get_string(beef, true);

	constexpr auto expected_beef_prep_1 = "0x000000000000000000000001deadbeef"sv;
	cjm_assert(34_szt == expected_beef_prep_1.size());
	const auto actual_beef_prep_1 = get_string(beef_prepend_1, true);

	constexpr auto expected_cafe_babe = "0xc0ded00dfea2cafebabeb00b600df00d"sv;
	cjm_assert(34_szt == expected_cafe_babe.size());
	const auto actual_cafe_babe = get_string(cafe_babe, true);
	
	cjm_assert(expected_one == actual_one);
	cjm_assert(expected_beef == actual_beef);
	cjm_assert(expected_beef_prep_1 == actual_beef_prep_1);
	cjm_assert(expected_cafe_babe == actual_cafe_babe );
	
}

void cjm::uint128_tests::execute_test_convert_to_float()
{
	using flt_t = float;
	constexpr auto oh_d00d_big_src = 0xc0de'd00d'dad0'0000'0000'0000'0000'0000_u128;
	constexpr auto oh_d00d_half_src = 0xc0de'd00d'dad0'0000_u128;
	const auto oh_d00d_big_ctrl = to_ctrl(oh_d00d_big_src);
	const auto oh_d00d_half_ctrl = to_ctrl(oh_d00d_half_src);
	const auto oh_d00d_big_alt_ctrl = to_alt_ctrl(oh_d00d_big_src);
	const auto oh_dood_half_alt_ctrl = to_alt_ctrl(oh_d00d_half_src);
	cjm_assert(to_test(oh_d00d_big_alt_ctrl) == oh_d00d_big_src);
	cjm_assert(to_test(oh_dood_half_alt_ctrl) == oh_d00d_half_src);
	const flt_t zero = 0.0f;
	const flt_t one = 1.0f;
	const flt_t zero_point_zero_one = 0.01f;
	const flt_t big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
	const flt_t not_quite_as_big_ass_num = static_cast<flt_t>(0x8000'0000'0000'0000_u128);
	const flt_t oh_d00d_big = static_cast<flt_t>(oh_d00d_big_src);
	const flt_t oh_d00d_half = static_cast<flt_t>(oh_d00d_half_src);
	const flt_t ctrl_oh_d00d_big = static_cast<flt_t>(oh_d00d_big_ctrl);
	const flt_t ctrl_oh_d00d_half = static_cast<flt_t>(oh_d00d_half_ctrl);
	const flt_t alt_ctrl_oh_d00d_big = static_cast<flt_t>(oh_d00d_big_alt_ctrl);

	cjm_assert_close_enough(static_cast<uint128_t>(zero), 0_u128);
	cjm_assert_close_enough(static_cast<uint128_t>(one), 1_u128);
	cjm_assert_close_enough(static_cast<uint128_t>(zero_point_zero_one), 0_u128);
	cjm_assert_close_enough(static_cast<uint128_t>(big_ass_num), 0x8000'0000'0000'0000'0000'0000'0000'0000_u128);
	cjm_assert_close_enough(static_cast<uint128_t>(not_quite_as_big_ass_num), 0x8000'0000'0000'0000_u128);
	cjm_assert_close_enough(static_cast<uint128_t>(oh_d00d_big), oh_d00d_big_src);
	cjm_assert_close_enough(static_cast<uint128_t>(oh_d00d_half), oh_d00d_half_src);
	const auto rt_oh_d00d_big = static_cast<uint128_t>(oh_d00d_big);
	const auto rt_oh_d00d_half = static_cast<uint128_t>(oh_d00d_half);
	cjm_assert_close_enough(rt_oh_d00d_big, oh_d00d_big_src);
	cjm_assert_close_enough(rt_oh_d00d_half, oh_d00d_half_src);
	cjm_assert_equal(static_cast<uint128_t>(ctrl_oh_d00d_big), rt_oh_d00d_big);
	cjm_assert_equal(static_cast<uint128_t>(alt_ctrl_oh_d00d_big), rt_oh_d00d_big);
	cjm_assert_equal(static_cast<uint128_t>(ctrl_oh_d00d_half), rt_oh_d00d_half);
	
}

void cjm::uint128_tests::execute_unary_op_pre_inc_test()
{
	constexpr auto test_name = "unary_op_pre_inc_test"sv;
	auto op_vec = generate_specified_un_ops(unary_op::pre_increment, 5, true);
	for (auto& itm : op_vec)
	{
		test_unary_operation(itm, test_name);
	}
	//print_n_static_assertions(op_vec, 10_szt);
}

void cjm::uint128_tests::execute_unary_op_post_inc_test()
{
	constexpr auto test_name = "unary_op_post_inc_test"sv;
	auto op_vec = generate_specified_un_ops(unary_op::post_increment, 5, true);
	for (auto& itm : op_vec)
	{
		test_unary_operation(itm, test_name);
	}
	//print_n_static_assertions(op_vec, 10_szt);
}

void cjm::uint128_tests::execute_unary_op_pre_dec_test()
{
	constexpr auto test_name = "unary_op_pre_dec_test"sv;
	auto op_vec = generate_specified_un_ops(unary_op::pre_decrement, 5, true);
	for (auto& itm : op_vec)
	{
		test_unary_operation(itm, test_name);
	}
	//print_n_static_assertions(op_vec, 10_szt);
}

void cjm::uint128_tests::execute_unary_op_post_dec_test()
{
	constexpr auto test_name = "unary_op_post_dec_test"sv;
	auto op_vec = generate_specified_un_ops(unary_op::post_decrement, 5, true);
	for (auto& itm : op_vec)
	{
		test_unary_operation(itm, test_name);
	}
}

void cjm::uint128_tests::execute_unary_op_unary_plus_test()
{
	constexpr auto test_name = "unary_op_unary_plus_test"sv;
	auto op_vec = generate_specified_un_ops(unary_op::unary_plus, 5, true);
	for (auto& itm : op_vec)
	{
		test_unary_operation(itm, test_name);
	}
	//print_n_static_assertions(op_vec, 10_szt);
}

void cjm::uint128_tests::execute_unary_op_unary_minus_test()
{
	constexpr auto test_name = "unary_op_unary_minus_test"sv;
	auto op_vec = generate_specified_un_ops(unary_op::unary_minus, 5, true);
	for (auto& itm : op_vec)
	{
		test_unary_operation(itm, test_name);
	}
	//print_n_static_assertions(op_vec, 10_szt);
}

void cjm::uint128_tests::execute_unary_op_bitwise_not_test()
{
	constexpr auto test_name = "unary_op_bitwise_not_test"sv;
	auto op_vec = generate_specified_un_ops(unary_op::bitwise_not, 5, true);
	for (auto& itm : op_vec)
	{
		test_unary_operation(itm, test_name);
	}
	//print_n_static_assertions(op_vec, 10_szt);
}

void cjm::uint128_tests::execute_unary_op_bool_cast_test()
{
	constexpr auto test_name = "unary_op_bool_cast_test"sv;
	auto op_vec = generate_specified_un_ops(unary_op::bool_cast, 5, true);
	for (auto& itm : op_vec)
	{
		test_unary_operation(itm, test_name);
	}
	//print_n_static_assertions(op_vec, 10_szt);
}

void cjm::uint128_tests::execute_unary_op_logical_negation_test()
{
	constexpr auto test_name = "unary_op_logical_negation_test"sv;
	auto op_vec = generate_specified_un_ops(unary_op::logical_negation, 5, true);
	for (auto& itm : op_vec)
	{
		test_unary_operation(itm, test_name);
	}
	//print_n_static_assertions(op_vec, 10_szt);
}

void cjm::uint128_tests::print_n_static_assertions(const binary_op_u128_vect_t& op_vec, size_t n)
{
	auto saver = cout_saver{ cout };
	if (op_vec.empty())
	{
		std::cerr << "Cannot print " << std::dec << n << " static assertions: vector contains no operations." << newl;
	}
	else
	{
		if (op_vec.size() < n)
		{
			std::cerr << "Cannot print " << std::dec << n << " static assertions: vector contains only " << op_vec.size() << " operations.  Will print " << op_vec.size() << " assertions instead." << newl;
			n = op_vec.size();			
		}
		std::cout << " Printing " << std::dec << n << " static assertions: " << newl;
		for (size_t i = 0; i < n; ++i)
		{
			append_static_assertion(std::cout, op_vec[i]) << newl;
		}
		std::cout << "DONE printing " << std::dec << n << " static assertions: " << newl;
	}	
}

void cjm::uint128_tests::print_n_static_assertions(const cjm::uint128_tests::unary_op_u128_vect_t& op_vec, size_t n)
{
	auto saver = cout_saver{cout};
	if (op_vec.empty())
	{
		std::cerr << "Cannot print " << std::dec << n << " static assertions: vector contains no operations." << newl;
	}
	else
	{
		if (op_vec.size() < n)
		{
			std::cerr << "Cannot print " << std::dec << n << " static assertions: vector contains only " << op_vec.size() << " operations.  Will print " << op_vec.size() << " assertions instead." << newl;
			n = op_vec.size();
		}
		std::cout << " Printing " << std::dec << n << " static assertions: " << newl;
		for (size_t i = 0; i < n; ++i)
		{
			append_static_assertion(std::cout, op_vec[i]) << newl;
		}
		std::cout << "DONE printing " << std::dec << n << " static assertions: " << newl;
	}
}


void cjm::uint128_tests::execute_basic_test_one()
{
	constexpr uint128_t big = 0xc0de'd00d'fea2'b00b'c0de'd00d'fea2'b00b_u128;
	cout_saver saver{cout};
	cout << std::hex << "Printing hex: [0x" <<  big << "]." << newl;
	cout << std::dec << "Printing dec: [" << big << "]." << newl;
}


void cjm::uint128_tests::execute_string_parse_test()
{
	constexpr uint128_t test_hex = 0xc0de'd00d'face'cafe'babe'b00b'fea2'dad0_u128;
	constexpr uint128_t test_dec = 256'368'684'942'083'501'355'085'096'987'188'714'192_u128;

	static_assert(test_hex == test_dec, "Values are not the same!");
	cout_saver saver{cout};
	cout << std::hex << "Printing hex: [0x" << test_hex << "]." << newl;
	cout << std::dec << "Printing dec: [" << test_hex << "]." << newl;

	using namespace std;
	using namespace cjm::string;
	using cjm::testing::cjm_assert;
	{
		auto default_strm = make_throwing_sstream<char>();
		auto wide_strm = make_throwing_sstream<wchar_t>();
		auto default_hex_strm = make_throwing_sstream<char>();
		auto wide_hex_strm = make_throwing_sstream<wchar_t>();
		auto utf8_stream = make_throwing_sstream<char8_t, std::char_traits<char8_t>, std::allocator<char8_t>>();
		auto utf8_hex_stream = make_throwing_sstream<char8_t, std::char_traits<char8_t>, std::allocator<char8_t>>();
		auto utf16_stream = make_throwing_sstream<char16_t>();
		auto utf16_hex_stream = make_throwing_sstream<char16_t>();
		auto utf32_stream = make_throwing_sstream<char32_t>();
		auto utf32_hex_stream = make_throwing_sstream<char32_t>();

		default_strm << test_dec;
		wide_strm << test_dec;
		utf8_stream << test_dec;
		utf16_stream << test_dec;
		utf32_stream << test_dec;

		default_hex_strm << test_hex;
		wide_hex_strm << test_hex;
		utf8_hex_stream << test_hex;
		utf16_hex_stream << test_hex;
		utf32_hex_stream << test_hex;
		

		 			
		uint128_t  v, w, x, y, z,   a, b, c, d, e;

		default_strm >> v;
		wide_strm >> w;
		utf8_stream >> x;
		utf16_stream >> y;
		utf32_stream >> z;

		default_hex_strm >> a;
		wide_hex_strm >> b;
		utf8_hex_stream >> c;
		utf16_hex_stream >> d;
		utf32_hex_stream >> e;
		
		
		cjm_assert(v == test_dec);
		cjm_assert(w == test_dec);
		cjm_assert(x == test_dec);
		cjm_assert(y == test_dec);
		cjm_assert(z == test_dec);

		cjm_assert(a == test_hex);
		cjm_assert(b == test_hex);
		cjm_assert(c == test_hex);
		cjm_assert(d == test_hex);
		cjm_assert(e == test_hex);

		cjm_assert(test_dec == test_hex);

	}

	{
		constexpr std::u8string_view u8as_hex = u8"0xc0ded00dfacecafebabeb00bfea2dad0"sv;
		constexpr std::u8string_view u8as_dec = u8"256368684942083501355085096987188714192"sv;
		constexpr std::u16string_view u16as_hex = u"0xc0ded00dfacecafebabeb00bfea2dad0"sv;
		constexpr std::u16string_view u16as_dec = u"256368684942083501355085096987188714192"sv;
		constexpr std::u32string_view u32as_hex = U"0xc0ded00dfacecafebabeb00bfea2dad0"sv;
		constexpr std::u32string_view u32as_dec = U"256368684942083501355085096987188714192"sv;

		uint128_t x = test_hex;
			//uint128_t ::make_from_string(u8as_hex);
		uint128_t y = uint128_t ::make_from_string(u8as_dec);
		cjm_assert(x == y && y == test_hex);
		y = uint128_t::make_from_string(u8as_hex);
		cjm_assert(y == x);

		//todo fixit add hex parse routines for string conversion ... should not be hard ... the literals already do this.
		x = uint128_t::make_from_string(u16as_dec);
		y = test_hex;
		cjm_assert(x == y && y == test_hex && x == test_dec);
		x = uint128_t::make_from_string(u16as_hex);
		cjm_assert(x == y && y == test_hex && x == test_dec);
		
		x = uint128_t::make_from_string(u32as_dec);
		y = test_hex;
		cjm_assert(x == y && y == test_hex && x == test_dec);
		x = uint128_t::make_from_string(u32as_hex);
		cjm_assert(x == y && y == test_hex && x == test_dec);
	}



}

void cjm::uint128_tests::execute_basic_multiplication_test()
{
	using cjm::testing::cjm_assert;
	using namespace numerics::uint128_literals;

	auto print_res = [](uint128_t res, std::string_view n) -> void
	{
		auto saver = cout_saver{ cout };
		std::cout << n << " hex: [0x" << std::hex << std::setw(32) << std::setfill('0') << res << "]." << newl;
		std::cout << n << " dec: [" << std::dec << res << "]." << newl;
	};

	constexpr uint128_t zero = 0;
	constexpr uint128_t one = 1;
	constexpr uint128_t max64 = std::numeric_limits<std::uint64_t>::max();
	constexpr uint128_t max64_less_one = max64 - 1;
	constexpr uint128_t fit_64 = 0xdead'beef'b00b'face;
	constexpr uint128_t big_one = 0xc0de'd00d'fea2'cafe'babe'b00b'600d'f00d_u128;
	constexpr uint128_t another_big_one = 0xdead'beef'600d'f00d'badd'd00d'c0de'face_u128;

	constexpr auto zero_times_one = zero * one;
	constexpr auto one_times_one = one * one;
	constexpr auto max64_less_one_times_one = max64_less_one * one;
	static_assert(zero_times_one == 0);
	static_assert(one_times_one == 1);
	static_assert(max64_less_one_times_one == max64_less_one);
	
	constexpr auto max64_times_max64_less_one = max64 * max64_less_one; 
	constexpr auto fit_times_big_one = fit_64 * big_one;
	constexpr auto fit_times_another_big_one = fit_64 * another_big_one;
	constexpr auto big_ones_product = big_one * another_big_one; 
	
	auto test_zero_times_one = zero * one;
	auto test_one_times_one = one * one;
	auto test_max64_less_one_times_one = max64_less_one * one;
	auto test_max64_times_max64_less_one = max64 * max64_less_one;
	auto test_fit_times_big_one = fit_64 * big_one;
	auto test_fit_times_another_big_one = fit_64 * another_big_one;
	auto test_big_ones_product = big_one * another_big_one;

	cjm_assert(test_zero_times_one == zero_times_one);
	cjm_assert(test_one_times_one == one_times_one);
	cjm_assert(test_max64_less_one_times_one == max64_less_one_times_one);
	cjm_assert(test_max64_times_max64_less_one == max64_times_max64_less_one);
	cjm_assert(test_fit_times_big_one == fit_times_big_one);
	cjm_assert(test_fit_times_another_big_one == fit_times_another_big_one);
	cjm_assert(test_big_ones_product == big_ones_product);

	cjm_assert(test_zero_times_one / one == zero);
	cjm_assert(test_one_times_one / one == one);
	cjm_assert(test_max64_times_max64_less_one / max64_less_one == 
		max64 && test_max64_times_max64_less_one / max64 == max64_less_one);


	ctrl_uint128_t ctrl_fit_times_big_one = to_ctrl(test_fit_times_big_one);
	test_interconversion(ctrl_fit_times_big_one, test_fit_times_big_one);
	ctrl_uint128_t ctrl_big_one = to_ctrl(big_one);
	test_interconversion(ctrl_big_one, big_one);
	ctrl_uint128_t ctrl_fit_64 = to_ctrl(fit_64);
	test_interconversion(ctrl_fit_64, fit_64);

	{
		auto save_me = cout_saver{ cout };
		ctrl_uint128_t ctrl_fit_big_div_res = ctrl_fit_times_big_one / ctrl_big_one;

		cjm_assert(to_test(ctrl_fit_times_big_one) == fit_times_big_one && to_test(ctrl_big_one) == big_one);
		
		cout	<< "ctrl_fit_big_div_res:= [0x" << std::hex << std::setw(32)
				<< std::setfill('0') << ctrl_fit_big_div_res << "]" << newl;
		
		uint128_t back_test_div_big = to_test(ctrl_fit_big_div_res);
		cout	<< "back_test_div_big := [0x" << std::hex << std::setw(32)
				<< std::setfill('0') << back_test_div_big << "]" << newl;

		uint128_t back_test_div_res = fit_times_big_one / big_one;
		cout	<< "back_test_div_res := [0x" << std::hex << std::setw(32)
				<< std::setfill('0') << back_test_div_res << "]" << newl;
		
		cjm_assert(back_test_div_big == fit_times_big_one / big_one);
	}

	ctrl_uint128_t ctrl_fit_fit_div_res = ctrl_fit_times_big_one / ctrl_fit_64;
	uint128_t back_test_div_fit = to_test(ctrl_fit_fit_div_res);
	cjm_assert(back_test_div_fit == fit_times_big_one / fit_64);
		
	print_res(test_zero_times_one, "test_zero_times_one"sv);
	print_res(test_one_times_one, "test_one_times_one"sv);
	print_res(test_max64_less_one_times_one, "test_max64_less_one_times_one"sv);
	print_res(test_max64_times_max64_less_one, "test_max64_times_max64_less_one"sv);
	print_res(test_fit_times_big_one, "test_fit_times_big_one"sv);
	print_res(test_fit_times_another_big_one, "test_fit_times_another_big_one"sv);
	print_res(test_big_ones_product, "test_big_ones_product"sv);
	
}

void cjm::uint128_tests::test_fls()
{
	constexpr auto final_test = 0x8000'0000'0000'0000u;
	constexpr auto final_res = cjm::numerics::internal::fls_default(final_test);
	constexpr auto numbers = get_pow2_arr();
	constexpr auto results = get_pow2_res_arr();
	static_assert(numbers.size() == results.size());
	static_assert(cjm::numerics::concepts::can_find_most_significant_set_bitpos<uint128_t>);
	using namespace cjm::numerics::uint128_literals;
	static_assert(cjm::numerics::most_sign_set_bit(3_u128) == 1);
	static_assert(cjm::numerics::most_sign_set_bit(3_u128 << 120) == 121);
	auto saver = cout_saver{ cout };
	for (auto i = 0_szt; i < numbers.size(); ++i)
	{
		cjm_assert(cjm::numerics::internal::fls_int_part(numbers[i]) == results[i]);
		cout << "For [0x" << std::hex << std::setw(16) << std::setfill('0') << numbers[i] << "], the result is: [" << std::dec << results[i] << "]." << newl;
	}
	cjm_assert(cjm::numerics::internal::fls_int_part(final_test) == final_res);
	cout << "For [0x" << std::hex << std::setw(16) << std::setfill('0') << final_test << "], the result is: [" << std::dec << final_res << "]." << newl;
}

void cjm::uint128_tests::test_interconversion(const ctrl_uint128_t& control, uint128_t test)
{
	const uint128_t ctrl_to_test = to_test(control);
	const ctrl_uint128_t test_to_control = to_ctrl(test);
	cjm_assert(test_to_control == control && ctrl_to_test == test);
}


void cjm::uint128_tests::print_uint128_eval_mode()
{
	using namespace numerics;
	cout_saver o_saver{cout};
	std::string_view eval_text = "Unknown/Unrecognized Arithmetic Mode"sv;
	switch (cjm::numerics::calculation_mode)
	{
		case uint128_calc_mode::intrinsic_u128:
			eval_text = "Built-in unsigned __int128 Arithmetic"sv;
			break;
		case uint128_calc_mode::msvc_x64:
			eval_text = "Microsoft x64 Compiler-Intrinsic Optimized Arithmetic"sv;
			break;
		case uint128_calc_mode::msvc_x64_clang_or_intel_llvm:
			eval_text = "Windows x64 Clang/Intel-LLVM Compiler-Intrinsic Optimized Arithmetic"sv;
			break;
		case uint128_calc_mode::default_eval:
			eval_text = "Fallback Arithmetic"sv;
			break;
	}
	cout << "Arithmetic mode: [" << eval_text << "]." << newl;
}
void cjm::uint128_tests::print_cpp20_bitops_available()
{
	constexpr auto avail_text = bool_to_yes_no(cjm::numerics::has_cpp20_bitops);
	using namespace numerics;
	cout_saver o_saver{cout};
	cout << "Are C++20 bit operations available?: ["sv << avail_text << "]."sv << newl;
}
void cjm::uint128_tests::print_constexpr_bitcast_available()
{
	constexpr auto avail_text = bool_to_yes_no(cjm::numerics::constexpr_bit_casting);
	using namespace numerics;
	cout_saver o_saver{cout};
	cout << "Is constexpr bitcasting available?: [" << avail_text << "]." << newl;
}

void cjm::uint128_tests::print_whether_has_consteval()
{
	auto saver = cout_saver{ std::cout };
	cout << "Is consteval (immediate function) available?: [" << std::boolalpha << cjm::numerics::internal::has_consteval << "]." << newl;
}

void cjm::uint128_tests::print_builtin_uint128_data_if_present()
{
	if constexpr (cjm::numerics::calculation_mode == numerics::uint128_calc_mode::intrinsic_u128)
	{
		constexpr auto arithmetic_text = bool_to_yes_no(std::is_arithmetic_v<cjm::numerics::natuint128_t>);
		constexpr auto integral_text = bool_to_yes_no(std::is_integral_v<cjm::numerics::natuint128_t>);
		constexpr auto unsigned_text = bool_to_yes_no(std::is_unsigned_v<cjm::numerics::natuint128_t>);

		cout << "Is unsigned __int128 an arithmetic type?: [" << arithmetic_text << "]." << newl;
		cout << "Is unsigned __int128 an integral type?: [" << integral_text << "]." << newl;
		cout << "Is unsigned __int128 an unsigned type?: [" << unsigned_text << "]." << newl;
		cout << "sizeof(std::uintmax_t): [" << sizeof(std::uintmax_t) << "]." << newl;
	}
	else
	{
		cout << "No built-in  uint128 data available." << newl;
	}
}
void cjm::uint128_tests::execute_first_bin_op_test()
{
	using bin_op_t = binary_operation<uint128_t, uint128_ctrl>;
	constexpr auto hash_width = std::numeric_limits<size_t>::digits / 4;
	//perform statically as well:
	static_assert(0x1_u128 + 0x2_u128 == 0x3_u128);
	auto saver = cout_saver {std::cout};
	auto op = bin_op_t { binary_op::add, 1_u128, 2_u128 };
	op.calculate_result();
	cjm_assert(op.has_correct_result());
	const auto& res = op.result();
	cjm_assert(res.value().first == res.value().second);
	std::cout << "Here is the first binary operation:" << newl;
	append_static_assertion(std::cout, op);
	std::cout << "Done appending the assertion." << newl;
	size_t hash = std::hash<binary_operation<uint128_t, uint128_ctrl>>{}(op);
	std::cout   << "hash code for op is: [0x" << std::hex
				<< std::setw(hash_width) << std::setfill('0')
				<< hash << "]." << newl;
	std::cout << "serialized operation: [" << op << "]" << newl;
	auto ss = string::make_throwing_sstream<char>();
	ss << op;
	bin_op_t round_tripped;
	ss >> round_tripped;
	cjm_assert(round_tripped == op);
	auto nothrow_op_1 = bin_op_t{binary_op::left_shift, 0, 127};
	auto nothrow_op_2 = bin_op_t{binary_op::right_shift, 0, 127 };
	auto nothrow_op_3 = bin_op_t{binary_op::divide, 0, 1};
	auto nothrow_op_4 = bin_op_t{binary_op::modulus, 0, 1};
	cjm_deny(nothrow_op_1.has_result());
	cjm_deny(nothrow_op_2.has_result());
	cjm_deny(nothrow_op_3.has_result());
	cjm_deny(nothrow_op_4.has_result());
	cjm_assert_throws<std::invalid_argument>([]() -> void
	{
		auto never_see_me = bin_op_t{binary_op::left_shift, 0, 128};
		std::cerr << "You should never see this: [" << never_see_me << "]." << newl;
	});
	cjm_assert_throws<std::invalid_argument>([]() -> void
	{
		auto never_see_me = bin_op_t{binary_op::right_shift, 0, 128};
		std::cerr << "You should never see this: [" << never_see_me << "]." << newl;
	});
	cjm_assert_throws<std::invalid_argument>([]() -> void
	{
		auto never_see_me = bin_op_t{binary_op::divide, 0, 0};
		std::cerr << "You should never see this: [" << never_see_me << "]." << newl;
	});
	cjm_assert_throws<std::invalid_argument>([]() -> void
	{
		 auto never_see_me = bin_op_t{binary_op::divide, 1, 0};
		 std::cerr << "You should never see this: [" << never_see_me << "]." << newl;
	});
	cjm_assert_throws<std::invalid_argument>([]() -> void
	{
		auto never_see_me = bin_op_t{binary_op::modulus, 0, 0};
		std::cerr << "You should never see this: [" << never_see_me << "]." << newl;
	});
	cjm_assert_throws<std::invalid_argument>([]() -> void
	{
		auto never_see_me = bin_op_t{binary_op::modulus, 1, 0};
		std::cerr << "You should never see this: [" << never_see_me << "]." << newl;
	});
 }

void cjm::uint128_tests::execute_gen_comp_ops_test()
{
	constexpr ctrl_uint128_t cmax = std::numeric_limits<ctrl_uint128_t>::max();
	constexpr uint128_t  tmax = std::numeric_limits<uint128_t>::max();

	cjm_assert(to_test(cmax) == tmax && to_ctrl(tmax) == cmax && u128_testing_constant_providers::testing_constant_provider<uint128_t>::maximum == tmax);

	auto gen = generator::rgen{};
	auto factory = [](const generator::rgen& g) -> binary_operation<uint128_t, ctrl_uint128_t>
	{
		return create_compare_op(g);
	};

	auto code_to_strong_ordering = [](const uint128_t& c) -> std::strong_ordering
	{
		if (c == 0)
			return std::strong_ordering::equal;
		if (c == 1)
			return std::strong_ordering::greater;
		return std::strong_ordering::less;
	};

	auto validate_according_to_ordering = [](const uint128_t& l, const uint128_t& r, std::strong_ordering ordering) -> bool
	{
		bool ret;
		if (ordering == std::strong_ordering::equivalent || ordering == std::strong_ordering::equal)
			ret= l == r && !(l > r) && !(l < r) && (l >= r) && (l <= r) && !(l != r) && (std::hash<uint128_t>{}(l) == std::hash<uint128_t>{}(r));
		else if (ordering == std::strong_ordering::less)
			ret= l != r && (l < r) && !(l > r) && !(l >= r) && (l <= r) && !(l == r);
		else // greater
			ret = l != r && (l > r) && !(l < r) && !(l <= r) && (l >= r) && !(l == r);
		return ret;
	};
	constexpr auto ops = 10'000u;
	auto vec = generator::create_many_ops(factory, gen, ops);
	cjm_assert(vec.size() == ops);
	cjm_assert(std::all_of(vec.cbegin(), vec.cend(), [](const binary_operation<uint128_t, ctrl_uint128_t>& op) -> bool
		{
			return op.op_code() == binary_op::compare;
		}));

	
	cjm_assert(std::all_of(vec.begin(), vec.end(), [=](binary_operation<uint128_t, ctrl_uint128_t>& op) -> bool
	{
			op.calculate_result();
			bool is_ok = false;
			bool thinks_ok = op.has_correct_result();
			if (thinks_ok)
			{
				const auto& res = op.result().value();
				std::strong_ordering t_ordering = code_to_strong_ordering(res.first);
				std::strong_ordering c_ordering = code_to_strong_ordering(res.second);
				if (t_ordering == c_ordering)
				{
					is_ok = validate_according_to_ordering(op.left_operand(), op.right_operand(), t_ordering);
				}				
			}
			return is_ok;
	}));
}

namespace cjm::uint128_tests::generator
{
	using twister_t = std::conditional_t<sizeof(std::uintptr_t) == 8, std::mt19937_64, std::mt19937>;
	using twister_seed_t = std::conditional_t<sizeof(std::uintptr_t) == 8, typename std::mt19937_64::result_type, typename std::mt19937::result_type>;

	[[nodiscard]] uint128_t unsafe_create_in_range(const rgen& rgen, size_t size);

	[[nodiscard]] binary_operation<uint128_t, ctrl_uint128_t>
		unsafe_create_division_op(const rgen& rgen, size_t max_dividend_size, size_t max_divisor_size);

	[[nodiscard]] binary_operation<uint128_t, ctrl_uint128_t>
		unsafe_create_modulus_op(const rgen& rgen, size_t max_dividend_size, size_t max_divisor_size);
}
namespace cjm::uint128_tests::generator::internal
{
	class rgen_impl final
	{
	public:
		friend class cjm::uint128_tests::generator::rgen;
		static std::unique_ptr<rgen_impl> make_rgen();
		~rgen_impl() = default;
		rgen_impl(const rgen_impl& other) = delete;
		rgen_impl(rgen_impl&& other) noexcept = delete;
		rgen_impl& operator=(const rgen_impl& other) = delete;
		rgen_impl& operator=(rgen_impl&& other) noexcept = delete;

		std::uint8_t random_byte();
		std::vector<std::uint8_t> generate(size_t bytes);
		int generate_shift_param(int num_digits);
		template<typename Int> requires(std::integral<Int> &&
			!cjm::numerics::concepts::character<Int> && (!std::is_same_v<std::remove_const_t<Int>, std::uint8_t>))
			Int random_from_range(std::uniform_int_distribution<Int> distrib)
		{
			return distrib(m_twister);
		}

	private:
		static twister_t init_twister();
		rgen_impl();
		twister_t m_twister;

	};


}

namespace
{
	using ssize_t = std::make_signed_t<size_t>;

}

std::unique_ptr<cjm::uint128_tests::generator::internal::rgen_impl> 
	cjm::uint128_tests::generator::internal::rgen_impl::make_rgen()
{
	return std::unique_ptr<rgen_impl>(new rgen_impl());
}

std::uint8_t cjm::uint128_tests::generator::internal::rgen_impl::random_byte()
{
	constexpr unsigned short bottom_half = 0x00ffu;
	auto distrib = std::uniform_int_distribution<unsigned short>();
	return static_cast<std::uint8_t>(distrib(m_twister) & bottom_half);
}

std::vector<std::uint8_t> cjm::uint128_tests::generator::internal::rgen_impl::generate(size_t bytes)
{

	auto distrib = std::uniform_int_distribution<std::uint64_t>();
	auto v = std::vector<uint8_t>{};
	v.reserve(bytes);
	auto diff = static_cast<ssize_t>(bytes - v.size());
	if (diff < 0) throw std::invalid_argument{ "Too many bytes ... conversion to signed causes overflow!" };
	do
	{
		auto generated = distrib(m_twister);
		const auto* byte_ptr_begin = reinterpret_cast<unsigned char*>(&generated);
		const auto* byte_ptr_end = byte_ptr_begin + (diff < static_cast<ssize_t>(sizeof(generated)) 
			? diff : sizeof(generated));
		for (const auto* it = byte_ptr_begin; it != byte_ptr_end; ++it)
		{
			v.push_back(*it);
		}
		diff = static_cast<ssize_t>(bytes - v.size());
	} while (diff > 0);
	return v;
}

int cjm::uint128_tests::generator::internal::rgen_impl::generate_shift_param(int num_digits)
{
	auto distrib = std::uniform_int_distribution<int>(0, num_digits - 1);
	return distrib(m_twister);
}



cjm::uint128_tests::generator::twister_t cjm::uint128_tests::generator::internal::rgen_impl::init_twister()
{
	auto seed_gen = std::random_device{};
	auto seed = seed_gen();
	auto twister = twister_t{ seed };
	return twister;
}

cjm::uint128_tests::generator::internal::rgen_impl::rgen_impl() : m_twister{ init_twister() }{}

cjm::uint128_tests::binary_op_u128_t cjm::uint128_tests::generator::create_multiplication_op(rgen& rgen, 
	size_t max_l_factor_size, size_t max_r_factor_size)
{
	if (std::popcount(max_l_factor_size) != 1 || std::popcount(max_r_factor_size) != 1) throw std::invalid_argument{ "factor sizes must greater than zero be power of two" };
	constexpr auto max_factor_size = std::numeric_limits<uint128_t>::digits / CHAR_BIT;
	if (max_l_factor_size > max_factor_size || max_r_factor_size > max_factor_size )
	{
		auto ss = string::make_throwing_sstream<char>();
		ss << "factor size may be no greater than [" << max_factor_size << "].";
		throw std::invalid_argument{ ss.str() };
	}
	uint128_t lhs = create_random_in_range(rgen, max_l_factor_size);
	uint128_t rhs = create_random_in_range(rgen, max_r_factor_size);
	return binary_op_u128_t{ binary_op::multiply, lhs, rhs };
	
}
cjm::uint128_tests::binary_operation<cjm::uint128_tests::generator::uint128_t,
	cjm::uint128_tests::ctrl_uint128_t> cjm::uint128_tests::generator::create_modulus_op(const rgen& rgen,
		size_t max_dividend_size, size_t max_divisor_size)
{
	if (max_dividend_size < 1 || max_dividend_size > sizeof(uint128_t))
		throw std::out_of_range{ "max dividend size must be non-zero and less than or equal to the sizeof uint128_t." };
	if (max_divisor_size < 1 || max_divisor_size > sizeof(uint128_t))
		throw std::out_of_range{ "max divisor size must be non-zero and less than or equal to the sizeof uint128_t." };
	return unsafe_create_modulus_op(rgen, max_dividend_size, max_dividend_size);
}

std::vector<cjm::uint128_tests::binary_operation<cjm::uint128_tests::uint128_t,
	cjm::uint128_tests::ctrl_uint128_t>> cjm::uint128_tests::generator::create_modulus_ops(const rgen& rgen,
		size_t num_ops, size_t max_dividend_size, size_t max_divisor_size)
{
	if (max_dividend_size < 1 || max_dividend_size > sizeof(uint128_t))
		throw std::out_of_range{ "max dividend size must be non-zero and less than or equal to the sizeof uint128_t." };
	if (max_divisor_size < 1 || max_divisor_size > sizeof(uint128_t))
		throw std::out_of_range{ "max divisor size must be non-zero and less than or equal to the sizeof uint128_t." };
	auto vector = std::vector<binary_operation<uint128_t, ctrl_uint128_t>>{};
	if (num_ops > 0)
	{
		vector.reserve(num_ops);
		while (vector.size() < num_ops)
		{
			vector.emplace_back(unsafe_create_modulus_op(rgen, max_dividend_size, max_divisor_size));
		}
	}
	return vector;
}

cjm::uint128_tests::unary_op cjm::uint128_tests::generator::create_random_unary_opcode(const rgen& rgen)
{
	using enum_t = std::underlying_type_t<unary_op>;
	constexpr auto min = static_cast<enum_t>(first_unary_op);
	constexpr auto max = static_cast<enum_t>(last_unary_op);

	enum_t val = (static_cast<enum_t>(generator::create_random_in_range<enum_t>(rgen)) + min) % (max + enum_t{1});
	cjm_assert(val >= min && val <= max && val < un_op_name_lookup.size());
	return static_cast<unary_op>(val);
}

cjm::uint128_tests::binary_operation<cjm::uint128_tests::uint128_t,
									 cjm::uint128_tests::ctrl_uint128_t> cjm::uint128_tests::generator::create_compare_op(const rgen& rgen)
{
	const auto op = binary_op::compare;
	const uint128_t left = create_random_in_range<uint128_t>(rgen);
	const uint128_t right = create_random_in_range<uint128_t>(rgen);
	return binary_operation<uint128_t, ctrl_uint128_t>{op, left, right};
}

cjm::uint128_tests::binary_operation<cjm::uint128_tests::uint128_t,
	cjm::uint128_tests::ctrl_uint128_t> cjm::uint128_tests::generator::create_shift_op(const rgen& rgen)
{
	const auto op = rgen.random_op(binary_op_category::bitshift);
	assert(op >= first_shift_op && op <= last_shift_op);
	const uint128_t left = create_random_in_range<uint128_t>(rgen);
	const uint128_t right = create_shift_param_for_type<uint128_t>(rgen);
	assert(right >= 0 && right < std::numeric_limits<uint128_t>::digits);
	return binary_operation<uint128_t, ctrl_uint128_t>{op, left, right};
}

cjm::uint128_tests::binary_operation<cjm::uint128_tests::uint128_t,
	cjm::uint128_tests::ctrl_uint128_t> cjm::uint128_tests::generator::create_bw_op(const rgen& rgen)
{
	const auto op = rgen.random_op(binary_op_category::bitwise);
	assert(op >= first_bw_op && op <= last_bw_op);
	const uint128_t left = create_random_in_range<uint128_t>(rgen);
	const uint128_t right = create_random_in_range<uint128_t>(rgen);
	return binary_operation<uint128_t, ctrl_uint128_t>{op, left, right};
}

cjm::uint128_tests::binary_operation<cjm::uint128_tests::generator::uint128_t,
	cjm::uint128_tests::ctrl_uint128_t> cjm::uint128_tests::generator::create_division_op(const rgen& rgen,
		size_t max_dividend_size, size_t max_divisor_size)
{
	if (max_dividend_size < 1 || max_dividend_size > sizeof(uint128_t))
		throw std::out_of_range{ "max dividend size must be non-zero and less than or equal to the sizeof uint128_t." };
	if (max_divisor_size < 1 || max_divisor_size > sizeof(uint128_t))
		throw std::out_of_range{ "max divisor size must be non-zero and less than or equal to the sizeof uint128_t." };
	return unsafe_create_division_op(rgen, max_dividend_size, max_dividend_size);
}

std::vector<cjm::uint128_tests::binary_operation<cjm::uint128_tests::uint128_t,
	cjm::uint128_tests::ctrl_uint128_t>> cjm::uint128_tests::generator::create_division_ops(const rgen& rgen,
		size_t num_ops, size_t max_dividend_size, size_t max_divisor_size)
{
	if (max_dividend_size < 1 || max_dividend_size > sizeof(uint128_t))
		throw std::out_of_range{ "max dividend size must be non-zero and less than or equal to the sizeof uint128_t." };
	if (max_divisor_size < 1 || max_divisor_size > sizeof(uint128_t))
		throw std::out_of_range{ "max divisor size must be non-zero and less than or equal to the sizeof uint128_t." };
	auto vector = std::vector<binary_operation<uint128_t, ctrl_uint128_t>>{};
	if (num_ops > 0)
	{
		vector.reserve(num_ops);
		while (vector.size() < num_ops)
		{
			vector.emplace_back(unsafe_create_division_op(rgen, max_dividend_size, max_divisor_size));
		}
	}
	return vector;
}

cjm::uint128_tests::generator::uint128_t cjm::uint128_tests::generator::create_random_in_range(const rgen& rgen,
	size_t byte_limit)
{
	if (byte_limit == 0) throw std::invalid_argument{ "zero is not a valid size." };
	return unsafe_create_in_range(rgen, byte_limit);
}

cjm::uint128_tests::generator::rgen::rgen() : m_gen()
{
	m_gen = internal::rgen_impl::make_rgen();
	assert(m_gen);
}

cjm::uint128_tests::generator::rgen::rgen(rgen&& other) noexcept : rgen(true)
{
	std::swap(m_gen, other.m_gen);
}

cjm::uint128_tests::generator::rgen& cjm::uint128_tests::generator::rgen::operator=(rgen&& other) noexcept
{
	if (other != *this)
	{
		std::swap(other.m_gen, m_gen);
	}
	return *this;
}

char cjm::uint128_tests::generator::rgen::random_decimal_digit(char max)
{
	assert(max != '0');
	const auto umax = static_cast<unsigned char>(max);
	if (max < 0x30 || max > 0x39)
	{
		auto strm = string::make_throwing_sstream<char>();
		strm
			<< std::hex << std::setw(2) << std::setfill('0') 
			<< "Parameter [max] must be between [0x30] ('0') "
			<< "and [0x39] ('9') inclusively.  Actual value: [0x"
			<< std::hex << std::setw(2) << std::setfill('0')
			<< static_cast<unsigned>(static_cast<unsigned char>(max))
			<< "] (" << max << ")." << newl;
		throw std::invalid_argument{ strm.str() };
	}
	unsigned random = 0x30;
	do
	{
		random = m_gen->random_byte() % 10u;
		random += 0x30u;
		assert(random >= 0x30 && random <= 0x39);		
	} while (random > umax);
	
	return *reinterpret_cast<char*>(&random);
}

char cjm::uint128_tests::generator::rgen::random_hex_digit()
{
	const auto random_byte = (static_cast<unsigned>(m_gen->random_byte()) % 0x10u);
	return hex_char_arr.at(random_byte);
}


cjm::uint128_tests::binary_op cjm::uint128_tests::generator::rgen::
random_op(binary_op_category category) const
{
	if (category == binary_op_category::compare) //only one in cat.
	{
		return binary_op::compare;
	}

	std::uniform_int_distribution<unsigned> distrib;

	auto to_unsigned = [](binary_op op) -> unsigned { return static_cast<unsigned>(op); };

	switch (category)  // NOLINT(clang-diagnostic-switch-enum)
	{
	default:
	case binary_op_category::any:
		distrib = std::uniform_int_distribution<unsigned>{ to_unsigned(first_op), to_unsigned(last_op) };
		break;
	case binary_op_category::bitwise:
		distrib = std::uniform_int_distribution<unsigned>{ to_unsigned(first_bw_op), to_unsigned(last_bw_op) };
		break;
	case binary_op_category::bitshift:
		distrib = std::uniform_int_distribution<unsigned>{ to_unsigned(first_shift_op), to_unsigned(last_shift_op) };
		break;
	case binary_op_category::add_subtract_multiply:
		distrib = std::uniform_int_distribution<unsigned>{ to_unsigned(first_add_sub_mul_op), to_unsigned(last_add_sub_mul_op) };
		break;
	case binary_op_category::div_mod:
		distrib = std::uniform_int_distribution<unsigned>{ to_unsigned(first_divmod_op), to_unsigned(last_divmod_op) };
		break;
	}
	assert(distrib.min() <= distrib.max());

	unsigned value = m_gen->random_from_range(distrib);
	auto ret = static_cast<binary_op>(value);
	assert(ret >= first_op && ret <= last_op);
	return ret;
}

std::vector<std::uint8_t> cjm::uint128_tests::generator::rgen::generate(size_t bytes) const
{
	assert(good());
	return m_gen->generate(bytes);
}
[[nodiscard]] int cjm::uint128_tests::generator::rgen::generate_shift_param(int binary_digits) const
{
	if (binary_digits - 1 <= 0) throw std::invalid_argument{ "binary digits must be at least two." };
	return m_gen->generate_shift_param(binary_digits);
}
cjm::uint128_tests::generator::rgen::rgen(bool) noexcept
{
	m_gen = nullptr;
}

bool cjm::uint128_tests::generator::operator==(const rgen& lhs, const rgen& rhs) noexcept
{
	return lhs.m_gen.get() == rhs.m_gen.get();
}

bool cjm::uint128_tests::generator::operator!=(const rgen& lhs, const rgen& rhs) noexcept
{
	return !(lhs == rhs);
}

size_t std::hash<cjm::uint128_tests::generator::rgen>::operator()(const cjm::uint128_tests::generator::rgen& key) const noexcept
{
	cjm::uint128_tests::generator::internal::rgen_impl* ptr = key.m_gen.get();
	return std::hash<cjm::uint128_tests::generator::internal::rgen_impl*>{}(ptr);
}

[[nodiscard]] cjm::uint128_tests::binary_operation<cjm::uint128_tests::generator::uint128_t, cjm::uint128_tests::ctrl_uint128_t>
cjm::uint128_tests::generator::unsafe_create_division_op(const cjm::uint128_tests::generator::rgen& rgen, size_t max_dividend_size, size_t max_divisor_size)
{
	assert(rgen.good() && max_dividend_size > 0 && max_dividend_size <= sizeof(uint128_t) && max_divisor_size > 0 && max_divisor_size <= sizeof(uint128_t));
	const uint128_t left_operand = unsafe_create_in_range(rgen, max_dividend_size);
	uint128_t right_operand = unsafe_create_in_range(rgen, max_divisor_size);
	while (right_operand == 0_u128)
	{
		right_operand = unsafe_create_in_range(rgen, max_divisor_size);
	}
	const auto op = binary_op::divide;
	return binary_operation<uint128_t, ctrl_uint128_t>{op, left_operand, right_operand, };
}

[[nodiscard]] cjm::uint128_tests::binary_operation<cjm::uint128_tests::generator::uint128_t, cjm::uint128_tests::ctrl_uint128_t>
cjm::uint128_tests::generator::unsafe_create_modulus_op(const cjm::uint128_tests::generator::rgen& rgen, size_t max_dividend_size, size_t max_divisor_size)
{
	assert(rgen.good() && max_dividend_size > 0 && max_dividend_size <= sizeof(uint128_t) && max_divisor_size > 0 && max_divisor_size <= sizeof(uint128_t));
	const uint128_t left_operand = unsafe_create_in_range(rgen, max_dividend_size);
	uint128_t right_operand = unsafe_create_in_range(rgen, max_divisor_size);
	while (right_operand == 0_u128)
	{
		right_operand = unsafe_create_in_range(rgen, max_divisor_size);
	}
	const auto op = binary_op::modulus;
	return binary_operation<uint128_t, ctrl_uint128_t>{op, left_operand, right_operand, };
}

cjm::uint128_tests::generator::uint128_t cjm::uint128_tests::generator::unsafe_create_in_range(const rgen& rgen, size_t size)
{
	using byte_array = typename uint128_t::byte_array;
	assert(rgen.good());
	assert(size > 0 && size <= sizeof(uint128_t));
	auto vec = rgen.generate(size);
	auto arr = byte_array{};
	assert(vec.size() <= arr.size());
	std::copy(vec.cbegin(), vec.cend(), arr.begin());
	return uint128_t::make_from_bytes_little_endian(arr);
}

cjm::uint128_tests::unary_op cjm::uint128_tests::parse_unary_op_symbol(cjm::uint128_tests::sv_t text)
{
	auto trimmed = cjm::string::trim_as_sv(text);
	if (trimmed.empty()) throw std::invalid_argument{"Supplied text is not a unary operation symbol."};
	for (unsigned idx = 0; idx < un_op_symbol_lookup.size(); ++idx)
	{
		if (un_op_symbol_lookup[idx] == text)
		{
			return static_cast<unary_op>(idx);
		}
	}
	throw std::invalid_argument{"Supplied text is not a unary operation symbol."};

}

cjm::uint128_tests::unary_op cjm::uint128_tests::parse_unary_op_symbol(cjm::uint128_tests::wsv_t text)
{
	auto trimmed = cjm::string::trim_as_sv(text);
	if (trimmed.empty()) throw std::invalid_argument{"Supplied text is not a unary operation symbol."};
	for (unsigned idx = 0; idx < un_op_symbol_lookup_wide.size(); ++idx)
	{
		if (un_op_symbol_lookup_wide[idx] == text)
		{
			return static_cast<unary_op>(idx);
		}
	}
	throw std::invalid_argument{"Supplied text is not a unary operation symbol."};
}

cjm::uint128_tests::unary_op cjm::uint128_tests::parse_unary_op_symbol(cjm::uint128_tests::u8sv_t text)
{
	auto trimmed = cjm::string::trim_as_sv(text);
	if (trimmed.empty()) throw std::invalid_argument{"Supplied text is not a unary operation symbol."};
	for (unsigned idx = 0; idx < un_op_symbol_lookup_u8.size(); ++idx)
	{
		if (un_op_symbol_lookup_u8[idx] == text)
		{
			return static_cast<unary_op>(idx);
		}
	}
	throw std::invalid_argument{"Supplied text is not a unary operation symbol."};
}

cjm::uint128_tests::unary_op cjm::uint128_tests::parse_unary_op_symbol(cjm::uint128_tests::u16sv_t text)
{
	auto trimmed = cjm::string::trim_as_sv(text);
	if (trimmed.empty()) throw std::invalid_argument{"Supplied text is not a unary operation symbol."};
	for (unsigned idx = 0; idx < un_op_symbol_lookup_u16.size(); ++idx)
	{
		if (un_op_symbol_lookup_u16[idx] == text)
		{
			return static_cast<unary_op>(idx);
		}
	}
	throw std::invalid_argument{"Supplied text is not a unary operation symbol."};
}

cjm::uint128_tests::unary_op cjm::uint128_tests::parse_unary_op_symbol(cjm::uint128_tests::u32sv_t text)
{
	auto trimmed = cjm::string::trim_as_sv(text);
	if (trimmed.empty()) throw std::invalid_argument{"Supplied text is not a unary operation symbol."};
	for (unsigned idx = 0; idx < un_op_symbol_lookup_u32.size(); ++idx)
	{
		if (un_op_symbol_lookup_u32[idx] == text)
		{
			return static_cast<unary_op>(idx);
		}
	}
	throw std::invalid_argument{"Supplied text is not a unary operation symbol."};
}


cjm::uint128_tests::binary_op cjm::uint128_tests::parse_binary_op_symbol(cjm::uint128_tests::sv_t text)
{
	auto trimmed = cjm::string::trim_as_sv(text);
	if (trimmed.empty()) throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
	for (unsigned idx = 0; idx < op_symbol_lookup.size(); ++idx)
	{
		if (op_symbol_lookup[idx] == text)
		{
			return static_cast<binary_op>(idx);
		}
	}
	throw std::invalid_argument{"Supplied text is not a binary operation symbol."};

}
cjm::uint128_tests::binary_op cjm::uint128_tests::parse_binary_op_symbol(cjm::uint128_tests::wsv_t text)
{
	auto trimmed = cjm::string::trim_as_sv(text);
	if (trimmed.empty()) throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
	for (unsigned idx = 0; idx < op_symbol_lookup.size(); ++idx)
	{
		if (op_symbol_lookup_wide[idx] == text)
		{
			return static_cast<binary_op>(idx);
		}
	}
	throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
}

cjm::uint128_tests::binary_op cjm::uint128_tests::parse_binary_op_symbol(cjm::uint128_tests::u8sv_t text)
{
	auto trimmed = cjm::string::trim_as_sv(text);
	if (trimmed.empty()) throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
	for (unsigned idx = 0; idx < op_symbol_lookup.size(); ++idx)
	{
		if (op_symbol_lookup_u8[idx] == text)
		{
			return static_cast<binary_op>(idx);
		}
	}
	throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
}

cjm::uint128_tests::binary_op cjm::uint128_tests::parse_binary_op_symbol(cjm::uint128_tests::u16sv_t text)
{
	auto trimmed = cjm::string::trim_as_sv(text);
	if (trimmed.empty()) throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
	for (unsigned idx = 0; idx < op_symbol_lookup.size(); ++idx)
	{
		if (op_symbol_lookup_u16[idx] == text)
		{
			return static_cast<binary_op>(idx);
		}
	}
	throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
}

cjm::uint128_tests::binary_op cjm::uint128_tests::parse_binary_op_symbol(cjm::uint128_tests::u32sv_t text)
{
	auto trimmed = cjm::string::trim_as_sv(text);
	if (trimmed.empty()) throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
	for (unsigned idx = 0; idx < op_symbol_lookup.size(); ++idx)
	{
		if (op_symbol_lookup_u32[idx] == text)
		{
			return static_cast<binary_op>(idx);
		}
	}
	throw std::invalid_argument{"Supplied text is not a binary operation symbol."};
}

void cjm::uint128_tests::execute_binary_operation_rt_ser_tests()
{
	auto n_stream = string::make_throwing_sstream<char>();
	auto w_stream = string::make_throwing_sstream<wchar_t>();
	auto u8_stream = string::make_throwing_sstream<char8_t>();
	auto u16_stream = string::make_throwing_sstream<char16_t>();
	auto u32_stream = string::make_throwing_sstream<char32_t>();

	const auto binary_operation = binary_op_u128_t{ binary_op::subtract,
		0xcafe'babe'fea2'dead'beef'badd'f00d'dadd_u128,
		0xc0de'd00d'fea2'cafe'babe'b00b'600d'f00d_u128, };
	const auto bad_rt_val = binary_op_u128_t{ binary_op::add, 0, 0 };
	auto rt_val = bad_rt_val;

	n_stream << binary_operation;
	n_stream >> rt_val;
	cjm_assert(rt_val == binary_operation);
	rt_val = bad_rt_val;

	w_stream << binary_operation;
	w_stream >> rt_val;
	cjm_assert(rt_val == binary_operation);
	rt_val = bad_rt_val;
	
	u8_stream << binary_operation;
	u8_stream >> rt_val;
	cjm_assert(rt_val == binary_operation);
	rt_val = bad_rt_val;

	u16_stream << binary_operation;
	u16_stream >> rt_val;
	cjm_assert(rt_val == binary_operation);
	rt_val = bad_rt_val;
	
	u32_stream << binary_operation;
	u32_stream >> rt_val;
	cjm_assert(rt_val == binary_operation);
	rt_val = bad_rt_val;
	
}

void cjm::uint128_tests::execute_unary_operation_rt_serialization_tests()
{
	auto n_stream = string::make_throwing_sstream<char>();
	auto w_stream = string::make_throwing_sstream<wchar_t>();
	auto u8_stream = string::make_throwing_sstream<char8_t>();
	auto u16_stream = string::make_throwing_sstream<char16_t>();
	auto u32_stream = string::make_throwing_sstream<char32_t>();

	const auto unary_operation = unary_op_u128_t{ unary_op::post_increment,
		0xcafe'babe'fea2'dead'beef'badd'f00d'dadd_u128};
	const auto bad_rt_val = unary_op_u128_t{ unary_op::pre_increment, 0_u128 };
	auto rt_val = bad_rt_val;

	n_stream << unary_operation;
	n_stream >> rt_val;
	cjm_assert(rt_val == unary_operation);
	rt_val = bad_rt_val;

	w_stream << unary_operation;
	w_stream >> rt_val;
	cjm_assert(rt_val == unary_operation);
	rt_val = bad_rt_val;
	
	u8_stream << unary_operation;
	u8_stream >> rt_val;
	cjm_assert(rt_val == unary_operation);
	rt_val = bad_rt_val;

	u16_stream << unary_operation;
	u16_stream >> rt_val;
	cjm_assert(rt_val == unary_operation);
	rt_val = bad_rt_val;
	
	u32_stream << unary_operation;
	u32_stream >> rt_val;
	cjm_assert(rt_val == unary_operation);
	rt_val = bad_rt_val;
}

void cjm::uint128_tests::execute_trim_tests()
{

	using namespace std::string_literals;
	using namespace std::string_view_literals;
	constexpr std::string_view should_be = "<<"sv;
	std::string x = " <<   "s;
	std::string_view xv;
	std::string x_trim = cjm::string::trim(x);
	cjm_deny(x == x_trim);
	cjm_assert(x_trim == should_be);
	x = " <<   "s;
	xv = x;
	xv = cjm::string::trim_as_sv(xv);
	cjm_assert(xv == should_be);
	cjm_deny(x == should_be);
}

void cjm::uint128_tests::execute_stream_insert_bin_op_test()
{
	auto default_init_wrong_val = [](binary_op o) -> binary_op
	{
		constexpr unsigned biggest = static_cast<unsigned>(last_op);
		constexpr unsigned modulo = biggest + 1;
		auto unsigned_val = (static_cast<unsigned>(o) + 1) % modulo;
		auto ret = static_cast<binary_op>(unsigned_val);
		assert(ret >= first_op && ret <= last_op && ret != o);
		return ret;
	};
	for (auto op_int = static_cast<unsigned>(cjm::uint128_tests::first_op);
		op_int <= static_cast<unsigned>(cjm::uint128_tests::last_op); ++op_int )
	{
		auto op = static_cast<binary_op>(op_int);
		const auto wrong_op = default_init_wrong_val(op);
		auto nstream = cjm::string::make_throwing_sstream<char>();
		auto wstream = cjm::string::make_throwing_sstream<wchar_t>();
		auto u8stream = cjm::string::make_throwing_sstream<char8_t>();
		auto u16stream = cjm::string::make_throwing_sstream<char16_t>();
		auto u32stream = cjm::string::make_throwing_sstream<char32_t>();

		nstream << op;
		wstream << op;
		u8stream << op;
		u16stream << op;
		u32stream << op;

		binary_op rt_op_n;
		nstream >> rt_op_n;
		cjm_assert(rt_op_n == op);

		binary_op rt_op_w;
		wstream >> rt_op_w;
		cjm_assert(rt_op_w == op);

		binary_op rt_op_u8 = wrong_op;
		cjm_assert(rt_op_u8 != op);
		u8stream >> rt_op_u8;
		cjm_assert(rt_op_u8 == op);

		binary_op rt_op_u16 = wrong_op;
		u16stream >> rt_op_u16;
		cjm_assert(rt_op_u16 == op);

		binary_op rt_op_u32 = wrong_op;
		u32stream >> rt_op_u32;
		cjm_assert(rt_op_u32 == op);
	}
}

void cjm::uint128_tests::execute_print_generated_filename_test()
{
	auto file_name_addition = create_generated_op_filename(binary_op::add);
	cjm_deny(file_name_addition.empty());
	std::cout << "GENERATED FILENAME FOR ADDITION: [" << file_name_addition << "]." << newl;
}

void cjm::uint128_tests::execute_generate_addition_ops_rt_ser_deser_test()
{
	constexpr binary_op operation = binary_op::add;
	constexpr size_t ops = 1'000;
	constexpr size_t num_standard_values = u128_testing_constant_providers::testing_constant_provider<uint128_t>::all_values.size();
	constexpr size_t num_standard_ops = ((num_standard_values * num_standard_values) + num_standard_values) / 2;
	constexpr size_t num_expected = ops + num_standard_ops;
	
	auto op_vect = generate_easy_ops(ops, operation, true);
	cjm_assert(op_vect.size() == num_expected);
	auto file_name = create_generated_op_filename(operation);
	{
		auto ofstream = string::make_throwing_ofstream<char>(file_name);
		ofstream << op_vect;
	}
	std::cout << "Wrote " << op_vect.size() << " operations to [" << file_name << "]." << std::endl;
	std::filesystem::path file = file_name;
	auto rt_op_vec = binary_op_u128_vect_t{};
	try
	{
		cjm_assert(std::filesystem::exists(file));

		if (!op_vect.empty())
		{
			rt_op_vec.reserve(op_vect.size());
		}
		{
			auto ifstream = string::make_throwing_ifstream<char>(file_name);
			ifstream >> rt_op_vec;
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Exception thrown after writing to file [" << file_name << "]. Msg: [" << ex.what() << "]." << newl;
		try
		{
			if (std::filesystem::exists(file_name))
			{
				std::filesystem::remove(file_name);
			}
		}
		catch (const std::exception& ex2)
		{
			std::cerr << "Exception throw deleting file [" << file_name << "]. Msg: [" << ex2.what() << "]." << newl;
		}
		throw;
	}

	try
	{
		if (std::filesystem::exists(file_name))
		{
			std::filesystem::remove(file_name);
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Exception throw deleting file [" << file_name << "]. Msg: [" << ex.what() << "]." << newl;
	}
	cjm_assert(rt_op_vec == op_vect);
	std::cout << "The round tripped vector is identical to the source vector." << newl;
}

void cjm::uint128_tests::execute_unary_operation_vec_rt_serialization_tests()
{
	constexpr auto& arr = u128_testing_constant_providers::testing_constant_provider<uint128_t>::all_values;
	constexpr size_t num_standard_values = arr.size();
	constexpr size_t num_standard_ops = num_standard_values;
	constexpr size_t num_expected = num_standard_ops;

	auto rgen = generator::rgen{};
	auto op_vect = unary_op_u128_vect_t{};
	op_vect.reserve(num_expected);
	for (const auto& itm : arr)
	{
		op_vect.emplace_back(generator::create_random_unary_opcode(rgen), itm);
	}

	cjm_assert(op_vect.size() == num_expected);
	
	auto file_name = std::filesystem::path{"random_un_ops.txt"};
	{
		auto ofstream = string::make_throwing_ofstream<char>(file_name);
		ofstream << op_vect;
	}
	
	std::cout << "Wrote " << op_vect.size() << " operations to [" << file_name << "]." << newl;
	std::filesystem::path file = file_name;
	auto rt_op_vec = unary_op_u128_vect_t{};
	try
	{
		cjm_assert(std::filesystem::exists(file));

		if (!op_vect.empty())
		{
			rt_op_vec.reserve(op_vect.size());
		}
		{
			auto ifstream = string::make_throwing_ifstream<char>(file_name);
			ifstream >> rt_op_vec;
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Exception thrown after writing to file [" << file_name << "]. Msg: [" << ex.what() << "]." << newl;
		try
		{
			if (std::filesystem::exists(file_name))
			{
				std::filesystem::remove(file_name);
			}
		}
		catch (const std::exception& ex2)
		{
			std::cerr << "Exception throw deleting file [" << file_name << "]. Msg: [" << ex2.what() << "]." << newl;
		}
		throw;
	}

	try
	{
		if (std::filesystem::exists(file_name))
		{
			std::filesystem::remove(file_name);
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Exception throw deleting file [" << file_name << "]. Msg: [" << ex.what() << "]." << newl;
	}
	cjm_assert(rt_op_vec == op_vect);
	std::cout << "The round tripped vector is identical to the source vector." << newl;
}

void cjm::uint128_tests::execute_addition_tests()
{
	constexpr auto test_name = "addition_tests"sv;
	constexpr binary_op operation = binary_op::add;
	constexpr size_t ops = 1'000;
	constexpr size_t num_standard_values = u128_testing_constant_providers::testing_constant_provider<uint128_t>::all_values.size();
	constexpr size_t num_standard_ops = ((num_standard_values * num_standard_values) + num_standard_values) / 2;
	constexpr size_t num_expected = ops + num_standard_ops;

	auto op_vect = generate_easy_ops(ops, operation, true);
	cjm_assert(op_vect.size() == num_expected);
	std::cout << "  Executing " << op_vect.size() << " addition tests: " << newl;
	for (auto& binary_operation : op_vect)
	{
		test_binary_operation(binary_operation, test_name);
	}
	std::cout << "All " << op_vect.size() << " tests PASS." << newl;
	//print_n_static_assertions(op_vect, num_standard_ops);
}

void cjm::uint128_tests::execute_subtraction_tests()
{
	constexpr auto test_name = "subtraction_tests"sv;
	constexpr binary_op operation = binary_op::subtract;
	constexpr size_t ops = 1'000;
	constexpr size_t num_standard_values = u128_testing_constant_providers::testing_constant_provider<uint128_t>::all_values.size();
	constexpr size_t num_standard_ops = ((num_standard_values * num_standard_values) + num_standard_values) / 2;
	constexpr size_t num_expected = ops + num_standard_ops;

	auto op_vect = generate_easy_ops(ops, operation, true);
	cjm_assert(op_vect.size() == num_expected);
	std::cout << "  Executing " << op_vect.size() << " subtraction tests: " << newl;
	for (auto& binary_operation : op_vect)
	{
		test_binary_operation(binary_operation, test_name);
	}
	std::cout << "All " << op_vect.size() << " tests PASS." << newl;
	//print_n_static_assertions(op_vect, num_standard_ops);
}

void cjm::uint128_tests::execute_shift_tests()
{
	constexpr auto test_name = "shift_tests"sv;
	constexpr size_t rand_ops = 10;
	static_assert(rand_ops > 0 && rand_ops % 2 == 0, "Need a positive even number.");
	constexpr size_t expected_standard_ops = u128_testing_constant_providers::testing_constant_provider<uint128_t>::all_values.size() * 128 * 2;
	constexpr size_t expected_random_ops = rand_ops;
	constexpr size_t expected_total = expected_random_ops + expected_standard_ops;
	auto op_vec = generate_shift_ops(rand_ops, true);
	cjm_assert(op_vec.size() == expected_total);
	std::cout << "  Executing " << op_vec.size() << " shifting tests: " << newl;
	for (auto& binary_operation : op_vec)
	{
		test_binary_operation(binary_operation, test_name);
	}
	std::cout << "All " << op_vec.size() << " tests PASS." << newl;
	//print_n_static_assertions(op_vec, expected_standard_ops);
}

void cjm::uint128_tests::execute_bw_tests()
{
	constexpr auto test_name = "bw_tests"sv;
	constexpr size_t rand_ops = 102;
	static_assert(rand_ops > 0 && rand_ops % 3 == 0, "Need a positive number divisible by three.");
	constexpr size_t num_standard_values = u128_testing_constant_providers::testing_constant_provider<uint128_t>::all_values.size();
	constexpr size_t expected_standard_ops = (((num_standard_values * num_standard_values) + num_standard_values) / 2) * 3;
	constexpr size_t expected_random_ops = rand_ops;
	constexpr size_t expected_total = expected_random_ops + expected_standard_ops;
	auto op_vec = generate_bw_ops(rand_ops, true);
	cjm_assert(op_vec.size() == expected_total);
	std::cout << "  Executing " << op_vec.size() << " shifting tests: " << newl;
	for (auto& binary_operation : op_vec)
	{
		test_binary_operation(binary_operation, test_name);
	}
	std::cout << "All " << op_vec.size() << " tests PASS." << newl;
	auto rgen = generator::rgen{};
	rgen.shuffle(op_vec, expected_standard_ops);
	//print_n_static_assertions(op_vec, expected_standard_ops / 3);
}

void cjm::uint128_tests::execute_comparison_tests()
{
	constexpr auto test_name = "comparison_tests"sv;
	constexpr binary_op operation = binary_op::compare;
	constexpr size_t ops = 1'000;
	constexpr size_t num_standard_values = u128_testing_constant_providers::testing_constant_provider<uint128_t>::all_values.size();
	constexpr size_t num_standard_ops = ((num_standard_values * num_standard_values) + num_standard_values) / 2;
	constexpr size_t num_expected = ops + num_standard_ops;

	auto op_vect = generate_easy_ops(ops, operation, true);
	cjm_assert(op_vect.size() == num_expected);
	std::cout << "  Executing " << op_vect.size() << " comparison tests: " << newl;
	for (auto& binary_operation : op_vect)
	{
		test_binary_operation(binary_operation, test_name);
	}
	std::cout << "All " << op_vect.size() << " tests PASS." << newl;
	//print_n_static_assertions(op_vect, num_standard_ops);
}

void cjm::uint128_tests::execute_multiplication_tests()
{
	constexpr auto test_name = "multiplication_tests"sv;
	constexpr size_t ops_per_range = 250;
	constexpr size_t num_ranges = 4;
	constexpr size_t ops = num_ranges * ops_per_range;
	constexpr size_t num_standard_values = u128_testing_constant_providers
		::testing_constant_provider<uint128_t>::all_values.size();
	constexpr size_t num_standard_ops = ((num_standard_values * num_standard_values)
		+ num_standard_values) / 2;
	constexpr size_t num_expected = ops + num_standard_ops;

	auto op_vec = generate_mult_ops(ops_per_range, true);
	cjm_assert(num_expected == op_vec.size());
	for (auto& binary_operation : op_vec)
	{
		test_binary_operation(binary_operation, test_name);
	}
	std::cout << "All " << op_vec.size() << " tests PASS." << newl;
	//print_n_static_assertions(op_vec, num_standard_ops);
}

void cjm::uint128_tests::execute_failing_division_test_1()
{
	uint128_t dividend = 0;
	uint128_t divisor = 1;

	uint128_t test_result = dividend / divisor;

	ctrl_uint128_t ctrl_dividend = to_ctrl(dividend);
	ctrl_uint128_t ctrl_divisor = to_ctrl(divisor);
	ctrl_uint128_t ctrl_result = ctrl_dividend / ctrl_divisor;
	cjm_assert(ctrl_result == 0);

	uint128_t ctrl_res_to_test = to_test(ctrl_result);

	auto divmod_res = uint128_t::div_mod(dividend, divisor);
	cjm_assert(test_result == 0 && ctrl_res_to_test == test_result);
	cjm_assert(test_result == divmod_res.quotient);

	auto op = binary_op_u128_t{ binary_op::divide, dividend, divisor };
	auto mod_ver = binary_op_u128_t{ binary_op::modulus, dividend, divisor };
	test_binary_operation(op, "failing_division_test_1"sv);
	test_binary_operation(mod_ver, "failing_division_test_1_modulus_ver"sv);
	cjm_assert(op.has_correct_result());
//	std::cout << "Printing static assertions from failing_division_test_1:" << newl;
//	append_static_assertion(cout, op) << newl;
//	append_static_assertion(cout, mod_ver) << newl;
//	std::cout << "Done printing static assertions." << newl;
}

void cjm::uint128_tests::execute_failing_division_test_2()
{
	constexpr auto test_name = "failing_division_test_2"sv;
	auto op_text = "/;256368684943268248658307433575740207117;16109687965047641490155963133754044755;"sv;
	constexpr auto lhs = 256368684943268248658307433575740207117_u128;
	constexpr auto rhs = 16109687965047641490155963133754044755_u128;
	auto control_lhs = to_ctrl(lhs);
	auto control_rhs = to_ctrl(rhs);
	auto control_res = control_lhs / control_rhs;
	std::cout << "This operation should indicate: [" << control_lhs << " / " << control_rhs << " == " << control_res << "]." << newl;

	static_assert((lhs / rhs) == 15_u128);
	
	auto stream = string::make_throwing_sstream<char>();
	
	stream << op_text;
	binary_op_u128_t temp;
	stream >> temp;
	std::cout << "About to test deserialization:..." << newl;
	cjm_assert(lhs == temp.left_operand() && rhs == temp.right_operand());
	cjm_assert(temp.op_code() == binary_op::divide);
	std::cout << "Deserialization succeeded." << newl;

	auto res = temp.left_operand() / temp.right_operand();
	cjm_assert(to_ctrl(res) == control_res);
	std::cout << "Result: [" << res << "]" << newl;

	temp.calculate_result();
	std::cout << "Printing static assertions from " << test_name << ": " << newl;
	append_static_assertion(cout, temp) << newl;
	test_binary_operation(temp, test_name);
	
}

void cjm::uint128_tests::execute_failing_modulus_test_1()
{
	constexpr auto test_name = "failing_modulus_test_1"sv;
	auto op_text = "%;295990755071965901746234089551998857227;168021802658398834664238297099768481736;"sv;
	auto stream = string::make_throwing_sstream<char>();
	stream << op_text;
	binary_op_u128_t temp;
	stream >> temp;
	test_binary_operation(temp, test_name);
}

void cjm::uint128_tests::execute_division_modulus_tests()
{
	constexpr auto test_name = "division_modulus_tests"sv;
	constexpr size_t ops_per_range = 250;
	constexpr size_t num_ranges = 4;
	constexpr size_t num_standard_values = 172;
	constexpr size_t num_rnd_ops = ops_per_range * num_ranges;
	constexpr size_t num_expected = num_rnd_ops + num_standard_values;

	auto op_vec = generate_divmod_ops(ops_per_range, true);
	cjm_assert(num_expected == op_vec.size());
	for (auto& binary_operation : op_vec)
	{
		test_binary_operation(binary_operation, test_name);
	}
	std::cout << "All " << std::dec << op_vec.size() << " tests PASS." << newl;
	/*auto rgen = generator::rgen{};
	rgen.shuffle(op_vec, op_vec.size());
	std::cout << "Going to print static assertions: " << newl;
	print_n_static_assertions(op_vec, 25);
	std::cout << newl << "Done printing static assertions." << newl;*/
}


void cjm::uint128_tests::execute_unary_op_code_rt_serialization_tests()
{
	auto test_rt_ser = [] (unary_op code) -> void
	{
		auto n_stream = string::make_throwing_sstream<char>();
		auto w_stream = string::make_throwing_sstream<wchar_t>();
		auto u8_stream = string::make_throwing_sstream<char8_t>();
		auto u16_stream = string::make_throwing_sstream<char16_t>();
		auto u32_stream = string::make_throwing_sstream<char32_t>();

		const auto wrong_code = static_cast<unary_op>((static_cast<unsigned>(code) + 1) % unary_op_count);
		cjm_assert(wrong_code != code && wrong_code >= first_unary_op && wrong_code <= last_unary_op);
		cjm_assert(code >= first_unary_op && code <= last_unary_op);

		auto rtd_code = wrong_code;
		n_stream << code;
		n_stream >> rtd_code;
		cjm_assert(rtd_code == code);

		rtd_code = wrong_code;
		w_stream << code;
		w_stream >> rtd_code;
		cjm_assert(rtd_code == code);

		rtd_code = wrong_code;
		u8_stream << code;
		u8_stream >> rtd_code;
		cjm_assert(rtd_code == code);

		rtd_code = wrong_code;
		u16_stream << code;
		u16_stream >> rtd_code;
		cjm_assert(rtd_code == code);

		rtd_code = wrong_code;
		u32_stream << code;
		u32_stream >> rtd_code;
		cjm_assert(rtd_code == code);
	};

	test_rt_ser(unary_op::pre_increment);
	test_rt_ser(unary_op::pre_decrement);
	test_rt_ser(unary_op::post_increment);
	test_rt_ser(unary_op::post_decrement);

	test_rt_ser(unary_op::unary_plus);
	test_rt_ser(unary_op::unary_minus);
	test_rt_ser(unary_op::bitwise_not);
	test_rt_ser(unary_op::bool_cast);

	test_rt_ser(unary_op::logical_negation);
}

void cjm::uint128_tests::execute_unary_op_basic_test()
{
	constexpr auto first_operand = 0xdead'beef'badd'f00d_u128;
	constexpr auto second_operand = 0xcafe'babe'600d'b00b_u128;
	constexpr auto first_op_expected_res = first_operand + 1;
	constexpr auto second_op_expected_res = -second_operand;
	auto unary_op_1 = unary_op_u128_t{unary_op::pre_increment, first_operand};
	auto unary_op_2 = unary_op_u128_t { unary_op::unary_minus, second_operand};
	cjm_assert(unary_op_1.calculate_result());
	cjm_assert(unary_op_2.calculate_result());
	auto first_res = unary_op_1.result();
	auto second_res = unary_op_2.result();

	cjm_assert(first_res.has_value() && first_res.value().first == first_res->second && !unary_op_1.has_post_result() && first_res->first == first_op_expected_res);
	cjm_assert(second_res.has_value() && second_res.value().first == second_res->second && !unary_op_2.has_post_result() && second_res->first == second_op_expected_res);
	cjm_deny(unary_op_1 == unary_op_2);

}

std::filesystem::path cjm::uint128_tests::create_generated_op_filename(binary_op op)
{
	return construct_op_filename(op, op_generated_tag);
}

std::filesystem::path cjm::uint128_tests::create_failing_op_pathname(binary_op op)
{
	return construct_op_filename(op, op_failed_test_tag);
}

std::filesystem::path cjm::uint128_tests::create_generated_op_filename(unary_op op)
{
	return construct_op_filename(op, op_generated_tag);
}

std::filesystem::path cjm::uint128_tests::create_failing_op_pathname(unary_op op)
{
	return construct_op_filename(op, op_failed_test_tag);
}

cjm::uint128_tests::unary_op_u128_vect_t cjm::uint128_tests::generate_random_standard_test_ops()
{
	constexpr auto& standard_op_arr = u128_testing_constant_providers::testing_constant_provider<uint128_t>::all_values;
	unary_op_u128_vect_t ret;
	ret.reserve(standard_op_arr.size());
	auto rgen = generator::rgen{};
	for (const auto& item : standard_op_arr)
	{
		ret.emplace_back(create_random_unary_opcode(rgen), item);
	}
	cjm_assert(ret.size() == standard_op_arr.size());
	return ret;
}

cjm::uint128_tests::unary_op_u128_vect_t cjm::uint128_tests::generate_post_inc_dec_ops(size_t num_ops_each_type,
																					   bool include_standard_tests)
{
	constexpr auto num_op_types = 2_szt;
	constexpr auto num_standard_ops_per_type = 
		u128_testing_constant_providers::testing_constant_provider<uint128_t>::all_values.size();
	const auto num_standard_ops = include_standard_tests ? num_op_types * num_standard_ops_per_type : 0_szt;
	const auto num_rnd_ops = num_ops_each_type * num_op_types;
	const auto num_ops_total = num_standard_ops + num_rnd_ops;
	unary_op_u128_vect_t ret;

	if (num_ops_total > 0) ret.reserve(num_ops_total);

	if (num_standard_ops > 0)
	{
		for (auto itm : u128_testing_constant_providers::testing_constant_provider<uint128_t>::all_values)
		{
			ret.emplace_back(unary_op::post_increment, itm);
			ret.emplace_back(unary_op::post_decrement, itm);
		}
		cjm_assert(ret.size() == num_standard_ops);
	}
	size_t rnd_so_far = 0;
	if (num_rnd_ops > 0)
	{
		auto rgen = generator::rgen{};
		while (rnd_so_far < num_ops_each_type)
		{
			auto value = generator::create_random_in_range<uint128_t>(rgen);
			ret.emplace_back(unary_op::post_increment, value);
			ret.emplace_back(unary_op::post_decrement, value);
			++rnd_so_far;
		}
	}
	cjm_assert(ret.size() == num_ops_total);
	return ret;
}

cjm::uint128_tests::unary_op_u128_vect_t cjm::uint128_tests::generate_specified_un_ops(unary_op op_code,
	size_t num_rnd_ops, bool include_standard_tests)
{
	if (op_code < first_unary_op || op_code > last_unary_op)
	{
		auto strm = string::make_throwing_sstream<char>();
		strm << "The specified unary operation code [" << op_code << "] is not a defined member of the unary_op enum class.";
		throw std::invalid_argument{ strm.str() };
	}

	constexpr auto& arr = u128_testing_constant_providers::testing_constant_provider<uint128_t>::all_values;
	const auto num_std = include_standard_tests ? arr.size() : 0_szt;
	const auto total_to_gen = num_std + num_rnd_ops;

	if (total_to_gen < 1) return unary_op_u128_vect_t{};
	
	auto rgen = generator::rgen{};
	unary_op_u128_vect_t ret;
	ret.reserve(total_to_gen);

	if (num_std > 0)
	{
		for (const auto& val : arr)
		{
			ret.emplace_back(op_code, val);
		}
	}

	auto num_rnd_so_far = 0_szt;
	while (num_rnd_so_far++ < num_rnd_ops)
	{
		ret.emplace_back(op_code, generator::create_random_in_range<uint128_t>(rgen));
	}
	cjm_assert(ret.size() == total_to_gen);
	return ret;
}

cjm::uint128_tests::binary_op_u128_vect_t cjm::uint128_tests::generate_easy_ops(size_t num_ops, binary_op op,
																				bool include_standard_tests)
{
	if (op == binary_op::divide || op == binary_op::modulus || op == binary_op::left_shift || op == binary_op::right_shift)
		throw std::invalid_argument{ "This function cannot be used for division, modulus or bit shifting: those operations may cause undefined behavior if the right operand is improper." };
	using provider_t = u128_testing_constant_providers::testing_constant_provider<uint128_t>;
	binary_op_u128_vect_t ret;
	if (include_standard_tests)
	{
		for (size_t i = 0; i < provider_t::all_values.size(); ++i)
			for (size_t j = i; j < provider_t::all_values.size(); ++j)
				ret.emplace_back(op, provider_t::all_values[i], provider_t::all_values[j]);
	}
	if (num_ops > 0)
	{
		if (ret.capacity() < ret.size() + num_ops)
		{
			ret.reserve(num_ops);
		}
		size_t added = 0;
		auto gen = generator::rgen{};
		while (added < num_ops)
		{
			ret.emplace_back(op, generator::create_random_in_range<uint128_t>(gen),
				generator::create_random_in_range<uint128_t>(gen));
			++added;
		}	   
	}
	return ret;
	
}

cjm::uint128_tests::binary_op_u128_vect_t cjm::uint128_tests::generate_shift_ops(size_t num_ops,
	bool include_standard_tests)
{
	using provider_t = u128_testing_constant_providers::testing_constant_provider<uint128_t>;
	binary_op_u128_vect_t ret;
	constexpr auto shift_values = get_shifting_arr();
	if (include_standard_tests)
	{
		for (auto all_value : provider_t::all_values)
		for (int shift_value : shift_values)
		{
			ret.emplace_back(binary_op::left_shift, all_value, shift_value);
			ret.emplace_back(binary_op::right_shift, all_value, shift_value);
		}
	} //todo fixit resume here!
	if (num_ops > 0)
	{
		if (num_ops % 2 != 0)
		{
			const auto old = num_ops++;
			std::cerr   << "Num_ops must be divisible by two: creating "
						<< num_ops << " operations rather than specified value of "
						<< old << " operations." << newl;			
		}
		if (ret.capacity() < ret.size() + num_ops)
		{
			ret.reserve(num_ops);
		}
		size_t added = 0;
		auto gen = generator::rgen{};
		while (added < num_ops)
		{
			auto lhs = generator::create_random_in_range<uint128_t>(gen);
			auto rhs = generator::create_random_in_range<std::uint8_t>(gen) % 128_u128;
			assert(rhs < 128_u128);
			ret.emplace_back(binary_op::left_shift, lhs, rhs);
			ret.emplace_back(binary_op::right_shift, lhs, rhs);
			added += 2;
		}
	}
	return ret;
}

cjm::uint128_tests::binary_op_u128_vect_t cjm::uint128_tests::generate_bw_ops(size_t num_ops,
	bool include_standard_tests)
{
	using provider_t = u128_testing_constant_providers::testing_constant_provider<uint128_t>;
	binary_op_u128_vect_t ret;
	if (include_standard_tests)
	{
		for (size_t i = 0; i < provider_t::all_values.size(); ++i)
		for (size_t j = i; j < provider_t::all_values.size(); ++j)
		{
			const auto& lhs = provider_t::all_values[i];
			const auto& rhs = provider_t::all_values[j];
			ret.emplace_back(binary_op::bw_and, lhs, rhs);
			ret.emplace_back(binary_op::bw_or, lhs, rhs);
			ret.emplace_back(binary_op::bw_xor, lhs, rhs);
		}				
	}
	if (num_ops > 0)
	{
		auto mod = num_ops % 3;
		if (mod != 0)
		{
			auto saver = cout_saver{ std::cerr };
			auto old = num_ops;
			num_ops += (3 - mod);
			assert(num_ops % 3 == 0);
			std::cerr   << "Unable to produce " << std::dec << old
						<< " bitwise operations: the number is not divisible by 3."
						<< "  Will produce " << num_ops
						<< " bitwise operations instead." << newl;			
		}
		
		if (ret.capacity() < ret.size() + num_ops)
		{
			ret.reserve(num_ops);
		}
		size_t added = 0;
		auto gen = generator::rgen{};
		while (added < num_ops)
		{
			auto lhs = generator::create_random_in_range<uint128_t>(gen);
			auto rhs = generator::create_random_in_range<uint128_t>(gen);
			ret.emplace_back(binary_op::bw_and, lhs, rhs);
			ret.emplace_back(binary_op::bw_or, lhs, rhs);
			ret.emplace_back(binary_op::bw_xor, lhs, rhs);
			added += 3;
		}
	}
	return ret;
}

cjm::uint128_tests::binary_op_u128_vect_t cjm::uint128_tests::generate_mult_ops(size_t num_each_range,
	bool include_standard_tests)
{
	using provider_t = u128_testing_constant_providers::testing_constant_provider<uint128_t>;
	binary_op_u128_vect_t ret;
	if (include_standard_tests)
	{
		for (size_t i = 0; i < provider_t::all_values.size(); ++i)
		for (size_t j = i; j < provider_t::all_values.size(); ++j)
		{
			const auto& lhs = provider_t::all_values[i];
			const auto& rhs = provider_t::all_values[j];
			ret.emplace_back(binary_op::multiply, lhs, rhs);			
		}
	}

	if (num_each_range > 0)
	{

		constexpr auto num_ranges = 4_szt;
		auto total_ops = num_each_range * num_ranges;
		if (ret.capacity() < ret.size() + total_ops)
		{
			ret.reserve(ret.size() + total_ops);
		}
		auto rgen = generator::rgen{};
		
		constexpr size_t size_x64 = std::numeric_limits<std::uint64_t>::digits / CHAR_BIT;
		constexpr size_t size_x128 = std::numeric_limits<uint128_t>::digits / CHAR_BIT;
		constexpr size_t size_x32 = std::numeric_limits<std::uint32_t>::digits / CHAR_BIT;

		//first range-- x64 * x64
		auto num_this_range = 0_szt;
		while (num_this_range < num_each_range)
		{
			ret.emplace_back(generator::create_multiplication_op(rgen, size_x64, size_x64));
			++num_this_range;
		}
		//second range x128 * x64
		num_this_range = 0;
		while (num_this_range < num_each_range)
		{
			ret.emplace_back(generator::create_multiplication_op(rgen, size_x128, size_x64));
			++num_this_range;
		}
		//third range x128 * x32
		num_this_range = 0;
		while (num_this_range < num_each_range)
		{
			ret.emplace_back(generator::create_multiplication_op(rgen, size_x128, size_x32));
			++num_this_range;
		}
		//fourth range x128 * x128
		num_this_range = 0;
		while (num_this_range < num_each_range)
		{
			ret.emplace_back(generator::create_multiplication_op(rgen, size_x128, size_x128));
			++num_this_range;
		}
	}
	return ret;
}

cjm::uint128_tests::binary_op_u128_vect_t cjm::uint128_tests::generate_divmod_ops(size_t num_each_range,
	bool include_standard_tests)
{
	auto saver = cout_saver{ std::cerr };
	constexpr auto num_ranges = 4;
	binary_op_u128_vect_t ret;
	if (include_standard_tests)
	{
		ret.reserve(172);
		insert_standard_divmod_ops(ret);
	}
	if (num_each_range > 0)
	{
		if (num_each_range % 2 != 0)
		{
			auto old = num_each_range++;
			std::cerr   << "Cannot add " << std::dec << old
						<< " operations per range -- not even.  Will add "
						<< std::dec << num_each_range << " ops instead." << newl;
		}

		const auto to_add = num_each_range * num_ranges;
		if (ret.capacity() - ret.size() < to_add)
			ret.reserve(ret.size() + to_add);
		auto added_this_range = 0_szt;
		auto rgen = generator::rgen{};

		//range: full / full
		while (added_this_range < num_each_range)
		{
			auto first = generator::create_random_in_range<uint128_t>(rgen);
			auto second = generator::create_random_in_range<uint128_t>(rgen);
			auto* dividend = first >= second ? &first : &second;
			auto* divisor = dividend == &first ? &second : &first;
			assert(dividend != divisor);
			while (*divisor == 0)
			{
				*divisor = generator::create_random_in_range<uint128_t>(rgen);				
			}
			ret.emplace_back(binary_op::divide, *dividend, *divisor);
			ret.emplace_back(binary_op::modulus, *dividend, *divisor);
			added_this_range += 2;
		}

		added_this_range = 0;
		//range full, x64
		while (added_this_range < num_each_range)
		{
			auto first = generator::create_random_in_range<uint128_t>(rgen);
			uint128_t second = generator::create_random_in_range<std::uint64_t>(rgen);
			auto* dividend = first >= second ? &first : &second;
			auto* divisor = dividend == &first ? &second : &first;
			while (*divisor == 0 )
			{
				*divisor = generator::create_random_in_range<std::uint64_t>(rgen);
			}
			assert(dividend != divisor);
			ret.emplace_back(binary_op::divide, *dividend, *divisor);
			ret.emplace_back(binary_op::modulus, *dividend, *divisor);
			added_this_range += 2;
		}

		added_this_range = 0;
		//range x64 + x32, x64
		while (added_this_range < num_each_range)
		{
			const uint128_t first_low = generator::create_random_in_range<std::uint64_t>(rgen);
			uint128_t first_high = generator::create_random_in_range<std::uint32_t>(rgen);
			first_high <<= 64;
			uint128_t first = first_high | first_low;
			uint128_t second = generator::create_random_in_range<std::uint64_t>(rgen);

			const uint128_t* dividend = first >= second ? &first : &second;
			uint128_t* divisor = dividend == &first ? &second : &first;
			assert(dividend != divisor);
			while (*divisor == 0)
			{
				*divisor = generator::create_random_in_range<std::uint32_t>(rgen);
			}
			ret.emplace_back(binary_op::divide, *dividend, *divisor);
			ret.emplace_back(binary_op::modulus, *dividend, *divisor);
			added_this_range += 2;			
		}

		added_this_range = 0;
		//range x64 + x32, x16
		while (added_this_range < num_each_range)
		{
			const uint128_t first_low = generator::create_random_in_range<std::uint64_t>(rgen);
			uint128_t first_high = generator::create_random_in_range<std::uint32_t>(rgen);
			first_high <<= 64;
			uint128_t first = first_high | first_low;
			uint128_t second = generator::create_random_in_range<std::uint16_t>(rgen);

			const uint128_t* dividend = first >= second ? &first : &second;
			uint128_t* divisor = dividend == &first ? &second : &first;
			assert(dividend != divisor);
			while (*divisor == 0)
			{
				*divisor = generator::create_random_in_range<std::uint16_t>(rgen);
			}
			
			ret.emplace_back(binary_op::divide, *dividend, *divisor);
			ret.emplace_back(binary_op::modulus, *dividend, *divisor);
			added_this_range += 2;
		}
	}
	return ret;
}

void cjm::uint128_tests::insert_standard_divmod_ops(binary_op_u128_vect_t& op_vec)
{
	using arr_t = std::array<uint128_t, 4_szt>;
	constexpr arr_t full_128_arr=	 
	{
		0xc0de'd00d'fea2'cafe'babe'b00b'600d'f00d_u128,
		0xdead'beef'badd'f00d'cafe'babe'600d'b00b_u128,
		0x8000'0000'0000'0000'0000'0000'0000'0000_u128,
		0x7fff'ffff'ffff'ffff'ffff'ffff'ffff'ffff_u128
	};

	constexpr arr_t half_128_arr =
	{
		0x0000'0000'fea2'cafe'babe'b00b'600d'f00d_u128,
		0x0000'0000'badd'f00d'cafe'babe'600d'b00b_u128,
		0x0000'0000'8000'0000'0000'0000'0000'0000_u128,
		0x0000'0000'7fff'ffff'ffff'ffff'ffff'ffff_u128
	};

	/*constexpr arr_t byte_over_x64_arr =
	{
		0x0000'0000'0000'00fe'babe'b00b'600d'f00d_u128,
		0x0000'0000'0000'000d'cafe'babe'600d'b00b_u128,
		0x0000'0000'0000'0071'8000'0000'0000'0000_u128,
		0x0000'0000'0000'00a0'7fff'ffff'ffff'ffff_u128
	};

	constexpr arr_t nibble_over_x64_arr =
	{
		0x0000'0000'0000'000e'babe'b00b'600d'f00d_u128,
		0x0000'0000'0000'000d'cafe'babe'600d'b00b_u128,
		0x0000'0000'0000'0001'8000'0000'0000'0000_u128,
		0x0000'0000'0000'0003'7fff'ffff'ffff'ffff_u128
	};*/

	constexpr arr_t x64_arr =
	{
		0xcafe'babe'dead'beef_u128,
		0x0110'a11a'abba'0084_u128,
		0x0400'0000'0000'0000_u128,
		0xcafe'babe'600d'b00b_u128
	};

	constexpr arr_t x32_arr =
	{
		0xcafe'babe_u128,
		0xc0de'd00d_u128,
		0xdead'beef_u128,
		0x600d'f00d_u128,
	};

	constexpr arr_t byte_arr =
	{
		0x04,
		0xef,
		0xa2,
		0x64,
	};


	constexpr arr_t nibble_arr =
	{
		0x02,
		0x03,
		0x0A,
		0x0C
	};
	
	//four operations added this segment
	// FOUR OPS ADDED SO FAR
	//add 0/1 and x/1
	op_vec.emplace_back(binary_op::divide, 0_u128, 1_u128);
	op_vec.emplace_back(binary_op::modulus, 0_u128, 1_u128);
	op_vec.emplace_back(binary_op::divide, full_128_arr[2], 1_u128);
	op_vec.emplace_back(binary_op::modulus, full_128_arr[2], 1_u128);

	//EIGHT operations added this segment
	//TWELVE OPS ADDED SO FAR
	//add full / random full
	auto rgen = generator::rgen{};
	for (const auto& dividend : full_128_arr)
	{
		auto rnd = generator::create_random_in_range<uint128_t>(rgen);
		op_vec.emplace_back(binary_op::divide, dividend, rnd);
		op_vec.emplace_back(binary_op::modulus, dividend, rnd);
	}

	//32 ops added this segment
	//44 OPS ADDED SO FAR
	//case: divisor x64 > top half of dividend
	for (const auto& dividend : half_128_arr)
	for (const auto& divisor : x64_arr)
	{
		op_vec.emplace_back(binary_op::divide, dividend, divisor);
		op_vec.emplace_back(binary_op::modulus, dividend, divisor);
	}

	//32 ops added this segment
	//76 OPS ADDED SO FAR
	//case divisor x64 < top half of dividend
	for (const auto& dividend : full_128_arr)
	for (const auto& divisor : x32_arr)
	{
		op_vec.emplace_back(binary_op::divide, dividend, divisor);
		op_vec.emplace_back(binary_op::modulus, dividend, divisor);
	}

	//32 ops added this segment
	//108 OPS ADDED SO FAR
	//case dividend fit x64 divisor fit x32
	for (const auto& dividend : x64_arr)
	for (const auto& divisor : x32_arr)
	{
		op_vec.emplace_back(binary_op::divide, dividend, divisor);
		op_vec.emplace_back(binary_op::modulus, dividend, divisor);
	}

	//32 ops added this segment
	//140 OPS ADDED SO FAR
	//case dividend fit x32 divisor fit x08
	for (const auto& dividend : x32_arr)
	for (const auto& divisor : byte_arr)
	{
		op_vec.emplace_back(binary_op::divide, dividend, divisor);
		op_vec.emplace_back(binary_op::modulus, dividend, divisor);
	}

	//32 ops added this segment
	//172 OPS ADDED SO FAR
	//case dividend fit x08 divisor fit nibble
	for (const auto& dividend : byte_arr)
	for (const auto& divisor : nibble_arr)
	{
		op_vec.emplace_back(binary_op::divide, dividend, divisor);
		op_vec.emplace_back(binary_op::modulus, dividend, divisor);
	}

	//172 operations total.
}

void cjm::uint128_tests::test_binary_operation(binary_op_u128_t& op, std::string_view test_name)
{
	using result_t = typename binary_op_u128_t::result_t;
	result_t result = std::nullopt;
	try
	{
		bool no_result_now = !op.has_result();
		op.calculate_result();
		if (no_result_now)
		{
			cjm_assert(op.has_result());
		}
		result = op.result();
		bool has_correct = op.has_correct_result();
		bool result_equals = result.value().first == result.value().second;
		if (!has_correct || !result_equals)
		{
			std::cerr << "Error ... has_correct: [" << std::boolalpha << has_correct << "]; result_equals: [" << result_equals << "]." << newl;
		}
		cjm_assert( has_correct && result_equals);
		if (op.op_code() == binary_op::divide || op.op_code() == binary_op::modulus)
		{
			validate_divmod_op(op);
		}
	}
	catch (const testing::testing_failure& ex)
	{
		auto saver = cout_saver{ std::cerr };
		std::cerr << "Test: [" << test_name << "], failed -- exception (" << ex.what() << ") thrown." << newl;
		std::cerr << "Operation: [" << op.op_code() << "]; Left operand: [" << std::dec << op.left_operand() << "]; Right operand: [" << op.right_operand() << "]." << newl;
		if (result.has_value())
		{
			std::cerr << "Result of operation.  Actual: [" << std::dec << result->first << "]; Expected by control: [" << std::dec << result->second << "]." << newl;
		}
		else
		{
			std::cerr << "No result available." << newl;
		}
		auto path = create_failing_op_pathname(op.op_code());
		std::cerr << "Going to write failing operation to [" << path << "]: " << newl;
		bool except_file_write = false;
		try
		{
			auto fstrm = string::make_throwing_ofstream<char>(path);
			fstrm << op;
		}
		catch (const std::exception& ex2)
		{
			except_file_write = true;
			std::cerr << "Unable to write failing operation to file [" << path << "]: exception thrown with msg: [" << ex2.what() << "]." << newl;
		}
		if (except_file_write)
		{
			std::cerr << "Logging failing operation to stderr instead (between square brackets): [" << op << "]." << newl;
		}
		else
		{
			std::cerr << "Successfully wrote failing operation to: [" << path << "]." << newl;
		}
		throw;
	}
}

void cjm::uint128_tests::test_unary_operation(unary_op_u128_t& op, std::string_view test_name)
{
	try
	{
		bool has_result = op.has_result();
		if (!has_result)
		{
			op.calculate_result();
			has_result = op.has_result();
		}
		cjm_assert(has_result && op.has_correct_result());
	}
	catch (const testing::testing_failure& ex)
	{
		auto saver = cout_saver{ std::cerr };
		std::cerr << "Test: [" << test_name << "], failed -- exception (" << ex.what() << ") thrown." << newl;
		std::cerr << "Operation: [" << op.op_code() << "]; Left operand: [" << std::dec << op.operand() << "]." << newl;

		auto path = create_failing_op_pathname(op.op_code());
		std::cerr << "Going to write failing operation to [" << path << "]: " << newl;
		bool except_file_write = false;
		try
		{
			auto fstrm = string::make_throwing_ofstream<char>(path);
			fstrm << op;
		}
		catch (const std::exception& ex2)
		{
			except_file_write = true;
			std::cerr << "Unable to write failing operation to file [" << path << "]: exception thrown with msg: [" << ex2.what() << "]." << newl;
		}
		if (except_file_write)
		{
			std::cerr << "Logging failing operation to stderr instead (between square brackets): [" << op << "]." << newl;
		}
		else
		{
			std::cerr << "Successfully wrote failing operation to: [" << path << "]." << newl;
		}
		throw;
	}
}

void cjm::uint128_tests::compile_time_addition_test() noexcept
{
	static_assert(340282366920938463463374607431768211455_u128 + 340282366920938463463374607431768211455_u128 == 340282366920938463463374607431768211454_u128);
	static_assert(340282366920938463463374607431768211455_u128 + 340282366920938463463374607431768211454_u128 == 340282366920938463463374607431768211453_u128);
	static_assert(340282366920938463463374607431768211455_u128 + 0_u128 == 340282366920938463463374607431768211455_u128);
	static_assert(340282366920938463463374607431768211455_u128 + 1_u128 == 0_u128);
	static_assert(340282366920938463463374607431768211455_u128 + 18446744073709551615_u128 == 18446744073709551614_u128);
	static_assert(340282366920938463463374607431768211455_u128 + 340282366920938463463374607431768211454_u128 == 340282366920938463463374607431768211453_u128);
	static_assert(340282366920938463463374607431768211455_u128 + 18446744073709551616_u128 == 18446744073709551615_u128);
	static_assert(340282366920938463463374607431768211454_u128 + 340282366920938463463374607431768211454_u128 == 340282366920938463463374607431768211452_u128);
	static_assert(340282366920938463463374607431768211454_u128 + 0_u128 == 340282366920938463463374607431768211454_u128);
	static_assert(340282366920938463463374607431768211454_u128 + 1_u128 == 340282366920938463463374607431768211455_u128);
	static_assert(340282366920938463463374607431768211454_u128 + 18446744073709551615_u128 == 18446744073709551613_u128);
	static_assert(340282366920938463463374607431768211454_u128 + 340282366920938463463374607431768211454_u128 == 340282366920938463463374607431768211452_u128);
	static_assert(340282366920938463463374607431768211454_u128 + 18446744073709551616_u128 == 18446744073709551614_u128);
	static_assert(0_u128 + 0_u128 == 0_u128);
	static_assert(0_u128 + 1_u128 == 1_u128);
	static_assert(0_u128 + 18446744073709551615_u128 == 18446744073709551615_u128);
	static_assert(0_u128 + 340282366920938463463374607431768211454_u128 == 340282366920938463463374607431768211454_u128);
	static_assert(0_u128 + 18446744073709551616_u128 == 18446744073709551616_u128);
	static_assert(1_u128 + 1_u128 == 2_u128);
	static_assert(1_u128 + 18446744073709551615_u128 == 18446744073709551616_u128);
	static_assert(1_u128 + 340282366920938463463374607431768211454_u128 == 340282366920938463463374607431768211455_u128);
	static_assert(1_u128 + 18446744073709551616_u128 == 18446744073709551617_u128);
	static_assert(18446744073709551615_u128 + 18446744073709551615_u128 == 36893488147419103230_u128);
	static_assert(18446744073709551615_u128 + 340282366920938463463374607431768211454_u128 == 18446744073709551613_u128);
	static_assert(18446744073709551615_u128 + 18446744073709551616_u128 == 36893488147419103231_u128);
	static_assert(340282366920938463463374607431768211454_u128 + 340282366920938463463374607431768211454_u128 == 340282366920938463463374607431768211452_u128);
	static_assert(340282366920938463463374607431768211454_u128 + 18446744073709551616_u128 == 18446744073709551614_u128);
	static_assert(18446744073709551616_u128 + 18446744073709551616_u128 == 36893488147419103232_u128);
}

void cjm::uint128_tests::compile_time_shift_test() noexcept
{
	static_assert((340'282'366'920'938'463'463'374'607'431'768'211'454_u128 << 0_u128) == 340'282'366'920'938'463'463'374'607'431'768'211'454_u128);
	static_assert((340282366920938463463374607431768211454_u128 >> 0_u128) == 340'282'366'920'938'463'463'374'607'431'768'211'454_u128);
	static_assert((340282366920938463463374607431768211454_u128 << 1_u128) == 340282366920938463463374607431768211452_u128);
	static_assert((340282366920938463463374607431768211454_u128 >> 1_u128) == 170141183460469231731687303715884105727_u128);

	static_assert((1_u128 << 0_u128) == 1_u128);
	static_assert((1_u128 >> 0_u128) == 1_u128);
	static_assert((1_u128 << 1_u128) == 2_u128);
	static_assert((1_u128 << 2_u128) == 4_u128);

	static_assert((1_u128 << 63_u128) == 9223372036854775808_u128);
	static_assert((1_u128 >> 63_u128) == 0_u128);
	static_assert((1_u128 << 64_u128) == 18446744073709551616_u128);
	static_assert((1_u128 >> 64_u128) == 0_u128);
	static_assert((1_u128 << 65_u128) == 36893488147419103232_u128);
	static_assert((1_u128 >> 65_u128) == 0_u128);

	static_assert((1_u128 << 126_u128) == 85070591730234615865843651857942052864_u128);
	static_assert((1_u128 >> 126_u128) == 0_u128);
	static_assert((1_u128 << 127_u128) == 170141183460469231731687303715884105728_u128);
	static_assert((1_u128 >> 127_u128) == 0_u128);

	static_assert((340282366920938463463374607431768211454_u128 << 13_u128) == 340282366920938463463374607431768195072_u128);
	static_assert((340282366920938463463374607431768211454_u128 >> 13_u128) == 41538374868278621028243970633760767_u128);
	static_assert((340282366920938463463374607431768211454_u128 << 14_u128) == 340282366920938463463374607431768178688_u128);

	static_assert((340282366920938463463374607431768211454_u128 << 63_u128) == 340282366920938463444927863358058659840_u128);
	static_assert((340282366920938463463374607431768211454_u128 >> 63_u128) == 36893488147419103231_u128);
	static_assert((340282366920938463463374607431768211454_u128 << 64_u128) == 340282366920938463426481119284349108224_u128);
	static_assert((340282366920938463463374607431768211454_u128 >> 64_u128) == 18446744073709551615_u128);
	static_assert((340282366920938463463374607431768211454_u128 << 65_u128) == 340282366920938463389587631136930004992_u128);
	static_assert((340282366920938463463374607431768211454_u128 >> 65_u128) == 9223372036854775807_u128);
	static_assert((340282366920938463463374607431768211454_u128 >> 85_u128) == 8796093022207_u128);
	static_assert((340282366920938463463374607431768211454_u128 << 86_u128) == 340282366920783720958463934897405820928_u128);
	static_assert((340282366920938463463374607431768211454_u128 >> 86_u128) == 4398046511103_u128);
	static_assert((340282366920938463463374607431768211454_u128 << 87_u128) == 340282366920628978453553262363043430400_u128);

	static_assert((340282366920938463463374607431768211454_u128 >> 122_u128) == 63_u128);
	static_assert((340282366920938463463374607431768211454_u128 << 123_u128) == 319014718988379809496913694467282698240_u128);
	static_assert((340282366920938463463374607431768211454_u128 >> 123_u128) == 31_u128);
	static_assert((18446744073709551616_u128 << 15_u128) == 604462909807314587353088_u128);
	static_assert((18446744073709551616_u128 >> 15_u128) == 562949953421312_u128);
	static_assert((18446744073709551616_u128 << 16_u128) == 1208925819614629174706176_u128);
	static_assert((18446744073709551616_u128 >> 16_u128) == 281474976710656_u128);
	static_assert((18446744073709551616_u128 << 17_u128) == 2417851639229258349412352_u128);
	static_assert((18446744073709551616_u128 >> 17_u128) == 140737488355328_u128);
	static_assert((18446744073709551616_u128 << 63_u128) == 170141183460469231731687303715884105728_u128);
	static_assert((18446744073709551616_u128 >> 63_u128) == 2_u128);
	static_assert((18446744073709551616_u128 << 64_u128) == 0_u128);
	static_assert((18446744073709551616_u128 >> 64_u128) == 1_u128);
	static_assert((18446744073709551616_u128 << 65_u128) == 0_u128);
	static_assert((18446744073709551615_u128 << 13_u128) == 151115727451828646830080_u128);
	static_assert((18446744073709551615_u128 >> 13_u128) == 2251799813685247_u128);
	static_assert((18446744073709551615_u128 << 14_u128) == 302231454903657293660160_u128);
	static_assert((18446744073709551615_u128 >> 14_u128) == 1125899906842623_u128);
	static_assert((18446744073709551615_u128 << 15_u128) == 604462909807314587320320_u128);
	static_assert((18446744073709551615_u128 >> 15_u128) == 562949953421311_u128);
	static_assert((18446744073709551615_u128 << 16_u128) == 1208925819614629174640640_u128);
	static_assert((18446744073709551615_u128 >> 16_u128) == 281474976710655_u128);
	static_assert((18446744073709551615_u128 << 17_u128) == 2417851639229258349281280_u128);
}

void cjm::uint128_tests::compile_time_bw_test() noexcept
{
	static_assert((340282366920938463463374607431768211454_u128 ^ 18446744073709551616_u128) == 340282366920938463444927863358058659838_u128);
	static_assert((340282366920938463463374607431768211454_u128 | 18446744073709551616_u128) == 340282366920938463463374607431768211454_u128);
	static_assert((340282366920938463463374607431768211455_u128 ^ 340282366920938463463374607431768211454_u128) == 1_u128);
	static_assert((340282366920938463463374607431768211454_u128 | 1_u128) == 340282366920938463463374607431768211455_u128);
	static_assert((340282366920938463463374607431768211455_u128 & 18446744073709551615_u128) == 18446744073709551615_u128);
	static_assert((340282366920938463463374607431768211454_u128 | 340282366920938463463374607431768211454_u128) == 340282366920938463463374607431768211454_u128);
	static_assert((340282366920938463463374607431768211454_u128 ^ 340282366920938463463374607431768211454_u128) == 0_u128);
	static_assert((340282366920938463463374607431768211454_u128 & 0_u128) == 0_u128);
	static_assert((340282366920938463463374607431768211455_u128 ^ 340282366920938463463374607431768211454_u128) == 1_u128);
	static_assert((1_u128 ^ 18446744073709551616_u128) == 18446744073709551617_u128);
	static_assert((1_u128 ^ 1_u128) == 0_u128);
	static_assert((340282366920938463463374607431768211454_u128 & 340282366920938463463374607431768211454_u128) == 340282366920938463463374607431768211454_u128);
	static_assert((340282366920938463463374607431768211455_u128 ^ 340282366920938463463374607431768211455_u128) == 0_u128);
	static_assert((340282366920938463463374607431768211454_u128 ^ 18446744073709551616_u128) == 340282366920938463444927863358058659838_u128);
	static_assert((1_u128 | 1_u128) == 1_u128);
	static_assert((1_u128 & 1_u128) == 1_u128);
	static_assert((340282366920938463463374607431768211454_u128 & 1_u128) == 0_u128);
	static_assert((340282366920938463463374607431768211455_u128 | 1_u128) == 340282366920938463463374607431768211455_u128);
	static_assert((340282366920938463463374607431768211455_u128 & 18446744073709551616_u128) == 18446744073709551616_u128);
	static_assert((340282366920938463463374607431768211454_u128 & 340282366920938463463374607431768211454_u128) == 340282366920938463463374607431768211454_u128);
	static_assert((0_u128 ^ 1_u128) == 1_u128);
	static_assert((340282366920938463463374607431768211455_u128 | 18446744073709551616_u128) == 340282366920938463463374607431768211455_u128);
	static_assert((18446744073709551615_u128 | 340282366920938463463374607431768211454_u128) == 340282366920938463463374607431768211455_u128);
	static_assert((340282366920938463463374607431768211454_u128 ^ 0_u128) == 340282366920938463463374607431768211454_u128);
	static_assert((340282366920938463463374607431768211454_u128 ^ 1_u128) == 340282366920938463463374607431768211455_u128);
	static_assert((18446744073709551615_u128 ^ 340282366920938463463374607431768211454_u128) == 340282366920938463444927863358058659841_u128);
	static_assert((340282366920938463463374607431768211455_u128 ^ 18446744073709551616_u128) == 340282366920938463444927863358058659839_u128);
	static_assert((0_u128 | 18446744073709551616_u128) == 18446744073709551616_u128);
}

void cjm::uint128_tests::compile_time_subtraction_test() noexcept
{
	static_assert(340282366920938463463374607431768211455_u128 - 340282366920938463463374607431768211455_u128 == 0_u128);
	static_assert(340282366920938463463374607431768211455_u128 - 340282366920938463463374607431768211454_u128 == 1_u128);
	static_assert(340282366920938463463374607431768211455_u128 - 0_u128 == 340282366920938463463374607431768211455_u128);
	static_assert(340282366920938463463374607431768211455_u128 - 1_u128 == 340282366920938463463374607431768211454_u128);
	static_assert(340282366920938463463374607431768211455_u128 - 18446744073709551615_u128 == 340282366920938463444927863358058659840_u128);
	static_assert(340282366920938463463374607431768211455_u128 - 340282366920938463463374607431768211454_u128 == 1_u128);
	static_assert(340282366920938463463374607431768211455_u128 - 18446744073709551616_u128 == 340282366920938463444927863358058659839_u128);
	static_assert(340282366920938463463374607431768211454_u128 - 340282366920938463463374607431768211454_u128 == 0_u128);
	static_assert(340282366920938463463374607431768211454_u128 - 0_u128 == 340282366920938463463374607431768211454_u128);
	static_assert(340282366920938463463374607431768211454_u128 - 1_u128 == 340282366920938463463374607431768211453_u128);
	static_assert(340282366920938463463374607431768211454_u128 - 18446744073709551615_u128 == 340282366920938463444927863358058659839_u128);
	static_assert(340282366920938463463374607431768211454_u128 - 340282366920938463463374607431768211454_u128 == 0_u128);
	static_assert(340282366920938463463374607431768211454_u128 - 18446744073709551616_u128 == 340282366920938463444927863358058659838_u128);
	static_assert(0_u128 - 0_u128 == 0_u128);
	static_assert(0_u128 - 1_u128 == 340282366920938463463374607431768211455_u128);
	static_assert(0_u128 - 18446744073709551615_u128 == 340282366920938463444927863358058659841_u128);
	static_assert(0_u128 - 340282366920938463463374607431768211454_u128 == 2_u128);
	static_assert(0_u128 - 18446744073709551616_u128 == 340282366920938463444927863358058659840_u128);
	static_assert(1_u128 - 1_u128 == 0_u128);
	static_assert(1_u128 - 18446744073709551615_u128 == 340282366920938463444927863358058659842_u128);
	static_assert(1_u128 - 340282366920938463463374607431768211454_u128 == 3_u128);
	static_assert(1_u128 - 18446744073709551616_u128 == 340282366920938463444927863358058659841_u128);
	static_assert(18446744073709551615_u128 - 18446744073709551615_u128 == 0_u128);
	static_assert(18446744073709551615_u128 - 340282366920938463463374607431768211454_u128 == 18446744073709551617_u128);
	static_assert(18446744073709551615_u128 - 18446744073709551616_u128 == 340282366920938463463374607431768211455_u128);
	static_assert(340282366920938463463374607431768211454_u128 - 340282366920938463463374607431768211454_u128 == 0_u128);
	static_assert(340282366920938463463374607431768211454_u128 - 18446744073709551616_u128 == 340282366920938463444927863358058659838_u128);
	static_assert(18446744073709551616_u128 - 18446744073709551616_u128 == 0_u128);
}

void cjm::uint128_tests::compile_time_multiplication_test() noexcept
{
	static_assert((340282366920938463463374607431768211455_u128 * 340282366920938463463374607431768211455_u128) == 1_u128);
	static_assert((340282366920938463463374607431768211455_u128 * 340282366920938463463374607431768211454_u128) == 2_u128);
	static_assert((340282366920938463463374607431768211455_u128 * 0_u128) == 0_u128);
	static_assert((340282366920938463463374607431768211455_u128 * 1_u128) == 340282366920938463463374607431768211455_u128);
	static_assert((340282366920938463463374607431768211455_u128 * 18446744073709551615_u128) == 340282366920938463444927863358058659841_u128);
	static_assert((340282366920938463463374607431768211455_u128 * 340282366920938463463374607431768211454_u128) == 2_u128);
	static_assert((340282366920938463463374607431768211455_u128 * 18446744073709551616_u128) == 340282366920938463444927863358058659840_u128);
	static_assert((340282366920938463463374607431768211454_u128 * 340282366920938463463374607431768211454_u128) == 4_u128);
	static_assert((340282366920938463463374607431768211454_u128 * 0_u128) == 0_u128);
	static_assert((340282366920938463463374607431768211454_u128 * 1_u128) == 340282366920938463463374607431768211454_u128);
	static_assert((340282366920938463463374607431768211454_u128 * 18446744073709551615_u128) == 340282366920938463426481119284349108226_u128);
	static_assert((340282366920938463463374607431768211454_u128 * 340282366920938463463374607431768211454_u128) == 4_u128);
	static_assert((340282366920938463463374607431768211454_u128 * 18446744073709551616_u128) == 340282366920938463426481119284349108224_u128);
	static_assert((0_u128 * 0_u128) == 0_u128);
	static_assert((0_u128 * 1_u128) == 0_u128);
	static_assert((0_u128 * 18446744073709551615_u128) == 0_u128);
	static_assert((0_u128 * 340282366920938463463374607431768211454_u128) == 0_u128);
	static_assert((0_u128 * 18446744073709551616_u128) == 0_u128);
	static_assert((1_u128 * 1_u128) == 1_u128);
	static_assert((1_u128 * 18446744073709551615_u128) == 18446744073709551615_u128);
	static_assert((1_u128 * 340282366920938463463374607431768211454_u128) == 340282366920938463463374607431768211454_u128);
	static_assert((1_u128 * 18446744073709551616_u128) == 18446744073709551616_u128);
	static_assert((18446744073709551615_u128 * 18446744073709551615_u128) == 340282366920938463426481119284349108225_u128);
	static_assert((18446744073709551615_u128 * 340282366920938463463374607431768211454_u128) == 340282366920938463426481119284349108226_u128);
	static_assert((18446744073709551615_u128 * 18446744073709551616_u128) == 340282366920938463444927863358058659840_u128);
	static_assert((340282366920938463463374607431768211454_u128 * 340282366920938463463374607431768211454_u128) == 4_u128);
	static_assert((340282366920938463463374607431768211454_u128 * 18446744073709551616_u128) == 340282366920938463426481119284349108224_u128);
	static_assert((18446744073709551616_u128 * 18446744073709551616_u128) == 0_u128);
	static_assert((std::numeric_limits<std::uint64_t>::max() * std::numeric_limits<std::uint64_t>::max()) == 1);
}
void cjm::uint128_tests::compile_time_divmod_test() noexcept
{
	using div_mod_t = typename uint128_t::divmod_result_t;
	static_assert((0_u128 / 1_u128) == 0_u128);
	static_assert((0_u128 % 1_u128) == 0_u128);
	static_assert((256368684943268248658307433575740207117_u128 / 16109687965047641490155963133754044755_u128) == 15_u128);
	static_assert(uint128_t::div_mod(0_u128, 1_u128) == div_mod_t{0, 0} );
	static_assert((295990755071965901746234089551998857227_u128 % 3235827725_u128) == 528850702_u128);
	static_assert((66246630108973215241078608754070084718_u128 / 7228281917011328983_u128) == 9164920636682104939_u128);
	static_assert((169936322178785030769376827207061232478_u128 % 7485361222267715344_u128) == 1976567201599658414_u128);
	static_assert((162_u128 / 2_u128) == 81_u128);
	static_assert((3235827725_u128 / 239_u128) == 13539028_u128);
	static_assert((8182809076109218472715192048_u128 / 12557_u128) == 651653187553493547241792_u128);
	static_assert((4_u128 % 10_u128) == 4_u128);
	static_assert((100_u128 % 10_u128) == 0_u128);
	static_assert((100_u128 / 2_u128) == 50_u128);
	static_assert((162_u128 % 3_u128) == 0_u128);
	static_assert((66117130356006657980658724180676477157_u128 % 7040797564508260637_u128) == 1834637398707570814_u128);
	static_assert((45218132496652697143700923240_u128 % 12667277179734853258_u128) == 8050870141438615144_u128);
	static_assert((1719979875970173947856030633_u128 / 13997_u128) == 122882037291574905183684_u128);
	static_assert((99258391579393355963748093012485398259_u128 / 16142001743698741087_u128) == 6149075756241958924_u128);
	static_assert((278450506667225178906488378352345708296_u128 % 5075200850161858457344756897931454770_u128) == 4389660758484822209871505864047150716_u128);
	static_assert((21109970526253136708242979755_u128 % 4043335572065853082_u128) == 1674839464675535439_u128);
	static_assert((16727536748470929772034052482_u128 / 13493369733211533381_u128) == 1239685644_u128);
	static_assert((208107056245992793703414435121683787583_u128 / 114544293650285138706008225025185245976_u128) == 1_u128);
	static_assert((300696748791043635522420816731213971065_u128 / 169820733485858017784125137622069351450_u128) == 1_u128);
	static_assert((69383047239906130264878645357_u128 % 15911982665839184726_u128) == 14634106630390292755_u128);
	static_assert((3280109385190515152272861871_u128 / 44978_u128) == 72926972857630733964890_u128);
	static_assert((295990755071965901746234089551998857227_u128 / 3735928559_u128) == 79228162529744429662214557768_u128);
	static_assert((55091963802398102628743599743_u128 % 784184839698078258_u128) == 449994647493900105_u128);
	static_assert((170141183460469231731687303715884105728_u128 / 3735928559_u128) == 45541872863332029185006506896_u128);
	static_assert((308818922083941790200970218656780137342_u128 % 3924503981542839365105580788109277489_u128) == 2707611523600319722734917184256493200_u128);
}
void cjm::uint128_tests::compile_time_postfix_un_op_test() noexcept
{
	static_assert((test_post_increment(1_u128)) == std::make_pair(1_u128, (1_u128 + 1)));
	static_assert((test_post_decrement(1_u128)) == std::make_pair(1_u128, (1_u128 - 1)));
}

void cjm::uint128_tests::compile_time_unary_op_pre_inc_test() noexcept
{
	static_assert((++340282366920938463463374607431768211455_u128) == 0_u128);
	static_assert((++340282366920938463463374607431768211454_u128) == 340282366920938463463374607431768211455_u128);
	static_assert((++0_u128) == 1_u128);
	static_assert((++1_u128) == 2_u128);
	static_assert((++170141183460469231731687303715884105728_u128) == 170141183460469231731687303715884105729_u128);
	static_assert((++9223372036854775808_u128) == 9223372036854775809_u128);
	static_assert((++18446744073709551615_u128) == 18446744073709551616_u128);
	static_assert((++340282366920938463463374607431768211454_u128) == 340282366920938463463374607431768211455_u128);
	static_assert((++18446744073709551616_u128) == 18446744073709551617_u128);
	static_assert((++16704806145508003165175326272464834289_u128) == 16704806145508003165175326272464834290_u128);
}

void cjm::uint128_tests::compile_time_unary_op_pre_dec_test() noexcept
{
	static_assert((--340282366920938463463374607431768211455_u128) == 340282366920938463463374607431768211454_u128);
	static_assert((--340282366920938463463374607431768211454_u128) == 340282366920938463463374607431768211453_u128);
	static_assert((--0_u128) == 340282366920938463463374607431768211455_u128);
	static_assert((--1_u128) == 0_u128);
	static_assert((--170141183460469231731687303715884105728_u128) == 170141183460469231731687303715884105727_u128);
	static_assert((--9223372036854775808_u128) == 9223372036854775807_u128);
	static_assert((--18446744073709551615_u128) == 18446744073709551614_u128);
	static_assert((--340282366920938463463374607431768211454_u128) == 340282366920938463463374607431768211453_u128);
	static_assert((--18446744073709551616_u128) == 18446744073709551615_u128);
	static_assert((--164126875270736222757305911110174077786_u128) == 164126875270736222757305911110174077785_u128);
}

void cjm::uint128_tests::compile_time_unary_op_unary_plus_test() noexcept
{
	static_assert((+340282366920938463463374607431768211455_u128) == 340282366920938463463374607431768211455_u128);
	static_assert((+340282366920938463463374607431768211454_u128) == 340282366920938463463374607431768211454_u128);
	static_assert((+0_u128) == 0_u128);
	static_assert((+1_u128) == 1_u128);
	static_assert((+170141183460469231731687303715884105728_u128) == 170141183460469231731687303715884105728_u128);
	static_assert((+9223372036854775808_u128) == 9223372036854775808_u128);
	static_assert((+18446744073709551615_u128) == 18446744073709551615_u128);
	static_assert((+340282366920938463463374607431768211454_u128) == 340282366920938463463374607431768211454_u128);
	static_assert((+18446744073709551616_u128) == 18446744073709551616_u128);
	static_assert((+337633647380721062220802820887106571678_u128) == 337633647380721062220802820887106571678_u128);
}

void cjm::uint128_tests::compile_time_unary_op_unary_minus_test() noexcept
{
	static_assert((-340282366920938463463374607431768211455_u128) == 1_u128);
	static_assert((-340282366920938463463374607431768211454_u128) == 2_u128);
	static_assert((-0_u128) == 0_u128);
	static_assert((-1_u128) == 340282366920938463463374607431768211455_u128);
	static_assert((-170141183460469231731687303715884105728_u128) == 170141183460469231731687303715884105728_u128);
	static_assert((-9223372036854775808_u128) == 340282366920938463454151235394913435648_u128);
	static_assert((-18446744073709551615_u128) == 340282366920938463444927863358058659841_u128);
	static_assert((-340282366920938463463374607431768211454_u128) == 2_u128);
	static_assert((-18446744073709551616_u128) == 340282366920938463444927863358058659840_u128);
	static_assert((-177815907624818037021882524112632073290_u128) == 162466459296120426441492083319136138166_u128);	
}

void cjm::uint128_tests::compile_time_unary_op_bitwise_not_test() noexcept
{
	static_assert((~340282366920938463463374607431768211455_u128) == 0_u128);
	static_assert((~340282366920938463463374607431768211454_u128) == 1_u128);
	static_assert((~0_u128) == 340282366920938463463374607431768211455_u128);
	static_assert((~1_u128) == 340282366920938463463374607431768211454_u128);
	static_assert((~170141183460469231731687303715884105728_u128) == 170141183460469231731687303715884105727_u128);
	static_assert((~9223372036854775808_u128) == 340282366920938463454151235394913435647_u128);
	static_assert((~18446744073709551615_u128) == 340282366920938463444927863358058659840_u128);
	static_assert((~340282366920938463463374607431768211454_u128) == 1_u128);
	static_assert((~18446744073709551616_u128) == 340282366920938463444927863358058659839_u128);
	static_assert((~71264099695017478707486265652617887073_u128) == 269018267225920984755888341779150324382_u128);
}

void cjm::uint128_tests::compile_time_unary_op_bool_cast_test() noexcept
{
	static_assert(static_cast<bool>(340282366920938463463374607431768211455_u128) == static_cast<bool>(1_u128));
	static_assert(static_cast<bool>(340282366920938463463374607431768211454_u128) == static_cast<bool>(1_u128));
	static_assert(static_cast<bool>(0_u128) == static_cast<bool>(0_u128));
	static_assert(static_cast<bool>(1_u128) == static_cast<bool>(1_u128));
	static_assert(static_cast<bool>(170141183460469231731687303715884105728_u128) == static_cast<bool>(1_u128));
	static_assert(static_cast<bool>(9223372036854775808_u128) == static_cast<bool>(1_u128));
	static_assert(static_cast<bool>(18446744073709551615_u128) == static_cast<bool>(1_u128));
	static_assert(static_cast<bool>(340282366920938463463374607431768211454_u128) == static_cast<bool>(1_u128));
	static_assert(static_cast<bool>(18446744073709551616_u128) == static_cast<bool>(1_u128));
	static_assert(static_cast<bool>(37514777061657380268812876834447067224_u128) == static_cast<bool>(1_u128));
}

void cjm::uint128_tests::compile_time_unary_op_logical_negation_test() noexcept
{
	static_assert(static_cast<bool>(!340282366920938463463374607431768211455_u128) == static_cast<bool>(0_u128));
	static_assert(static_cast<bool>(!340282366920938463463374607431768211454_u128) == static_cast<bool>(0_u128));
	static_assert(static_cast<bool>(!0_u128) == static_cast<bool>(1_u128));
	static_assert(static_cast<bool>(!1_u128) == static_cast<bool>(0_u128));
	static_assert(static_cast<bool>(!170141183460469231731687303715884105728_u128) == static_cast<bool>(0_u128));
	static_assert(static_cast<bool>(!9223372036854775808_u128) == static_cast<bool>(0_u128));
	static_assert(static_cast<bool>(!18446744073709551615_u128) == static_cast<bool>(0_u128));
	static_assert(static_cast<bool>(!340282366920938463463374607431768211454_u128) == static_cast<bool>(0_u128));
	static_assert(static_cast<bool>(!18446744073709551616_u128) == static_cast<bool>(0_u128));
	static_assert(static_cast<bool>(!334345165931069997008122611702154128321_u128) == static_cast<bool>(0_u128));
}

void cjm::uint128_tests::compile_time_comparison_test() noexcept
{
	// ReSharper disable CppIdenticalOperandsInBinaryExpression
	static_assert((340282366920938463463374607431768211455_u128 <=> 340282366920938463463374607431768211455_u128) == std::strong_ordering::equal);
	static_assert((340282366920938463463374607431768211455_u128 <=> 340282366920938463463374607431768211454_u128) == std::strong_ordering::greater);
	static_assert((340282366920938463463374607431768211455_u128 <=> 0_u128) == std::strong_ordering::greater);
	static_assert((340282366920938463463374607431768211455_u128 <=> 1_u128) == std::strong_ordering::greater);
	static_assert((340282366920938463463374607431768211455_u128 <=> 18446744073709551615_u128) == std::strong_ordering::greater);
	static_assert((340282366920938463463374607431768211455_u128 <=> 340282366920938463463374607431768211454_u128) == std::strong_ordering::greater);
	static_assert((340282366920938463463374607431768211455_u128 <=> 18446744073709551616_u128) == std::strong_ordering::greater);
	static_assert((340282366920938463463374607431768211454_u128 <=> 340282366920938463463374607431768211454_u128) == std::strong_ordering::equal);
	static_assert((340282366920938463463374607431768211454_u128 <=> 0_u128) == std::strong_ordering::greater);
	static_assert((340282366920938463463374607431768211454_u128 <=> 1_u128) == std::strong_ordering::greater);
	static_assert((340282366920938463463374607431768211454_u128 <=> 18446744073709551615_u128) == std::strong_ordering::greater);
	static_assert((340282366920938463463374607431768211454_u128 <=> 340282366920938463463374607431768211454_u128) == std::strong_ordering::equal);
	static_assert((340282366920938463463374607431768211454_u128 <=> 18446744073709551616_u128) == std::strong_ordering::greater);
	static_assert((0_u128 <=> 0_u128) == std::strong_ordering::equal);
	static_assert((0_u128 <=> 1_u128) == std::strong_ordering::less);
	static_assert((0_u128 <=> 18446744073709551615_u128) == std::strong_ordering::less);
	static_assert((0_u128 <=> 340282366920938463463374607431768211454_u128) == std::strong_ordering::less);
	static_assert((0_u128 <=> 18446744073709551616_u128) == std::strong_ordering::less);
	static_assert((1_u128 <=> 1_u128) == std::strong_ordering::equal);
	static_assert((1_u128 <=> 18446744073709551615_u128) == std::strong_ordering::less);
	static_assert((1_u128 <=> 340282366920938463463374607431768211454_u128) == std::strong_ordering::less);
	static_assert((1_u128 <=> 18446744073709551616_u128) == std::strong_ordering::less);
	static_assert((18446744073709551615_u128 <=> 18446744073709551615_u128) == std::strong_ordering::equal);
	static_assert((18446744073709551615_u128 <=> 340282366920938463463374607431768211454_u128) == std::strong_ordering::less);
	static_assert((18446744073709551615_u128 <=> 18446744073709551616_u128) == std::strong_ordering::less);
	static_assert((340282366920938463463374607431768211454_u128 <=> 340282366920938463463374607431768211454_u128) == std::strong_ordering::equal);
	static_assert((340282366920938463463374607431768211454_u128 <=> 18446744073709551616_u128) == std::strong_ordering::greater);
	static_assert((18446744073709551616_u128 <=> 18446744073709551616_u128) == std::strong_ordering::equal);
	// ReSharper restore CppIdenticalOperandsInBinaryExpression
}

//void cjm::uint128_tests::execute_gen_comp_ops_test()
//{
//	auto rgen = cjm::uint128_tests::generator::rgen{};
//	const size_t num_ops = 10;
//	
//	auto v = generator::create_compare_op(rgen);
//	cjm_assert(v.size() == num_ops);
//}
void cjm::uint128_tests::generator::rgen::shuffle(binary_op_u128_vect_t& vec, size_t shuffle_depth)
{
	if (shuffle_depth > vec.size())
	{
		auto strm = string::make_throwing_sstream<char>();
		strm << "shuffle_deck parameter (value: " << shuffle_depth << ") is greater than size of vector (size: " << vec.size() << ").";
		throw std::invalid_argument{ strm.str() };
	}
	auto begin = vec.begin();
	auto end = begin + shuffle_depth;
	assert(end <= vec.end());
	std::shuffle(begin, end, m_gen->m_twister);		
}
void cjm::uint128_tests::execute_builtin_add_with_carry_test()
{
	if constexpr (cjm::numerics::concepts::builtin_128bit_unsigned_integer<cjm::numerics::natuint128_t>)
	{
		using nat_u128_t = cjm::numerics::natuint128_t;

		auto print_input = [] (unsigned char in, uint128_t lhs, uint128_t rhs) -> void
		{
			auto saver = cout_saver{cout};
			std::cout
				<< "Executing add with carry for inputs -- carry in: [" << std::dec
				<< static_cast<unsigned>(in) << "], lhs: [0x" << std::hex << std::setw(32)
				<< std::setfill('0') << lhs << "], rhs: [0x" << std::hex << std::setw(32)
				<< std::setfill('0') << rhs << "]." << newl;
		};

		auto print_sum = [] (unsigned char out, uint128_t sum) -> void
		{
			auto saver = cout_saver{cout};
			std::cout
				<< "Result: sum: [0x" << std::hex << std::setw(32) << std::setfill('0')
				<< sum << "], out: [" << std::dec
				<< static_cast<unsigned>(out) << "]." << newl;
		};

		constexpr auto lhs_1_src = 0xc0de'd00d'fea2'cafe'babe'b00b'600d'f00d_u128;
		constexpr auto rhs_1_src = 0xf00d'600d'dead'beef'600d'c0de'600d'd00d_u128;
		auto lhs_1 = static_cast<nat_u128_t>(lhs_1_src);
		auto rhs_1 = static_cast<nat_u128_t>(rhs_1_src);
		constexpr unsigned char carry_in_1 = 0;
		unsigned char carry_out = 0;
		print_input(carry_in_1, lhs_1_src, rhs_1_src);

		auto result = cjm::numerics::internal::add_with_carry(lhs_1, rhs_1, carry_in_1, carry_out);
		print_sum(carry_out, static_cast<uint128_t>(result));

		constexpr auto expected_sum = lhs_1_src + rhs_1_src;
		constexpr unsigned char expected_carry_out = expected_sum < lhs_1_src ? 1 : 0;

		cjm_assert(carry_out == expected_carry_out && result == expected_sum);

		constexpr auto ctime_res_1 = cjm::numerics::add_with_carry(lhs_1_src, rhs_1_src, carry_in_1);
		cjm_assert(ctime_res_1.first == expected_sum && ((ctime_res_1.second == 0) == (expected_carry_out == 0)));

	}
	else
	{
		std::cout << "NOT EXECUTING BECAUSE BUILT-IN UINT128 NOT AVAILABLE." << newl;
	}
}

void cjm::uint128_tests::execute_builtin_sub_with_borrow_test()
{
	if constexpr (cjm::numerics::concepts::builtin_128bit_unsigned_integer<cjm::numerics::natuint128_t>)
	{
		using nat_u128_t = cjm::numerics::natuint128_t;

		auto print_input = [] (unsigned char in, uint128_t lhs, uint128_t rhs) -> void
		{
			auto saver = cout_saver{cout};
			std::cout
					<< "Executing sub with borrow for inputs -- borrow in: [" << std::dec
					<< static_cast<unsigned>(in) << "], lhs: [0x" << std::hex << std::setw(32)
					<< std::setfill('0') << lhs << "], rhs: [0x" << std::hex << std::setw(32)
					<< std::setfill('0') << rhs << "]." << newl;
		};

		auto print_sum = [] (unsigned char out, uint128_t difference) -> void
		{
			auto saver = cout_saver{cout};
			std::cout
					<< "Result: difference: [0x" << std::hex << std::setw(32) << std::setfill('0')
					<< difference << "], out: [" << std::dec
					<< static_cast<unsigned>(out) << "]." << newl << newl;
		};

		constexpr auto minuend_1 = 0xc0de'd00d'fea2'cafe'babe'b00b'600d'f00d_u128;
		constexpr auto subtrahend_1 = 0xf00d'600d'dead'beef'600d'c0de'600d'd00d_u128;
		auto nat_minuend_1 = static_cast<nat_u128_t>(minuend_1);
		auto nat_subtrahend_1 = static_cast<nat_u128_t>(subtrahend_1);
		constexpr unsigned char borrow_in_1 = 0;
		unsigned char borrow_out_1 = 0;
		print_input(borrow_in_1, minuend_1, subtrahend_1);

		auto result_1 = cjm::numerics::internal::sub_with_borrow(nat_minuend_1, nat_subtrahend_1, borrow_in_1, borrow_out_1);
		print_sum(borrow_out_1, static_cast<uint128_t>(result_1));

		constexpr auto expected_diff_1 = minuend_1 - subtrahend_1;
		constexpr unsigned char expected_borrow_out_1 = expected_diff_1 > minuend_1 ? 1 : 0;

		cjm_assert(borrow_out_1 == expected_borrow_out_1 && result_1 == expected_diff_1);

		constexpr auto minuend_2 = 0x01_u128;
		constexpr auto subtrahend_2 = 0x600d'd00d_u128;
		auto nat_minuend_2 = static_cast<nat_u128_t>(minuend_2);
		auto nat_subtrahend_2 = static_cast<nat_u128_t>(subtrahend_2);
		constexpr unsigned char borrow_in_2 = 0;
		unsigned char borrow_out_2 = 0;
		print_input(borrow_in_2, minuend_2, subtrahend_2);

		nat_u128_t result_2 = cjm::numerics::internal::sub_with_borrow(nat_minuend_2, nat_subtrahend_2, borrow_in_2, borrow_out_2);
		print_sum(borrow_out_2, static_cast<uint128_t>(result_2));

		constexpr auto expected_diff_2 = minuend_2 - subtrahend_2;
		constexpr unsigned char expected_borrow_out_2 = expected_diff_2 > minuend_2 ? 1 : 0;

		cjm_assert(borrow_out_2 == expected_borrow_out_2 && result_2 == expected_diff_2);

		const auto& minuend_3 = minuend_2;
		const auto& subtrahend_3 = subtrahend_2;
		const auto& nat_minuend_3 = nat_minuend_2;
		const auto& nat_subtrahend_3 = nat_subtrahend_2;
		constexpr unsigned char borrow_in_3 = 1;
		unsigned char borrow_out_3 = 0;
		print_input(borrow_in_3, minuend_3, subtrahend_3);

		nat_u128_t result_3 = cjm::numerics::internal::sub_with_borrow(nat_minuend_3, nat_subtrahend_3, borrow_in_3, borrow_out_3);
		print_sum(borrow_out_3, static_cast<uint128_t>(result_3));

		constexpr auto expected_diff_3 = expected_diff_2 - 1;
		constexpr auto expected_borrow_out_3 = expected_borrow_out_2;

		cjm_assert(borrow_out_3 == expected_borrow_out_3 && result_3 == expected_diff_3);

		constexpr auto ctime_res_1 = cjm::numerics::sub_with_borrow(minuend_1, subtrahend_1, borrow_in_1);
		constexpr auto ctime_res_2 = cjm::numerics::sub_with_borrow(minuend_2, subtrahend_2, borrow_in_2);
		constexpr auto ctime_res_3 =  cjm::numerics::sub_with_borrow(minuend_2, subtrahend_2, borrow_in_3);

		cjm_assert(ctime_res_1.first == expected_diff_1 && ((ctime_res_1.second == 0) == (expected_borrow_out_1 == 0)));
		cjm_assert(ctime_res_2.first == expected_diff_2 && ((ctime_res_2.second == 0) == (expected_borrow_out_2 == 0)));
		cjm_assert(ctime_res_3.first == expected_diff_3 && ((ctime_res_3.second == 0) == (expected_borrow_out_3 == 0)));
	}
	else
	{
		std::cout << "NOT EXECUTING BECAUSE BUILT-IN UINT128 NOT AVAILABLE." << newl;
	}
}

void cjm::uint128_tests::execute_basic_u128_adc_test()
{
	constexpr auto addend_1_left = 0xdead'beef'600d'f00d'ffff'ffff'ffff'ffff_u128;
	constexpr auto addend_1_right = 0xffff'ffff'ffff'ffff'c0de'd00d'fea2'b00b_u128;

	constexpr auto addend_2_left = 0xffff'ffff'ffff'ffff_u128;
	constexpr auto addend_2_right = 0xc0de'd00d'fea2'b00b_u128;
	constexpr auto expected_sum_3 = addend_2_left + addend_2_right;
	constexpr auto expected_sum_4 = expected_sum_3 + 1;
	constexpr bool expected_carry_out_3 = false;
	constexpr bool expected_carry_out_4 = false;
	
	constexpr auto expected_sum_1 = addend_1_left + addend_1_right;
	constexpr auto expected_sum_2 = expected_sum_1 + 1;
	constexpr bool expected_carry_out_1 = true;
	constexpr bool expected_carry_out_2 = true;
	constexpr auto ctime_res_1 = add_with_carry(addend_1_left, addend_1_right, 0);
	constexpr auto ctime_res_2 = add_with_carry(addend_1_left, addend_1_right, 1);
	constexpr auto ctime_res_3 = add_with_carry(addend_2_left, addend_2_right, 0);
	constexpr auto ctime_res_4 = add_with_carry(addend_2_left, addend_2_right, 1);
	
	auto [actual_sum, actual_carry_out] = add_with_carry(addend_1_left, addend_1_right, 0);
	auto [actual_sum_2, actual_carry_out_2] = add_with_carry(addend_1_left, addend_1_right, 1);
	auto [actual_sum_3, actual_carry_out_3] = add_with_carry(addend_2_left, addend_2_right, 0);
	auto [actual_sum_4, actual_carry_out_4] = add_with_carry(addend_2_left, addend_2_right, 1);
	
	cjm_assert(actual_sum == expected_sum_1); 
	cjm_assert((actual_carry_out != 0) == expected_carry_out_1);
	cjm_assert(actual_sum == ctime_res_1.first);
	cjm_assert((actual_carry_out != 0) == (ctime_res_1.second!= 0));

	cjm_assert(actual_sum_2 == expected_sum_2);
	cjm_assert((actual_carry_out_2 != 0) == expected_carry_out_2);
	cjm_assert(actual_sum_2 == ctime_res_2.first);
	cjm_assert((actual_carry_out_2 != 0) == (ctime_res_2.second != 0));

	cjm_assert(actual_sum_3 == expected_sum_3);
	cjm_assert((actual_carry_out_3 != 0) == expected_carry_out_3);
	cjm_assert(actual_sum_3 == ctime_res_3.first);
	cjm_assert((actual_carry_out_3 != 0) == (ctime_res_3.second != 0));

	cjm_assert(actual_sum_4 == expected_sum_4);
	cjm_assert((actual_carry_out_4 != 0) == expected_carry_out_4);
	cjm_assert(actual_sum_4 == ctime_res_4.first);
	cjm_assert((actual_carry_out_4 != 0) == (ctime_res_4.second != 0));
}

void cjm::uint128_tests::execute_basic_u128_sbb_test()
{
	constexpr auto subtrahend_1 = 0xdead'beef'600d'f00d'ffff'ffff'ffff'ffff_u128;
	constexpr auto minuend_1 = 0xffff'ffff'ffff'ffff'c0de'd00d'fea2'b00b_u128;
	constexpr bool expected_borrow_out_1 = false;

	constexpr auto minuend_2 = 0xffff'ffff'ffff'ffff_u128;
	constexpr auto subtrahend_2 = 0xc0de'd00d'fea2'b00b_u128;
	constexpr bool expected_borrow_out_2 = false;

	constexpr auto minuend_3 = subtrahend_1;
	constexpr auto subtrahend_3 = minuend_1;
	constexpr auto expected_borrow_out_3 = true;
	
	constexpr auto minuend_4 = subtrahend_2;
	constexpr auto subtrahend_4 = minuend_2;
	constexpr auto expected_borrow_out_4 = true;

	constexpr auto expected_difference_1 = minuend_1 - subtrahend_1;
	constexpr auto expected_difference_2 = minuend_2 - subtrahend_2;
	constexpr auto expected_difference_3 = minuend_3 - subtrahend_3;
	constexpr auto expected_difference_4 = minuend_4 - subtrahend_4;
		
	constexpr auto ctime_res_1_no_bin = sub_with_borrow(minuend_1, subtrahend_1, 0);
	constexpr auto ctime_res_1_bin = sub_with_borrow(minuend_1, subtrahend_1, 1);
	constexpr auto ctime_res_2_no_bin = sub_with_borrow(minuend_2, subtrahend_2, 0);
	constexpr auto ctime_res_2_bin = sub_with_borrow(minuend_2, subtrahend_2, 1);
	constexpr auto ctime_res_3_no_bin = sub_with_borrow(minuend_3, subtrahend_3, 0);
	constexpr auto ctime_res_3_bin = sub_with_borrow(minuend_3, subtrahend_3, 1);
	constexpr auto ctime_res_4_no_bin = sub_with_borrow(minuend_4, subtrahend_4, 0);
	constexpr auto ctime_res_4_bin = sub_with_borrow(minuend_4, subtrahend_4, 1);

	cjm_assert(ctime_res_1_no_bin.first == expected_difference_1);
	cjm_assert((ctime_res_1_no_bin.second != 0) == expected_borrow_out_1);
	cjm_assert(ctime_res_1_bin.first == expected_difference_1 -1);
	cjm_assert((ctime_res_1_bin.second != 0) == expected_borrow_out_1);

	cjm_assert(ctime_res_2_no_bin.first == expected_difference_2);
	cjm_assert((ctime_res_2_no_bin.second != 0) == expected_borrow_out_2);
	cjm_assert(ctime_res_2_bin.first == expected_difference_2 - 1);
	cjm_assert((ctime_res_2_bin.second != 0) == expected_borrow_out_2);

	cjm_assert(ctime_res_3_no_bin.first == expected_difference_3);
	cjm_assert((ctime_res_3_no_bin.second != 0) == expected_borrow_out_3);
	cjm_assert(ctime_res_3_bin.first == expected_difference_3 - 1);
	cjm_assert((ctime_res_3_bin.second != 0) == expected_borrow_out_3);

	cjm_assert(ctime_res_4_no_bin.first == expected_difference_4);
	cjm_assert((ctime_res_4_no_bin.second != 0) == expected_borrow_out_4);
	cjm_assert(ctime_res_4_bin.first == expected_difference_4 - 1);
	cjm_assert((ctime_res_4_bin.second != 0) == expected_borrow_out_4);

	auto [actual_diff_1_no_bin, act_bout_1_no_bin] = sub_with_borrow(minuend_1, subtrahend_1, 0);
	auto [actual_diff_1_bin, act_bout_1_bin] = sub_with_borrow(minuend_1, subtrahend_1, 1);
	auto [actual_diff_2_no_bin, act_bout_2_no_bin] = sub_with_borrow(minuend_2, subtrahend_2, 0);
	auto [actual_diff_2_bin, act_bout_2_bin] = sub_with_borrow(minuend_2, subtrahend_2, 1);
	auto [actual_diff_3_no_bin, act_bout_3_no_bin] = sub_with_borrow(minuend_3, subtrahend_3, 0);
	auto [actual_diff_3_bin, act_bout_3_bin] = sub_with_borrow(minuend_3, subtrahend_3, 1);
	auto [actual_diff_4_no_bin, act_bout_4_no_bin] = sub_with_borrow(minuend_4, subtrahend_4, 0);
	auto [actual_diff_4_bin, act_bout_4_bin] = sub_with_borrow(minuend_4, subtrahend_4, 1);

	cjm_assert(actual_diff_1_no_bin == ctime_res_1_no_bin.first && ((act_bout_1_no_bin != 0) == (ctime_res_1_no_bin.second != 0)));
	cjm_assert(actual_diff_1_bin == ctime_res_1_bin.first && ((act_bout_1_bin != 0) == (ctime_res_1_bin.second != 0)));

	cjm_assert(actual_diff_2_no_bin == ctime_res_2_no_bin.first && ((act_bout_2_no_bin != 0) == (ctime_res_2_no_bin.second != 0)));
	cjm_assert(actual_diff_2_bin == ctime_res_2_bin.first && ((act_bout_2_bin != 0) == (ctime_res_2_bin.second != 0)));

	cjm_assert(actual_diff_3_no_bin == ctime_res_3_no_bin.first && ((act_bout_3_no_bin != 0) == (ctime_res_3_no_bin.second != 0)));
	cjm_assert(actual_diff_3_bin == ctime_res_3_bin.first && ((act_bout_3_bin != 0) == (ctime_res_3_bin.second != 0)));

	cjm_assert(actual_diff_4_no_bin == ctime_res_4_no_bin.first && ((act_bout_4_no_bin != 0) == (ctime_res_4_no_bin.second != 0)));
	cjm_assert(actual_diff_4_bin == ctime_res_4_bin.first && ((act_bout_4_bin != 0) == (ctime_res_4_bin.second != 0)));
}

void cjm::uint128_tests::execute_umult_spec_tests()
{
	constexpr std::uint16_t left_factor_16 = 0xfe'a2;
	constexpr std::uint16_t right_factor_16 = 0xf0'0f;
	constexpr std::uint32_t expected_product_32 = 0xeec6'cb7e;
	static_assert(cjm::numerics::internal::concepts::is_uint_16_or_32_t<std::uint16_t>);
	auto actual_product_32 = cjm::numerics::internal::umult(left_factor_16, right_factor_16);
	constexpr auto actual_ctime_product_32 = cjm::numerics::internal::umult(left_factor_16, right_factor_16);
	auto saver = testing::cout_saver{std::cout};
	std::cout
		<< "[0x" << std::hex << std::setw(std::numeric_limits<decltype(left_factor_16)>::digits / 4)
		<< std::setfill('0') << left_factor_16 << "] * [0x" << std::hex
		<< std::setw(std::numeric_limits<decltype(right_factor_16)>::digits / 4) << std::setfill('0')
		<< right_factor_16 << "] == [0x" << std::hex
		<< std::setw(std::numeric_limits<decltype(actual_product_32)>::digits / 4)
		<< std::setfill('0') << actual_product_32 << "]." << newl;

	//runtime assert
	cjm_assert(actual_product_32 == expected_product_32);
	//compile-time assert
	static_assert(actual_ctime_product_32 == expected_product_32,
			"Should be same if evaluated during compilation.");

	constexpr std::uint8_t left_factor_8 = 0xff;
	constexpr std::uint8_t right_factor_8 = 0xff;
	constexpr std::uint16_t expected_product_16 = 0xfe01;
	auto actual_product_16 = cjm::numerics::internal::umult(left_factor_8, right_factor_8);
	constexpr auto actual_ctime_product_16 = cjm::numerics::internal::umult(left_factor_8, right_factor_8);

	std::cout
			<< "[0x" << std::hex << std::setw(std::numeric_limits<decltype(left_factor_8)>::digits / 4)
			<< std::setfill('0') << (+left_factor_8) << "] * [0x" << std::hex
			<< std::setw(std::numeric_limits<decltype(right_factor_8)>::digits / 4) << std::setfill('0')
			<< (+right_factor_8) << "] == [0x" << std::hex
			<< std::setw(std::numeric_limits<decltype(actual_product_16)>::digits / 4)
			<< std::setfill('0') << actual_product_16 << "]." << newl;

	//runtime assert
	cjm_assert(actual_product_16 == expected_product_16);
	//compile-time assert
	static_assert(actual_ctime_product_16 == expected_product_16,
				  "Should be same if evaluated during compilation.");

	constexpr std::uint32_t left_factor_32 = 0xffff'ffff;
	constexpr std::uint32_t right_factor_32 = 0xffff'ffff;
	constexpr std::uint64_t expected_product_64 = 0xffff'fffe'0000'0001;
	auto actual_product_64 = cjm::numerics::internal::umult(left_factor_32, right_factor_32);
	constexpr auto actual_ctime_product_64 = cjm::numerics::internal::umult(left_factor_32, right_factor_32);

	std::cout
			<< "[0x" << std::hex << std::setw(std::numeric_limits<decltype(left_factor_32)>::digits / 4)
			<< std::setfill('0') << left_factor_32 << "] * [0x" << std::hex
			<< std::setw(std::numeric_limits<decltype(right_factor_32)>::digits / 4) << std::setfill('0')
			<< right_factor_32 << "] == [0x" << std::hex
			<< std::setw(std::numeric_limits<decltype(actual_product_64)>::digits / 4)
			<< std::setfill('0') << actual_product_64 << "]." << newl;

	//runtime assert
	cjm_assert(actual_product_64 == expected_product_64);
	//compile-time assert
	static_assert(actual_ctime_product_64 == expected_product_64,
				  "Should be same if evaluated during compilation.");


	constexpr std::uint64_t left_factor_64 = 0xc0de'd00d'fea2'cafeull;
	constexpr std::uint64_t right_factor_64 = 0xbabe'b00b'600d'f00dull;
	constexpr auto expected_product_128 = static_cast<uint128_t>(left_factor_64) * right_factor_64;
	auto actual_product_128
		= cjm::numerics::internal::umult(left_factor_64, right_factor_64);
	constexpr auto actual_ctime_product_128 = cjm::numerics::internal::umult(left_factor_64, right_factor_64);
	constexpr auto converted_ctime = uint128_t{ actual_ctime_product_128.m_high, actual_ctime_product_128.m_low };
	auto converted_rtime = bit_cast<uint128_t>(actual_product_128);

	std::cout
		<< "[0x" << std::hex << std::setw(std::numeric_limits<decltype(left_factor_64)>::digits / 4)
		<< std::setfill('0') << left_factor_64 << "] * [0x" << std::hex
		<< std::setw(std::numeric_limits<decltype(right_factor_64)>::digits / 4) << std::setfill('0')
		<< right_factor_64 << "] == [0x" << std::hex
		<< std::setw(std::numeric_limits<decltype(converted_rtime)>::digits / 4)
		<< std::setfill('0') << converted_rtime << "]." << newl;

	//runtime assert
	cjm_assert(converted_rtime == expected_product_128);
	//compile-time assert
	static_assert(converted_ctime == expected_product_128);
	
}

constexpr uint128_t to_uint128_t(const fixed_uint_container::add_carry_result_t<std::uint64_t>& input) noexcept
{
	auto value = static_cast<uint128_t>(fixed_uint_container::get_carry_out<std::uint64_t>(input));
	value <<= 64;
	value |= fixed_uint_container::get_sum<std::uint64_t>(input);
	return value;
}

void cjm::uint128_tests::execute_uintcontainer_adc_tests()
{
	constexpr std::uint64_t first_addend  = 13'897'774'258'637'163'262ull;
	constexpr std::uint64_t second_addend = 13'456'386'308'122'210'317ull;
	constexpr uint128_t sum = static_cast<uint128_t>(first_addend) + second_addend;
	static_assert(sum == 27'354'160'566'759'373'579_u128);
	constexpr auto res = cjm::numerics::fixed_uint_container::add_with_carry(first_addend, second_addend, 0);
	static_assert(sum == to_uint128_t(res));
	auto res_runtime = cjm::numerics::fixed_uint_container::add_with_carry(first_addend, second_addend, 0);
	auto widened = bit_cast<uint128_t>(res_runtime);
	cjm_assert(widened == sum);
	auto saver = cout_saver{ cout };
	std::cout << "[" << std::dec << first_addend << "] + [" << second_addend << "] == [" << widened << "]." << newl;
	
}

void cjm::uint128_tests::execute_issue27_bug_test()
{
	constexpr auto expected_value = 0xc0de'd00d'ea75'dead'beef'600d'f00d_u128;
	constexpr auto narrow_text = "0xc0ded00dea75deadbeef600df00d"sv;
	constexpr auto wide_text = L"0xc0de_d00dea75deadbeef600df00d"sv;
	constexpr auto utf8_text = u8"0xc0de,d00dea75deadbeef600df00d"sv;
	constexpr auto utf16_text = u"0xc0ded00dea75deadbeef600df00d"sv;
	constexpr auto utf32_text = U"0xc0ded00dea75deadbeef600df00d"sv;

	auto narrow_stream = cjm::string::make_throwing_sstream<char>();
	auto wide_stream = cjm::string::make_throwing_sstream<wchar_t>();
	auto utf8_stream = cjm::string::make_throwing_sstream<char8_t>();
	auto utf16_stream = cjm::string::make_throwing_sstream<char16_t>();
	auto utf32_stream = cjm::string::make_throwing_sstream<char32_t>();

	uint128_t narrow_result, wide_result, utf8_result, utf16_result, utf32_result;
	
	cout << "Testing narrow text conversion...." << newl;
	narrow_stream << narrow_text;
	narrow_stream >> narrow_result;

	cjm_assert(narrow_result == expected_value);
	
	cout << "Testing wide text conversion...." << newl;
	wide_stream << wide_text;
	wide_stream >> wide_result;
	cjm_assert(wide_result == expected_value);

	cout << "Testing utf8 text conversion...." << newl;
	utf8_stream << utf8_text;
	utf8_stream >> utf8_result;
	cjm_assert(utf8_result == expected_value);

	cout << "Testing utf16 text conversion...." << newl;
	utf16_stream << utf16_text;
	utf16_stream >> utf16_result;
	cjm_assert(utf16_result == expected_value);

	cout << "Testing utf32 text conversion...." << newl;
	utf32_stream << utf32_text;
	utf32_stream >> utf32_result;
	cjm_assert(utf32_result == expected_value);
}

void cjm::uint128_tests::execute_literal_test()
{
	std::cout << "Beginning single digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0x0 }), 0x0_u128, "0x0"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0x1 }), 0x1_u128, "0x1"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0x2 }), 0x2_u128, "0x2"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0x3 }), 0x3_u128, "0x3"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0x4 }), 0x4_u128, "0x4"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0x5 }), 0x5_u128, "0x5"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0x6 }), 0x6_u128, "0x6"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0x7 }), 0x7_u128, "0x7"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0x8 }), 0x8_u128, "0x8"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0x9 }), 0x9_u128, "0x9"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0xa }), 0xa_u128, "0xa"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0xb }), 0xb_u128, "0xb"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0xc }), 0xc_u128, "0xc"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0xd }), 0xd_u128, "0xd"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0xe }), 0xe_u128, "0xe"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0xf }), 0xf_u128, "0xf"sv);
	std::cout << "End single digit hex literal tests..." << newl;

	std::cout << "Begin single digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ 0 }), 0_u128, "0"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 1 }), 1_u128, "1"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 2 }), 2_u128, "2"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 3 }), 3_u128, "3"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 4 }), 4_u128, "4"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 5 }), 5_u128, "5"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 6 }), 6_u128, "6"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 7 }), 7_u128, "7"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 8 }), 8_u128, "8"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ 9 }), 9_u128, "9"sv);
	std::cout << "End single digit decimal literal tests..." << newl;

	std::cout << "Begin 2 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xa8" }), 0xa8_u128, "0xa8"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x40" }), 0x40_u128, "0x40"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x49" }), 0x49_u128, "0x49"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x25" }), 0x25_u128, "0x25"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xf1" }), 0xf1_u128, "0xf1"sv);
	std::cout << "End 2 digit hex literal tests..." << newl;

	std::cout << "Begin 2 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "53" }), 53_u128, "53"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "92" }), 92_u128, "92"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "18" }), 18_u128, "18"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "67" }), 67_u128, "67"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "74" }), 74_u128, "74"sv);
	std::cout << "End 2 digit decimal literal tests..." << newl;

	std::cout << "Begin 3 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xe65" }), 0xe65_u128, "0xe65"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x5ee" }), 0x5ee_u128, "0x5ee"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x8c3" }), 0x8c3_u128, "0x8c3"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xf3f" }), 0xf3f_u128, "0xf3f"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xc1c" }), 0xc1c_u128, "0xc1c"sv);
	std::cout << "End 3 digit hex literal tests..." << newl;

	std::cout << "Begin 3 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "161" }), 161_u128, "161"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "292" }), 292_u128, "292"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "694" }), 694_u128, "694"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "369" }), 369_u128, "369"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "374" }), 374_u128, "374"sv);
	std::cout << "End 3 digit decimal literal tests..." << newl;

	std::cout << "Begin 4 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xf6a6" }), 0xf6a6_u128, "0xf6a6"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x962c" }), 0x962c_u128, "0x962c"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x13a2" }), 0x13a2_u128, "0x13a2"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x7940" }), 0x7940_u128, "0x7940"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x441e" }), 0x441e_u128, "0x441e"sv);
	std::cout << "End 4 digit hex literal tests..." << newl;

	std::cout << "Begin 4 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "6958" }), 6958_u128, "6958"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "8152" }), 8152_u128, "8152"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "1733" }), 1733_u128, "1733"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "5465" }), 5465_u128, "5465"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "3886" }), 3886_u128, "3886"sv);
	std::cout << "End 4 digit decimal literal tests..." << newl;

	std::cout << "Begin 5 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x71f0e" }), 0x71f0e_u128, "0x71f0e"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x7ec83" }), 0x7ec83_u128, "0x7ec83"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x4ae6f" }), 0x4ae6f_u128, "0x4ae6f"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xb7f07" }), 0xb7f07_u128, "0xb7f07"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xcf4ec" }), 0xcf4ec_u128, "0xcf4ec"sv);
	std::cout << "End 5 digit hex literal tests..." << newl;

	std::cout << "Begin 5 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "77283" }), 77283_u128, "77283"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "58414" }), 58414_u128, "58414"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "50152" }), 50152_u128, "50152"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "45123" }), 45123_u128, "45123"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "43501" }), 43501_u128, "43501"sv);
	std::cout << "End 5 digit decimal literal tests..." << newl;

	std::cout << "Begin 6 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xb91a13" }), 0xb91a13_u128, "0xb91a13"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x921adb" }), 0x921adb_u128, "0x921adb"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xeec80a" }), 0xeec80a_u128, "0xeec80a"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x74ca68" }), 0x74ca68_u128, "0x74ca68"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xa161d4" }), 0xa161d4_u128, "0xa161d4"sv);
	std::cout << "End 6 digit hex literal tests..." << newl;

	std::cout << "Begin 6 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "892654" }), 892654_u128, "892654"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "210889" }), 210889_u128, "210889"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "873926" }), 873926_u128, "873926"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "872750" }), 872750_u128, "872750"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "225912" }), 225912_u128, "225912"sv);
	std::cout << "End 6 digit decimal literal tests..." << newl;

	std::cout << "Begin 7 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x097e7e5" }), 0x097e7e5_u128, "0x097e7e5"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x44ef74b" }), 0x44ef74b_u128, "0x44ef74b"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x6cea0f7" }), 0x6cea0f7_u128, "0x6cea0f7"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xd581803" }), 0xd581803_u128, "0xd581803"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x2efc1f9" }), 0x2efc1f9_u128, "0x2efc1f9"sv);
	std::cout << "End 7 digit hex literal tests..." << newl;

	std::cout << "Begin 7 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "3381953" }), 3381953_u128, "3381953"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "6385320" }), 6385320_u128, "6385320"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "2313918" }), 2313918_u128, "2313918"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "2873174" }), 2873174_u128, "2873174"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "9951700" }), 9951700_u128, "9951700"sv);
	std::cout << "End 7 digit decimal literal tests..." << newl;

	std::cout << "Begin 8 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xe3866ee1" }), 0xe3866ee1_u128, "0xe3866ee1"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x75892b60" }), 0x75892b60_u128, "0x75892b60"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x8c93acb1" }), 0x8c93acb1_u128, "0x8c93acb1"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x0a72152e" }), 0x0a72152e_u128, "0x0a72152e"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x3ec096b7" }), 0x3ec096b7_u128, "0x3ec096b7"sv);
	std::cout << "End 8 digit hex literal tests..." << newl;

	std::cout << "Begin 8 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "11598152" }), 11598152_u128, "11598152"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "40602245" }), 40602245_u128, "40602245"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "28245587" }), 28245587_u128, "28245587"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "32705753" }), 32705753_u128, "32705753"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "48429011" }), 48429011_u128, "48429011"sv);
	std::cout << "End 8 digit decimal literal tests..." << newl;

	std::cout << "Begin 9 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xf2667090b" }), 0xf2667090b_u128, "0xf2667090b"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xb310c1ee9" }), 0xb310c1ee9_u128, "0xb310c1ee9"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x041ed7de9" }), 0x041ed7de9_u128, "0x041ed7de9"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x410fd258e" }), 0x410fd258e_u128, "0x410fd258e"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x948beed3b" }), 0x948beed3b_u128, "0x948beed3b"sv);
	std::cout << "End 9 digit hex literal tests..." << newl;

	std::cout << "Begin 9 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "551968750" }), 551968750_u128, "551968750"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "638237546" }), 638237546_u128, "638237546"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "131417069" }), 131417069_u128, "131417069"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "103255397" }), 103255397_u128, "103255397"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "420808281" }), 420808281_u128, "420808281"sv);
	std::cout << "End 9 digit decimal literal tests..." << newl;

	std::cout << "Begin 10 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x739a04bf99" }), 0x739a04bf99_u128, "0x739a04bf99"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x9108bd23e5" }), 0x9108bd23e5_u128, "0x9108bd23e5"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xab323f9e14" }), 0xab323f9e14_u128, "0xab323f9e14"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x359e29a981" }), 0x359e29a981_u128, "0x359e29a981"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xa7576d58be" }), 0xa7576d58be_u128, "0xa7576d58be"sv);
	std::cout << "End 10 digit hex literal tests..." << newl;

	std::cout << "Begin 10 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "3681653863" }), 3681653863_u128, "3681653863"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "2423934938" }), 2423934938_u128, "2423934938"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "2183423382" }), 2183423382_u128, "2183423382"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "5641480195" }), 5641480195_u128, "5641480195"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "2151507309" }), 2151507309_u128, "2151507309"sv);
	std::cout << "End 10 digit decimal literal tests..." << newl;

	std::cout << "Begin 11 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x54eafebcac3" }), 0x54eafebcac3_u128, "0x54eafebcac3"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x88cd8fb70dc" }), 0x88cd8fb70dc_u128, "0x88cd8fb70dc"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xba854f5137d" }), 0xba854f5137d_u128, "0xba854f5137d"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x90fc1b67c8c" }), 0x90fc1b67c8c_u128, "0x90fc1b67c8c"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xecd1be313fd" }), 0xecd1be313fd_u128, "0xecd1be313fd"sv);
	std::cout << "End 11 digit hex literal tests..." << newl;

	std::cout << "Begin 11 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "27395111710" }), 27395111710_u128, "27395111710"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "52570782213" }), 52570782213_u128, "52570782213"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "54167263522" }), 54167263522_u128, "54167263522"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "71946009954" }), 71946009954_u128, "71946009954"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "43902233596" }), 43902233596_u128, "43902233596"sv);
	std::cout << "End 11 digit decimal literal tests..." << newl;

	std::cout << "Begin 12 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x4500ed7f36c1" }), 0x4500ed7f36c1_u128, "0x4500ed7f36c1"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x5a0e6d4a6541" }), 0x5a0e6d4a6541_u128, "0x5a0e6d4a6541"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xf649135b3eca" }), 0xf649135b3eca_u128, "0xf649135b3eca"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x994aa0f99408" }), 0x994aa0f99408_u128, "0x994aa0f99408"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xe158ee495104" }), 0xe158ee495104_u128, "0xe158ee495104"sv);
	std::cout << "End 12 digit hex literal tests..." << newl;

	std::cout << "Begin 12 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "564131105399" }), 564131105399_u128, "564131105399"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "851696496832" }), 851696496832_u128, "851696496832"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "787312722049" }), 787312722049_u128, "787312722049"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "926366390794" }), 926366390794_u128, "926366390794"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "711603834446" }), 711603834446_u128, "711603834446"sv);
	std::cout << "End 12 digit decimal literal tests..." << newl;

	std::cout << "Begin 13 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x6bc793355c212" }), 0x6bc793355c212_u128, "0x6bc793355c212"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x01417ade2d35c" }), 0x01417ade2d35c_u128, "0x01417ade2d35c"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x45bd83616c624" }), 0x45bd83616c624_u128, "0x45bd83616c624"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x929e582ac8cc5" }), 0x929e582ac8cc5_u128, "0x929e582ac8cc5"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xd82b11852bbb2" }), 0xd82b11852bbb2_u128, "0xd82b11852bbb2"sv);
	std::cout << "End 13 digit hex literal tests..." << newl;

	std::cout << "Begin 13 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "5010678957531" }), 5010678957531_u128, "5010678957531"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "5916439284821" }), 5916439284821_u128, "5916439284821"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "6893333363335" }), 6893333363335_u128, "6893333363335"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "9643585652391" }), 9643585652391_u128, "9643585652391"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "5764610944471" }), 5764610944471_u128, "5764610944471"sv);
	std::cout << "End 13 digit decimal literal tests..." << newl;

	std::cout << "Begin 14 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x138fcbd6b4d95e" }), 0x138fcbd6b4d95e_u128, "0x138fcbd6b4d95e"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xac38afefb692c2" }), 0xac38afefb692c2_u128, "0xac38afefb692c2"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x2e144054929eb1" }), 0x2e144054929eb1_u128, "0x2e144054929eb1"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xd9a328be93dfef" }), 0xd9a328be93dfef_u128, "0xd9a328be93dfef"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xb1f2fc7252b642" }), 0xb1f2fc7252b642_u128, "0xb1f2fc7252b642"sv);
	std::cout << "End 14 digit hex literal tests..." << newl;

	std::cout << "Begin 14 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "15743401181148" }), 15743401181148_u128, "15743401181148"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "71204038313303" }), 71204038313303_u128, "71204038313303"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "89240585450833" }), 89240585450833_u128, "89240585450833"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "30824121461694" }), 30824121461694_u128, "30824121461694"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "83336589252615" }), 83336589252615_u128, "83336589252615"sv);
	std::cout << "End 14 digit decimal literal tests..." << newl;

	std::cout << "Begin 15 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xd5746d5d75feef1" }), 0xd5746d5d75feef1_u128, "0xd5746d5d75feef1"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x08e9184343246bc" }), 0x08e9184343246bc_u128, "0x08e9184343246bc"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x0df5b96df7199bb" }), 0x0df5b96df7199bb_u128, "0x0df5b96df7199bb"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xc532a8c98942b6f" }), 0xc532a8c98942b6f_u128, "0xc532a8c98942b6f"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xfa09eaebfa4e8b6" }), 0xfa09eaebfa4e8b6_u128, "0xfa09eaebfa4e8b6"sv);
	std::cout << "End 15 digit hex literal tests..." << newl;

	std::cout << "Begin 15 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "899143729224474" }), 899143729224474_u128, "899143729224474"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "141194965711942" }), 141194965711942_u128, "141194965711942"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "576143944425034" }), 576143944425034_u128, "576143944425034"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "521346230373490" }), 521346230373490_u128, "521346230373490"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "122570844038145" }), 122570844038145_u128, "122570844038145"sv);
	std::cout << "End 15 digit decimal literal tests..." << newl;

	std::cout << "Begin 16 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x09925387c37f9294" }), 0x09925387c37f9294_u128, "0x09925387c37f9294"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xb65f979e74922b87" }), 0xb65f979e74922b87_u128, "0xb65f979e74922b87"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xc107cc710eac4d79" }), 0xc107cc710eac4d79_u128, "0xc107cc710eac4d79"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xf0a09b4043fb36c0" }), 0xf0a09b4043fb36c0_u128, "0xf0a09b4043fb36c0"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xabbeccf7bc59cc23" }), 0xabbeccf7bc59cc23_u128, "0xabbeccf7bc59cc23"sv);
	std::cout << "End 16 digit hex literal tests..." << newl;

	std::cout << "Begin 16 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "6894682934528874" }), 6894682934528874_u128, "6894682934528874"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "2185372000648954" }), 2185372000648954_u128, "2185372000648954"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "9809517029227668" }), 9809517029227668_u128, "9809517029227668"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "6043988758872243" }), 6043988758872243_u128, "6043988758872243"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "2574959293830917" }), 2574959293830917_u128, "2574959293830917"sv);
	std::cout << "End 16 digit decimal literal tests..." << newl;

	std::cout << "Begin 17 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x560bff9204c1b9141" }), 0x560bff9204c1b9141_u128, "0x560bff9204c1b9141"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x126c6c63885f31f2b" }), 0x126c6c63885f31f2b_u128, "0x126c6c63885f31f2b"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xa3490f60036a39b6a" }), 0xa3490f60036a39b6a_u128, "0xa3490f60036a39b6a"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x06226fd0e44276bb5" }), 0x06226fd0e44276bb5_u128, "0x06226fd0e44276bb5"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x5a44f5d790a3f9ec7" }), 0x5a44f5d790a3f9ec7_u128, "0x5a44f5d790a3f9ec7"sv);
	std::cout << "End 17 digit hex literal tests..." << newl;

	std::cout << "Begin 17 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "31069982170914479" }), 31069982170914479_u128, "31069982170914479"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "14582690360123422" }), 14582690360123422_u128, "14582690360123422"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "56485144834117324" }), 56485144834117324_u128, "56485144834117324"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "64723502656848771" }), 64723502656848771_u128, "64723502656848771"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "15545599109386577" }), 15545599109386577_u128, "15545599109386577"sv);
	std::cout << "End 17 digit decimal literal tests..." << newl;

	std::cout << "Begin 18 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xcb92451ffa219fb0cf" }), 0xcb92451ffa219fb0cf_u128, "0xcb92451ffa219fb0cf"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xf78f1640f13ec33dc7" }), 0xf78f1640f13ec33dc7_u128, "0xf78f1640f13ec33dc7"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x9bf37ac963796b51aa" }), 0x9bf37ac963796b51aa_u128, "0x9bf37ac963796b51aa"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x8966c9a6ac5e65e07b" }), 0x8966c9a6ac5e65e07b_u128, "0x8966c9a6ac5e65e07b"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x46e0398b9179675cfe" }), 0x46e0398b9179675cfe_u128, "0x46e0398b9179675cfe"sv);
	std::cout << "End 18 digit hex literal tests..." << newl;

	std::cout << "Begin 18 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "634320558329735749" }), 634320558329735749_u128, "634320558329735749"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "417855220141871972" }), 417855220141871972_u128, "417855220141871972"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "534290238317387553" }), 534290238317387553_u128, "534290238317387553"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "970998737874495372" }), 970998737874495372_u128, "970998737874495372"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "387869420158849531" }), 387869420158849531_u128, "387869420158849531"sv);
	std::cout << "End 18 digit decimal literal tests..." << newl;

	std::cout << "Begin 19 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x99693e264934eae2dd7" }), 0x99693e264934eae2dd7_u128, "0x99693e264934eae2dd7"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xb1523b4234be846c860" }), 0xb1523b4234be846c860_u128, "0xb1523b4234be846c860"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xcd6898dcb4f8efde777" }), 0xcd6898dcb4f8efde777_u128, "0xcd6898dcb4f8efde777"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xf4e3b28a94eacc3348a" }), 0xf4e3b28a94eacc3348a_u128, "0xf4e3b28a94eacc3348a"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x8b3378bbc51e868faf7" }), 0x8b3378bbc51e868faf7_u128, "0x8b3378bbc51e868faf7"sv);
	std::cout << "End 19 digit hex literal tests..." << newl;

	std::cout << "Begin 19 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "7488761567462008154" }), 7488761567462008154_u128, "7488761567462008154"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "9292196408585599067" }), 9292196408585599067_u128, "9292196408585599067"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "5728690975437488562" }), 5728690975437488562_u128, "5728690975437488562"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "5199885297471073614" }), 5199885297471073614_u128, "5199885297471073614"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "5315340657807934018" }), 5315340657807934018_u128, "5315340657807934018"sv);
	std::cout << "End 19 digit decimal literal tests..." << newl;

	std::cout << "Begin 20 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x4d53bff4ad7ac93b2b99" }), 0x4d53bff4ad7ac93b2b99_u128, "0x4d53bff4ad7ac93b2b99"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xcabe4060bf2f825f718e" }), 0xcabe4060bf2f825f718e_u128, "0xcabe4060bf2f825f718e"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x1146b1f820b25191d8af" }), 0x1146b1f820b25191d8af_u128, "0x1146b1f820b25191d8af"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x8df7638ab7b3706c1ecd" }), 0x8df7638ab7b3706c1ecd_u128, "0x8df7638ab7b3706c1ecd"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x1516b9758df78167e5fa" }), 0x1516b9758df78167e5fa_u128, "0x1516b9758df78167e5fa"sv);
	std::cout << "End 20 digit hex literal tests..." << newl;

	std::cout << "Begin 20 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "88103549888338422025" }), 88103549888338422025_u128, "88103549888338422025"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "68702126617805365963" }), 68702126617805365963_u128, "68702126617805365963"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "63344348402450333446" }), 63344348402450333446_u128, "63344348402450333446"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "12326989625342666066" }), 12326989625342666066_u128, "12326989625342666066"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "58945895557763372932" }), 58945895557763372932_u128, "58945895557763372932"sv);
	std::cout << "End 20 digit decimal literal tests..." << newl;

	std::cout << "Begin 21 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xe1bfbd22d4717ad05259a" }), 0xe1bfbd22d4717ad05259a_u128, "0xe1bfbd22d4717ad05259a"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x2dcb4ecf53e7d6f2aa6f7" }), 0x2dcb4ecf53e7d6f2aa6f7_u128, "0x2dcb4ecf53e7d6f2aa6f7"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xe6543160acf5395cba866" }), 0xe6543160acf5395cba866_u128, "0xe6543160acf5395cba866"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x4baaec3b382b23d09d335" }), 0x4baaec3b382b23d09d335_u128, "0x4baaec3b382b23d09d335"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x19e437cca3b5109171512" }), 0x19e437cca3b5109171512_u128, "0x19e437cca3b5109171512"sv);
	std::cout << "End 21 digit hex literal tests..." << newl;

	std::cout << "Begin 21 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "994186040960193280634" }), 994186040960193280634_u128, "994186040960193280634"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "548371444819620614296" }), 548371444819620614296_u128, "548371444819620614296"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "728001727256220560165" }), 728001727256220560165_u128, "728001727256220560165"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "113052864341767335574" }), 113052864341767335574_u128, "113052864341767335574"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "908809312226596937120" }), 908809312226596937120_u128, "908809312226596937120"sv);
	std::cout << "End 21 digit decimal literal tests..." << newl;

	std::cout << "Begin 22 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x74f0e0ceb36c68f1754636" }), 0x74f0e0ceb36c68f1754636_u128, "0x74f0e0ceb36c68f1754636"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xd49f81950864a9e27387dc" }), 0xd49f81950864a9e27387dc_u128, "0xd49f81950864a9e27387dc"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x27bb9883535d177102f5e0" }), 0x27bb9883535d177102f5e0_u128, "0x27bb9883535d177102f5e0"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xe95d022d7c138f006bd066" }), 0xe95d022d7c138f006bd066_u128, "0xe95d022d7c138f006bd066"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xee4db8118b360e787362d1" }), 0xee4db8118b360e787362d1_u128, "0xee4db8118b360e787362d1"sv);
	std::cout << "End 22 digit hex literal tests..." << newl;

	std::cout << "Begin 22 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "7462610915640073872647" }), 7462610915640073872647_u128, "7462610915640073872647"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "9510259020244264999011" }), 9510259020244264999011_u128, "9510259020244264999011"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "6526373392714906107579" }), 6526373392714906107579_u128, "6526373392714906107579"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "9280913339351554966547" }), 9280913339351554966547_u128, "9280913339351554966547"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "7389204218221194685513" }), 7389204218221194685513_u128, "7389204218221194685513"sv);
	std::cout << "End 22 digit decimal literal tests..." << newl;

	std::cout << "Begin 23 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x7319ae9a32c39beea4d2b8e" }), 0x7319ae9a32c39beea4d2b8e_u128, "0x7319ae9a32c39beea4d2b8e"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x1d3b5638a520dc718e58ffb" }), 0x1d3b5638a520dc718e58ffb_u128, "0x1d3b5638a520dc718e58ffb"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xc4282fc3ac0260ac01d22ed" }), 0xc4282fc3ac0260ac01d22ed_u128, "0xc4282fc3ac0260ac01d22ed"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x8eb4eaa48eb412024c83e98" }), 0x8eb4eaa48eb412024c83e98_u128, "0x8eb4eaa48eb412024c83e98"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xc836d42bf34038465a38a87" }), 0xc836d42bf34038465a38a87_u128, "0xc836d42bf34038465a38a87"sv);
	std::cout << "End 23 digit hex literal tests..." << newl;

	std::cout << "Begin 23 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "12441301274496335645733" }), 12441301274496335645733_u128, "12441301274496335645733"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "11670309304772857707038" }), 11670309304772857707038_u128, "11670309304772857707038"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "81489474363966142065392" }), 81489474363966142065392_u128, "81489474363966142065392"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "32007160093358686971493" }), 32007160093358686971493_u128, "32007160093358686971493"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "73242488230820464485822" }), 73242488230820464485822_u128, "73242488230820464485822"sv);
	std::cout << "End 23 digit decimal literal tests..." << newl;

	std::cout << "Begin 24 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x274365f29c5bca5979733fcd" }), 0x274365f29c5bca5979733fcd_u128, "0x274365f29c5bca5979733fcd"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xbf95d106473e4cc1fb8e1792" }), 0xbf95d106473e4cc1fb8e1792_u128, "0xbf95d106473e4cc1fb8e1792"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x55e6356d23a818a24b28f7e0" }), 0x55e6356d23a818a24b28f7e0_u128, "0x55e6356d23a818a24b28f7e0"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x4c8f88751c893fe07540fa0c" }), 0x4c8f88751c893fe07540fa0c_u128, "0x4c8f88751c893fe07540fa0c"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x48f404c60650432ec0b0c03c" }), 0x48f404c60650432ec0b0c03c_u128, "0x48f404c60650432ec0b0c03c"sv);
	std::cout << "End 24 digit hex literal tests..." << newl;

	std::cout << "Begin 24 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "409909085547757069186596" }), 409909085547757069186596_u128, "409909085547757069186596"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "741617182721104298875048" }), 741617182721104298875048_u128, "741617182721104298875048"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "834614413010799880980977" }), 834614413010799880980977_u128, "834614413010799880980977"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "547499797537311195932087" }), 547499797537311195932087_u128, "547499797537311195932087"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "372886308085665112660059" }), 372886308085665112660059_u128, "372886308085665112660059"sv);
	std::cout << "End 24 digit decimal literal tests..." << newl;

	std::cout << "Begin 25 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x1593c3eaecacdb487fa735899" }), 0x1593c3eaecacdb487fa735899_u128, "0x1593c3eaecacdb487fa735899"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xf81b77e0ab14f1b9f578b0e13" }), 0xf81b77e0ab14f1b9f578b0e13_u128, "0xf81b77e0ab14f1b9f578b0e13"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x35b13684b2ffdd8934beb0ea9" }), 0x35b13684b2ffdd8934beb0ea9_u128, "0x35b13684b2ffdd8934beb0ea9"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xec83361f3e70e900d4a901c21" }), 0xec83361f3e70e900d4a901c21_u128, "0xec83361f3e70e900d4a901c21"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x548cd44742a19ba1b76a5f950" }), 0x548cd44742a19ba1b76a5f950_u128, "0x548cd44742a19ba1b76a5f950"sv);
	std::cout << "End 25 digit hex literal tests..." << newl;

	std::cout << "Begin 25 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "9377254784523933187870136" }), 9377254784523933187870136_u128, "9377254784523933187870136"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "6114724258425701920202859" }), 6114724258425701920202859_u128, "6114724258425701920202859"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "6309012230012981936339199" }), 6309012230012981936339199_u128, "6309012230012981936339199"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "3203853433829122773687621" }), 3203853433829122773687621_u128, "3203853433829122773687621"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "5656085179642274678338685" }), 5656085179642274678338685_u128, "5656085179642274678338685"sv);
	std::cout << "End 25 digit decimal literal tests..." << newl;

	std::cout << "Begin 26 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x949a4e7b7185291ff551a28e9e" }), 0x949a4e7b7185291ff551a28e9e_u128, "0x949a4e7b7185291ff551a28e9e"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xe3489c9d225c8429d2be7e549f" }), 0xe3489c9d225c8429d2be7e549f_u128, "0xe3489c9d225c8429d2be7e549f"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x509af6f9d731c151962cf77999" }), 0x509af6f9d731c151962cf77999_u128, "0x509af6f9d731c151962cf77999"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xe7c01f7ae2703a2ce841b19de5" }), 0xe7c01f7ae2703a2ce841b19de5_u128, "0xe7c01f7ae2703a2ce841b19de5"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xe0d9ce9a58462b9b13a6b8553e" }), 0xe0d9ce9a58462b9b13a6b8553e_u128, "0xe0d9ce9a58462b9b13a6b8553e"sv);
	std::cout << "End 26 digit hex literal tests..." << newl;

	std::cout << "Begin 26 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "50487514300570755466971541" }), 50487514300570755466971541_u128, "50487514300570755466971541"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "55733877632973150917588851" }), 55733877632973150917588851_u128, "55733877632973150917588851"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "96127995512797197262990390" }), 96127995512797197262990390_u128, "96127995512797197262990390"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "79136460918641922068341211" }), 79136460918641922068341211_u128, "79136460918641922068341211"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "45788616200047977101412244" }), 45788616200047977101412244_u128, "45788616200047977101412244"sv);
	std::cout << "End 26 digit decimal literal tests..." << newl;

	std::cout << "Begin 27 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xaf06ebf00c6c2548340c0678030" }), 0xaf06ebf00c6c2548340c0678030_u128, "0xaf06ebf00c6c2548340c0678030"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x1b30093897e5399342a9ae7cf91" }), 0x1b30093897e5399342a9ae7cf91_u128, "0x1b30093897e5399342a9ae7cf91"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x95802dfeb7924100e8b7a5a42d3" }), 0x95802dfeb7924100e8b7a5a42d3_u128, "0x95802dfeb7924100e8b7a5a42d3"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xfe6b76c1607dab4b90573e70062" }), 0xfe6b76c1607dab4b90573e70062_u128, "0xfe6b76c1607dab4b90573e70062"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x9dec5ae788c3a21e6250720e8b6" }), 0x9dec5ae788c3a21e6250720e8b6_u128, "0x9dec5ae788c3a21e6250720e8b6"sv);
	std::cout << "End 27 digit hex literal tests..." << newl;

	std::cout << "Begin 27 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "555408221911463147108790008" }), 555408221911463147108790008_u128, "555408221911463147108790008"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "183159222826764875851354364" }), 183159222826764875851354364_u128, "183159222826764875851354364"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "513141717159055236365766172" }), 513141717159055236365766172_u128, "513141717159055236365766172"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "248062555063247312721930171" }), 248062555063247312721930171_u128, "248062555063247312721930171"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "580018905437049014429124525" }), 580018905437049014429124525_u128, "580018905437049014429124525"sv);
	std::cout << "End 27 digit decimal literal tests..." << newl;

	std::cout << "Begin 28 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xb96a2442bc12ef1657b006a1eaf4" }), 0xb96a2442bc12ef1657b006a1eaf4_u128, "0xb96a2442bc12ef1657b006a1eaf4"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x1b3776a74457a9a4014483712057" }), 0x1b3776a74457a9a4014483712057_u128, "0x1b3776a74457a9a4014483712057"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x57557b5e8078db716cc881f3668e" }), 0x57557b5e8078db716cc881f3668e_u128, "0x57557b5e8078db716cc881f3668e"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x31b8a288ffbba12ff4000176c41d" }), 0x31b8a288ffbba12ff4000176c41d_u128, "0x31b8a288ffbba12ff4000176c41d"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xcaf98e5fcbd659e035780ac39e5b" }), 0xcaf98e5fcbd659e035780ac39e5b_u128, "0xcaf98e5fcbd659e035780ac39e5b"sv);
	std::cout << "End 28 digit hex literal tests..." << newl;

	std::cout << "Begin 28 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "5340483565643130671055043100" }), 5340483565643130671055043100_u128, "5340483565643130671055043100"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "9872310643723015677233761160" }), 9872310643723015677233761160_u128, "9872310643723015677233761160"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "4691605661678649177686315717" }), 4691605661678649177686315717_u128, "4691605661678649177686315717"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "8901588280316220976092285548" }), 8901588280316220976092285548_u128, "8901588280316220976092285548"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "5034890168301328917736513614" }), 5034890168301328917736513614_u128, "5034890168301328917736513614"sv);
	std::cout << "End 28 digit decimal literal tests..." << newl;

	std::cout << "Begin 29 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x67ee962aa85c3f020349f0b0ff7cb" }), 0x67ee962aa85c3f020349f0b0ff7cb_u128, "0x67ee962aa85c3f020349f0b0ff7cb"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x3a7595cb10e64bb02713ff5dacdab" }), 0x3a7595cb10e64bb02713ff5dacdab_u128, "0x3a7595cb10e64bb02713ff5dacdab"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xa5df0354c11bb5461993e12e3309a" }), 0xa5df0354c11bb5461993e12e3309a_u128, "0xa5df0354c11bb5461993e12e3309a"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x1b2f96ad73e74d0ae74188818bf64" }), 0x1b2f96ad73e74d0ae74188818bf64_u128, "0x1b2f96ad73e74d0ae74188818bf64"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xded54fdef3a0ee9e13abaf309d89d" }), 0xded54fdef3a0ee9e13abaf309d89d_u128, "0xded54fdef3a0ee9e13abaf309d89d"sv);
	std::cout << "End 29 digit hex literal tests..." << newl;

	std::cout << "Begin 29 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "45352839094777993068243294868" }), 45352839094777993068243294868_u128, "45352839094777993068243294868"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "92219620551123172264403502557" }), 92219620551123172264403502557_u128, "92219620551123172264403502557"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "10223169707068171196643105625" }), 10223169707068171196643105625_u128, "10223169707068171196643105625"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "69881121214294085778538857590" }), 69881121214294085778538857590_u128, "69881121214294085778538857590"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "19306925075854883958151467032" }), 19306925075854883958151467032_u128, "19306925075854883958151467032"sv);
	std::cout << "End 29 digit decimal literal tests..." << newl;

	std::cout << "Begin 30 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x1166e3e6acc87bbde6e91259d70f66" }), 0x1166e3e6acc87bbde6e91259d70f66_u128, "0x1166e3e6acc87bbde6e91259d70f66"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xdd2d150f7598708f8ebdc249c7a785" }), 0xdd2d150f7598708f8ebdc249c7a785_u128, "0xdd2d150f7598708f8ebdc249c7a785"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xe2547c3cc4aa68a0f1ff98e8fd66cd" }), 0xe2547c3cc4aa68a0f1ff98e8fd66cd_u128, "0xe2547c3cc4aa68a0f1ff98e8fd66cd"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x2a64580b01c0b743765187b5e3c20d" }), 0x2a64580b01c0b743765187b5e3c20d_u128, "0x2a64580b01c0b743765187b5e3c20d"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x55eb0ab7f63d6c931662ce9e919e8a" }), 0x55eb0ab7f63d6c931662ce9e919e8a_u128, "0x55eb0ab7f63d6c931662ce9e919e8a"sv);
	std::cout << "End 30 digit hex literal tests..." << newl;

	std::cout << "Begin 30 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "687760243228157494055674669782" }), 687760243228157494055674669782_u128, "687760243228157494055674669782"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "802402324940527575873192691939" }), 802402324940527575873192691939_u128, "802402324940527575873192691939"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "310785433584936830421779872911" }), 310785433584936830421779872911_u128, "310785433584936830421779872911"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "520050801271555854950350829700" }), 520050801271555854950350829700_u128, "520050801271555854950350829700"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "701061752887740889722342317518" }), 701061752887740889722342317518_u128, "701061752887740889722342317518"sv);
	std::cout << "End 30 digit decimal literal tests..." << newl;

	std::cout << "Begin 31 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x79dafc551c3c8ea1fe598e668eb95c3" }), 0x79dafc551c3c8ea1fe598e668eb95c3_u128, "0x79dafc551c3c8ea1fe598e668eb95c3"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x1279116d454357dd1b55a70fa9c095d" }), 0x1279116d454357dd1b55a70fa9c095d_u128, "0x1279116d454357dd1b55a70fa9c095d"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xd53e9b3898de86ec53a518f9aa8deaa" }), 0xd53e9b3898de86ec53a518f9aa8deaa_u128, "0xd53e9b3898de86ec53a518f9aa8deaa"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x09e437573475fefd2849a09cf67f8f0" }), 0x09e437573475fefd2849a09cf67f8f0_u128, "0x09e437573475fefd2849a09cf67f8f0"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xbcefb3fc71f3dd03ab62a7f9e30b34c" }), 0xbcefb3fc71f3dd03ab62a7f9e30b34c_u128, "0xbcefb3fc71f3dd03ab62a7f9e30b34c"sv);
	std::cout << "End 31 digit hex literal tests..." << newl;

	std::cout << "Begin 31 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "3109445587782285157232850251609" }), 3109445587782285157232850251609_u128, "3109445587782285157232850251609"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "2487464038076863230619266272378" }), 2487464038076863230619266272378_u128, "2487464038076863230619266272378"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "2354660307095417264030667492032" }), 2354660307095417264030667492032_u128, "2354660307095417264030667492032"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "7779604928055265487605000602899" }), 7779604928055265487605000602899_u128, "7779604928055265487605000602899"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "8463572621800622535228310288732" }), 8463572621800622535228310288732_u128, "8463572621800622535228310288732"sv);
	std::cout << "End 31 digit decimal literal tests..." << newl;

	std::cout << "Begin 32 digit hex literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xe9451c875fe187ce0032d7b61b9d59e9" }), 0xe9451c875fe187ce0032d7b61b9d59e9_u128, "0xe9451c875fe187ce0032d7b61b9d59e9"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xd993e4a95cbe4a7c43a6a3d1ee517194" }), 0xd993e4a95cbe4a7c43a6a3d1ee517194_u128, "0xd993e4a95cbe4a7c43a6a3d1ee517194"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0x856675107792d9c749ba4dcb1d5ca2be" }), 0x856675107792d9c749ba4dcb1d5ca2be_u128, "0x856675107792d9c749ba4dcb1d5ca2be"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xf8a842cb247db35c442a471d402b3864" }), 0xf8a842cb247db35c442a471d402b3864_u128, "0xf8a842cb247db35c442a471d402b3864"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "0xf38daa5ae7818d02d227a2817440c35f" }), 0xf38daa5ae7818d02d227a2817440c35f_u128, "0xf38daa5ae7818d02d227a2817440c35f"sv);
	std::cout << "End 32 digit hex literal tests..." << newl;

	std::cout << "Begin 32 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "60109703877362215053664934490261" }), 60109703877362215053664934490261_u128, "60109703877362215053664934490261"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "96927727383453023442639088831597" }), 96927727383453023442639088831597_u128, "96927727383453023442639088831597"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "70628480265238664304453412784600" }), 70628480265238664304453412784600_u128, "70628480265238664304453412784600"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "74770308902922250807480071980540" }), 74770308902922250807480071980540_u128, "74770308902922250807480071980540"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "43030408543380403174155045611034" }), 43030408543380403174155045611034_u128, "43030408543380403174155045611034"sv);
	std::cout << "End 32 digit decimal literal tests..." << newl;

	std::cout << "Begin 33 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "500075789468993843055168989686367" }), 500075789468993843055168989686367_u128, "500075789468993843055168989686367"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "446206476790895848311038556056171" }), 446206476790895848311038556056171_u128, "446206476790895848311038556056171"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "573159404543773378202145359748632" }), 573159404543773378202145359748632_u128, "573159404543773378202145359748632"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "823684001911461174684834447465247" }), 823684001911461174684834447465247_u128, "823684001911461174684834447465247"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "569615932415041647476353610167923" }), 569615932415041647476353610167923_u128, "569615932415041647476353610167923"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "118259068484574665179568942496879" }), 118259068484574665179568942496879_u128, "118259068484574665179568942496879"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "545241246824506718576262116203466" }), 545241246824506718576262116203466_u128, "545241246824506718576262116203466"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "282331240550807462543706669044164" }), 282331240550807462543706669044164_u128, "282331240550807462543706669044164"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "616919910029163576668018919156945" }), 616919910029163576668018919156945_u128, "616919910029163576668018919156945"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "381413817121565341004042719227380" }), 381413817121565341004042719227380_u128, "381413817121565341004042719227380"sv);
	std::cout << "End 33 digit decimal literal tests..." << newl;

	std::cout << "Begin 34 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "3174927285700513083252925216322802" }), 3174927285700513083252925216322802_u128, "3174927285700513083252925216322802"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "9376018350915097336378554914914498" }), 9376018350915097336378554914914498_u128, "9376018350915097336378554914914498"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "3349603832525919887329855350720780" }), 3349603832525919887329855350720780_u128, "3349603832525919887329855350720780"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "1250686857782914607430165020216437" }), 1250686857782914607430165020216437_u128, "1250686857782914607430165020216437"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "4891887260233324754756207189383058" }), 4891887260233324754756207189383058_u128, "4891887260233324754756207189383058"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "9242550014755908563284396694427685" }), 9242550014755908563284396694427685_u128, "9242550014755908563284396694427685"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "2151851770044358964653204273523729" }), 2151851770044358964653204273523729_u128, "2151851770044358964653204273523729"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "1608431238830376902779085144221593" }), 1608431238830376902779085144221593_u128, "1608431238830376902779085144221593"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "5850813900654754340991822037321402" }), 5850813900654754340991822037321402_u128, "5850813900654754340991822037321402"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "4633784172695947409659918432138873" }), 4633784172695947409659918432138873_u128, "4633784172695947409659918432138873"sv);
	std::cout << "End 34 digit decimal literal tests..." << newl;

	std::cout << "Begin 35 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "17259654675846973731563272432212094" }), 17259654675846973731563272432212094_u128, "17259654675846973731563272432212094"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "96621553725731346202959931798482265" }), 96621553725731346202959931798482265_u128, "96621553725731346202959931798482265"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "60648495012063071058753811029343264" }), 60648495012063071058753811029343264_u128, "60648495012063071058753811029343264"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "28186616501286457363379614403423332" }), 28186616501286457363379614403423332_u128, "28186616501286457363379614403423332"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "25336033649800727352919738809363634" }), 25336033649800727352919738809363634_u128, "25336033649800727352919738809363634"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "39851929081049364142174253928446694" }), 39851929081049364142174253928446694_u128, "39851929081049364142174253928446694"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "57533072483257715095145752189746395" }), 57533072483257715095145752189746395_u128, "57533072483257715095145752189746395"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "95097923203643201030160438962716452" }), 95097923203643201030160438962716452_u128, "95097923203643201030160438962716452"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "18338353642732329850883637657114044" }), 18338353642732329850883637657114044_u128, "18338353642732329850883637657114044"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "19952022127765064952040784919482782" }), 19952022127765064952040784919482782_u128, "19952022127765064952040784919482782"sv);
	std::cout << "End 35 digit decimal literal tests..." << newl;

	std::cout << "Begin 36 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "226072537861963855027010324332781406" }), 226072537861963855027010324332781406_u128, "226072537861963855027010324332781406"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "892029965184704636224052951647163529" }), 892029965184704636224052951647163529_u128, "892029965184704636224052951647163529"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "505673735313684392579935793301913539" }), 505673735313684392579935793301913539_u128, "505673735313684392579935793301913539"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "852531051498785049642032510012369426" }), 852531051498785049642032510012369426_u128, "852531051498785049642032510012369426"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "821267236179651980961925741433750679" }), 821267236179651980961925741433750679_u128, "821267236179651980961925741433750679"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "371552993287024628434862733340195249" }), 371552993287024628434862733340195249_u128, "371552993287024628434862733340195249"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "801158514882557959287003297416326343" }), 801158514882557959287003297416326343_u128, "801158514882557959287003297416326343"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "196691147152783028117808547927092806" }), 196691147152783028117808547927092806_u128, "196691147152783028117808547927092806"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "776666083014738948301546796265421238" }), 776666083014738948301546796265421238_u128, "776666083014738948301546796265421238"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "577915043121580633598737755095983346" }), 577915043121580633598737755095983346_u128, "577915043121580633598737755095983346"sv);
	std::cout << "End 36 digit decimal literal tests..." << newl;

	std::cout << "Begin 37 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "9768304718275935340386999471767116696" }), 9768304718275935340386999471767116696_u128, "9768304718275935340386999471767116696"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "4182401100296040938330311828815699810" }), 4182401100296040938330311828815699810_u128, "4182401100296040938330311828815699810"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "4333742971602265085511171822078019656" }), 4333742971602265085511171822078019656_u128, "4333742971602265085511171822078019656"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "8935715850682405531359155518074211874" }), 8935715850682405531359155518074211874_u128, "8935715850682405531359155518074211874"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "5301955007834006301936359651525353174" }), 5301955007834006301936359651525353174_u128, "5301955007834006301936359651525353174"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "1227032796209116561768746175437346826" }), 1227032796209116561768746175437346826_u128, "1227032796209116561768746175437346826"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "4834436143931196892542425273166652907" }), 4834436143931196892542425273166652907_u128, "4834436143931196892542425273166652907"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "7291071613207749671121409368273313924" }), 7291071613207749671121409368273313924_u128, "7291071613207749671121409368273313924"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "9733017147267909447007957640081150462" }), 9733017147267909447007957640081150462_u128, "9733017147267909447007957640081150462"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "5570081610655235492714591004576354875" }), 5570081610655235492714591004576354875_u128, "5570081610655235492714591004576354875"sv);
	std::cout << "End 37 digit decimal literal tests..." << newl;

	std::cout << "Begin 38 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "93601647113226868556580660006490889575" }), 93601647113226868556580660006490889575_u128, "93601647113226868556580660006490889575"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "53690935288163170969117411637211594848" }), 53690935288163170969117411637211594848_u128, "53690935288163170969117411637211594848"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "32063254837094168053787400753965539765" }), 32063254837094168053787400753965539765_u128, "32063254837094168053787400753965539765"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "94716394119831515896145274336994168831" }), 94716394119831515896145274336994168831_u128, "94716394119831515896145274336994168831"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "76641046753894786667368464239263961969" }), 76641046753894786667368464239263961969_u128, "76641046753894786667368464239263961969"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "95463555671512740784818635027568504866" }), 95463555671512740784818635027568504866_u128, "95463555671512740784818635027568504866"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "33641334618771284400609310900330412182" }), 33641334618771284400609310900330412182_u128, "33641334618771284400609310900330412182"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "83224560689407622794553953507784305729" }), 83224560689407622794553953507784305729_u128, "83224560689407622794553953507784305729"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "10090718434292515600637655101522700108" }), 10090718434292515600637655101522700108_u128, "10090718434292515600637655101522700108"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "66009911210991920739378434906987259386" }), 66009911210991920739378434906987259386_u128, "66009911210991920739378434906987259386"sv);
	std::cout << "End 38 digit decimal literal tests..." << newl;

	std::cout << "Begin 39 digit decimal literal tests..." << newl;
	cjm_assert_equal(to_test(ctrl_uint128_t{ "340185592574115266867233957625061256432" }), 340185592574115266867233957625061256432_u128, "340185592574115266867233957625061256432"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "201761860872477485139816079489830737715" }), 201761860872477485139816079489830737715_u128, "201761860872477485139816079489830737715"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "241768129843747398380848007426742685294" }), 241768129843747398380848007426742685294_u128, "241768129843747398380848007426742685294"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "287850505696004090572896267781238024894" }), 287850505696004090572896267781238024894_u128, "287850505696004090572896267781238024894"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "321150292922701869819467303347057211560" }), 321150292922701869819467303347057211560_u128, "321150292922701869819467303347057211560"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "314653445313711070343340791323542112050" }), 314653445313711070343340791323542112050_u128, "314653445313711070343340791323542112050"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "258416517439837936199583456012849773064" }), 258416517439837936199583456012849773064_u128, "258416517439837936199583456012849773064"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "246328830188481326098015363584017882931" }), 246328830188481326098015363584017882931_u128, "246328830188481326098015363584017882931"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "115189086700490636927931486807940316249" }), 115189086700490636927931486807940316249_u128, "115189086700490636927931486807940316249"sv);
	cjm_assert_equal(to_test(ctrl_uint128_t{ "195680483610558517134018811288019513635" }), 195680483610558517134018811288019513635_u128, "195680483610558517134018811288019513635"sv);
	std::cout << "End 39 digit decimal literal tests..." << newl;
} 

std::basic_ostream<char>& cjm::uint128_tests::operator<<(std::basic_ostream<char>& os, lit_type v)
{
	using namespace std::literals::string_view_literals;
	switch (v)
	{
	default:
	case lit_type::Illegal:
		os << "Illegal"sv;
		break;
	case lit_type::Decimal:
		os << "Decimal"sv;
		break;
	case lit_type::Hexadecimal:
		os << "Hexadecimal"sv;
		break;
	case lit_type::Zero:
		os << "Zero"sv;
		break;
	}
	return os;
}

std::pair<cjm::uint128_tests::ctrl_uint128_t, std::string> cjm::uint128_tests::create_random_dec_n_digits_long(
	size_t decimal_digits, generator::rgen& rgen)
{
	constexpr size_t max_dec_digits = std::numeric_limits<ctrl_uint128_t>::digits10 + 1u;
	if (decimal_digits == 0 || decimal_digits > max_dec_digits)
	{
		auto strm = string::make_throwing_sstream<char>();
		strm
			<< "The parameter [hex_digits] must be greater than zero"
			<< " and no greater than [" << max_dec_digits << "].  "
			<< "Actual value: [" << decimal_digits << "]." << newl;
		throw std::invalid_argument{ strm.str() };
	}

	constexpr auto& max = lit_helper::max_decimal_digits_v<ctrl_uint128_t>;
	auto char_array = std::string{};
	char_array.reserve(decimal_digits);
	size_t max_char_idx = 0;
	
	bool can_ignore_max_char = decimal_digits < max_dec_digits;

	ctrl_uint128_t result = 0;
	ctrl_uint128_t current_digit = 1;
	constexpr ctrl_uint128_t base_factor = 10u;
	while (char_array.size() < decimal_digits)
	{
		const bool not_zero = char_array.empty();
		char random;
		if (can_ignore_max_char)
		{
			do
			{
				random = rgen.random_decimal_digit();
			} while (not_zero && random == '0');
		}
		else
		{
			const char max_char = max.at(max_char_idx);
			do
			{
				random = max_char == '0' ? '0' : rgen.random_decimal_digit(max_char);
			} while (not_zero && random == '0');
			assert(random <= max.at(max_char_idx));
			can_ignore_max_char = random < max.at(max_char_idx);
			++max_char_idx;
		}
		char_array.push_back(random);
	}
	constexpr auto& arr = lit_helper::digit_lookup_v<lit_type::Decimal>;
	
	for (auto it = char_array.crbegin(); it != char_array.crend(); ++it)
	{
		char c = *it;
		const auto idx = static_cast<unsigned>(*reinterpret_cast<unsigned char*>(&c)) % 0x100u;
		unsigned value = arr.at(idx).value();
		result += (value * current_digit);
		current_digit *= base_factor;
		
	}	
	return std::make_pair(result, char_array);
}

std::pair<cjm::uint128_tests::ctrl_uint128_t, std::string> cjm::uint128_tests::
	create_random_hex_n_digits_long(size_t hex_digits, generator::rgen& rgen)
{
	constexpr size_t max_hex_digits = std::numeric_limits<ctrl_uint128_t>::digits / 4u;
	if (hex_digits == 0 || hex_digits > max_hex_digits)
	{
		auto strm = string::make_throwing_sstream<char>();
		strm
			<< "The parameter [hex_digits] must be greater than zero"
			<< " and no greater than [" << max_hex_digits << "].  "
			<< "Actual value: [" << hex_digits << "]." << newl;
		throw std::invalid_argument{ strm.str() };
	}
		
	ctrl_uint128_t result = 0;
	ctrl_uint128_t current_digit = 1;
	constexpr ctrl_uint128_t base_factor = 0x10;
	auto vec = std::vector<char>{ };
	vec.reserve(hex_digits);
	while (vec.size() < hex_digits)
	{
		const char c = rgen.random_hex_digit();
		unsigned value = lit_helper::digit_lookup_v<lit_type::Hexadecimal>[static_cast<unsigned>(static_cast<unsigned char>(c)) % 0x0100u].value();
		result += (value * current_digit);
		current_digit *= base_factor;
		vec.push_back(c);
	}
	auto strm = string::make_throwing_sstream<char>();
	strm << "0x";
	for (auto it = vec.crbegin(); it != vec.crend(); ++it)
	{
		strm << *it;
	}
	return std::make_pair(result, strm.str());
}


std::string cjm::uint128_tests::generate_literal_test(lit_type literal_type, size_t num_digits, generator::rgen& gen)
{
	//ctrl_uint128_t
	if (literal_type != lit_type::Decimal && literal_type != lit_type::Hexadecimal)
	{
		auto strm = string::make_throwing_sstream<char>();
		strm << "Only hexadecimal and decimal literals supported.  Specified literal type: [" << literal_type << "]." << newl;
		throw std::invalid_argument{ strm.str() };
	}
	if (num_digits == 0)
	{
		throw std::invalid_argument{ "At least one digit must be specified." };
	}
	if (literal_type == lit_type::Hexadecimal)
	{
		const auto&& [value, value_text] = create_random_hex_n_digits_long(num_digits, gen);
		auto strm = string::make_throwing_sstream<char>();
		strm << std::hex;
		strm
			<< "\tcjm_assert_equal(to_test(ctrl_uint128_t{\"" << value_text << "\"}), "
			<< value_text << "_u128, \"" << value_text << "\"sv);\n";
		return strm.str();
	}
	const auto&& [value, value_text] = create_random_dec_n_digits_long(num_digits, gen);
	auto strm = string::make_throwing_sstream<char>();
	strm
		<< "\tcjm_assert_equal(to_test(ctrl_uint128_t{\"" << value_text << "\"}), "
		<< std::dec << value_text << "_u128, \"" << value_text << "\"sv);\n";
	return strm.str();
}

std::vector<std::string> cjm::uint128_tests::generate_literal_tests()
{
	constexpr auto arr = init_num_tests_arr<ctrl_uint128_t, true>();
	const size_t total_tests = std::accumulate(arr.cbegin(), arr.cend(), 0_szt);
	constexpr size_t max_hex_digits = std::numeric_limits<ctrl_uint128_t>::digits / 4u;
	assert(total_tests > 0);
	auto vec = std::vector<std::string>{};
	vec.reserve(total_tests + 150);
	constexpr size_t max_digits = std::numeric_limits<ctrl_uint128_t>::digits10 + 1u;
	using namespace std::string_literals;
	using namespace cjm::numerics::uint128_literals;
	using namespace boost::multiprecision::literals;
	
	vec.emplace_back("\tstd::cout << \"Beginning single digit hex literal tests...\";\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0x0}), 0x0_u128, \"0x0\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0x1}), 0x1_u128, \"0x1\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0x2}), 0x2_u128, \"0x2\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0x3}), 0x3_u128, \"0x3\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0x4}), 0x4_u128, \"0x4\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0x5}), 0x5_u128, \"0x5\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0x6}), 0x6_u128, \"0x6\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0x7}), 0x7_u128, \"0x7\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0x8}), 0x8_u128, \"0x8\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0x9}), 0x9_u128, \"0x9\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0xa}), 0xa_u128, \"0xa\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0xb}), 0xb_u128, \"0xb\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0xc}), 0xc_u128, \"0xc\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0xd}), 0xd_u128, \"0xd\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0xe}), 0xe_u128, \"0xe\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0xf}), 0xf_u128, \"0xf\"sv);\n");
	vec.emplace_back("\tstd::cout << \"End single digit hex literal tests...\";\n\n");

	vec.emplace_back("\tstd::cout << \"Begin single digit decimal literal tests...\";\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{0), 0_u128, \"0\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{1}), 1_u128, \"1\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{2}), 2_u128, \"2\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{3}), 3_u128, \"3\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{4}), 4_u128, \"4\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{5}), 5_u128, \"5\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{6}), 6_u128, \"6\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{7}), 7_u128, \"7\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{8}), 8_u128, \"8\"sv);\n");
	vec.emplace_back("\tcjm_assert_equal(to_test(ctrl_uint128_t{9}), 9_u128, \"9\"sv);\n");
	vec.emplace_back("\tstd::cout << \"End single digit decimal literal tests...\";\n\n");

	auto get_entry_exit_line = [](size_t num, bool entry, bool hex) -> std::string
	{
		std::string_view hex_or_dec = hex ? "hex"sv : "decimal"sv;
		std::string_view begin_end = entry ? "Begin"sv : "End"sv;
		auto strm = string::make_throwing_sstream<char>();
		strm << "\tstd::cout << \"" << begin_end << " " << num << " digit " << hex_or_dec << " literal tests...\" << newl;\n";
		return strm.str();
	};
	auto rgen = generator::rgen{};
	
	for (size_t i = 2; i < arr.size() && i <= max_hex_digits; ++i)
	{
		vec.emplace_back(get_entry_exit_line(i, true, true));
		const auto total_num_tests = arr[i];
		const auto total_each_hex_dec_set_tests = total_num_tests / 2;
		size_t tests_so_far = 0;
		do
		{
			vec.emplace_back(generate_literal_test(lit_type::Hexadecimal, i, rgen));
			++tests_so_far;
		} while (tests_so_far < total_each_hex_dec_set_tests);
		vec.emplace_back(get_entry_exit_line(i, false, true) + "\n"s);

		vec.emplace_back(get_entry_exit_line(i, true, false));
		do
		{
			vec.emplace_back(generate_literal_test(lit_type::Decimal, i, rgen));
			++tests_so_far;
		} while (tests_so_far < total_num_tests);
		vec.emplace_back(get_entry_exit_line(i, false, false) + "\n"s);		
	}
	for (size_t i = max_hex_digits + 1; i <= max_digits; ++i )
	{
		vec.emplace_back(get_entry_exit_line(i, true, false));
		const auto total_num_tests = arr[i];
		size_t tests_so_far = 0;
		do
		{
			vec.emplace_back(generate_literal_test(lit_type::Decimal, i, rgen));
			++tests_so_far;
		} while (tests_so_far < total_num_tests);
		vec.emplace_back(get_entry_exit_line(i, false, false) + "\n"s);
	}
	return vec;
}

void cjm::uint128_tests::generate_then_print_literal_tests()
{
	std::cout << "Generating literal tests." << newl;
	auto vec = generate_literal_tests();
	std::cout << "Literal tests generated and will be printed ...." << newl;

	for (const auto& str : vec)
	{
		std::cout << str;
	}
	std::cout << newl << newl << "Done printing generated literal tests." << newl; 
}


#ifdef CJM_HAVE_BUILTIN_128
void cjm::uint128_tests::execute_builtin_u128fls_test_if_avail()
{
	static_assert(cjm::numerics::concepts::can_find_most_significant_set_bitpos<unsigned __int128>);
	unsigned __int128 x = 3;
	auto most_sign_bit = cjm::numerics::most_sign_set_bit(x);
	std::cout << "Test for 3 yields bitpos: [" << most_sign_bit << "]." << newl;
	cjm_assert(most_sign_bit == 1);
	x <<= 120;
	most_sign_bit = cjm::numerics::most_sign_set_bit(x);
	std::cout << "Test for (3 << 120) yields: [" << most_sign_bit << "]." << newl;
	cjm_assert(most_sign_bit == 121);

}
#else
void cjm::uint128_tests::execute_builtin_u128fls_test_if_avail()
{
	std::cout << "Will not test builtin_u128_fls because not available in this environment." << newl;
}


#endif
