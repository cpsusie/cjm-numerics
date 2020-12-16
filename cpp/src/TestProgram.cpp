#include <iostream>
#include "testing.hpp"
#include"streams.hpp"
#include <string>
#include <iomanip>
#include "uint128.hpp"
#include "fixed_uint.hpp"
//#include "numerics.hpp"
//#include "spatial_measurement.hpp"
#include <exception>
#include "rational.hpp"
#include "cjm_string.hpp"
#include <deque>
#include "boost_uint_compare_test.hpp"
#include <fstream>
#include <sstream>
namespace
{
	std::ostream& cout = cjm::streams::cout;
	std::ostream& cerr = cjm::streams::cerr;
	constexpr auto newl = cjm::streams::newl;
	using uint128_t = boost::multiprecision::uint128_t;
	using uint128 = cjm::numerics::uint128;
	using decimal = cjm::numerics::norm_decimal;
	
	
	uint128_t get_bigass_int();
	uint128_t get_bigass_int()
	{
		using namespace boost::multiprecision::literals;
		uint128_t source = 0x000056789ABCDEF0_cppui128;
	    uint128_t temp = (source << 64) | source;
		return temp;		
	}	
}

namespace test_program
{
#ifdef _DEBUG
	constexpr static size_t NUMBER_BINARY_OP256_TESTS = 1'000;
#else
	constexpr static size_t NUMBER_BINARY_OP256_TESTS = 1'000'000;
#endif
	using cjm::testing::cjm_assert;
	using cjm::testing::cjm_assert_throws;
	using cjm::testing::cjm_deny;

	cjm::testing::binary_u256_operation_col_t execute_u256_random_bin_op_tests(size_t numTests);

	void do_u256_big_little_endian_array_test();
	void do_string_round_trip_tests();
	void do_string_round_trip_test(const cjm::numerics::uint256_t& testMe, bool hex = false);
	void do_u256_random_bin_op_test();

	void do_256_bit_lshift_test();
	void do_binary_random_op_test_u256();
	void do_random_boost_uint_div_128_test();
	void do_random_boost_uint_div_test();

	void do_random_boost_uint_mult_test();

	void do_measure_test();

	void do_comparisons_test_length();

	void do_simple_multiplication_test();

	/*void do_shift_too_much_test();

	void do_mult_div_no_compile_test();*/

	void do_simple_division_tests();

	void do_multi_dim_add_sub_tests();

	void do_div_tests();

	void do_bin_fl_tests();

	void do_threw_wrong_thing_test();

	void do_google_int_tests();

	void do_boost_literal_test();

	std::string convert_to_dec_string(std::uint64_t x);
	
	void do_little_endian_array_tests();

	void do_boost_uint_comparison_tests();

	void do_comparative_hex_modulus_tests();
	
	void do_divide_by_16_test();
	
	void do_boost_uint_pair_gen_tests();

	void do_wacky_subtraction_test();

	void perform_comparison_u256_tests(size_t numTests);

	void perform_unary_op_u256_tests(size_t numTests);

	void do_u256_literal_printing_test();

	void do_instrumented_uint128_div_mod(std::basic_ostream<char>& stream, 
		uint128 dividend, uint128 divisor);
	void do_instrumented_uint256_div_mod(std::basic_ostream<char>& stream, 
		const cjm::numerics::uint256_t& dividend, const cjm::numerics::uint256_t& divisor);
	void do_instrumented_uint256_div_mod_test_1();
	void do_instrumented_uint128_div_mod_test_1();
	void do_intrumented_uint128_div_mod_test_by16();
	void do_intrumented_uint256_div_mod_test_by16_1();
	void do_fixed_uint_from_u32str_test();
	void do_fixed_uint_from_u16str_test();
	void do_fixed_uint_from_wstr_test();
	void do_fixed_uint_from_str_test();
	void do_fixed_uint_test();
	void do_fixed_uint_out_test();
	void do_floor_log2_tests();
	//void do_rational_test();
	//void do_rational_test_2();
	//void do_rational_add_test();
	//void do_rational_mult_test();
	//void do_rational_truncation_test();
	//void do_rational_floor_test();
	//void do_rational_ceiling_test();
	//void do_rational_round_test();
	//void do_rational_pow_test();
	//void do_rational_to_float_test();
	//void do_rational_conversion_test();
	//void do_rational_print_float_test();
	//void do_float_comparer_test();
	//void do_find_smallest_safe_test();
	//void do_rational_hash_test();
	//void do_safe_comparison_test();
	//void do_rational_safe_mult_test();

	void do_trim_tests();

	//void do_static_uint256_basic_tests();

	/*void do_static_rational_literals_test();
	void do_static_rational_test();
	void do_static_rational_add_test();
	void do_static_rational_mult_test();
	void do_static_rational_truncation_test();
	void do_static_rational_floor_test();
	void do_static_rational_ceiling_test();
	void do_static_rational_round_test();
	void do_static_rational_type_query_integer_test();
	void do_static_rational_abs_test();
	void do_static_rational_whole_and_frac_test();
	void do_static_rational_pow_test();
	void do_static_256_lit_test();
	void do_dynamic_256_lit_test();
	void do_static_rational_conversion_test();*/

	template<typename T>
	std::string get_rel(T first, T second)
	{
		if (first == second)
		{
			return "First == Second";
		}
		if (first < second)
		{
			return "First < Second";
		}
		return "First > Second";
	}

	void do_little_endian_array_tests()
	{
		using uint256_t = cjm::numerics::uint256_t;
		using namespace cjm::numerics::fixed_uint_literals;
		using byte = unsigned char;
		static constexpr size_t arr_size = sizeof(uint256_t);
		using arr_t = std::array<byte, arr_size>;
		static constexpr uint256_t expected = 0x12'34'56'78'9a'bc'de'f0'12'34'56'78'9a'bc'de'f0'12'34'56'78'9a'bc'de'f0'12'34'56'78'9a'bc'de'f0_u256;
		static constexpr arr_t array = { 0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12, 0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12, 0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12, 0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12 };
		static_assert(array.size() == arr_size);

		auto fromArr = uint256_t::make_from_bytes_little_endian(array);
		cjm_assert(fromArr == expected);
		arr_t fromNum = fromArr.to_little_endian_arr();
		cjm_assert(fromNum == array);
		cout << "do_little_endian_array_tests PASS" << newl;
	}

	cjm::testing::binary_u256_operation_col_t execute_u256_random_bin_op_tests(size_t numTests)
	{
		auto testCol = cjm::testing::get_binary_op_u256_collection(numTests);
		return execute_test(testCol);
	}

	void do_u256_big_little_endian_array_test()
	{
		using byte = cjm::testing::byte;
		auto randomU256 = cjm::testing::random_u256();
		auto bigEArray = randomU256.to_big_endian_arr();
		auto littleEArray = randomU256.to_little_endian_arr();
		size_t bigIndex = littleEArray.size() - 1;
		cjm_assert(littleEArray.size() == bigEArray.size());
		for (size_t index=0; index < littleEArray.size(); ++index)
		{
			byte littleByte = littleEArray[index];
			byte bigByte = bigEArray[bigIndex--];
			cjm_assert(littleByte == bigByte);
		}
		cout << "do_u256_big_little_endian_array_test PASSED" << newl;
	}

	void do_string_round_trip_tests()
	{
		using cjm::numerics::uint256_t;
		using namespace cjm::numerics::fixed_uint_literals;
		do_string_round_trip_test(0x1234'5678'9abc'def0'1234'5678'9abc'def0'1234'5678'9abc'def0'1234'5678'9abc'def0_u256);
		do_string_round_trip_test(std::numeric_limits<uint256_t>::max());
		do_string_round_trip_test(0);
		do_string_round_trip_test(1);
		do_string_round_trip_test(0x1234'5678'9abc'def0'1234'5678'9abc'def0'1234'5678'9abc'def0'1234'5678'9abc'def0_u256, true);
		do_string_round_trip_test(std::numeric_limits<uint256_t>::max(), true);
		do_string_round_trip_test(0, true);
		do_string_round_trip_test(1, true);
		cout << "do_string_round_trip_tests PASSED." << newl;
	}

	void do_string_round_trip_test(const cjm::numerics::uint256_t& testMe, bool hex)
	{
		using std::stringstream;
		using std::wstringstream;
		using std::string;
		using std::wstring;
		using std::u16string;
		using std::u32string;
		using u16stringstream = std::basic_stringstream<char16_t>;
		using u32stringstream = std::basic_stringstream<char32_t>;
		using cjm::numerics::uint256_t;
		
		stringstream ss;
		wstringstream wss;
		u16stringstream u16ss;
		u32stringstream u32ss;
		if (hex)
		{
			ss << std::hex << "0x" << testMe;
			wss << std::hex << L"0X" << testMe;
			u16ss << std::hex << u"0x" << testMe;
			u32ss << std::hex << U"0X" << testMe;
		}
		else
		{
			ss << std::dec << testMe;
			wss << std::dec << testMe;
			u16ss << std::dec << testMe;
			u32ss << std::dec << testMe;
		}
		

		string str = ss.str();
		wstring wstr = wss.str();
		u16string u16str = u16ss.str();
		u32string u32str = u32ss.str();

		uint256_t result = uint256_t::make_from_string(str);
		cjm_assert(result == testMe);
		result = uint256_t::make_from_string(wstr);
		cjm_assert(result == testMe);
		result = uint256_t::make_from_string(u16str);
		cjm_assert(result == testMe);
		result = uint256_t::make_from_string(u32str);
		cjm_assert(result == testMe);		
	}

	void do_u256_random_bin_op_test()
	{
		constexpr static size_t numTests = NUMBER_BINARY_OP256_TESTS;
		using res_t = decltype(execute_u256_random_bin_op_tests(numTests));
		res_t failingResults = execute_u256_random_bin_op_tests(numTests);
		const auto failurePrinter = [](std::basic_ostream<char>& str, const res_t& res)
		{
			str << "Of " << numTests << " tests, " << res.size() << " tests failed." << newl;
			std::unordered_map<std::string, size_t> lookup;
			for (auto itm : res)
			{
				const auto opType = itm.operation_name();
				auto it = lookup.find(opType);
				if (it == lookup.end())
				{
					lookup[opType] = 1;
				}
				else
				{
					it->second += 1;
				}
			}

			for (const auto kvp : lookup)
			{
				str << "For operation named: " << kvp.first << " there were " << kvp.second << " failures." << newl;
			}
		};
		try
		{
			cjm_assert(failingResults.empty());
		}
		catch (const cjm::testing::testing_failure&)
		{
			failurePrinter(cerr, failingResults);
			throw;
		}
		
		cout << "do_u256_random_bin_op_test PASSED" << newl;
	}

	void do_256_bit_lshift_test()
	{
		using cjm128 = cjm::testing::test_u128;
		using boost256 = cjm::testing::ctrtl_u256;
		using cjm256 = cjm::testing::test_u256;
		using namespace cjm::testing;
		using namespace cjm::numerics::fixed_uint_literals;
		using namespace boost_cjm_uint_comparison;

		constexpr static auto sixteen = 0x10_u256;
		constexpr static auto sixteenControl = 0x10_cppui256;

		constexpr static auto oneTwenty = 0x78_u256;
		constexpr static auto oneTwentyControl = 0x78u;

		constexpr static auto lilTestSixteen = 0x10_u128;
		constexpr static auto lilTestOneTwenty = 0x78_u128;

		constexpr static auto subtrahend =  0x1234'5678'9abc'def0'1234'5678'9abc'def0_u256;
		constexpr static cjm128 lilSubtrahend = static_cast<cjm128>(subtrahend);

		constexpr static auto denominator = 0x1000'0000'0000'0000'0000'0000'0000'0000_u256;
		constexpr static cjm128 lilDenominator = static_cast<cjm128>(denominator);
		
		constexpr static cjm128 expectedLilDiff = 0x23456789abcdef0123456789abcdef0_u128;
		constexpr static cjm256 expectedBigDiff = expectedLilDiff;

		auto bigTempSubtrahend = subtrahend;
		auto lilTempSubtrahend = lilSubtrahend;
		
		static_assert(subtrahend == lilSubtrahend);
		cjm_assert(denominator == lilDenominator);
		cjm_assert(subtrahend == lilSubtrahend);
		cjm_assert(bigTempSubtrahend == lilTempSubtrahend);

		cjm_assert(bigTempSubtrahend > denominator);
		cjm_assert(lilTempSubtrahend > lilDenominator);

		bigTempSubtrahend -= denominator;
		lilTempSubtrahend -= lilDenominator;
		cjm_assert(bigTempSubtrahend == expectedBigDiff);
		cjm_assert(lilTempSubtrahend == expectedLilDiff);
		cjm_assert(bigTempSubtrahend == lilTempSubtrahend);
		

		constexpr static auto lilControl = lilTestSixteen << lilTestOneTwenty;
		constexpr int oneTwentyStandard = 0x78;
		cjm256 lilControlPromoted = lilControl;
		cout << "lilControl 16 lshift 120 := 0x" << lilControl << newl;
		auto test = sixteen << oneTwenty;
		cout << "test 16 lshift 120 := 0x" << test.tostring() << newl;
		auto test2 = sixteen;
		test2 <<= oneTwenty;
		cout << "test2 16 lshift 120 := 0x" << test2.tostring() << newl;
		cjm_assert(test2 == test);
		test2 = sixteen;
		test2 <<= oneTwentyStandard;
		cout << "test2 16 lshift 120 := 0x" << test2.tostring() << newl;
		cjm_assert(test2 == test);

		auto control = sixteenControl << oneTwentyControl;
		cjm_assert(lilControlPromoted == test);

		cjm_assert(test == control);
		cout << "do_256_bit_lshift_test PASS" << newl;
	}

	void do_binary_random_op_test_u256()
	{
		using namespace cjm::testing;
		auto binaryOp = get_binary_op_for_u256();
		cjm_assert(binaryOp());
		cout << "do_binary_random_op_test_u256 PASSED" << newl;
	}

	void do_random_boost_uint_div_128_test()
	{
		using boost128 = cjm::testing::ctrtl_u128;
		using cjm128 = cjm::testing::test_u128;
		using namespace cjm::testing;
		using namespace boost_cjm_uint_comparison;

		auto dividendPair = create_random_128_test_pair();
		auto divisorPair = create_random_128_test_pair();
		cjm_assert(dividendPair.first == dividendPair.second && divisorPair.first == divisorPair.second);

		boost128 controlQuotient = dividendPair.first / divisorPair.first;
		boost128 constrolRemainder = dividendPair.first % divisorPair.first;

		cjm128 testQuotient = dividendPair.second / divisorPair.second;
		cjm128 testRemainer = dividendPair.second % divisorPair.second;

		cout << "Control quotient: " << "0x" << controlQuotient << newl;
		cout << "Control remainder: " << "0x" << constrolRemainder << newl;
		cout << "Test quotient: " << "0x" << testQuotient << newl;
		cout << "Test remainder: " << "0x" << testRemainer << newl;
		cout << std::dec;

		cjm_assert(testQuotient == controlQuotient && testRemainer == constrolRemainder);
		cout << "do_rand_boost_uint_div_128_test PASSES" << newl;


	}
	void do_random_boost_uint_div_test()
	{
		using boost256 = cjm::testing::ctrtl_u256;
		using cjm256 = cjm::testing::test_u256;
		using namespace cjm::testing;
		using namespace boost_cjm_uint_comparison;

		auto dividendPair = create_random_test_pair();
		auto divisorPair = create_random_test_pair();
		cjm_assert(dividendPair.first == dividendPair.second && divisorPair.first == divisorPair.second);
		cout << "dividendPair: " << std::hex << "0x" << dividendPair.first << newl;
		cout << "divisorPair: " << "0x" << divisorPair.first << newl;
		boost256 controlQuotient = dividendPair.first / divisorPair.first;
		boost256 constrolRemainder = dividendPair.first % divisorPair.first;

		cjm256 testQuotient = dividendPair.second / divisorPair.second;
		cjm256 testRemainer = dividendPair.second % divisorPair.second;

		cout << "Control quotient: " << "0x" << controlQuotient << newl;
		cout << "Control remainder: " << "0x" << constrolRemainder << newl;
				
		cout << std::dec;
		
		cjm_assert(testQuotient == controlQuotient && testRemainer == constrolRemainder);
		cout << "do_rand_boost_uint_div_test PASSES" << newl;

	}

	void do_random_boost_uint_mult_test()
	{
		using boost256 = cjm::testing::ctrtl_u256;
		using cjm256 = cjm::testing::test_u256;
		using namespace cjm::testing;
		using namespace boost_cjm_uint_comparison;

		auto firstFactorPair = create_random_test_pair();
		auto secondFactorPair = create_random_test_pair();
		cjm_assert(firstFactorPair.first == firstFactorPair.second && secondFactorPair.first == secondFactorPair.second);
		cout << "firstFactorPair: " << std::hex << "0x" << firstFactorPair.first << newl;
		cout << "secondFactorPair: " << "0x" << secondFactorPair.first << newl;

		boost256 controlProduct = firstFactorPair.first * secondFactorPair.first;
		cjm256 testProduct = firstFactorPair.second * secondFactorPair.second;

		cout << "Product: 0x" << controlProduct << newl;

		cout << std::dec;

		cjm_assert(controlProduct == testProduct);
		cout << "do_random_boost_uint_mult_test PASSES" << newl;
	}

	void do_boost_uint_comparison_tests()
	{
		using boost256 = cjm::testing::ctrtl_u256;
		using cjm256 = cjm::testing::test_u256;
		using namespace cjm::testing;
		using namespace boost_cjm_uint_comparison;

		auto pair = get_pair();
		cout << "pair.first == 0x" << std::hex << pair.first << newl;
		test_u256 fromControl = to_test(pair.first);
		ctrtl_u256 fromTest = to_control(pair.second);
		cjm_assert(pair.first == pair.second && fromControl == pair.second && fromTest == pair.first);
		cout << "do_boost_uint_comparison_tests PASS." << newl;
	}
	template<typename TUInt, size_t Base=16>
	unsigned char div_mod(TUInt& doMe)
	{
		static_assert(Base == 16 || Base == 10);
		static_assert(std::numeric_limits<TUInt>::is_integer && !std::numeric_limits<TUInt>::is_signed);
		unsigned char ret = static_cast<unsigned char>(doMe % Base);
		doMe /= Base;
		return ret;
	}

	void do_divide_by_16_test()
	{
		using namespace cjm::numerics::fixed_uint_literals;
		using namespace cjm::numerics::uint128_literals;
		using namespace cjm::testing::boost_cjm_uint_comparison;
		using namespace cjm::testing;

		constexpr auto startingUInt128 = 0x1234'5678'9abc'def0'1234'5678'9abc'def0_u128;
		constexpr auto startingUInt256 = 0x1234'5678'9abc'def0'1234'5678'9abc'def0'1234'5678'9abc'def0'1234'5678'9abc'def0_u256;
		constexpr auto expectedUInt256Res = 0x0123'4567'89AB'CDEF'0123'4567'89AB'CDEF'0123'4567'89AB'CDEF'0123'4567'89AB'CDEF_u256;
		constexpr auto startingU128Div16 = startingUInt128 / 0x10;
		ctrtl_u256 control256 = cjm::testing::to_control(startingUInt256);
		ctrtl_u128 control128 = cjm::testing::to_control(startingUInt128);
		ctrtl_u128 ctrlRes128 = control128 / 0x10;
		ctrtl_u256 ctrlRes256 = control256 / 0x10;
		cjm_assert(startingU128Div16 == ctrlRes128);
		cout << std::hex << "Control result 128: 0x" << ctrlRes128 << newl;
		cout << "Control result 256: 0x" << ctrlRes256 << newl;
		cout << "startingU128Div16: 0x" << startingU128Div16 << newl;
		auto actualResult = startingUInt256 / 0x10;
		auto putBack = expectedUInt256Res * 0x10;
		cjm_assert(putBack == startingUInt256);
		cjm_assert(expectedUInt256Res == actualResult);
		cout << std::dec;
		cout << "do_divide_by_16_test passes";

	}

	void do_comparative_hex_modulus_tests()
	{
		using byte = unsigned char;
		using boost256 = cjm::testing::ctrtl_u256;
		using cjm256 = cjm::testing::test_u256;
		using namespace cjm::testing;
		using namespace boost_cjm_uint_comparison;
		using namespace cjm::numerics::fixed_uint_literals;
		using namespace cjm::testing::boost_cjm_uint_comparison;
		using namespace boost::multiprecision;
		static constexpr size_t divModAmount = 16;
		auto pair = get_pair();
		constexpr auto origOneTwoEight = 0x1234'5678'9abc'def0'1234'5678'9abc'def0_u128;
		const cjm256 origTestTwoFiveSix = pair.second;
		const boost256 origControlTwoFiveSix = pair.first;
		uint128 oneTwoEight = origOneTwoEight;
		cjm256 testTwoFiveSix = origTestTwoFiveSix;
		boost256 controlTwoFiveSix = origControlTwoFiveSix;
		cjm_assert(testTwoFiveSix == controlTwoFiveSix);
		auto bt = div_mod(controlTwoFiveSix);
		auto checkBt = div_mod(oneTwoEight);
		auto testBt = div_mod(testTwoFiveSix);
		cjm_assert(bt == checkBt);
		cjm_assert(bt == testBt);
		cjm_assert(testTwoFiveSix == controlTwoFiveSix);
		bt = div_mod(controlTwoFiveSix);
		testBt = div_mod(testTwoFiveSix);
		checkBt = div_mod(oneTwoEight);
		cjm_assert(bt == checkBt);
		cjm_assert(bt == testBt);
		cjm_assert(testTwoFiveSix == controlTwoFiveSix);
		cout << "secondB := " << std::hex << "0x" << +bt << newl;
		cout << "do_comparative_hex_modulus_tests PASS" << newl;
	}

	void do_boost_uint_pair_gen_tests()
	{
		static constexpr size_t numElems = 10;
		using namespace cjm::testing;
		using namespace boost_cjm_uint_comparison;
		auto col = create_random_test_collection(numElems);
		size_t count = 0;
		for (const auto& pair : col)
		{
			++count;
			cjm_assert(pair.first == pair.second);
			cout << std::dec << "Item#: \t" << count << 
				"\tValue: \t0x" << std::hex << pair.first << "." << newl;
		}
		cjm_assert(count == numElems);
		cout << "do_boost_uint_pair_gen_tests PASS" << std::endl;
	}

	void do_wacky_subtraction_test()
	{
		using boost256 = cjm::testing::ctrtl_u256;
		using cjm256 = cjm::testing::test_u256;
		using namespace cjm::numerics::fixed_uint_literals;
		using namespace cjm::testing;
		using namespace boost_cjm_uint_comparison;

		constexpr static cjm256 shouldBeSubtrahend = 0x10_u256 << 0x78;

		constexpr static cjm256 minued =			0x0000'0000'0000'0000'0000'0000'0000'0000'1234'5678'9abc'def0'1234'5678'9abc'def0_u256;
		constexpr static cjm256 subtrahend =		0x0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0000'0001'0000'0000'0000'0000_u256;

//		cjm_assert(subtrahend == shouldBeSubtrahend);

		constexpr static boost256 controlMinued =   0x00000000000000000000000000000000123456789abcdef0123456789abcdef0_cppui256;
		constexpr static boost256 controlSubtr =    0x0000000000000000000000000000000000000000000000010000000000000000_cppui256;
		auto tempMinued = minued;
		auto tempControlMinued = controlMinued;
		tempMinued -= subtrahend;
		tempControlMinued -= controlSubtr;
		cout << "temp minued:= 0x" << std::hex << tempMinued.tostring() << newl;
		cout << "temp ctrl:=   0x" << std::hex << tempControlMinued << newl;
		cjm_assert(tempMinued == tempControlMinued);
		cout << "do_wacky_subtraction_test PASSED" << newl;
	}

	void perform_comparison_u256_tests(size_t numTests)
	{
		assert(numTests > 0);
		auto col = cjm::testing::create_random_comparison_opset256(numTests);
		cjm_assert(std::all_of(col.cbegin(), col.cend(), [](auto op) -> bool {return op.all_pass(); }));
		cout << "All " << std::dec << numTests << " comparison u256 tests PASSED." << std::hex << newl;
	}

	void perform_unary_op_u256_tests(size_t numTests)
	{
		cout << std::dec << "starting " << numTests << " perform_unary_op_u256_tests." << std::hex << newl;
		auto testSet = cjm::testing::get_random_unary_256_opset256(numTests);
		cjm_assert(std::all_of(testSet.cbegin(), testSet.cend(), [](const auto& unop) -> bool
		{
			return unop.all_passed();
		}));
		cout << "perform_unary_op_u256_tests PASSED " << newl;
	}

	void do_u256_literal_printing_test()
	{
		using boost128 = cjm::testing::ctrtl_u128;
		using boost256 = cjm::testing::ctrtl_u256;
		using cjm256 = cjm::testing::test_u256;
		using cjm128 = cjm::testing::test_u128;
		using namespace cjm::numerics::fixed_uint_literals;
		using namespace cjm::testing;
		using namespace boost_cjm_uint_comparison;
				
		boost256 boostMAX = std::numeric_limits<boost256>::max();
		constexpr static auto cjm256MAX = std::numeric_limits<cjm256>::max();
		constexpr static auto bigLiteralSource = 0x1234'5678'9abc'def0'1234'5678'9abc'def0'1234'5678'9abc'def0'1234'5678'9abc'def0_u256;
		constexpr static auto literalSource =											 0x1234'5678'9abc'def0'1234'5678'9abc'def0_u128;
		constexpr static boost128 boostSource = 0x123456789abcdef0123456789abcdef0_cppui128;
		cjm_assert(literalSource == boostSource);
		const static boost256 bigBoost = ((static_cast<boost256>(boostSource) << 0x80) | (static_cast<boost256>(boostSource)));
		cjm_assert(bigBoost == bigLiteralSource);

		cout << std::hex << "boost hex: 0x" << bigBoost << newl;
		cout << std::dec << "boost ded: " << bigBoost << newl;
		cout << std::hex << "u256 hex: 0x" << bigLiteralSource << newl;
		cout << std::dec << "u256 dec: " << bigLiteralSource << newl;
		cout << "biggest boost dec: " << boostMAX << newl;
		cout << "biggest cjm dec: " << cjm256MAX << newl;
		cout << std::hex;

		std::string boostHexString = convert_to_upper_hex_string(bigBoost);
		std::string cjmHexString = convert_to_upper_hex_string(bigLiteralSource);
		std::string boostDecString = convert_to_decimal_string(bigBoost);
		std::string cjmDecString = convert_to_decimal_string(bigLiteralSource);

		std::string biggestCjmDec = convert_to_decimal_string(cjm256MAX);
		std::string biggestBoostDec = convert_to_decimal_string(boostMAX);

		cjm_assert(boostHexString == cjmHexString);
		cjm_assert(boostDecString == cjmDecString);
		cjm_assert(biggestCjmDec == biggestBoostDec);

		cout << "do_u256_literal_printing_test PASSED" << newl;
	}

	void do_instrumented_uint256_div_mod_test_1()
	{
		using boost256 = cjm::testing::ctrtl_u256;
		using cjm256 = cjm::testing::test_u256;
		using namespace cjm::numerics::fixed_uint_literals;
		using namespace cjm::testing;
		using namespace boost_cjm_uint_comparison;
		auto fstream = std::ofstream("instr_div_mod_256_1_test.txt", std::ios_base::out);
		constexpr auto dividend = 0xBABF'B3F7'2E02'4534'A030'3D20'BA36'5174'4531'9F52'E652'BAB6'9726'6214'43C2'2442_u256;
		constexpr auto divisor = 0x7C53'770E'6BEF'55CE'0DF0'E2A6'0923'C948'4D57'D82F'0267'3BBC'E9CD'8902'5725'88F2_u256;
		do_instrumented_uint256_div_mod(fstream, dividend, divisor);
		cout << "do_instrumental_uint256_div_mod_test_1 PASSED " << newl;
	}

	void do_instrumented_uint128_div_mod_test_1()
	{
		using boost128 = cjm::testing::ctrtl_u128;
		using cjm128 = cjm::testing::test_u128;
		using namespace cjm::testing;
		using namespace boost_cjm_uint_comparison;
		auto fstream = std::ofstream("instr_div_mod_128_1_test.txt", std::ios_base::out);
		constexpr auto dividend = 0xBABF'B3F7'2E02'4534'A030'3D20'BA36'5174_u128;
		constexpr auto divisor = 0x7C53'770E'6BEF'55CE'0DF0'E2A6'0923'C948_u128;
		do_instrumented_uint128_div_mod(fstream, dividend, divisor);
		cout << "do_instrumented_uint128_div_mod_test_1 PASSED " << newl;
	}
	void do_intrumented_uint128_div_mod_test_by16()
	{
		using boost128 = cjm::testing::ctrtl_u128;
		using cjm128 = cjm::testing::test_u128;
		using namespace cjm::testing;
		using namespace boost_cjm_uint_comparison;
		constexpr auto dividend = 0x1234'5678'9abc'def0'1234'5678'9abc'def0_u128;
		constexpr auto divisor = 0x10_u128;
		auto fstream = std::ofstream("instr_div_mod_128_by16_test.txt", std::ios_base::out);
		do_instrumented_uint128_div_mod(fstream, dividend, divisor);
		cout << "do_instrumented_uint128_div_mod_test_by16 PASSED " << newl;
	}

	void do_intrumented_uint256_div_mod_test_by16_1()
	{
		using boost256 = cjm::testing::ctrtl_u256;
		using cjm256 = cjm::testing::test_u256;
		using namespace cjm::testing;
		using namespace boost_cjm_uint_comparison;
		constexpr auto dividend = 0x1234'5678'9abc'def0'1234'5678'9abc'def0_u256;
		constexpr auto divisor = 0x10_u256;
		auto fstream = std::ofstream("instr_div_mod_256_by16_test_1.txt", std::ios_base::out);
		do_instrumented_uint256_div_mod(fstream, dividend, divisor);
		cout << "do_intrumented_uint256_div_mod_test_by16_1 TEST PASSES" << newl;
	}

	void do_instrumented_uint128_div_mod(std::basic_ostream<char>& stream, uint128 dividend, uint128 divisor)
	{
		using namespace cjm::testing::boost_cjm_uint_comparison;
		using namespace cjm::testing;

		auto boostDividend = to_control(dividend);
		auto boostDivisor = to_control(divisor);
		auto controlQuotient = boostDividend / boostDivisor;
		auto controlRemainder = boostDividend % boostDivisor;

		auto control2Quotient = dividend / divisor;
		auto control2Remainder = dividend % divisor;

		uint128 quotient;
		uint128 remainder;
		uint128::instrumented_div_mod(stream, dividend, divisor, &quotient, &remainder);
		cjm_assert(controlQuotient == control2Quotient && controlRemainder == control2Remainder);
		cjm_assert(quotient == controlQuotient && remainder == controlRemainder);		
	}

	void do_instrumented_uint256_div_mod(std::basic_ostream<char>& stream, 
		const cjm::numerics::uint256_t& dividend, const cjm::numerics::uint256_t& divisor)
	{
		using namespace cjm::testing::boost_cjm_uint_comparison;
		using namespace cjm::testing;
		using uint256_t = cjm::numerics::uint256_t;
		auto boostDividend = to_control(dividend);
		auto boostDivisor = to_control(divisor);
		auto controlQuotient = boostDividend / boostDivisor;
		auto controlRemainder = boostDividend % boostDivisor;

		uint256_t quotient;
		uint256_t remainder;
		uint256_t::instrumented_div_mod(stream, dividend, divisor, &quotient, &remainder);
		cjm_assert(quotient == controlQuotient && remainder == controlRemainder);
		cout << "do_instrumented_uint128_div_mod test PASSED" << newl;
	}


	//void do_rational_test()
	//{
	//	auto a = 1ull;
	//	auto b = std::numeric_limits<std::uint64_t>::max();
	//	auto c = 3ull;
	//	auto d = std::numeric_limits<std::uint64_t>::max() - 2;
	//	cout << "USING UINT64" << newl;
	//	cout << "a/b == " << a << "/" << b << "." << newl;
	//	cout << "c/d == " << c << "/" << d << "." << newl;
	//	cout << "a*d == " << a * d << " b*c == " << b * c << "." << newl;
	//	cout << "a/b, c/d relationship: " << get_rel(a*d, b*c) << "." << newl;
	//	cout << newl << std::endl;

	//	uint128 bigA = a;
	//	uint128 bigB = b;
	//	uint128 bigC = c;
	//	uint128 bigD = d;

	//	cout << "USING UINT128" << newl;
	//	cout << "a/b == " << bigA << "/" << bigB << "." << newl;
	//	cout << "c/d == " << bigC << "/" << bigD << "." << newl;
	//	cout << "a*d == " << bigA * bigD << " b*c == " << bigB * bigC << "." << newl;
	//	cout << "a/b, c/d relationship: " << get_rel(bigA*bigD, bigB*bigC) << "." << newl;
	//}

	//void do_rational_test_2()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using rat = rational<std::uint64_t>;
	//	using bigrat = rational<cjm::numerics::uint128>;
	//	rat fiveTwentySevenths = rat::make_rational( 5, 27);
	//	rat negFivTS = -rat::make_rational( 5, 27);
	//	rat sixTwentyNinths = rat::make_rational( 6, 29 );
	//	rat negSixTwN = -rat::make_rational( 6, 29);
	//	rat alsoFiveTwentySevents = rat::make_rational(10, 54 );
	//	bigrat bigfts = bigrat::make_rational(5, 27 );

	//	uint64_t max = std::numeric_limits<uint64_t>::max();
	//	uint64_t maxLessTwo = max - 2;
	//	rat comparisonShouldThrowX = rat::make_rational(1, max );
	//	rat comparisonShouldThrow  = rat::make_rational(3, maxLessTwo );
	//	bigrat bigNoThrowX = bigrat::make_rational( 1, max );
	//	bigrat bigNoThrowY = bigrat::make_rational( 3, max - 2 );

	//	rat d00d = 12;
	//	rat ohD00dz = -12;
	//	rat noD00dz;
	//	rat uNoD00dz = 0u;
	//	rat uD00d = 12u;

	//	auto big_d00d = static_cast<bigrat>(12_u128);
	//	bigrat big_ohD00dz = -12;
	//	bigrat big_noD00dz;
	//	bigrat big_uNoD00dz = 0u;
	//	bigrat big_uD00d = 12u;
	//	
	//	
	//	cjm_assert(uD00d == d00d && -uD00d == ohD00dz && uNoD00dz == noD00dz && uD00d > ohD00dz && ohD00dz < d00d);
	//	cjm_assert(noD00dz == static_cast<rat>(0));
	//	cjm_assert(d00d > ohD00dz);
	//	cjm_assert(ohD00dz < d00d);
	//	cjm_assert(d00d == -ohD00dz);
	//	cjm_assert(big_uD00d == big_d00d && -big_uD00d == big_ohD00dz && big_uNoD00dz == big_noD00dz && big_uD00d > big_ohD00dz && big_ohD00dz < big_d00d);
	//	cjm_assert(noD00dz == static_cast<rat>(0));
	//	cjm_assert(d00d > ohD00dz);
	//	cjm_assert(ohD00dz < d00d);
	//	cjm_assert(d00d == -ohD00dz);
	//	cjm::testing::cjm_assert_throws<rational_overflow>([=]() -> void
	//	{
	//		bool isLessThan = comparisonShouldThrowX < comparisonShouldThrow;
	//		std::cerr << "YOU SHOULD NOT SEE ME!" << isLessThan << newl;
	//	});
	//	cjm_assert(bigNoThrowX < bigNoThrowY);
	//	cjm_assert(fiveTwentySevenths != negFivTS);
	//	cjm_assert(sixTwentyNinths != fiveTwentySevenths);
	//	cjm_assert(negSixTwN != negFivTS);
	//	cjm_assert(negFivTS != sixTwentyNinths);
	//	cjm_assert(negFivTS != alsoFiveTwentySevents);
	//	cjm_assert(sixTwentyNinths != alsoFiveTwentySevents);
	//	cjm_assert(negSixTwN != alsoFiveTwentySevents);
	//	cjm_assert(fiveTwentySevenths == alsoFiveTwentySevents);
	//	// ReSharper disable CppIdenticalOperandsInBinaryExpression
	//	cjm_assert(+fiveTwentySevenths == +fiveTwentySevenths);  // NOLINT
	//	cjm_assert(+fiveTwentySevenths == +alsoFiveTwentySevents);		
	//	cjm_assert(-fiveTwentySevenths == -fiveTwentySevenths); // NOLINT
	//	// ReSharper restore CppIdenticalOperandsInBinaryExpression
	//	cjm_assert(-fiveTwentySevenths == -alsoFiveTwentySevents);
	//	cjm_assert(sixTwentyNinths == -negSixTwN);
	//	cjm_assert(fiveTwentySevenths >= negFivTS);
	//	cjm_assert(fiveTwentySevenths > negFivTS);
	//	cjm_assert(sixTwentyNinths > negSixTwN);
	//	cjm_assert(sixTwentyNinths > negFivTS);
	//	cjm_assert(negFivTS < sixTwentyNinths);
	//	cjm_assert(negSixTwN < sixTwentyNinths);
	//	cjm_assert(sixTwentyNinths > fiveTwentySevenths && sixTwentyNinths >= fiveTwentySevenths);
	//	cjm_assert(fiveTwentySevenths < sixTwentyNinths && fiveTwentySevenths <= sixTwentyNinths);
	//	cjm_assert(-sixTwentyNinths < -fiveTwentySevenths && -sixTwentyNinths <= -fiveTwentySevenths);
	//	cjm_assert(-fiveTwentySevenths > -sixTwentyNinths && -alsoFiveTwentySevents > -sixTwentyNinths);
	//	

	//	cout << "fivetwenty sevents hash: " << fiveTwentySevenths.hash_code() << newl;
	//	cout << "big five twenty sevenths hash: " << bigfts.hash_code() << newl;
	//	cout << "Do rational test 2 PASSED" << newl;
	//}

	//void do_rational_add_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational::rational_literals;
	//	using namespace cjm::numerics::rational;
	//	using rat = rational<std::uint64_t>;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	rat zero;
	//	rat oneHalf = rat::make_rational(1, 2);
	//	rat oneThird = rat::make_rational( 1, 3 );
	//	rat fiveSixths = rat::make_rational( 5, 6 );
	//	rat oneSixth = rat::make_rational( 1, 6 );
	//	rat maxInt = rat::make_rational(std::numeric_limits<std::uint64_t>::max(), 1);
	//	rat maxFrac = rat::make_rational(std::numeric_limits<std::uint64_t>::max() - 1, std::numeric_limits<std::uint64_t>::max());
	//	rat anotherBigFrac = rat::make_rational(std::numeric_limits<std::uint64_t>::max() - 3, std::numeric_limits<std::uint64_t>::max());
	//	rat reallySmall = std::numeric_limits<rat>::min();
	//	bigrat oneHalfBig = bigrat::make_rational( 1, 2 );
	//	bigrat oneThirdBig = bigrat::make_rational( 1, 3 );
	//	bigrat fiveSixthsBig = bigrat::make_rational( 5, 6 );
	//	bigrat oneSixthBig = bigrat::make_rational( 1, 6 );
	//	bigrat bigMaxInt = bigrat::make_rational(std::numeric_limits<uint128>::max(), 1);
	//	bigrat bigMaxFrac = bigrat::make_rational(std::numeric_limits<uint128>::max() - 1, std::numeric_limits<uint128>::max());
	//	bigrat bigAnotherBigFrac = bigrat::make_rational(std::numeric_limits<uint128>::max() - 3, std::numeric_limits<uint128>::max());
	//	bigrat bigReallySmall = std::numeric_limits<bigrat>::min();

	//	cjm_assert(oneHalf + oneThird == fiveSixths);
	//	cjm_assert(oneHalfBig + oneThirdBig == fiveSixthsBig);
	//	cjm_assert(fiveSixths - oneHalf == oneThird && fiveSixthsBig - oneHalfBig == oneThirdBig);
	//	cjm_assert(fiveSixths - oneThird == oneHalf && fiveSixthsBig - oneThirdBig == oneHalfBig);
	//	cjm_assert(oneThird - oneHalf == -oneSixth && oneThirdBig - oneHalfBig == -oneSixthBig);
	//	cjm_assert(oneHalf - oneThird == oneSixth && oneHalfBig - oneThirdBig == oneSixthBig);
	//	cjm_assert(oneHalf + 1 == rat::make_rational( 3, 2 ));
	//	cjm_assert(oneHalf + 0 == oneHalf && 0 + oneHalf == oneHalf && 0 - oneHalf == -oneHalf);
	//	oneSixthBig += oneHalfBig;
	//	cjm_assert(oneSixthBig == bigrat::make_rational( 2, 3 ));
	//	oneSixthBig -= fiveSixthsBig;
	//	cjm_assert(oneSixthBig == -bigrat::make_rational( 1, 6 ));

	//	oneSixth += oneSixth;
	//	cjm_assert(oneSixth == oneThird);
	//	
	//	oneSixth += zero;
	//	cjm_assert(oneSixth == oneThird);
	//	oneSixth -= zero;
	//	cjm_assert(oneSixth == oneThird);

	//	oneSixth -= (oneThird + oneThird);
	//	cjm_assert(oneSixth == -rat::make_rational( 1, 3));

	//	cjm_assert_throws<std::overflow_error>([=] () -> void
	//	{
	//		auto sum = maxInt + 1;
	//		cerr << "Never see me: " << sum << newl;
	//	});
	//	cjm_assert_throws<std::overflow_error>([=]() -> void
	//	{
	//		auto sum = maxFrac + anotherBigFrac;
	//		cerr << "Never see me: " << sum << newl;
	//	});
	//	cjm_assert(maxFrac + reallySmall == 1);

	//	cjm_assert_throws<std::overflow_error>([=]() -> void
	//	{
	//		auto sum = bigMaxInt + 1;
	//		cerr << "Never see me: " << sum << newl;
	//	});
	//	cjm_assert_throws<std::overflow_error>([=]() -> void
	//	{
	//		auto sum = bigMaxFrac + bigAnotherBigFrac;
	//		cerr << "Never see me: " << sum << newl;
	//	});
	//	cjm_assert(bigMaxFrac + bigReallySmall == 1);

	//	cout << "unsafe tests pass." << newl;

	//	cout << "beginning safe tests." << newl;
	//	
	//	auto res = rat::safe_add(oneHalf, oneThird);
	//	auto bigRes = bigrat::safe_add(oneHalfBig, oneThirdBig);
	//	cjm_assert(bigRes.first && bigRes.second == fiveSixthsBig);
	//	cjm_assert(res.first && res.second == fiveSixths);
	//	
	//	res = rat::safe_subtract(fiveSixths, oneHalf);
	//	bigRes = bigrat::safe_subtract(fiveSixthsBig, oneHalfBig);
	//	cjm_assert(res.first && res.second == oneThird);
	//	cjm_assert(bigRes.first && bigRes.second == oneThirdBig);

	//	res = rat::safe_add((1 / 2_r64), (1 / 2_r64));
	//	bigRes = bigrat::safe_add((1 / 2_r128), (1 / 2_r128));
	//	cjm_assert(res.first && res.second == 1_r64);
	//	cjm_assert(bigRes.first && bigRes.second == 1_r128);

	//	res = rat::safe_subtract(1_r64, (1 / 2_r64));
	//	bigRes = bigrat::safe_subtract(1_r128, (1 / 2_r128));
	//	cjm_assert(res.first && res.second == (1 / 2_r64));
	//	cjm_assert(bigRes.first && bigRes.second == (1 / 2_r128));

	//	res = rat::safe_subtract(0_r64, (1 / 2_r64));
	//	bigRes = bigrat::safe_subtract(0_r128, (1 / 2_r128));
	//	cjm_assert(res.first && res.second == -(1 / 2_r64));
	//	cjm_assert(bigRes.first && bigRes.second == -(1 / 2_r128));

	//	res = rat::safe_add((1 / 2_r64), 0_r64);
	//	bigRes = bigrat::safe_add((1 / 2_r128), 0_r128);
	//	cjm_assert(res.first && res.second == (1 / 2_r64));
	//	cjm_assert(bigRes.first && bigRes.second == (1 / 2_r128));
	//	
	//	res = rat::safe_add(maxInt, 1);
	//	bigRes = bigrat::safe_add(bigMaxInt, 1);
	//	cjm_deny(res.first);
	//	cjm_deny(bigRes.first);

	//	res = rat::safe_add(maxFrac, anotherBigFrac);
	//	bigRes = bigrat::safe_add(bigMaxFrac, bigAnotherBigFrac);
	//	cjm_deny(res.first);
	//	cjm_deny(bigRes.first);

	//	res = rat::safe_subtract(1_r64, reallySmall);
	//	bigRes = bigrat::safe_subtract(1_r128, bigReallySmall);
	//	cjm_assert(res.first && res.second == maxFrac);
	//	cjm_assert(bigRes.first && bigRes.second == bigMaxFrac);

	//	cout << "rational_add_test PASSED" << newl;		
	//}

	//void do_rational_mult_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using rat = rational<std::uint64_t>;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	rat reallySmallOne = rat::make_rational( 1ull, std::numeric_limits<uint64_t>::max() );
	//	rat justALilBigger = rat::make_rational(1ull, std::numeric_limits<uint64_t>::max() - 1ull );
	//	rat oneAndAHalf = rat::make_rational( 3, 2 );
	//	
	//	rat twentyFiveSixtyFourths = rat::make_rational( 390'625, 1'000'000 );
	//	rat twoPointFiveSix = 2 + rat::make_rational( 56, 100 );
	//	
	//	oneAndAHalf *= 2;
	//	cjm_assert(oneAndAHalf == 3);
	//	oneAndAHalf *= rat::make_rational( 10821, 331 );
	//	oneAndAHalf /= rat::make_rational( 3, 2 );
	//	cjm_assert(oneAndAHalf == rat::make_rational( 21642, 331 ));
	//	cjm_assert(twentyFiveSixtyFourths.reciprocal() == twoPointFiveSix);
	//	cjm::testing::cjm_assert_throws<std::domain_error>([]()->void
	//	{
	//		auto x =rat{}.reciprocal();
	//		cerr << "You should never see me: " << static_cast<int>(x) << newl;
	//	});
	//	cjm::testing::cjm_assert_throws<rational_overflow>([=] () -> void
	//	{
	//		auto x = reallySmallOne + justALilBigger;
	//		cerr << "You should never see me: " << static_cast<uint64_t>(x) << newl;
	//	});
	//	rat negativeThirteenQuarters = -rat::make_rational(13, 4);
	//	cjm_assert(static_cast<int>(negativeThirteenQuarters) == -3);
	//	cout << "do rational mult tests PASSED";

	//	
	//}

	//void do_static_rational_literals_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational::rational_literals;
	//	using namespace cjm::numerics::rational;
	//	using lilrat = rational<std::uint64_t>;
	//	using rat = rational<uint128>;
	//	constexpr rat zeroWeird = -rat::make_rational(0, 0, 3);
	//	constexpr rat nineAndSevenEighths = rat::make_rational(9, 14, 16_u128);
	//	constexpr rat negThreeAndAHalf = -rat::make_rational(3, 1, 2);
	//	constexpr rat expected_result = rat::make_rational( 90, 7 );
	//	constexpr rat also_expected_result = rat::make_rational(90, 7);
	//	constexpr rat expected_result_two = -rat::make_rational( 15, 4);
	//	constexpr rat threeAndAHalf = rat::make_rational(14, 4);
	//	constexpr rat x = 12_r128;
	//	constexpr rat test = (12 + 6 / 7_r128);
	//	constexpr rat test_two = -(3 + 3 / 4_r128);
	//	static_assert(also_expected_result == expected_result);
	//	static_assert(-negThreeAndAHalf == threeAndAHalf);
	//	static_assert(negThreeAndAHalf == -(3 + 1 / 2_r128));
	//	static_assert(x == 12);
	//	static_assert(test == expected_result);
	//	static_assert(zeroWeird == 0_r128);
	//	static_assert(expected_result_two == test_two);
	//	static_assert(expected_result == also_expected_result);
	//	static_assert(nineAndSevenEighths == (9 + 7 / 8_r128));
	//}

	//void do_static_rational_add_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using rat = rational<std::uint64_t>;
	//	using bigrat = rational<cjm::numerics::uint128>;
	//	using uint128 = cjm::numerics::uint128;
	//	constexpr int zeroI = 0;
	//	
	//	constexpr rat zero{};
	//	constexpr rat oneHalf = rat::make_rational( 1, 2 );
	//	constexpr rat oneThird = rat::make_rational( 1, 3 );
	//	constexpr rat fiveSixths = rat::make_rational( 5, 6 );
	//	constexpr rat oneSixth = rat::make_rational( 1, 6 );
	//	constexpr rat oneAndAHalf = rat::make_rational( 3, 2 );
	//	constexpr bigrat oneHalfBig = bigrat::make_rational( 1, 2 );
	//	constexpr bigrat oneThirdBig = bigrat::make_rational( 1, 3 );
	//	constexpr bigrat fiveSixthsBig = bigrat::make_rational( 5, 6 );
	//	constexpr bigrat oneSixthBig = bigrat::make_rational( 1, 6 );
	//	constexpr bigrat oneAndAHalfBig = bigrat::make_rational( 3_u128, 2_u128 );
	//	constexpr bigrat bigZero{};

	//	static_assert(oneHalf + oneThird == fiveSixths);
	//	static_assert(oneHalfBig + oneThirdBig == fiveSixthsBig);
	//	static_assert(fiveSixths - oneHalf == oneThird && fiveSixthsBig - oneHalfBig == oneThirdBig);
	//	static_assert(fiveSixths - oneThird == oneHalf && fiveSixthsBig - oneThirdBig == oneHalfBig);
	//	static_assert(oneThird - oneHalf == -oneSixth && oneThirdBig - oneHalfBig == -oneSixthBig);
	//	static_assert(oneHalf - oneThird == oneSixth && oneHalfBig - oneThirdBig == oneSixthBig);
	//	static_assert(oneHalf + 1 == oneAndAHalf);
	//	static_assert(1_u128 + oneHalfBig == oneAndAHalfBig);
	//	static_assert(oneHalfBig + (2-1) == oneAndAHalfBig);
	//	static_assert(oneHalf + zero == oneHalf && zero + oneHalf == oneHalf && zero - oneHalf == -oneHalf);
	//	static_assert(oneHalfBig + bigZero == oneHalfBig && bigZero + oneHalfBig == oneHalfBig && bigZero - oneHalfBig == -oneHalfBig);
	//	static_assert(zero == -zero && bigZero == -bigZero && zero + zero == zero && zero - zero == zero && bigZero + bigZero == bigZero
	//		&& bigZero - bigZero == bigZero	&& 0 + zero == zero && zero + 0 == zero && zeroI == zero && 0_u128 == bigZero && 
	//		bigZero + 0_u128 == bigZero && bigZero + 0 == bigZero && 0 + bigZero == bigZero);

	//}

	//void do_static_rational_mult_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using rat = rational<std::uint64_t>;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	constexpr rat oneHalf = rat::make_rational( 1, 2 );
	//	constexpr rat fiveSixths = rat::make_rational( 5, 6 );
	//	constexpr rat oneAndAHalf = rat::make_rational( 3, 2 );
	//	constexpr rat twentyFiveSixtheens = rat::make_rational( 25, 16 );
	//	constexpr rat twentyFiveSixtyFourths = rat::make_rational( 390'625, 1'000'000 );
	//	constexpr rat oneQuarter = rat::make_rational( 25, 100 );
	//	constexpr rat oneAndAQuarter = rat::make_rational( 5, 4 );
	//	constexpr bigrat oneHalfBig = bigrat::make_rational( 1, 2 );
	//	constexpr bigrat oneThirdBig = bigrat::make_rational( 1, 3 );
	//	constexpr bigrat bigZero{};

	//	static_assert(oneHalf * oneHalf.reciprocal() == 1 && oneHalfBig * oneHalfBig.reciprocal() == 1_u128);
	//	static_assert(oneAndAHalf * fiveSixths == oneAndAQuarter);
	//	static_assert(oneAndAQuarter / fiveSixths == oneAndAHalf);
	//	static_assert(oneAndAQuarter * oneAndAQuarter == twentyFiveSixtheens);
	//	static_assert(oneThirdBig * 1 == oneThirdBig && oneThirdBig * 0 == bigZero && oneThirdBig * 1_u128 == oneThirdBig && bigZero == oneThirdBig * 0_u128);
	//	static_assert((twentyFiveSixtheens * oneQuarter) == twentyFiveSixtyFourths);
	//}

	//void do_static_rational_truncation_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	constexpr bigrat zero{};
	//	constexpr bigrat one = bigrat::make_rational( 1, 1 );
	//	constexpr bigrat negativeOne = -bigrat::make_rational( 1, 1);
	//	constexpr bigrat two = bigrat::make_rational( 2, 1 );
	//	constexpr bigrat negativeTwo{ -two };
	//	constexpr bigrat three= bigrat::make_rational( 3, 1 );
	//	constexpr bigrat negativeThree { -three };

	//	constexpr bigrat oneHalf{ two.reciprocal() };
	//	constexpr bigrat negativeOneHalf{ -oneHalf };
	//	constexpr bigrat twoFifths= bigrat::make_rational( 2, 5 );
	//	constexpr bigrat negativeTwoFifths { -twoFifths };
	//	constexpr bigrat threeFifths = bigrat::make_rational( 3, 5 );
	//	constexpr bigrat negativeThreeFifths{ -threeFifths };

	//	constexpr bigrat twoAndAHalf = two + oneHalf;
	//	constexpr bigrat negativeTwoAndAHalf = -twoAndAHalf;
	//	constexpr bigrat twoAndTwoFifths = two + twoFifths;
	//	constexpr bigrat negativeTwoAndTwoFifths = -twoAndTwoFifths;
	//	constexpr bigrat twoAndThreeFifths = two + threeFifths;
	//	constexpr bigrat negativeTwoAndThreeFifths = -twoAndThreeFifths;

	//	static_assert(trunc(threeFifths) == zero);
	//	static_assert(trunc(negativeThreeFifths) == zero);
	//	static_assert(trunc(zero) == zero);
	//	static_assert(trunc(one) == one);
	//	static_assert(trunc(negativeOne) == negativeOne);
	//	static_assert(three == trunc(three));
	//	static_assert(-three == trunc(negativeThree));
	//	static_assert(trunc(threeFifths) == zero);
	//	static_assert(trunc(negativeThreeFifths) == zero);
	//	static_assert(trunc(twoFifths) == zero);
	//	static_assert(trunc(negativeTwoFifths) == zero);
	//	static_assert(trunc(twoAndTwoFifths) == two);
	//	static_assert(trunc(negativeTwoAndTwoFifths) == negativeTwo);
	//	static_assert(trunc(oneHalf) == zero);
	//	static_assert(trunc(negativeOneHalf) == zero);
	//	static_assert(trunc(negativeTwoAndAHalf) == negativeTwo);
	//	static_assert(trunc(twoAndAHalf) == two);
	//	static_assert(trunc(twoAndThreeFifths) == two);
	//	static_assert(trunc(negativeTwoAndThreeFifths) == negativeTwo);
	//	static_assert(!(trunc(negativeTwoAndThreeFifths) > negativeTwo));
	//	static_assert(!(trunc(negativeTwoAndThreeFifths) < negativeTwo));
	//	static_assert(trunc(negativeTwoAndThreeFifths) >= negativeTwo);
	//	static_assert(trunc(negativeTwoAndThreeFifths) <= negativeTwo);
	//	static_assert(!(trunc(negativeTwoAndThreeFifths) != negativeTwo));
	//}

	//void do_static_rational_floor_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	constexpr bigrat zero{};
	//	constexpr bigrat one = bigrat::make_rational( 1, 1 );
	//	constexpr bigrat negativeOne = -bigrat::make_rational( 1, 1);
	//	constexpr bigrat two = bigrat::make_rational( 2, 1 );
	//	constexpr bigrat negativeTwo{ -two };
	//	constexpr bigrat three = bigrat::make_rational( 3, 1 );
	//	constexpr bigrat negativeThree{ -three };

	//	constexpr bigrat oneHalf{ two.reciprocal() };
	//	constexpr bigrat negativeOneHalf{ -oneHalf };
	//	constexpr bigrat twoFifths = bigrat::make_rational( 2, 5 );
	//	constexpr bigrat negativeTwoFifths{ -twoFifths };
	//	constexpr bigrat threeFifths = bigrat::make_rational( 3, 5 );
	//	constexpr bigrat negativeThreeFifths{ -threeFifths };

	//	constexpr bigrat twoAndAHalf = two + oneHalf;
	//	constexpr bigrat negativeTwoAndAHalf = -twoAndAHalf;
	//	constexpr bigrat twoAndTwoFifths = two + twoFifths;
	//	constexpr bigrat negativeTwoAndTwoFifths = -twoAndTwoFifths;
	//	constexpr bigrat twoAndThreeFifths = two + threeFifths;
	//	constexpr bigrat negativeTwoAndThreeFifths = -twoAndThreeFifths;


	//	static_assert(floor(threeFifths) == zero);
	//	static_assert(floor(negativeThreeFifths) == negativeOne);
	//	static_assert(floor(zero) == zero);
	//	static_assert(floor(one) == one);
	//	static_assert(floor(negativeOne) == negativeOne);
	//	static_assert(three == floor(three));
	//	static_assert(-three == floor(negativeThree));
	//	static_assert(floor(threeFifths) == zero);
	//	static_assert(floor(negativeThreeFifths) == -one);
	//	static_assert(floor(twoFifths) == zero);
	//	static_assert(floor(negativeTwoFifths) == -one);
	//	static_assert(floor(twoAndTwoFifths) == two);
	//	static_assert(floor(negativeTwoAndTwoFifths) == negativeThree);
	//	static_assert(floor(oneHalf) == zero);
	//	static_assert(floor(negativeOneHalf) == negativeOne);
	//	static_assert(floor(negativeTwoAndAHalf) == negativeThree);
	//	static_assert(floor(twoAndAHalf) == two);
	//	static_assert(floor(twoAndThreeFifths) == two);
	//	static_assert(floor(negativeTwoAndThreeFifths) == negativeThree);
	//	static_assert(floor(negativeOne - oneHalf) == negativeTwo);
	//}

	//void do_static_rational_ceiling_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	constexpr bigrat zero{};
	//	constexpr bigrat one = bigrat::make_rational( 1, 1 );
	//	constexpr bigrat negativeOne = -bigrat::make_rational( 1, 1 );
	//	constexpr bigrat two = bigrat::make_rational( 2, 1 );
	//	constexpr bigrat negativeTwo{ -two };
	//	constexpr bigrat three = bigrat::make_rational( 3, 1 );
	//	constexpr bigrat negativeThree{ -three };

	//	constexpr bigrat oneHalf{ two.reciprocal() };
	//	constexpr bigrat negativeOneHalf{ -oneHalf };
	//	constexpr bigrat twoFifths = bigrat::make_rational( 2, 5 );
	//	constexpr bigrat negativeTwoFifths{ -twoFifths };
	//	constexpr bigrat threeFifths = bigrat::make_rational( 3, 5 );
	//	constexpr bigrat negativeThreeFifths{ -threeFifths };

	//	constexpr bigrat twoAndAHalf = two + oneHalf;
	//	constexpr bigrat negativeTwoAndAHalf = -twoAndAHalf;
	//	constexpr bigrat twoAndTwoFifths = two + twoFifths;
	//	constexpr bigrat negativeTwoAndTwoFifths = -twoAndTwoFifths;
	//	constexpr bigrat twoAndThreeFifths = two + threeFifths;
	//	constexpr bigrat negativeTwoAndThreeFifths = -twoAndThreeFifths;


	//	static_assert(ceil(threeFifths) == one);
	//	static_assert(ceil(negativeThreeFifths) == zero);
	//	static_assert(ceil(zero) == zero);
	//	static_assert(ceil(one) == one);
	//	static_assert(ceil(negativeOne) == negativeOne);
	//	static_assert(three == ceil(three));
	//	static_assert(-three == ceil(negativeThree));
	//	static_assert(ceil(threeFifths) == one);
	//	static_assert(ceil(negativeThreeFifths) == zero);
	//	static_assert(ceil(twoFifths) == one);
	//	static_assert(ceil(negativeTwoFifths) == zero);
	//	static_assert(ceil(twoAndTwoFifths) == three);
	//	static_assert(ceil(negativeTwoAndTwoFifths) == negativeTwo);
	//	static_assert(ceil(oneHalf) == one);
	//	static_assert(ceil(negativeOneHalf) == zero);
	//	static_assert(ceil(negativeTwoAndAHalf) == negativeTwo);
	//	static_assert(ceil(twoAndAHalf) == three);
	//	static_assert(ceil(twoAndThreeFifths) == three);
	//	static_assert(ceil(negativeTwoAndThreeFifths) == negativeTwo);
	//	static_assert(ceil(negativeOne - oneHalf) == negativeOne);
	//}
	//void do_static_rational_round_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	constexpr bigrat zero{};
	//	constexpr bigrat one = bigrat::make_rational( 1, 1 );
	//	constexpr bigrat negativeOne = -bigrat::make_rational( 1, 1);
	//	constexpr bigrat two = bigrat::make_rational( 2, 1 );
	//	constexpr bigrat negativeTwo{ -two };
	//	constexpr bigrat three = bigrat::make_rational( 3, 1 );
	//	constexpr bigrat negativeThree{ -three };

	//	constexpr bigrat oneHalf{ two.reciprocal() };
	//	constexpr bigrat negativeOneHalf{ -oneHalf };
	//	constexpr bigrat twoFifths = bigrat::make_rational( 2, 5 );
	//	constexpr bigrat negativeTwoFifths{ -twoFifths };
	//	constexpr bigrat threeFifths = bigrat::make_rational( 3, 5 );
	//	constexpr bigrat negativeThreeFifths{ -threeFifths };

	//	constexpr bigrat twoAndAHalf = two + oneHalf;
	//	constexpr bigrat negativeTwoAndAHalf = -twoAndAHalf;
	//	constexpr bigrat twoAndTwoFifths = two + twoFifths;
	//	constexpr bigrat negativeTwoAndTwoFifths = -twoAndTwoFifths;
	//	constexpr bigrat twoAndThreeFifths = two + threeFifths;
	//	constexpr bigrat negativeTwoAndThreeFifths = -twoAndThreeFifths;


	//	static_assert(round(threeFifths) == one);
	//	static_assert(round(negativeThreeFifths) == -one);
	//	static_assert(round(zero) == zero);
	//	static_assert(round(one) == one);
	//	static_assert(round(negativeOne) == negativeOne);
	//	static_assert(three == round(three));
	//	static_assert(-three == round(negativeThree));
	//	static_assert(round(threeFifths) == one);
	//	static_assert(round(negativeThreeFifths) == -one);
	//	static_assert(round(twoFifths) == zero);
	//	static_assert(round(negativeTwoFifths) == zero);
	//	static_assert(round(twoAndTwoFifths) == two);
	//	static_assert(round(negativeTwoAndTwoFifths) == negativeTwo);
	//	static_assert(round(oneHalf) == one);
	//	static_assert(round(negativeOneHalf) == negativeOne);
	//	static_assert(round(negativeTwoAndAHalf) == negativeThree);
	//	static_assert(round(twoAndAHalf) == three);
	//	static_assert(round(twoAndThreeFifths) == three);
	//	static_assert(round(negativeTwoAndThreeFifths) == negativeThree);
	//	static_assert(round(negativeOne - oneHalf) == negativeTwo);
	//	
	//} 

	//void do_static_rational_type_query_integer_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	constexpr bigrat zero{};
	//	constexpr bigrat one = bigrat::make_rational(1, 1);
	//	constexpr bigrat negativeOne = -bigrat::make_rational(1, 1);
	//	constexpr bigrat two = bigrat::make_rational(2, 1);
	//	constexpr bigrat negativeTwo{ -two };
	//	constexpr bigrat three = bigrat::make_rational(3, 1);
	//	constexpr bigrat negativeThree{ -three };

	//	constexpr bigrat oneHalf{ two.reciprocal() };
	//	constexpr bigrat negativeOneHalf{ -oneHalf };
	//	constexpr bigrat twoFifths = bigrat::make_rational(2, 5);
	//	constexpr bigrat negativeTwoFifths{ -twoFifths };
	//	constexpr bigrat threeFifths = bigrat::make_rational(3, 5);
	//	constexpr bigrat negativeThreeFifths{ -threeFifths };

	//	constexpr bigrat twoAndAHalf = two + oneHalf;
	//	constexpr bigrat negativeTwoAndAHalf = -twoAndAHalf;
	//	constexpr bigrat twoAndTwoFifths = two + twoFifths;
	//	constexpr bigrat negativeTwoAndTwoFifths = -twoAndTwoFifths;
	//	constexpr bigrat twoAndThreeFifths = two + threeFifths;
	//	constexpr bigrat negativeTwoAndThreeFifths = -twoAndThreeFifths;

	//	static_assert(zero.is_integer());
	//	static_assert(one.is_integer() && negativeOne.is_integer());
	//	static_assert(two.is_integer() && negativeTwo.is_integer() && 
	//		!two.is_fraction() && !negativeTwo.is_mixed());
	//	static_assert(three.is_integer() && negativeThree.is_integer());
	//	static_assert(!oneHalf.is_integer() && !negativeOneHalf.is_integer() && oneHalf.is_fraction() 
	//		&& !oneHalf.is_mixed() && !negativeOneHalf.is_mixed() && negativeOneHalf.is_fraction());
	//	static_assert(!twoFifths.is_integer() && !negativeTwoFifths.is_integer());
	//	static_assert(!threeFifths.is_integer() && !negativeThreeFifths.is_integer());
	//	static_assert(!twoAndAHalf.is_integer() && !negativeTwoAndAHalf.is_integer());
	//	static_assert(!twoAndThreeFifths.is_integer() && !negativeTwoAndTwoFifths.is_integer() && 
	//		!negativeTwoAndThreeFifths.is_integer());
	//	static_assert(twoAndThreeFifths.is_mixed() && !twoAndThreeFifths.is_fraction() && 
	//		negativeTwoAndTwoFifths.is_mixed() && !negativeTwoAndTwoFifths.is_integer() && 
	//		!negativeTwoAndTwoFifths.is_fraction());

	//}

	//void do_static_rational_abs_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using bigrat = rational<cjm::numerics::uint128>;
	//	constexpr bigrat zero{};
	//	constexpr bigrat one = bigrat::make_rational( 1, 1 );
	//	constexpr bigrat negativeOne = -bigrat::make_rational( 1, 1);
	//	constexpr bigrat oneHalf = bigrat::make_rational( 1, 2 );
	//	constexpr bigrat negativeOneHalf{ -oneHalf };
	//	constexpr bigrat twoAndAHalf = one + one + oneHalf;
	//	constexpr bigrat negativeTwoAndAHalf = -twoAndAHalf;

	//	static_assert(abs(zero) == zero);
	//	static_assert(abs(negativeOne) == one);
	//	static_assert(abs(one) == one);
	//	static_assert(abs(negativeOneHalf) == oneHalf && 
	//		oneHalf == abs(oneHalf));
	//	static_assert(twoAndAHalf == abs(twoAndAHalf) && 
	//		negativeTwoAndAHalf < abs(negativeTwoAndAHalf) && twoAndAHalf == abs(negativeTwoAndAHalf));
	//}

	//void do_static_rational_whole_and_frac_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	constexpr bigrat zero{};
	//	constexpr auto zeroWf = zero.whole_and_fractional();
	//	constexpr bigrat one = bigrat::make_rational( 1, 1 );
	//	constexpr auto oneWf = one.whole_and_fractional();
	//	constexpr bigrat negativeOne = -bigrat::make_rational( 1, 1);
	//	constexpr auto negOneWf = negativeOne.whole_and_fractional();

	//	constexpr bigrat oneHalf = bigrat::make_rational( 1, 2);
	//	constexpr auto oneHalfWf = oneHalf.whole_and_fractional();
	//	constexpr bigrat negativeOneHalf{ -oneHalf };
	//	constexpr auto negativeOneHalfWf = negativeOneHalf.whole_and_fractional();

	//	constexpr bigrat oneAndAHalf = one + oneHalf;
	//	constexpr auto oneAndAHalfWf = oneAndAHalf.whole_and_fractional();
	//	
	//	static_assert(zeroWf.first + zeroWf.second == zero);
	//	static_assert(one == oneWf.second + oneWf.first);
	//	static_assert(negOneWf.first + negOneWf.second == -one);
	//	static_assert(negativeOneHalfWf.first + negativeOneHalfWf.second == negativeOneHalf);
	//	static_assert(oneHalf == oneHalfWf.first + oneHalfWf.second);
	//	static_assert(-oneAndAHalf == -oneAndAHalfWf.first + -oneAndAHalfWf.second);

	//}

	//void do_static_rational_pow_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using rat = rational<cjm::numerics::uint128>;

	//	constexpr rat zero{};
	//	constexpr rat one = rat::make_rational( 1, 1 );
	//	constexpr rat negOne{ -one };
	//	constexpr rat oneQuarter = rat::make_rational( 1, 4 );
	//	constexpr rat negOneQuarter = -rat::make_rational( 1, 4);
	//	constexpr rat fourAndSevenNinths = rat::make_rational( 43, 9 );
	//	constexpr rat negFourSevNinths = -fourAndSevenNinths;

	//	static_assert(pow(zero, 1) == 0);
	//	static_assert(pow(zero, -1) == 0);
	//	static_assert(pow(zero, 2) == zero);
	//	static_assert(pow(zero, -2) == zero);
	//	static_assert(pow(one, 0) == one);
	//	static_assert(pow(-one, 0) == one);
	//	static_assert(pow(fourAndSevenNinths, 0) == one);
	//	static_assert(pow(one, 1) == one);
	//	static_assert(pow(one, -1) == one);
	//	static_assert(pow(-one, 1) == negOne);
	//	static_assert(pow(-one, -1) == negOne);
	//	static_assert(pow(oneQuarter, -1) == oneQuarter.reciprocal());
	//	static_assert(pow(negOneQuarter, -1) == negOneQuarter.reciprocal());
	//	static_assert(pow(fourAndSevenNinths, -1) == fourAndSevenNinths.reciprocal() &&
	//		pow(fourAndSevenNinths, -1) == rat::make_rational( 9, 43 ));
	//	static_assert(pow(negFourSevNinths, -1) == negFourSevNinths.reciprocal());
	//	static_assert(pow(one * 4, 2) == 16 * one);
	//	static_assert(pow(oneQuarter, -2) == one * 16);
	//	static_assert(pow(fourAndSevenNinths, 3) == rat::make_rational( 79507, 729 ));
	//	static_assert(pow(fourAndSevenNinths, -3) == rat::make_rational( 729, 79507 ));
	//	static_assert(pow(negFourSevNinths, 2) == rat::make_rational( 1849, 81 ));
	//	static_assert(pow(negFourSevNinths, -3) == -rat::make_rational( 729, 79507));
	//	static_assert(pow(negOne, 4) == one);
	//	static_assert(pow(negOne, 5) == negOne);
	//}

	//void do_static_256_lit_test()
	//{
	//	using namespace cjm::numerics::fixed_uint_literals;
	//	using namespace cjm::numerics;
	//	constexpr static uint256_t biggest = 0xFFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF_u256;
	//	constexpr static uint256_t biggestControl = std::numeric_limits<uint256_t>::max();
	//	static_assert(biggest == biggestControl);
	//	constexpr static uint256_t biggestDecimal = 115'792'089'237'316'195'423'570'985'008'687'907'853'269'984'665'640'564'039'457'584'007'913'129'639'935_u256;
	//	static_assert(biggestDecimal == biggestControl);
	//	//The following line should not compile
	//	//constexpr uint256_t tooBigByOne = 115'792'089'237'316'195'423'570'985'008'687'907'853'269'984'665'640'564'039'457'584'007'913'129'639'936_u256;

	//}

	//void do_dynamic_256_lit_test()
	//{
	//	using namespace cjm::numerics::fixed_uint_literals;
	//	using namespace cjm::numerics;
	//	uint256_t biggest = 0xFFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF_u256;
	//	uint256_t biggestControl = std::numeric_limits<uint256_t>::max();
	//	cjm_assert(biggest == biggestControl);
	//	uint256_t biggestDecimal = 115'792'089'237'316'195'423'570'985'008'687'907'853'269'984'665'640'564'039'457'584'007'913'129'639'935_u256;
	//	cjm_assert(biggestDecimal == biggestControl);
	//	cjm_assert_throws<std::domain_error>([]()
	//	{
	//		uint256_t tooBigByOne = 115'792'089'237'316'195'423'570'985'008'687'907'853'269'984'665'640'564'039'457'584'007'913'129'639'936_u256;
	//		std::cerr << "You should never see me!" << tooBigByOne << newl;
	//	});
	//	cjm_assert(biggest + 1 == 0);
	//	cjm_assert(1_u256 - 2 == biggest);
	//	cout << "do_dynamic_256_lit_test PASSES" << newl;		
	//}

	//void do_rational_hash_test()
	//{
	//	using namespace cjm::numerics::rational::rational_literals;
	//	using namespace cjm::numerics::rational;
	//	using rat = rational<std::uint64_t>;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	constexpr std::hash<rat> lilHasher;
	//	constexpr std::hash<bigrat> bigHasher;

	//	constexpr bigrat bigNegativeFiveAndSevenNinths = -bigrat::make_rational(52, 9);
	//	constexpr rat negativeFiveAndSevenNinths = -rat::make_rational(52, 9);
	//	cjm_assert(lilHasher(negativeFiveAndSevenNinths) == lilHasher(-(5 + 7 / 9_r64)));
	//	cjm_assert(bigHasher(bigNegativeFiveAndSevenNinths) == bigHasher(-(5 + 7 / 9_r128)));
	//	cjm_deny(lilHasher(negativeFiveAndSevenNinths) == lilHasher((5 + 7 / 9_r64)));
	//	cjm_deny(bigHasher(bigNegativeFiveAndSevenNinths) == bigHasher((5 + 7 / 9_r128)));

	//	cout << "rational hash test PASSED." << newl;
	//}

	//void do_safe_comparison_test()
	//{
	//	using namespace cjm::numerics::rational::rational_literals;
	//	using namespace cjm::numerics::rational;
	//	using rat = rational<std::uint64_t>;

	//	constexpr auto max = std::numeric_limits<std::uint64_t>::max();
	//	constexpr auto maxLessTen = max - 10;
	//	constexpr auto maxLessEleven = max - 11;
	//	constexpr auto maxLessTwenty = max - 20;
	//	constexpr auto maxLessTwentyOne = max - 21;

	//	rat zero = 0;
	//	rat oneAndHalf = (1 + 1 / 2_r64);
	//	rat oneAndThreeQuarters = (1 + 3 / 4_r64);
	//	rat oneAndSixEighths = (1 + 6 / 8_r64);

	//	rat bigAss1 = rat::make_rational(maxLessEleven, maxLessTen);
	//	rat bigAss2 = rat::make_rational(maxLessTwentyOne, maxLessTwenty);
	//	rat alsoBigAss1 = rat::make_rational(maxLessTwentyOne + 10, maxLessTwenty + 10);

	//	auto compRes = rat::safe_compare(oneAndHalf, oneAndThreeQuarters);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second < 0);
	//	compRes = rat::safe_compare(oneAndThreeQuarters, oneAndHalf);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second > 0);
	//	compRes = rat::safe_compare(oneAndSixEighths, oneAndThreeQuarters);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second == 0);
	//	compRes = rat::safe_compare(-oneAndThreeQuarters, -oneAndHalf);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second < 0);
	//	compRes = rat::safe_compare(-oneAndHalf, -oneAndThreeQuarters);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second > 0);

	//	compRes = rat::safe_compare(-oneAndHalf, oneAndHalf);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second < 0);
	//	compRes = rat::safe_compare(oneAndHalf, -oneAndHalf);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second > 0);
	//	compRes = rat::safe_compare(zero, oneAndHalf);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second < 0);
	//	compRes = rat::safe_compare(zero, -oneAndHalf);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second > 0);
	//	compRes = rat::safe_compare(oneAndHalf, zero);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second > 0);
	//	compRes = rat::safe_compare(-oneAndHalf, zero);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second < 0);
	//	compRes = rat::safe_compare(-zero, oneAndHalf);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second < 0);
	//	compRes = rat::safe_compare(-zero, -oneAndHalf);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second > 0);
	//	compRes = rat::safe_compare(oneAndHalf, -zero);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second > 0);
	//	compRes = rat::safe_compare(-oneAndHalf, -zero);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second < 0);

	//	cjm_assert_throws<std::overflow_error>([=]() -> void
	//	{
	//		bool isGreater = bigAss1 > bigAss2;
	//		cerr << "isGreater: " << isGreater << " you SHOULD NEVER SEE THIS LINE!" << newl;
	//	});
	//	cjm_assert(alsoBigAss1 == bigAss1);
	//	cjm_deny(alsoBigAss1 != bigAss1);

	//	compRes = rat::safe_compare(bigAss1, bigAss2);
	//	cjm_deny(compRes.first);
	//	compRes = rat::safe_compare(bigAss2, bigAss1);
	//	cjm_deny(compRes.first);
	//	
	//	compRes = rat::safe_compare(bigAss1, alsoBigAss1);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second == 0);

	//	compRes = rat::safe_compare(alsoBigAss1, bigAss1);
	//	cjm_assert(compRes.first);
	//	cjm_assert(compRes.second == 0);

	//	cout << "safe_comparison_test PASSED." << newl;
	//}

	//void do_rational_safe_mult_test()
	//{
	//	using namespace cjm::numerics::rational::rational_literals;
	//	using namespace cjm::numerics::rational;
	//	using rat = rational<std::uint64_t>;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	constexpr rat reallySmall = 1_r64 / std::numeric_limits<std::uint64_t>::max();
	//	constexpr rat biggest = reallySmall.reciprocal();
	//	constexpr rat bigFrac = rat::make_rational(std::numeric_limits<std::uint64_t>::max() - 1, 
	//		std::numeric_limits<std::uint64_t>::max());
	//	constexpr rat firstProduct = (75 + 9 / 16_r64) * (131 + 4 / 7_r64);
	//	constexpr rat expectedFirstProduct = (1113489 / 112_r64);
	//	constexpr rat zeroProduct = (75 + 9 / 16_r64) * 0_r64;
	//	constexpr rat oneProduct = 1_r64 * (75 + 9 / 16_r64);
	//	constexpr bigrat bigOneProduct = (75 + 9 / 16_r128) * 1_r128;
	//	constexpr bigrat bigZeroProduct = 0_r128 * (75 + 9 / 16_r128);
	//	constexpr bigrat bigFirstProduct = (75 + 9 / 16_r128) * (131 + 4 / 7_r128);
	//	constexpr bigrat bigExpectedFirstProduct = (1113489 / 112_r128);
	//	static_assert(zeroProduct == 0 && 0 == bigZeroProduct);
	//	static_assert(firstProduct == expectedFirstProduct);
	//	static_assert(bigExpectedFirstProduct == bigFirstProduct);
	//	static_assert(oneProduct == (75 + 9 / 16_r64));
	//	static_assert((75 + 9 / 16_r128) == bigOneProduct);

	//	cjm_assert_throws<std::overflow_error>([=]() -> void
	//	{
	//		auto product = bigFrac * reallySmall;
	//		cerr << "This line should not appear: " << product << newl;
	//	});
	//	cjm_assert_throws<std::overflow_error>([=]() -> void
	//	{
	//		auto product = bigFrac * bigFrac;
	//		cerr << "This line should not appear: " << product << newl;
	//	});
	//	cjm_assert_throws<std::overflow_error>([=]() -> void
	//	{
	//		auto product = reallySmall * reallySmall;
	//		cerr << "This line should not appear: " << product << newl;
	//	});
	//	cjm_assert_throws<std::overflow_error>([=]() -> void
	//	{
	//		auto product = biggest * biggest;
	//		cerr << "This line should not appear: " << product << newl;
	//	});
	//	cjm_assert_throws<std::domain_error>([=]() -> void
	//	{
	//		auto quotient = bigExpectedFirstProduct / 0_r128;
	//		cerr << "This line should not appear: " << quotient << newl;
	//	});
	//	cjm_assert(reallySmall * biggest == 1_r64);

	//	cjm_assert(expectedFirstProduct / (131 + 4 / 7_r64) == (75 + 9 / 16_r64));
	//	cjm_assert(bigExpectedFirstProduct / (75 + 9 / 16_r128) == (131 + 4 / 7_r128));

	//	auto res = rat::safe_multiply((131 + 4 / 7_r64), (75 + 9 / 16_r64));
	//	auto bigRes = bigrat::safe_multiply((75 + 9 / 16_r128), (131 + 4 / 7_r128));
	//	cjm_assert(res.first && res.second == expectedFirstProduct);
	//	cjm_assert(bigRes.first && bigRes.second == bigExpectedFirstProduct);

	//	res = rat::safe_multiply(0_r64, (75 + 9 / 16_r64));
	//	bigRes = bigrat::safe_multiply((131 + 4 / 7_r128), 0_r128);
	//	cjm_assert(res.first && res.second == 0_r64);
	//	cjm_assert(bigRes.first && bigRes.second == 0_r128);

	//	res = rat::safe_multiply((75 + 9 / 16_r64), 1_r64);
	//	bigRes = bigrat::safe_multiply(1_r128, (131 + 4 / 7_r128));
	//	cjm_assert(res.first && res.second == (75 + 9 / 16_r64));
	//	cjm_assert(bigRes.first && bigRes.second == (131 + 4 / 7_r128));

	//	res = rat::safe_multiply(bigFrac, reallySmall);
	//	cjm_deny(res.first);
	//	res = rat::safe_multiply(bigFrac, bigFrac);
	//	cjm_deny(res.first);
	//	res = rat::safe_multiply(reallySmall, reallySmall);
	//	cjm_deny(res.first);
	//	res = rat::safe_multiply(biggest, biggest);
	//	cjm_deny(res.first);
	//	bigRes = bigrat::safe_divide(bigExpectedFirstProduct, 0_r128);
	//	cjm_deny(bigRes.first);
	//	
	//	res = rat::safe_multiply(reallySmall, biggest);
	//	cjm_assert(res.first && res.second == 1_r64);

	//	res = rat::safe_divide(expectedFirstProduct, (75 + 9 / 16_r64));
	//	bigRes = bigrat::safe_divide(bigExpectedFirstProduct, (131 + 4 / 7_r128));
	//	cjm_assert(res.first && res.second == (131 + 4 / 7_r64));
	//	cjm_assert(bigRes.first && bigRes.second == (75 + 9 / 16_r128));

	//	rational_formatter<uint128, char> brf;
	//	cout << (brf << rational_format::mixed_number << (75 + 9 / 16_r128)) << " * " << (brf << ((131 + 4 / 7_r128))) << " == " << (brf << bigExpectedFirstProduct) << newl;


	//	cout << "do_rational_safe_mult_test PASSED" << newl;
	//}

	//void do_static_rational_conversion_test()
	//{
	//	using namespace cjm::numerics::rational::rational_literals;
	//	using namespace cjm::numerics::rational;
	//	using rat = rational<std::uint64_t>;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	constexpr rat zero = 0;
	//	constexpr bigrat bigZero{ zero };
	//	constexpr rat threeAndSevenEighths = 3 + 7 / 8_r64;
	//	constexpr bigrat bigThreeSevenEighths = 3 + 7 / 8_r128;
	//	constexpr rat negThreeAndSevenEighths = -(3 + 7 / 8_r64);
	//	constexpr bigrat negBigThreeSevenEighths = -(3 + 7 / 8_r128);
	//	static_assert(static_cast<bigrat>(zero) == bigZero);
	//	static_assert(static_cast<bigrat>(threeAndSevenEighths) == bigThreeSevenEighths);
	//	static_assert(static_cast<bigrat>(negThreeAndSevenEighths) == negBigThreeSevenEighths);
	//}

	//void do_rational_truncation_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	 bigrat zero{};
	//	 bigrat one = bigrat::make_rational( 1, 1 );
	//	 bigrat negativeOne = -bigrat::make_rational(1, 1);
	//	 bigrat two = bigrat::make_rational( 2, 1 );
	//	 bigrat negativeTwo{ -two };
	//	 bigrat three = bigrat::make_rational( 3, 1 );
	//	 bigrat negativeThree{ -three };

	//	 bigrat oneHalf{ two.reciprocal() };
	//	 bigrat negativeOneHalf{ -oneHalf };
	//	 bigrat twoFifths = bigrat::make_rational( 2, 5 );
	//	 bigrat negativeTwoFifths{ -twoFifths };
	//	 bigrat threeFifths = bigrat::make_rational(3, 5);
	//	 bigrat negativeThreeFifths{ -threeFifths };

	//	 bigrat twoAndAHalf = two + oneHalf;
	//	 bigrat negativeTwoAndAHalf = -twoAndAHalf;
	//	 bigrat twoAndTwoFifths = two + twoFifths;
	//	 bigrat negativeTwoAndTwoFifths = -twoAndTwoFifths;
	//	 bigrat twoAndThreeFifths = two + threeFifths;
	//	 bigrat negativeTwoAndThreeFifths = -twoAndThreeFifths;


	//	cjm_assert(trunc(threeFifths) == zero);
	//	cjm_assert(trunc(negativeThreeFifths) == zero);
	//	cjm_assert(trunc(zero) == zero);
	//	cjm_assert(trunc(one) == one);
	//	cjm_assert(trunc(negativeOne) == negativeOne);
	//	cjm_assert(three == trunc(three));
	//	cjm_assert(-three == trunc(negativeThree));
	//	cjm_assert(trunc(threeFifths) == zero);
	//	cjm_assert(trunc(negativeThreeFifths) == zero);
	//	cjm_assert(trunc(twoFifths) == zero);
	//	cjm_assert(trunc(negativeTwoFifths) == zero);
	//	cjm_assert(trunc(twoAndTwoFifths) == two);
	//	cjm_assert(trunc(negativeTwoAndTwoFifths) == negativeTwo);
	//	cjm_assert(trunc(oneHalf) == zero);
	//	cjm_assert(trunc(negativeOneHalf) == zero);
	//	cjm_assert(trunc(negativeTwoAndAHalf) == negativeTwo);
	//	cjm_assert(trunc(twoAndAHalf) == two);
	//	cjm_assert(trunc(twoAndThreeFifths) == two);
	//	cjm_assert(trunc(negativeTwoAndThreeFifths) == negativeTwo);
	//	cjm_deny(trunc(negativeTwoAndThreeFifths) > negativeTwo);
	//	cjm_deny(trunc(negativeTwoAndThreeFifths) < negativeTwo);
	//	cjm_assert(trunc(negativeTwoAndThreeFifths) >= negativeTwo);
	//	cjm_assert(trunc(negativeTwoAndThreeFifths) <= negativeTwo);
	//	cjm_deny(trunc(negativeTwoAndThreeFifths) != negativeTwo);

	//	cout << "rational truncation tests PASSED" << newl;
	//}

	//void do_rational_floor_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	bigrat zero{};
	//	bigrat one = bigrat::make_rational(1, 1);
	//	bigrat negativeOne = -bigrat::make_rational(1, 1);
	//	bigrat two = bigrat::make_rational(2, 1);
	//	bigrat negativeTwo{ -two };
	//	bigrat three = bigrat::make_rational(3, 1);
	//	bigrat negativeThree{ -three };

	//	bigrat oneHalf{ two.reciprocal() };
	//	bigrat negativeOneHalf{ -oneHalf };
	//	bigrat twoFifths = bigrat::make_rational(2, 5);
	//	bigrat negativeTwoFifths{ -twoFifths };
	//	bigrat threeFifths = bigrat::make_rational(3, 5);
	//	bigrat negativeThreeFifths{ -threeFifths };

	//	bigrat twoAndAHalf = two + oneHalf;
	//	bigrat negativeTwoAndAHalf = -twoAndAHalf;
	//	bigrat twoAndTwoFifths = two + twoFifths;
	//	bigrat negativeTwoAndTwoFifths = -twoAndTwoFifths;
	//	bigrat twoAndThreeFifths = two + threeFifths;
	//	bigrat negativeTwoAndThreeFifths = -twoAndThreeFifths;

	//	cjm_assert(floor(threeFifths) == zero);
	//	cjm_assert(floor(negativeThreeFifths) == negativeOne);
	//	cjm_assert(floor(zero) == zero);
	//	cjm_assert(floor(one) == one);
	//	cjm_assert(floor(negativeOne) == negativeOne);
	//	cjm_assert(three == floor(three));
	//	cjm_assert(-three == floor(negativeThree));
	//	cjm_assert(floor(threeFifths) == zero);
	//	cjm_assert(floor(negativeThreeFifths) == -one);
	//	cjm_assert(floor(twoFifths) == zero);
	//	cjm_assert(floor(negativeTwoFifths) == -one);
	//	cjm_assert(floor(twoAndTwoFifths) == two);
	//	cjm_assert(floor(negativeTwoAndTwoFifths) == negativeThree);
	//	cjm_assert(floor(oneHalf) == zero);
	//	cjm_assert(floor(negativeOneHalf) == negativeOne);
	//	cjm_assert(floor(negativeTwoAndAHalf) == negativeThree);
	//	cjm_assert(floor(twoAndAHalf) == two);
	//	cjm_assert(floor(twoAndThreeFifths) == two);
	//	cjm_assert(floor(negativeTwoAndThreeFifths) == negativeThree);
	//	cjm_assert(floor(negativeOne - oneHalf) == negativeTwo);

	//	cout << "rational floor tests PASSED" << newl;
	//}

	//void do_rational_ceiling_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	bigrat zero{};
	//	bigrat one = bigrat::make_rational(1, 1);
	//	bigrat negativeOne = -bigrat::make_rational(1, 1);
	//	bigrat two = bigrat::make_rational(2, 1);
	//	bigrat negativeTwo{ -two };
	//	bigrat three = bigrat::make_rational(3, 1);
	//	bigrat negativeThree{ -three };

	//	bigrat oneHalf{ two.reciprocal() };
	//	bigrat negativeOneHalf{ -oneHalf };
	//	bigrat twoFifths = bigrat::make_rational(2, 5);
	//	bigrat negativeTwoFifths{ -twoFifths };
	//	bigrat threeFifths = bigrat::make_rational(3, 5);
	//	bigrat negativeThreeFifths{ -threeFifths };

	//	bigrat twoAndAHalf = two + oneHalf;
	//	bigrat negativeTwoAndAHalf = -twoAndAHalf;
	//	bigrat twoAndTwoFifths = two + twoFifths;
	//	bigrat negativeTwoAndTwoFifths = -twoAndTwoFifths;
	//	bigrat twoAndThreeFifths = two + threeFifths;
	//	bigrat negativeTwoAndThreeFifths = -twoAndThreeFifths;

	//	cjm_assert(ceil(threeFifths) == one);
	//	cjm_assert(ceil(negativeThreeFifths) == zero);
	//	cjm_assert(ceil(zero) == zero);
	//	cjm_assert(ceil(one) == one);
	//	cjm_assert(ceil(negativeOne) == negativeOne);
	//	cjm_assert(three == ceil(three));
	//	cjm_assert(-three == ceil(negativeThree));
	//	cjm_assert(ceil(threeFifths) == one);
	//	cjm_assert(ceil(negativeThreeFifths) == zero);
	//	cjm_assert(ceil(twoFifths) == one);
	//	cjm_assert(ceil(negativeTwoFifths) == zero);
	//	cjm_assert(ceil(twoAndTwoFifths) == three);
	//	cjm_assert(ceil(negativeTwoAndTwoFifths) == negativeTwo);
	//	cjm_assert(ceil(oneHalf) == one);
	//	cjm_assert(ceil(negativeOneHalf) == zero);
	//	cjm_assert(ceil(negativeTwoAndAHalf) == negativeTwo);
	//	cjm_assert(ceil(twoAndAHalf) == three);
	//	cjm_assert(ceil(twoAndThreeFifths) == three);
	//	cjm_assert(ceil(negativeTwoAndThreeFifths) == negativeTwo);
	//	cjm_assert(ceil(negativeOne - oneHalf) == negativeOne);

	//	cout << "rational ceiling tests PASSED" << newl;
	//}

	//void do_rational_round_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	bigrat zero{};
	//	bigrat one = bigrat::make_rational(1, 1);
	//	bigrat negativeOne = -bigrat::make_rational(1, 1);
	//	bigrat two = bigrat::make_rational(2, 1);
	//	bigrat negativeTwo{ -two };
	//	bigrat three = bigrat::make_rational(3, 1);
	//	bigrat negativeThree{ -three };

	//	bigrat oneHalf{ two.reciprocal() };
	//	bigrat negativeOneHalf{ -oneHalf };
	//	bigrat twoFifths = bigrat::make_rational(2, 5);
	//	bigrat negativeTwoFifths{ -twoFifths };
	//	bigrat threeFifths = bigrat::make_rational(3, 5);
	//	bigrat negativeThreeFifths{ -threeFifths };

	//	bigrat twoAndAHalf = two + oneHalf;
	//	bigrat negativeTwoAndAHalf = -twoAndAHalf;
	//	bigrat twoAndTwoFifths = two + twoFifths;
	//	bigrat negativeTwoAndTwoFifths = -twoAndTwoFifths;
	//	bigrat twoAndThreeFifths = two + threeFifths;
	//	bigrat negativeTwoAndThreeFifths = -twoAndThreeFifths;


	//	cjm_assert(round(threeFifths) == one);
	//	cjm_assert(round(negativeThreeFifths) == -one);
	//	cjm_assert(round(zero) == zero);
	//	cjm_assert(round(one) == one);
	//	cjm_assert(round(negativeOne) == negativeOne);
	//	cjm_assert(three == round(three));
	//	cjm_assert(-three == round(negativeThree));
	//	cjm_assert(round(threeFifths) == one);
	//	cjm_assert(round(negativeThreeFifths) == -one);
	//	cjm_assert(round(twoFifths) == zero);
	//	cjm_assert(round(negativeTwoFifths) == zero);
	//	cjm_assert(round(twoAndTwoFifths) == two);
	//	cjm_assert(round(negativeTwoAndTwoFifths) == negativeTwo);
	//	cjm_assert(round(oneHalf) == one);
	//	cjm_assert(round(negativeOneHalf) == negativeOne);
	//	cjm_assert(round(negativeTwoAndAHalf) == negativeThree);
	//	cjm_assert(round(twoAndAHalf) == three);
	//	cjm_assert(round(twoAndThreeFifths) == three);
	//	cjm_assert(round(negativeTwoAndThreeFifths) == negativeThree);
	//	cjm_assert(round(negativeOne - oneHalf) == negativeTwo);

	//	cout << "rational round tests PASSED" << newl;
	//}

	//void do_rational_pow_test()
	//{
	//	using namespace cjm::numerics::rational::rational_literals;
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using rat = rational<cjm::numerics::uint128>;

	//	constexpr rat zero{};
	//	constexpr rat one = rat::make_rational( 1, 1 );
	//	constexpr rat negOne{ -one };
	//	constexpr rat oneQuarter = rat::make_rational( 1, 4 );
	//	constexpr rat negOneQuarter = -rat::make_rational( 1, 4 );
	//	constexpr rat fourAndSevenNinths = rat::make_rational( 43, 9 );
	//	constexpr rat negFourSevNinths = -fourAndSevenNinths;
	//	

	//	cjm_assert_throws<std::domain_error>([=] () -> void
	//	{
	//		auto result = pow(zero, 0);
	//		cerr << "You should not see this result: " << result.hash_code() << newl;
	//	});
	//	auto res = safe_pow(zero, 0);
	//	cjm_deny(res.first);


	//	cjm_assert(pow(zero, 1) == 0 && safe_pow(zero, 1).second == 0);
	//	cjm_assert(pow(zero, -1) == 0);
	//	cjm_assert(pow(zero, 2) == zero);
	//	cjm_assert(pow(zero, -2) == zero);
	//	cjm_assert(pow(one, 0) == one);
	//	cjm_assert(pow(-one, 0) == one);
	//	cjm_assert(pow(fourAndSevenNinths, 0) == one);
	//	cjm_assert(pow(one, 1) == one);
	//	cjm_assert(pow(one, -1) == one);
	//	cjm_assert(pow(-one, 1) == negOne);
	//	cjm_assert(pow(-one, -1) == negOne);
	//	cjm_assert(pow(oneQuarter, -1) == oneQuarter.reciprocal());
	//	cjm_assert(pow(negOneQuarter, -1) == negOneQuarter.reciprocal());
	//	cjm_assert(pow(fourAndSevenNinths, -1) == fourAndSevenNinths.reciprocal() && 
	//		pow(fourAndSevenNinths, -1) == rat::make_rational( 9, 43 ));
	//	cjm_assert(pow(negFourSevNinths, -1) == negFourSevNinths.reciprocal());
	//	cjm_assert(pow(one * 4, 2) == 16 * one);
	//	cjm_assert(pow(oneQuarter, -2) == one * 16);
	//	cjm_assert(pow(fourAndSevenNinths, 3) == rat::make_rational(79507, 729));
	//	cjm_assert(pow(fourAndSevenNinths, -3) == rat::make_rational( 729, 79507 ));
	//	cjm_assert(pow(negFourSevNinths, 2) == rat::make_rational( 1849, 81 ));
	//	cjm_assert(pow(negFourSevNinths, -3) == -rat::make_rational( 729, 79507));
	//	cjm_assert(pow(negOne, 4) == one);
	//	cjm_assert(pow(negOne, 5) == negOne);

	//	res = safe_pow(zero, -1);
	//	cjm_assert(res.first && res.second == 0);
	//	res = safe_pow(zero, 2);
	//	cjm_assert(res.first && res.second == zero);
	//	res = safe_pow(zero, -2);
	//	cjm_assert(res.first && res.second == zero);
	//	res = safe_pow(one, 0);
	//	cjm_assert(res.first && res.second == one);
	//	res = safe_pow(-one, 0);
	//	cjm_assert(res.first && res.second == one);
	//	res = safe_pow(fourAndSevenNinths, 0);
	//	cjm_assert(res.first && res.second == one);
	//	res = safe_pow(one, 1);
	//	cjm_assert(res.first && res.second == one);
	//	res = safe_pow(one, -1);
	//	cjm_assert(res.first && res.second == one);
	//	res = safe_pow(-one, 1);
	//	cjm_assert(res.first && res.second == -one);
	//	res = safe_pow(-one, -1);
	//	cjm_assert(res.first && res.second == -one);
	//	res = safe_pow(oneQuarter, -1);
	//	cjm_assert(res.first && res.second == oneQuarter.reciprocal());
	//	res = safe_pow(fourAndSevenNinths, -1);
	//	cjm_assert(res.first && res.second == fourAndSevenNinths.reciprocal() && fourAndSevenNinths.reciprocal() == (9 /43_r128));
	//	res = safe_pow(4_r128, 2);
	//	cjm_assert(res.first && res.second == 16_r128);
	//	res = safe_pow(oneQuarter, -2);
	//	cjm_assert(res.first && res.second == 16_r128);
	//	res = safe_pow(fourAndSevenNinths, 3);
	//	cjm_assert(res.first && res.second == (79507 / 729_r128));
	//	res = safe_pow(fourAndSevenNinths, -3);
	//	cjm_assert(res.first && res.second == (729 / 79507_r128));
	//	res = safe_pow(negFourSevNinths, 2);
	//	cjm_assert(res.first && res.second == (1849 / 81_r128));
	//	res = safe_pow(negFourSevNinths, -3);
	//	cjm_assert(res.first && res.second == -(729 / 79507_r128));
	//	res = safe_pow(negOne, 4);
	//	cjm_assert(res.first && res.second == 1_r128);
	//	res = safe_pow(negOne, 5);
	//	cjm_assert(res.first && res.second == -1_r128);

	//	static constexpr rat tinLilFrac = 1_r128 / std::numeric_limits<uint128>::max();
	//	static constexpr rat biggestPos = tinLilFrac.reciprocal();

	//	cjm_assert_throws<std::overflow_error>([=]() -> void
	//	{
	//		auto squared = pow(tinLilFrac, 2);
	//		cerr << "You should not see me: " << squared << newl;
	//	});

	//	cjm_assert_throws<std::overflow_error>([=]() -> void
	//	{
	//		auto squared = pow(biggestPos, 2);
	//		cerr << "You should not see me: " << squared << newl;
	//	});
	//	cjm_assert_throws<std::overflow_error>([=]() -> void
	//	{
	//		auto squared = pow(5_r128, 10'000'000);
	//		cerr << "You should not see me: " << squared << newl;
	//	});
	//	cjm_assert(pow(tinLilFrac, -1) == biggestPos);

	//	res = safe_pow(tinLilFrac, 2);
	//	cjm_deny(res.first);

	//	res = safe_pow(biggestPos, 2);
	//	cjm_deny(res.first);

	//	res = safe_pow(5_r128, 10'000'000);
	//	cjm_deny(res.first);

	//	res = safe_pow(tinLilFrac, -1);
	//	cjm_assert(res.first && res.second == biggestPos);

	//	cout << "rational_pow_test PASSED" << newl;

	//}

	//void do_rational_to_float_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using rat = rational<cjm::numerics::uint128>;

	//	auto eqCompare = [](long double x, long double y) -> bool
	//	{
	//		static constexpr long double epsilon{ 0.0001 };
	//		auto diff = std::abs(x - y);
	//		return diff < epsilon;
	//	};
	//			
	//	constexpr rat oneQuarter = rat::make_rational(1, 4 );
	//	constexpr rat negOneQuarter = -rat::make_rational( 1, 4 );
	//	constexpr rat fourAndSevenNinths = rat::make_rational(43, 9);
	//	constexpr rat oneThird = rat::make_rational( 1, 3 );
	//	
	//	cjm_assert(eqCompare(0.25, static_cast<long double>(oneQuarter)));
	//	cjm_deny(eqCompare(0.25, static_cast<long double>(-oneQuarter)));
	//	cjm_assert(eqCompare(-0.25, static_cast<long double>(negOneQuarter)));
	//	cjm_assert(eqCompare(0.3333, static_cast<long double>(oneThird)));
	//	cjm_assert(eqCompare(-0.3333, static_cast<long double>(-oneThird)));
	//	cjm_deny(eqCompare(-0.333, static_cast<long double>(-oneThird)));
	//	cjm_assert(eqCompare(static_cast<long double>(fourAndSevenNinths), 4.7778));
	//	cjm_deny(eqCompare(static_cast<long double>(fourAndSevenNinths), 4.7779));
	//	cjm_deny(eqCompare(static_cast<long double>(fourAndSevenNinths), -4.7778));
	//	cjm_assert(eqCompare(static_cast<long double>(fourAndSevenNinths.reciprocal()), 1 / 4.777777));
	//	cout << "rational to float tests PASS" << newl;
	//}

	//void do_rational_conversion_test()
	//{
	//	using namespace cjm::numerics::rational::rational_literals;
	//	using namespace cjm::numerics::rational;
	//	using rat = rational<std::uint64_t>;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	rat zero = 0;
	//	bigrat bigZero{ zero };
	//	rat threeAndSevenEighths = 3 + 7 / 8_r64;
	//	bigrat bigThreeSevenEighths = 3 + 7 / 8_r128;
	//	rat negThreeAndSevenEighths = -(3 + 7 / 8_r64);
	//	bigrat negBigThreeSevenEighths = -(3 + 7 / 8_r128);
	//	
	//	cjm_assert(static_cast<bigrat>(zero) == bigZero);
	//	cjm_assert(static_cast<bigrat>(threeAndSevenEighths) == bigThreeSevenEighths);
	//	cjm_assert(static_cast<bigrat>(negThreeAndSevenEighths) == negBigThreeSevenEighths);
	//	cout << "rational conversion tests PASS." << newl;
	//}

	//void do_rational_print_float_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using rat = rational<cjm::numerics::uint128>;
	//	using formatter = rational_formatter<cjm::numerics::uint128, char>;
	//	formatter rf;
	//	constexpr rat fourAndSevenNinths = rat::make_rational( 43, 9 );
	//	constexpr long double floatV = 4.777777;

	//	constexpr rat zero{};
	//	constexpr rat two = rat::make_rational( 2, 1 );
	//	constexpr rat oneHalf = two.reciprocal();
	//	constexpr rat oneThird = rat::make_rational( 1, 3 );

	//	cout << "floatV: " << floatV << newl;
	//	cout << "fourSevenNinths: " << fourAndSevenNinths << newl;

	//	//rf << rational_format::floating_point;
	//	

	//	cout << "as float: " << (rf << rational_format::floating_point << fourAndSevenNinths) << newl;
	//	cout << "as ratio: " << (rf << rational_format::ratio << fourAndSevenNinths) << newl;
	//	cout << "as mixed number: " << (rf << rational_format::mixed_number << fourAndSevenNinths) << newl;

	//	cout << "as float: " << (rf << rational_format::floating_point << -fourAndSevenNinths) << newl;
	//	cout << "as ratio: " << (rf << rational_format::ratio << -fourAndSevenNinths) << newl;
	//	cout << "as mixed number: " << (rf << rational_format::mixed_number << -fourAndSevenNinths) << newl;

	//	cout << "as float: " << (rf << rational_format::floating_point << zero) << newl;
	//	cout << "as ratio: " << (rf << rational_format::ratio << zero) << newl;
	//	cout << "as mixed number: " << (rf << rational_format::mixed_number << zero) << newl;

	//	cout << "as float: " << (rf << rational_format::floating_point << -zero) << newl;
	//	cout << "as ratio: " << (rf << rational_format::ratio << -zero) << newl;
	//	cout << "as mixed number: " << (rf << rational_format::mixed_number << -zero) << newl;

	//	cout << "as float: " << (rf << rational_format::floating_point << two) << newl;
	//	cout << "as ratio: " << (rf << rational_format::ratio << two) << newl;
	//	cout << "as mixed number: " << (rf << rational_format::mixed_number << two) << newl;

	//	cout << "as float: " << (rf << rational_format::floating_point << -two) << newl;
	//	cout << "as ratio: " << (rf << rational_format::ratio << -two) << newl;
	//	cout << "as mixed number: " << (rf << rational_format::mixed_number << -two) << newl;

	//	cout << "as float: " << (rf << rational_format::floating_point << oneHalf) << newl;
	//	cout << "as ratio: " << (rf << rational_format::ratio << oneHalf) << newl;
	//	cout << "as mixed number: " << (rf << rational_format::mixed_number << oneHalf) << newl;

	//	cout << "as float: " << (rf << rational_format::floating_point << -oneHalf) << newl;
	//	cout << "as ratio: " << (rf << rational_format::ratio << -oneHalf) << newl;
	//	cout << "as mixed number: " << (rf << rational_format::mixed_number << -oneHalf) << newl;

	//	cout << "as float: " << (rf << rational_format::floating_point << oneThird) << newl;
	//	cout << "as ratio: " << (rf << rational_format::ratio << oneThird) << newl;
	//	cout << "as mixed number: " << (rf << rational_format::mixed_number << oneThird) << newl;

	//	cout << "as float: " << (rf << rational_format::floating_point << -oneThird) << newl;
	//	cout << "as ratio: " << (rf << rational_format::ratio << -oneThird) << newl;
	//	cout << "as mixed number: " << (rf << rational_format::mixed_number << -oneThird) << newl;

	//	cout << "using string conversion method: " << rf.str() << newl;

	//	cout << "do_rational_print_float_test PASSED" << newl;
	//}

	//void do_float_comparer_test()
	//{
	//	using fc = cjm::numerics::float_comparer<long double, -4>;
	//	
	//	long double x = 0.001;
	//	long double y = 0.001;
	//	long double z = 12.01;
	//	long double w = 0.0001;
	//	long double u = 0.00011;
	//	long double v = 0.0011;
	//	long double s = 0.0012;
	//	long double t = 0.002;

	//	cjm_assert(fc::compare(x, y) == 0);
	//	cout << "do float comparer tests PASS." << newl;

	//}

	void do_trim_tests()
	{
		using std::string;
		using std::wstring;
		using namespace cjm::string;

		const string nempty = "";
		const wstring wempty = L"";
		const string nwhitespace = " ";
		const wstring wwhitespace = L" ";
		const string nlotsawhitespace =  "     ";
		const wstring wlotsawhitespace = L"     ";

		wstring toBeMovedWide = L"   This is the season of our discontent.  ";
		string toBeMovedNarrow = "   This is the season of our discontent.  ";

		string expectedNarrow = "This is the season of our discontent.";
		wstring expectedWide = L"This is the season of our discontent.";

		string testNarrow1 = "This is the season of our discontent. ";
		string testNarrow2 = " This is the season of our discontent.";
		string testNarrow3 = expectedNarrow;
		string testNarrow4 = "   This is the season of our discontent.  ";

		wstring testWide1 = L"This is the season of our discontent. ";
		wstring testWide2 = L" This is the season of our discontent.";
		wstring testWide3 = expectedWide;
		wstring testWide4 = L"   This is the season of our discontent.  ";

		string narrowResult1 = trim(testNarrow1);
		string narrowResult2 = trim(testNarrow2);
		string narrowResult3 = trim(testNarrow3);
		string narrowResult4 = trim(testNarrow4);

		wstring wideResult1 = trim(testWide1);
		wstring wideResult2 = trim(testWide2);
		wstring wideResult3 = trim(testWide3);
		wstring wideResult4 = trim(testWide4);

		string number = " 1,000,069,000  ";
		string expectedNumber = "1000069000";

		wstring wnumber = L" 1,000,069,000  ";
		wstring wexpectedNumber = L"1000069000";
		
		cjm_deny(expectedNarrow == testNarrow1 || expectedNarrow == testNarrow2 || expectedNarrow == testNarrow4);
		cjm_assert(expectedNarrow == testNarrow3);

		cjm_deny(expectedWide == testWide1 || expectedWide == testWide2 || expectedWide == testWide4);
		cjm_assert(expectedWide == testWide3);

		cjm_assert(expectedNarrow == narrowResult1 && expectedNarrow == narrowResult2 && expectedNarrow == narrowResult3 && expectedNarrow == narrowResult4);
		cjm_assert(expectedWide == wideResult1 && expectedWide == wideResult2 && expectedWide == wideResult3 && expectedWide == wideResult4);

		cjm_assert(cjm::numerics::u128_parsing_helper<char>::trim_and_strip(number) == expectedNumber);
		cjm_assert(cjm::numerics::u128_parsing_helper<wchar_t>::trim_and_strip(wnumber) == wexpectedNumber);

		cjm_assert(trim(nempty).empty());
		cjm_assert(trim(wempty).empty());
		cjm_assert(trim(nwhitespace).empty());
		cjm_assert(trim(wwhitespace).empty());
		cjm_assert(trim(nlotsawhitespace).empty());
		cjm_assert(trim(wlotsawhitespace).empty());
		cjm_assert(trim(std::move(toBeMovedWide)) == expectedWide);
		cjm_assert(trim(std::move(toBeMovedNarrow)) == expectedNarrow);
		cout << "do_trim_tests PASSES." << newl;

	}

	//void do_static_uint256_basic_tests()
	//{
	//	using uint256 = cjm::numerics::uint256_t;
	//	using namespace cjm::numerics::fixed_uint_literals;
	//	constexpr uint256 zero{ 0 };
	//	constexpr uint256 biggest = std::numeric_limits<uint256>::max();
	//	constexpr uint256 biggestLessOne = biggest - 1;
	//	static_assert(biggestLessOne + 1 == biggest);
	//	static_assert(biggestLessOne != 0);
	//	static_assert(zero < 1 && 1 > zero && zero <= 1 && 1 >= zero && 1 != zero && 0 == zero && zero == 0 && zero + 1 == 1);
	//}

	//void do_find_smallest_safe_test()
	//{
	//	using namespace cjm::numerics::rational::rational_literals;
	//	using namespace cjm::numerics::rational;
	//	using rat = rational<std::uint64_t>;
	//	using bigrat = rational<cjm::numerics::uint128>;

	//	uint64_t max64 = std::numeric_limits<uint64_t>::max();
	//	uint32_t max32 = std::numeric_limits<uint32_t>::max();

	//	rat rat64SmallerThanEpsilon = rat::make_rational(1, static_cast<uint64_t>(max32) + 1);
	//	rat rat64SmallerStill = rat::make_rational(1, rat64SmallerThanEpsilon.denominator() + 1);
	//	rat rat64Epsilon = rat::make_rational(1, max32);
	//	bigrat rat128Epsilon = bigrat::make_rational(1, max64);
	//	bigrat rat128SmallerThanEpsilon = bigrat::make_rational(1, static_cast<cjm::numerics::uint128>(max64) + 1);
	//	bigrat rat128SmallerStill = bigrat::make_rational(1, rat128SmallerThanEpsilon.denominator() + 1);

	//	cjm_assert_throws<std::overflow_error>([=] () -> void
	//	{
	//		rat shouldOverflow64 = rat64SmallerThanEpsilon + rat64SmallerStill;
	//		cerr << "You should never see this value: " << shouldOverflow64 << newl;
	//	});
	//	cjm_assert_throws<std::overflow_error>([=]() -> void
	//	{
	//		bigrat shouldOverflow128 = rat128SmallerThanEpsilon + rat128SmallerStill;
	//		cerr << "You should never see this value: " << shouldOverflow128 << newl;
	//	});		

	//	using big_formatter = rational_formatter<cjm::numerics::uint128, char>;
	//	using formatter = rational_formatter<uint64_t, char>;

	//	formatter rf;
	//	big_formatter big_rf;

	//	cout << "rat64Epsilon: " << (rf << rational_format::mixed_number << rat64Epsilon) << newl;
	//	cout << "rat128Epsilon: " << (big_rf << rational_format::mixed_number << rat128Epsilon) << newl;

	//	cout << "rat64Epsilon + 1: = " << (rf << rat64Epsilon + 1) << newl;
	//	cout << "rat128Epsilon + 1 : = " << (big_rf << rat128Epsilon + 1) << newl;

	//	cout << "rat64SmallerThanEpsilon: " << (rf << rat64SmallerThanEpsilon) << newl;
	//	cout << "rat128SmallerThanEpsilon: " << (big_rf << rat128SmallerThanEpsilon) << newl;

	//	cout << "do_find_smallest_safe_test_passed!" << newl;
	//}

	//void do_static_rational_test()
	//{
	//	using namespace cjm::numerics::uint128_literals;
	//	using namespace cjm::numerics::rational;
	//	using rat = rational<std::uint64_t>;
	//	using bigrat = rational<cjm::numerics::uint128>;
	//	constexpr rat fiveTwentySevenths = rat::make_rational( 5, 27 );
	//	constexpr rat negFivTS = -rat::make_rational( 5, 27);
	//	constexpr rat sixTwentyNinths = rat::make_rational( 6, 29 );
	//	constexpr rat negSixTwN = -rat::make_rational( 6, 29);
	//	constexpr rat alsoFiveTwentySevents = rat::make_rational( 10, 54 );
	//	constexpr bigrat bigfts = bigrat::make_rational( 5, 27 );
	//	constexpr rat d00d = 12;
	//	constexpr rat ohD00dz = -12;
	//	constexpr rat noD00dz;
	//	constexpr rat uNoD00dz = 0u;
	//	constexpr rat uD00d = 12u;
	//	constexpr auto twelveBigones{ 12_u128 };
	//	constexpr bigrat big_d00d =twelveBigones;
	//	constexpr bigrat big_ohD00dz{ -12 };
	//	constexpr bigrat big_noD00dz{};
	//	constexpr bigrat big_uNoD00dz{ 0u };
	//	constexpr bigrat big_uD00d{ 12u };


	//	static_assert(uD00d == d00d && -uD00d == ohD00dz && uNoD00dz == noD00dz &&
	//		uD00d > ohD00dz && ohD00dz < d00d);
	//	static_assert(noD00dz == static_cast<rat>(0));
	//	static_assert(d00d > ohD00dz);
	//	static_assert(ohD00dz < d00d);
	//	static_assert(d00d == -ohD00dz);
	//	static_assert(big_uD00d == big_d00d && -big_uD00d == big_ohD00dz &&
	//		big_uNoD00dz == big_noD00dz && big_uD00d > big_ohD00dz && big_ohD00dz < big_d00d);
	//	static_assert(noD00dz == static_cast<rat>(0));
	//	static_assert(d00d > ohD00dz);
	//	static_assert(ohD00dz < d00d);
	//	static_assert(d00d == -ohD00dz);

	//	static_assert(fiveTwentySevenths != negFivTS);
	//	static_assert(sixTwentyNinths != fiveTwentySevenths);
	//	static_assert(negSixTwN != negFivTS);
	//	static_assert(negFivTS != sixTwentyNinths);
	//	static_assert(negFivTS != alsoFiveTwentySevents);
	//	static_assert(sixTwentyNinths != alsoFiveTwentySevents);
	//	static_assert(negSixTwN != alsoFiveTwentySevents);
	//	static_assert(fiveTwentySevenths == alsoFiveTwentySevents);
	//	// ReSharper disable CppIdenticalOperandsInBinaryExpression
	//	static_assert(+fiveTwentySevenths == +fiveTwentySevenths); //NOLINT
	//	static_assert(+fiveTwentySevenths == +alsoFiveTwentySevents);
	//	static_assert(-fiveTwentySevenths == -fiveTwentySevenths); //NOLINT
	//	// ReSharper restore CppIdenticalOperandsInBinaryExpression
	//	static_assert(-fiveTwentySevenths == -alsoFiveTwentySevents);
	//	static_assert(sixTwentyNinths == -negSixTwN);
	//	static_assert(bigfts == +bigfts);
	//	static_assert(-bigfts != bigfts);
	//	static_assert(-bigfts == +(-bigfts));
	//}

	//constexpr bool do_static_div_tests()
	//{
	//	using namespace cjm::spatial_measurement::volume_literals;
	//	using namespace cjm::spatial_measurement::area_literals;
	//	using namespace cjm::spatial_measurement::length_literals;
	//	using namespace cjm::spatial_measurement;
	//	static_assert(4_in / 2 == 2_in);
	//	static_assert(10_m / 5 == 200_cm);
	//	static_assert(length_cast<inches>(1_ft) / 2 == 6_in);
	//	static_assert(4_in / 2_in == 2);
	//	static_assert(20_cm2 / 2_cm == 10_cm);
	//	static_assert(20_cm2 / 20_cm2 == 1);
	//	static_assert(1000_cm3 / 10_cm == 100_cm2);
	//	static_assert(1000_cm3 / 100_cm2 == 10_cm);
	//	static_assert(1000_cm3 / 2_cm3 == 500);
	//	return true;
	//}

	//constexpr bool do_are_same_test()
	//{
	//	using iu = cjm::spatial_measurement::inchesunit;
	//	using identicaltoinches_unit = std::ratio<2540, 100'000>;
	//	static_assert(iu::num == identicaltoinches_unit::num && iu::den == identicaltoinches_unit::den);
	//	using same_as_inches = cjm::spatial_measurement::spatial_measure<cjm::spatial_measurement::standardrep, identicaltoinches_unit, 1>;
	//	using same_as_sq_inches = cjm::spatial_measurement::spatial_measure<cjm::spatial_measurement::standardrep, identicaltoinches_unit, 2>;
	//	using same_as_cu_inches = cjm::spatial_measurement::spatial_measure<cjm::spatial_measurement::standardrep, identicaltoinches_unit, 3>;
	//	using namespace cjm::spatial_measurement::volume_literals;
	//	using namespace cjm::spatial_measurement::area_literals;
	//	using namespace cjm::spatial_measurement::length_literals;
	//	using namespace cjm::spatial_measurement;
	//	static_assert(!are_meas_units_and_dimensions_same_v<inches, centimeters>, "Should not be true.");
	//	static_assert(!are_meas_units_and_dimensions_same_v<inches, inches2>, "Should not be true.");
	//	static_assert(are_meas_units_and_dimensions_same_v<inches, same_as_inches>, "Should be true.");
	//	static_assert(are_meas_units_and_dimensions_same_v<inches2, same_as_sq_inches>, "Should be true.");
	//	static_assert(are_meas_units_and_dimensions_same_v<inches3, same_as_cu_inches>, "Should be true.");

	//	return true;
	//}

	//constexpr bool do_simple_squaring_tests()
	//{
	//	using namespace cjm::spatial_measurement::volume_literals;
	//	using namespace cjm::spatial_measurement::area_literals;
	//	using namespace cjm::spatial_measurement::length_literals;
	//	using namespace cjm::spatial_measurement;
	//	static_assert(4_cm * 3_cm == 12_cm2);
	//	static_assert(5_ft * 6_ft == 30_ft2);
	//	static_assert(12_cm2 * 3_cm == 36_cm3, "12_cm2 * 3_cm == 36_cm3");
	//	return true;
	//}

	//bool do_floating_point_tests()
	//{
	//	using namespace cjm::spatial_measurement::volume_literals;
	//	using namespace cjm::spatial_measurement::area_literals;
	//	using namespace cjm::spatial_measurement::length_literals;
	//	using namespace cjm::spatial_measurement;
	//	inches_bf twelve = 1.0_ftbf;
	//	cout << "twelve inches bf: " << twelve.count();
	//	static_assert(12.0_inbf == 1.0_ftbf);
	//	return true;
	//}

	void do_google_literal_test();
	

	int main()
	{
		int ret;
		try
		{
			static constexpr size_t numComparisonTests = NUMBER_BINARY_OP256_TESTS;
			using namespace cjm::numerics::uint128_literals;
			static constexpr size_t bitness = sizeof(size_t) * CHAR_BIT;
			cout << "Hello, world!" << newl;
			cout << std::dec << "This is a " << bitness << " bit build." << std::hex << newl;
			cout << "Starting string round trip tests." << newl;
			do_string_round_trip_tests();
			cout << "starting do_fixed_uint_from_u32str_test" << newl;
			do_fixed_uint_from_u32str_test();
			cout << "starting do_fixed_uint_from_u16str_test" << newl;
			do_fixed_uint_from_u16str_test();
			cout << "starting do_fixed_uint_from_wstr_test " << newl;
			do_fixed_uint_from_wstr_test();
			cout << "starting do_fixed_uint_from_str_test" << newl;
			do_fixed_uint_from_str_test();
			cout << "starting do_u256_big_little_endian_array_test" << newl;
			do_u256_big_little_endian_array_test();
			cout << "starting perform_unary_op_u256_tests" << newl;
			perform_unary_op_u256_tests(numComparisonTests);
			cout << "starting perform_comparison_u256_tests with " << std::dec << numComparisonTests << " tests." << std::hex << newl;
			perform_comparison_u256_tests(numComparisonTests);
			cout << "starting do_binary_random_op_test_u256 " << newl;
			do_binary_random_op_test_u256();
			cout << "starting do_u256_random_bin_op_test() with "<< std::dec << NUMBER_BINARY_OP256_TESTS << " elements." << std::hex << newl;
			do_u256_random_bin_op_test();
			/*cout << "starting do_dynamic_256_lit_test " << newl;
			do_dynamic_256_lit_test();*/
			cout << "starting do_u256_literal_printing_test" << newl;
			do_u256_literal_printing_test();
			cout << "Starting wacky subtraction test: " << newl;
			do_wacky_subtraction_test();
			cout << "starting lshift test" << newl;
			do_256_bit_lshift_test();
			cout << "starting instrumented div mod test 128 by 16." << newl;
			do_intrumented_uint128_div_mod_test_by16();
			cout << "starting instrumented div mod 128 test 1." << newl;
			do_instrumented_uint128_div_mod_test_1();
			cout << "starting instrumented div mod 256 by 16 test 1" << newl;
			do_intrumented_uint256_div_mod_test_by16_1();

			cout << "starting do_random_boost_uint_mult_tests" << newl;
			do_random_boost_uint_mult_test();
			cout << "starting do_random_boost_uint_div_128_tests " << newl;
			do_random_boost_uint_div_128_test();
			cout << "starting do_random_boost_uint_div_tests" << newl;
			do_random_boost_uint_div_test();
			cout << "starting do_divide_by_16_tests" << newl;
			do_divide_by_16_test();
			cout << "Starting do_comparative_hex_modulus_tests" << newl;
			do_comparative_hex_modulus_tests();
			cout << "starting boost uint pair gen tests" << newl;
			do_boost_uint_pair_gen_tests();
			cout << "Starting do_boost_uint_comparison_tests" << newl;
			do_boost_uint_comparison_tests();
			cout << "Starting little endian array test" << newl;
			do_little_endian_array_tests();
			cout << "Starting fixed_uint_out test" << newl;
			do_fixed_uint_out_test();
			cout << "starting safe rat mult tests" << newl;
			/*do_rational_safe_mult_test();
			cout << "starting safe rational compariosn tests" << newl;
			do_safe_comparison_test();
			cout << "starting rational hash tests" << newl;
			do_rational_hash_test();
			cout << "starting find epsilon tests" << newl;
			do_find_smallest_safe_test();
			cout << "starting rational conversion tests" << newl;
			do_rational_conversion_test();*/
			cout << "starting trim tests" << newl;
			do_trim_tests();
			/*cout << "starting print rational float test" << newl;
			do_rational_print_float_test();
			cout << "starting rational float tests" << newl;
			do_rational_to_float_test();
			cout << "starting rational test" << newl;
			do_rational_test();
			cout << "starting second rational test" << newl;
			do_rational_test_2();
			cout << "starting rational addition test" << newl;
			do_rational_add_test();
			cout << "starting rational mult tests: " << newl;
			do_rational_mult_test();
			cout << "starting rational trunaction tests: " << newl;
			do_rational_truncation_test();
			cout << "starting rational floor tests: " << newl;
			do_rational_floor_test();
			cout << "starting rational ceiling tests: " << newl;
			do_rational_ceiling_test();
			cout << "starting rational rounding tests: " << newl;
			do_rational_round_test();
			cout << "starting rational pow tests: " << newl;
			do_rational_pow_test();*/
			cout << "starting floor log2 tests." << newl;
			do_floor_log2_tests();
			cout << "starting boost literal test." << newl;
			do_boost_literal_test();
			cout << "starting Google literal test." << newl;
			do_google_literal_test();
			cout << "starting threw wrong thing test." << newl;
			do_threw_wrong_thing_test();
			uint128_t bigAssInt = get_bigass_int();
			uint128_t bigAssTimes3 = bigAssInt * 3;
			uint128_t backToBigAss = bigAssTimes3 / 3;
			//static_assert(std::is_arithmetic_v<decimal>);
			cjm_assert(bigAssTimes3 != bigAssInt);
			cjm_assert(backToBigAss == bigAssInt);
			cout << "starting google test." << newl;
			do_google_int_tests();
			/*cout << "Starting measure_tests." << newl;
			do_measure_test();*/
			//cout << "Starting multidimensional add/sub tests." << newl;
			//do_multi_dim_add_sub_tests();
		/*	cout << "Starting simple multiplication tests." << newl;
			do_simple_multiplication_test();
			cout << "Starting simple division tests." << newl;
			do_simple_division_tests();
			cout << "Starting more division tests: " << newl;
			do_div_tests();
			cout << "Starting comparisons_tests." << newl;
			do_comparisons_test_length();
			cout << "Starting bin_float tests." << newl;
			do_bin_fl_tests();*/
			cout << "Starting fixed_uint tests." << newl;
			do_fixed_uint_test();
			cout << "Goodbye, world!" << newl;
			ret = 0;
		}
		catch (const std::exception& ex)
		{
			cerr << "One or more tests failed.  Exception: " << ex.what() << newl;
			ret = -1;
		}
		catch (...)
		{
			cerr << "One or more tests failed because a non-standard exception was thrown." << newl;
			ret = -1;
		}
		cjm::testing::ComparisonOperation256::release_static_memory();
		std::string dummy;
		cout << "Type something then press enter..." << std::endl;
		std::cin >> dummy;
		return ret;
	}

	void do_floor_log2_tests()
	{
		using namespace cjm::numerics::uint128_literals;
		using namespace cjm::numerics::math_functions;
		using namespace cjm::testing;
		cjm_assert_throws<std::domain_error>([]() -> void
		{
			auto x = floor_log2(0);
			cerr << "x should never be printed: " << x << newl;
		});
		cjm_assert_throws<std::domain_error>([]() -> void
		{
			auto x = floor_log2(-1);
			cerr << "x should never be printed: " << x << newl;
		});
		cjm_assert_nothrow([]() -> void
		{
			unsigned negativeOne = static_cast<unsigned>(-1);
			auto x = floor_log2(negativeOne);
			cout << "The floor log2 of the two's complement of 1 is: " << x << "." << newl;
		});
		static_assert(floor_log2(1) == 0);
		static_assert(floor_log2(2) == 1);
		static_assert(floor_log2(3_u128) == 1);
		static_assert(floor_log2(4) == 2);
		static_assert(floor_log2(5) == 2);
		static_assert(floor_log2(6) == 2);
		static_assert(floor_log2(7_u128) == 2);
		static_assert(floor_log2(8_u128) == 3);
		static_assert(floor_log2(9_u128) == 3);
		static_assert(floor_log2(10_u128) == 3_u128);
		static_assert(floor_log2(11) == 3_u128);
		static_assert(floor_log2(12) == 3);
		static_assert(floor_log2(13) == 3);
		static_assert(floor_log2(14) == 3);
		static_assert(floor_log2(15) == 3);
		static_assert(floor_log2(16) == 4);
		static_assert(floor_log2(17) == 4_u128);
		cout << "floor_log2_tests PASS" << newl;
	}

	void do_boost_literal_test()
	{
		using namespace boost::multiprecision::literals;
		uint128_t x = 0x123456789ABCDEF012345678_cppui128;
		cout << "boost literal: " << std::hex << x << newl;

	}

	void do_fixed_uint_from_u32str_test()
	{
		using uint256_t = cjm::numerics::uint256_t;
		uint256_t x;
		using namespace cjm::numerics::fixed_uint_literals;
		using namespace cjm::numerics::uint128_literals;
		static constexpr auto oneTwoEight = 0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u256;
		static constexpr auto twoFiveSix = 0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u256;
		static constexpr auto biggestDecimal = 115'792'089'237'316'195'423'570'985'008'687'907'853'269'984'665'640'564'039'457'584'007'913'129'639'935_u256;
		static_assert(twoFiveSix > oneTwoEight);
		std::u32string str128 = U"0x1234_5678_9ABC_DEF0_1234_5678_9ABC_DEF0";
		std::u32string str256 = U"0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0";
		std::u32string oneTooManyHexDigits = U"0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'0";
		std::u32string strBigDec = U"115,792,089,237,316,195,423,570,985,008,687,907,853,269,984,665,640,564,039,457,584,007,913,129,639,935";
		std::u32string tooBigByOne = U"115,792,089,237,316,195,423,570,985,008,687,907,853,269,984,665,640,564,039,457,584,007,913,129,639,936";
		std::u32string fromStrOne = U"1";
		std::u32string fromStrZero = U"0";
		std::u32string fromHexStrOne = U"0x01";
		std::u32string fromHexStrZero = U"0x00";
		std::u32string invalidOctal = U"01";
		std::u32string invalidNonHexDigitInHex = U"0x01g";
		std::u32string invalidCharacter = U"12s2";
		auto fromStrOneTwoEight = uint256_t::make_from_string(str128);
		auto fromStrTwoFiveSix = uint256_t::make_from_string(str256);
		auto fromStrBigDec = uint256_t::make_from_string(strBigDec);
		auto one = uint256_t::make_from_string(fromStrOne);
		auto zero = uint256_t::make_from_string(fromStrZero);
		auto hexZero = uint256_t::make_from_string(fromHexStrZero);
		auto hexOne = uint256_t::make_from_string(fromHexStrOne);

		cjm_assert(fromStrOneTwoEight == oneTwoEight);
		cjm_assert(fromStrTwoFiveSix == twoFiveSix);
		cjm_assert(fromStrBigDec == biggestDecimal);
		cjm_assert(one == 1);
		cjm_assert(zero == 0);
		cjm_assert(hexZero == 0);
		cjm_assert(hexOne == 1);
		cjm_assert_throws<std::overflow_error>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(tooBigByOne);
			std::wcerr << L"You should never see this value: " << std::hex << val << newl;
		});
		cjm_assert_throws<std::overflow_error>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(oneTooManyHexDigits);
			std::wcerr << L"You should never see this value: " << std::hex << val << newl;
		});
		cjm_assert_throws<std::invalid_argument>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(invalidOctal);
			std::wcerr << L"You should never see this value: " << std::hex << val << newl;
		});
		cjm_assert_throws<std::invalid_argument>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(invalidNonHexDigitInHex);
			std::wcerr << L"You should never see this value: " << std::hex << val << newl;
		});
		cjm_assert_throws<std::invalid_argument>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(invalidCharacter);
			std::wcerr << L"You should never see this value: " << std::hex << val << newl;
		});
		cout << "do_fixed_uint_from_u32str_test PASSED" << newl;
	}
	
	void do_fixed_uint_from_u16str_test()
	{
		using uint256_t = cjm::numerics::uint256_t;
		uint256_t x;
		using namespace cjm::numerics::fixed_uint_literals;
		using namespace cjm::numerics::uint128_literals;
		static constexpr auto oneTwoEight = 0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u256;
		static constexpr auto twoFiveSix = 0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u256;
		static constexpr auto biggestDecimal = 115'792'089'237'316'195'423'570'985'008'687'907'853'269'984'665'640'564'039'457'584'007'913'129'639'935_u256;
		static_assert(twoFiveSix > oneTwoEight);
		std::u16string str128 = u"0x1234_5678_9ABC_DEF0_1234_5678_9ABC_DEF0";
		std::u16string str256 = u"0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0";
		std::u16string oneTooManyHexDigits = u"0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'0";
		std::u16string strBigDec = u"115,792,089,237,316,195,423,570,985,008,687,907,853,269,984,665,640,564,039,457,584,007,913,129,639,935";
		std::u16string tooBigByOne = u"115,792,089,237,316,195,423,570,985,008,687,907,853,269,984,665,640,564,039,457,584,007,913,129,639,936";
		std::u16string fromStrOne = u"1";
		std::u16string fromStrZero = u"0";
		std::u16string fromHexStrOne = u"0x01";
		std::u16string fromHexStrZero = u"0x00";
		std::u16string invalidOctal = u"01";
		std::u16string invalidNonHexDigitInHex = u"0x01g";
		std::u16string invalidCharacter = u"12s2";
		auto fromStrOneTwoEight = uint256_t::make_from_string(str128);
		auto fromStrTwoFiveSix = uint256_t::make_from_string(str256);
		auto fromStrBigDec = uint256_t::make_from_string(strBigDec);
		auto one = uint256_t::make_from_string(fromStrOne);
		auto zero = uint256_t::make_from_string(fromStrZero);
		auto hexZero = uint256_t::make_from_string(fromHexStrZero);
		auto hexOne = uint256_t::make_from_string(fromHexStrOne);

		cjm_assert(fromStrOneTwoEight == oneTwoEight);
		cjm_assert(fromStrTwoFiveSix == twoFiveSix);
		cjm_assert(fromStrBigDec == biggestDecimal);
		cjm_assert(one == 1);
		cjm_assert(zero == 0);
		cjm_assert(hexZero == 0);
		cjm_assert(hexOne == 1);
		cjm_assert_throws<std::overflow_error>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(tooBigByOne);
			std::wcerr << L"You should never see this value: " << std::hex << val << newl;
		});
		cjm_assert_throws<std::overflow_error>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(oneTooManyHexDigits);
			std::wcerr << L"You should never see this value: " << std::hex << val << newl;
		});
		cjm_assert_throws<std::invalid_argument>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(invalidOctal);
			std::wcerr << L"You should never see this value: " << std::hex << val << newl;
		});
		cjm_assert_throws<std::invalid_argument>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(invalidNonHexDigitInHex);
			std::wcerr << L"You should never see this value: " << std::hex << val << newl;
		});
		cjm_assert_throws<std::invalid_argument>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(invalidCharacter);
			std::wcerr << L"You should never see this value: " << std::hex << val << newl;
		});
		cout << "do_fixed_uint_from_u16str_test PASSED" << newl;
	}

	void do_fixed_uint_from_wstr_test()
	{
		using uint256_t = cjm::numerics::uint256_t;
		uint256_t x;
		using namespace cjm::numerics::fixed_uint_literals;
		using namespace cjm::numerics::uint128_literals;
		static constexpr auto oneTwoEight = 0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u256;
		static constexpr auto twoFiveSix = 0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u256;
		static constexpr auto biggestDecimal = 115'792'089'237'316'195'423'570'985'008'687'907'853'269'984'665'640'564'039'457'584'007'913'129'639'935_u256;
		static_assert(twoFiveSix > oneTwoEight);
		std::wstring str128 = L"0x1234_5678_9ABC_DEF0_1234_5678_9ABC_DEF0";
		std::wstring str256 = L"0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0";
		std::wstring oneTooManyHexDigits = L"0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'0";
		std::wstring strBigDec = L"115,792,089,237,316,195,423,570,985,008,687,907,853,269,984,665,640,564,039,457,584,007,913,129,639,935";
		std::wstring tooBigByOne = L"115,792,089,237,316,195,423,570,985,008,687,907,853,269,984,665,640,564,039,457,584,007,913,129,639,936";
		std::wstring fromStrOne = L"1";
		std::wstring fromStrZero = L"0";
		std::wstring fromHexStrOne = L"0x01";
		std::wstring fromHexStrZero = L"0x00";
		std::wstring invalidOctal = L"01";
		std::wstring invalidNonHexDigitInHex = L"0x01g";
		std::wstring invalidCharacter = L"12s2";
		auto fromStrOneTwoEight = uint256_t::make_from_string(str128);
		auto fromStrTwoFiveSix = uint256_t::make_from_string(str256);
		auto fromStrBigDec = uint256_t::make_from_string(strBigDec);
		auto one = uint256_t::make_from_string(fromStrOne);
		auto zero = uint256_t::make_from_string(fromStrZero);
		auto hexZero = uint256_t::make_from_string(fromHexStrZero);
		auto hexOne = uint256_t::make_from_string(fromHexStrOne);

		cjm_assert(fromStrOneTwoEight == oneTwoEight);
		cjm_assert(fromStrTwoFiveSix == twoFiveSix);
		cjm_assert(fromStrBigDec == biggestDecimal);
		cjm_assert(one == 1);
		cjm_assert(zero == 0);
		cjm_assert(hexZero == 0);
		cjm_assert(hexOne == 1);
		cjm_assert_throws<std::overflow_error>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(tooBigByOne);
			std::wcerr << L"You should never see this value: " << std::hex << val << newl;
		});
		cjm_assert_throws<std::overflow_error>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(oneTooManyHexDigits);
			std::wcerr << L"You should never see this value: " << std::hex << val << newl;
		});
		cjm_assert_throws<std::invalid_argument>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(invalidOctal);
			std::wcerr << L"You should never see this value: " << std::hex << val << newl;
		});
		cjm_assert_throws<std::invalid_argument>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(invalidNonHexDigitInHex);
			std::wcerr << L"You should never see this value: " << std::hex << val << newl;
		});
		cjm_assert_throws<std::invalid_argument>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(invalidCharacter);
			std::wcerr << L"You should never see this value: " << std::hex << val << newl;
		});
		cout << "do_fixed_uint_from_wstr_test PASSED" << newl;
	}

	void do_fixed_uint_from_str_test()
	{
		using uint256_t = cjm::numerics::uint256_t;
		uint256_t x;
		using namespace cjm::numerics::fixed_uint_literals;
		using namespace cjm::numerics::uint128_literals;
		static constexpr auto oneTwoEight = 0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u256;
		static constexpr auto twoFiveSix = 0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u256;
		static constexpr auto biggestDecimal = 115'792'089'237'316'195'423'570'985'008'687'907'853'269'984'665'640'564'039'457'584'007'913'129'639'935_u256;
		static_assert(twoFiveSix > oneTwoEight);
		std::string str128 = "0x1234_5678_9ABC_DEF0_1234_5678_9ABC_DEF0";
		std::string str256 = "0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0";
		std::string oneTooManyHexDigits = "0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'0";
		std::string strBigDec = "115,792,089,237,316,195,423,570,985,008,687,907,853,269,984,665,640,564,039,457,584,007,913,129,639,935";
		std::string tooBigByOne = "115,792,089,237,316,195,423,570,985,008,687,907,853,269,984,665,640,564,039,457,584,007,913,129,639,936";
		std::string fromStrOne = "1";
		std::string fromStrZero = "0";
		std::string fromHexStrOne = "0x01";
		std::string fromHexStrZero = "0x00";
		std::string invalidOctal = "01";
		std::string invalidNonHexDigitInHex = "0x01g";
		std::string invalidCharacter = "12s2";
		auto fromStrOneTwoEight = uint256_t::make_from_string(str128);
		auto fromStrTwoFiveSix = uint256_t::make_from_string(str256);
		auto fromStrBigDec = uint256_t::make_from_string(strBigDec);
		auto one = uint256_t::make_from_string(fromStrOne);
		auto zero = uint256_t::make_from_string(fromStrZero);
		auto hexZero = uint256_t::make_from_string(fromHexStrZero);
		auto hexOne = uint256_t::make_from_string(fromHexStrOne);

		cjm_assert(fromStrOneTwoEight == oneTwoEight);
		cjm_assert(fromStrTwoFiveSix == twoFiveSix);
		cjm_assert(fromStrBigDec == biggestDecimal);
		cjm_assert(one == 1);
		cjm_assert(zero == 0);
		cjm_assert(hexZero == 0);
		cjm_assert(hexOne == 1);
		cjm_assert_throws<std::overflow_error>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(tooBigByOne);
			cerr << "You should never see this value: " << std::hex << val << newl;
		});
		cjm_assert_throws<std::overflow_error>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(oneTooManyHexDigits);
			cerr << "You should never see this value: " << std::hex << val << newl;
		});
		cjm_assert_throws<std::invalid_argument>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(invalidOctal);
			cerr << "You should never see this value: " << std::hex << val << newl;
		});
		cjm_assert_throws<std::invalid_argument>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(invalidNonHexDigitInHex);
			cerr << "You should never see this value: " << std::hex << val << newl;
		});
		cjm_assert_throws<std::invalid_argument>([=]()-> void
		{
			uint256_t val = uint256_t::make_from_string(invalidCharacter);
			cerr << "You should never see this value: " << std::hex << val << newl;
		});
		cout << "do_fixed_uint_from_str_test PASSED" << newl;
	}
	void do_fixed_uint_test()
	{
		using uint256_t = cjm::numerics::uint256_t;
		uint256_t x;
		using rat = cjm::numerics::rational::rational<std::uint64_t>;
		using bigrat = cjm::numerics::rational::rational<uint128>;
		using namespace cjm::numerics::fixed_uint_literals;
		using namespace cjm::numerics::uint128_literals;
		cout << "x.low := " << x.low_part() << newl;
		cout << "x.hi := " << x.high_part() << newl;
		cout << "sizeof(x) := " << sizeof(uint256_t) << newl;
		cout << "do_fixed_uint_test PASSED " << newl;

		constexpr auto oneTwoEight = 0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u128;
		constexpr auto twoFiveSix = 0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u256;
		static_assert(twoFiveSix > oneTwoEight);

		static_assert(cjm::numerics::is_valid_limb_type_v<uint128>, "uint128 should be a valid limb type.");
		static_assert(cjm::numerics::is_valid_limb_type_v<uint256_t>, "uint256_t should be a valid limb type.");
		static_assert(cjm::numerics::is_valid_limb_type_v<uint256_t>, "uint256_t should be a valid limb type.");
		static_assert(!cjm::numerics::is_valid_limb_type_v<std::uint64_t>, "uint64_t should not be a valid limb type.");
		static_assert(!cjm::numerics::is_valid_limb_type_v<rat>, "rat should not be a valid limb type.");
		static_assert(!cjm::numerics::is_valid_limb_type_v<bigrat>, "bigrat should not be a valid limb type.");
	}

	void do_fixed_uint_out_test()
	{
		using uint256_t = cjm::numerics::uint256_t;
		using namespace cjm::numerics::fixed_uint_literals;
		using namespace cjm::numerics::uint128_literals;

		constexpr auto oneTwoEight = 0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u128;
		constexpr auto twoFiveSix =  0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u256;
		uint256_t y = twoFiveSix - 1;
		uint256_t x = 12;

		x *= 2;

		cjm_assert(x == 24);
		x = y + 1;
		cjm_assert(x == twoFiveSix);

		std::string oneNinerEight = convert_to_dec_string(198);

		cout << "128 bit: 0x" << std::hex << oneTwoEight << newl;
		//cout << "256 bit: 0x" << twoFiveSix << newl;

		cout << "198 decimal: " << oneNinerEight << newl;

		static_assert(twoFiveSix > oneTwoEight);

		cout << "do_fixed_uint_out_test PASSED." << newl;
	}

	std::string convert_to_dec_string(std::uint64_t x)
	{
		constexpr std::array<char, 10> lookup = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
		if (x == 0)
			return "0";
		
		std::deque<char> buffer = (std::deque<char>((std::numeric_limits<std::uint64_t>::digits10 + 1)));
		size_t count = 0;
		while (x > 0)
		{
			char insertMe = lookup[x % 10ull];
			buffer.push_front(insertMe);
			++count;
			x /= 10;
		}

		std::string ret;
		ret.reserve(count);
		for (size_t i = 0; i < count; i++)
		{
			ret.push_back(buffer[i]);
		}
		return ret;		
	}

	//void do_multi_dim_add_sub_tests()
	//{
	//	using namespace cjm::spatial_measurement::volume_literals;
	//	using namespace cjm::spatial_measurement::area_literals;
	//	using namespace cjm::spatial_measurement::length_literals;
	//	using namespace cjm::spatial_measurement;
	//	try
	//	{
	//		cjm_assert(5_in + 6_in == 11_in);
	//		cjm_assert(11_in - 5_in == 6_in);
	//		cjm_assert(100_cm + 2_m == 300_cm);
	//		cjm_assert(300_cm - 100_cm == 2_m);

	//		
	//		auto five_plus_6_in2 = 5_in2 + 6_in2;
	//		cout << "five_plus_6_in2:= " << five_plus_6_in2.count() << newl;
	//		cout << "five_plus_6_in2 num: " << decltype(5_in2 + 6_in2)::unit::num << " den: " << decltype(5_in2 + 6_in2)::unit::den << newl;
	//		cjm_assert(5_in2 + 6_in2 == 11_in2);
	//		cjm_assert(11_in2 - 6_in2 == 5_in2);
	//		cjm_assert(100_cm2 + 200_cm2 == 300_cm2);
	//		cjm_assert(300_cm2 - 200_cm2 == 100_cm2);
	//		cout << "do_multi_dim_add_sub_tests PASSED." << newl;
	//	}
	//	catch (const std::exception& ex)
	//	{
	//		cerr << "do_multi_dim_add_sub_tests FAILED.  Exception message: " << ex.what() << newl;
	//		throw;
	//	}
	//	catch (...)
	//	{
	//		cerr << "do_multi_dim_add_sub_tests FAILED because of an unexpected non-standard exception." << newl;
	//		throw;
	//	}

	//}
//#pragma warning( push )
//#pragma warning( disable : 4723) 
//	void do_div_tests() //WARNING DISABLED BECAUSE TESTING DIV/0 behavior
//	{
//		using namespace cjm::spatial_measurement::volume_literals;
//		using namespace cjm::spatial_measurement::area_literals;
//		using namespace cjm::spatial_measurement::length_literals;
//		using namespace cjm::spatial_measurement;
//
//		auto halfAFoot = 1_ft / 2;
//		cout << "half a foot in feet (truncates): " << halfAFoot.count() << newl;
//		using halfAFootType = decltype(halfAFoot);
//		using halfAFootUnit = halfAFootType::unit;
//		static_assert(std::ratio_equal_v<feetunit, halfAFootUnit>);
//		auto remainder = 1_ft % 2;
//		cjm_assert(remainder == 1_ft);
//		cjm::testing::cjm_assert_throws<std::domain_error>([] () -> void
//		{
//			auto result = 11_in / 0;
//			cerr << "Hope you don't see this: " << result.count() << newl;
//		});
//		cjm::testing::cjm_assert_throws<std::domain_error>([]() -> void
//		{
//			inches_bf legal = inches_bf(12.0);
//			double zero = 0;
//			double wickedness = 12.0 / zero;
//			inches_bf illegal = inches_bf(wickedness);
//			cout << "illegal: " << illegal.count() << newl;
//			cerr << "legal: " << legal.count() << newl;
//		});
//		cjm::testing::cjm_assert_throws<std::domain_error>([]() -> void
//		{
//			double nan = std::numeric_limits<double>::quiet_NaN();
//			inches_bf illegal = inches_bf(nan);
//			cout << "illegal: " << illegal.count() << newl;
//		});
//		cout << "do_div_tests PASSED" << newl;
//	}
//#pragma warning ( pop )
//	void do_bin_fl_tests()
//	{
//		using namespace cjm::spatial_measurement::area_literals;
//		using namespace cjm::spatial_measurement::length_literals;
//		using namespace cjm::spatial_measurement;
//		cjm_assert(5.0_inbf < 6.0_inbf);
//		cjm_assert(5.0_inbf + 6.0_inbf > 3.0_inbf * 2);
//		inches2_bf inchesSquared = 3.0_inbf * 2.0_inbf;
//		//inches shouldntCompile = inchesSquared;
//		inches3_bf inchesCubed = inchesSquared * 5.0_inbf;
//		cout << "six inches squared: " << inchesSquared.count() << newl;
//		cout << "thirty inches cubed: " << inchesCubed.count() << newl;
//
//		inchesSquared /= 2.0;
//		cout << "three inches squared: " << inchesSquared.count() << newl;
//		inches_bf fiveInches = inchesCubed / 6.0_in2bf;
//		cout << "fiveInches: " << fiveInches.count() << newl;
//		inches_dec fiveInchesDec = fiveInches;
//		cjm_assert(fiveInchesDec == length_cast<inches_dec>(5_in));
//		feet eighteenFeet = 6_yd;
//		cjm_assert(eighteenFeet / 4 == 4_ft);
//		cjm_assert(eighteenFeet % 4 == 2_ft);
//		eighteenFeet /= 4;
//		cjm_assert(eighteenFeet == 4_ft);
//		eighteenFeet %= 3;
//		cjm_assert(eighteenFeet == 1_ft);
//
//		
//		cjm::testing::cjm_assert_throws<std::domain_error>([]() -> void
//		{
//			double nan = std::numeric_limits<double>::quiet_NaN();
//			inches_bf inchesBf = inches_bf(nan);
//			cerr << "inchesBf:= " << inchesBf.count() << newl;
//		});
//
//		cjm::testing::cjm_assert_throws<std::domain_error>([]() -> void
//		{
//			double posInf = std::numeric_limits<double>::infinity();
//			inches_bf inchesBf = inches_bf(posInf);
//			cerr << "inchesBf:= " << inchesBf.count() << newl;
//		});
//
//		cjm::testing::cjm_assert_throws<std::domain_error>([]() -> void
//		{
//			double negInf = -(std::numeric_limits<double>::infinity());
//			inches_bf inchesBf = inches_bf(negInf);
//			cerr << "inchesBf:= " << inchesBf.count() << newl;
//		});
//		inches_bf thisIsOk = 0.0_inbf;
//		thisIsOk += 5.0_inbf;
//		thisIsOk *= 3;
//		thisIsOk /= 5;
//		cout << "thisIsOk: " << thisIsOk.count() << newl;
//		cjm::testing::cjm_assert_throws<std::domain_error>([]() -> void
//		{
//			inches_bf zeroInches = 0.0_inbf;
//			binfltrep inchesBf = 15.0_inbf / zeroInches;
//			cerr << "inchesBf:= " << inchesBf << newl;
//		});
//
//		cout << "do_bin_fl_tests PASSES" << newl;
//	}

	void do_threw_wrong_thing_test()
	{
		bool didntThrow;
		try
		{

			cjm::testing::cjm_assert_throws<std::invalid_argument>([]() -> void
			{
				throw std::domain_error("This is a dummy domain error to make sure the testing facility can identify expected exception types.");
			});
			didntThrow = true;
		}
		catch (const cjm::testing::testing_failure&)
		{
			cout << "do_threw_wrong_thing_test PASSED." << newl;
			didntThrow = false;
		}
		catch (const std::exception& ex)
		{
			cerr << "do_threw_wrong_thing_test FAILED -- threw unexpected standard exception with message: " << ex.what() << newl;
			didntThrow = false;
		}
		catch (...)
		{
			cerr << "do_do_threw_wrong_thing_test FAILED -- threw non-standard exception." << newl;
			didntThrow = false;
		}
		if (didntThrow)
		{
			throw cjm::testing::testing_failure("A method that was expected to throw did not.");
		}

	}

	//constexpr bool is_4in_less_than_5in()
	//{
	//	using namespace cjm::spatial_measurement::area_literals;
	//	using namespace cjm::spatial_measurement::length_literals;
	//	using namespace cjm::spatial_measurement;
	//	return 4_in < 5_in;
	//}

	//constexpr bool is_10001_in_greater_than_254_m()
	//{
	//	using namespace cjm::spatial_measurement::area_literals;
	//	using namespace cjm::spatial_measurement::length_literals;
	//	using namespace cjm::spatial_measurement;
	//	constexpr bool ret = 10001_in > 254_m;
	//	static_assert((10001_in - 1_in) == 254_m);
	//	//static_assert(10000_in2 != 10000_in); line should cause compilation failure.
	//	return ret;
	//}

	//constexpr bool mult_div_comp_test()
	//{
	//	using namespace cjm::spatial_measurement::volume_literals;
	//	using namespace cjm::spatial_measurement::area_literals;
	//	using namespace cjm::spatial_measurement::length_literals;
	//	using namespace cjm::spatial_measurement;

	//	static_assert(4_cm * 3 == 12_cm);
	//	static_assert(12_cm / 3 == 4_cm);
	//	static_assert(5_mi / 2 == 2_mi);
	//	static_assert(5_mi % 2 == 1_mi);
	//	return 4_m == 400_cm;
	//}

	//
	//void do_simple_multiplication_test()
	//{
	//	using namespace cjm::spatial_measurement::volume_literals;
	//	using namespace cjm::spatial_measurement::area_literals;
	//	using namespace cjm::spatial_measurement::length_literals;
	//	using namespace cjm::spatial_measurement;

	//	inches fourInches = 4_in;
	//	micrometers oneOneThreeMicrons = 113_um;
	//	miles2 threeSquareMiles = 3_mi2;
	//	centimeters3 twelveCcs = 12_cm3;
	//	try
	//	{
	//		auto cubicTemp = twelveCcs * 4;
	//		cjm_assert(fourInches * 3 == 12_in);
	//		cjm_assert(oneOneThreeMicrons * 2 == 226_um);
	//		cjm_assert(threeSquareMiles *3 == 9_mi2);
	//		cjm_assert(twelveCcs * 4 == 48_cm3);
	//		cjm_assert(cubicTemp.count() == 48);

	//		fourInches *= 3;
	//		oneOneThreeMicrons *= 2;
	//		threeSquareMiles *= 3;
	//		twelveCcs *= 4;

	//		cjm_assert(fourInches == 12_in);
	//		cjm_assert(oneOneThreeMicrons == 226_um);
	//		cjm_assert(threeSquareMiles == 9_mi2);
	//		cjm_assert(twelveCcs == 48_cm3);

	//		std::cout << "do_simple_multiplication_test PASSED." << newl;
	//	}
	//	catch (const std::exception& ex)
	//	{
	//		std::cerr << "do_simple_multiplication_test FAILED.  Exception message: " << ex.what() << newl;
	//		throw;
	//	}
	//	catch (...)
	//	{
	//		std::cerr << "do_simple_multiplication_test FAILED because of an unexpected non-standard exception." << newl;
	//		throw;
	//	}
	//}
	//

	//void do_simple_division_tests()
	//{
	//	using namespace cjm::spatial_measurement::volume_literals;
	//	using namespace cjm::spatial_measurement::area_literals;
	//	using namespace cjm::spatial_measurement::length_literals;
	//	using namespace cjm::spatial_measurement;

	//	inches twelveInches = 1_ft;
	//	micrometers twoTwoSixMicrons = 226_um;
	//	miles2 nineSquareMiles = 9_mi2;
	//	centimeters3 fortyEightCcs = 48_cm3;
	//	try
	//	{
	//		cjm_assert(twelveInches / 3 == 4_in);
	//		cjm_assert(twelveInches % 3 == 0_in);
	//		cjm_assert(twoTwoSixMicrons / 2 == 113_um);
	//		cjm_assert(twoTwoSixMicrons % 2 == 0_um);
	//		cjm_assert(nineSquareMiles / 3 == 3_mi2);
	//		cjm_assert(nineSquareMiles % 3 == 0_mi2);
	//		cjm_assert(fortyEightCcs / 4 == 12_cm3);
	//		cjm_assert(fortyEightCcs % 4 == 0_cm3);

	//		twelveInches /= 3;
	//		twoTwoSixMicrons /= 2;
	//		nineSquareMiles /= 3;
	//		fortyEightCcs /= 4;

	//		cjm_assert(twelveInches == 4_in);
	//		cjm_assert(twoTwoSixMicrons == 113_um);
	//		cjm_assert(nineSquareMiles == 3_mi2);
	//		cjm_assert(fortyEightCcs == 12_cm3);
	//		
	//		cout << "do_simple_division_test PASSED." << newl;
	//	}
	//	catch (const std::exception& ex)
	//	{
	//		std::cerr << "do_simple_division_test FAILED.  Exception message: " << ex.what() << newl;
	//		throw;
	//	}
	//	catch (...)
	//	{
	//		std::cerr << "do_simple_division_test FAILED because of an unexpected non-standard exception." << newl;
	//		throw;
	//	}
	//}


	//void do_comparisons_test_length()
	//{
	//	using namespace cjm::spatial_measurement::area_literals;
	//	using namespace cjm::spatial_measurement::length_literals;
	//	using namespace cjm::spatial_measurement;
	//	constexpr inches fourInches = 4_in;
	//	constexpr inches fiveInches = 5_in;
	//	constexpr inches sixInches = 6_in;
	//	constexpr inches fiveInchesCopy = fiveInches;
	//	auto fiveInchesInSomeWeirdUnit = fiveInchesCopy + 0_cm;
	//	static_assert(is_10001_in_greater_than_254_m());
	//	try
	//	{
	//		cjm_assert(fourInches < fiveInches && fourInches < fiveInchesInSomeWeirdUnit 
	//			&& fiveInchesInSomeWeirdUnit < sixInches && fiveInches < sixInches);
	//		cjm_assert(fourInches <= fiveInches && fourInches <= fiveInchesInSomeWeirdUnit 
	//			&& fiveInchesInSomeWeirdUnit <= sixInches && fiveInches <= sixInches);
	//		cjm_assert(sixInches > fiveInches && sixInches > fiveInchesInSomeWeirdUnit && 
	//			fiveInchesInSomeWeirdUnit > fourInches && fiveInches > fourInches);
	//		cjm_assert(sixInches >= fiveInches && sixInches >= fiveInchesInSomeWeirdUnit && 
	//			fiveInchesInSomeWeirdUnit >= fourInches && fiveInches >= fourInches);
	//		cjm_assert(fourInches != fiveInches && fiveInches != fourInches && fiveInches != sixInches 
	//			&& sixInches != fiveInches && sixInches != fiveInchesInSomeWeirdUnit && 
	//			fiveInchesInSomeWeirdUnit != sixInches && fourInches != fiveInchesInSomeWeirdUnit
	//			&& fiveInchesInSomeWeirdUnit != fourInches);
	//		cjm_assert(fiveInches == fiveInchesCopy && fiveInchesCopy == fiveInches && fiveInches == 
	//			fiveInchesInSomeWeirdUnit && fiveInchesInSomeWeirdUnit == fiveInches && fiveInchesCopy
	//			== fiveInchesInSomeWeirdUnit && fiveInchesInSomeWeirdUnit == fiveInchesCopy);
	//		// ReSharper disable CppIdenticalOperandsInBinaryExpression
	//		cjm_assert(fiveInches == fiveInches && fourInches == fourInches && fiveInchesCopy == fiveInchesCopy && fiveInchesInSomeWeirdUnit == fiveInchesInSomeWeirdUnit);  // NOLINT
	//		// ReSharper restore CppIdenticalOperandsInBinaryExpression
	//		cjm_assert(254_m <= 10'000_in && 254_m >= 10'000_in && 254_m == 10'000_in && 1_in < 3_cm && 1_in > 2_cm && 1000_m == 1_km && 1_km < 1001_m && 1001_m != 1_km);
	//	}
	//	catch (cjm::testing::testing_failure& testingFailure)
	//	{
	//		std::cerr << "do_comparisons_test_length FAILED.  Exception message: " << testingFailure.what() << newl;
	//		throw;
	//	}
	//	catch (...)
	//	{
	//		std::cerr << "do_comparisons_test_length FAILED.  An unexpected exception was thrown." << newl;
	//		throw;
	//	}		
	//	
	//	cout << "comparisons tests PASSED." << newl;
	//}

	void do_google_int_tests()
	{
		static constexpr std::uint64_t seedInt{ 0x1234'5678'9ABC'DEF0ull };
		static constexpr uint128 seedBigAss{ uint128::MakeUint128(seedInt, seedInt) };
		uint128 x = seedBigAss;
		cout << "Here is a google int: 0x" << std::hex << x << newl;
		cout << "Here is a google int in decimal: " << std::dec << x << newl;
		std::uint64_t seedRight3 = seedInt >> 3;
		x >>= (64 + 3);
		cjm_assert(x == seedRight3);
		x = seedBigAss;
		x *= 3;
		x /= 3;
		cjm_assert(x == seedBigAss);
		uint128 from64 = seedInt;
		from64 += seedInt;
		cjm_assert(from64 > seedInt);
		cjm_assert(from64 - seedInt <= seedInt);
		cjm_assert(from64 - seedInt >= seedInt);
		cjm_assert(from64 != seedInt);
		cjm_assert(from64 - seedInt == seedInt);
		cjm_assert(seedInt < from64);
		auto from64Times3 = from64 * 3;
		cjm_assert(from64Times3 / 3 == from64);
		cjm_assert(from64Times3 % 3 == 0);
		auto from64Times3Copy = from64Times3++;
		cjm_assert(from64Times3Copy == from64Times3 - 1);
		cjm_assert(from64Times3 % 3 == 1);
		from64Times3Copy = from64Times3--;
		cjm_assert(from64Times3Copy == from64Times3 + 1);
		from64Times3Copy = --from64Times3;
		cjm_assert(from64Times3Copy == from64Times3);
		cjm::testing::cjm_assert_throws<std::domain_error>([] () -> void
		{
			uint128 zero = 0;
			uint128 mySeedInt = seedBigAss;
			uint128 quot = mySeedInt / zero;
			cerr << quot << newl;
		});
		cjm::testing::cjm_assert_throws<std::domain_error>([]() -> void
		{
			uint128 zero = 0;
			uint128 mySeedInt = seedBigAss;
			uint128 quot = mySeedInt % zero;
			cerr << quot << newl;
		});
		cjm::testing::cjm_assert_throws<std::domain_error>([]() -> void
		{
			uint128 zero = 0;
			uint128 mySeedInt = seedBigAss;
			mySeedInt /= zero;
			cerr << mySeedInt << newl;
		});
		cjm::testing::cjm_assert_throws<std::domain_error>([]() -> void
		{
			uint128 zero = 0;
			uint128 mySeedInt = seedBigAss;
			mySeedInt %= zero;
			cerr << mySeedInt << newl;
		});
		cout << "google int tests PASS " << newl;
	}

	/*void do_measure_test()
	{

		using namespace cjm::spatial_measurement::area_literals;
		using namespace cjm::spatial_measurement::length_literals;
		using namespace cjm::spatial_measurement;
		try
		{
			cjm::streams::nios_state_saver saver(std::cout);
			std::cout << std::setprecision(64);
			centimeters2_bf binaryFloatCm = centimeters2_bf(10'123.45L);
			centimeters2_dec convertedToDec = binaryFloatCm;
			inches2_dec threeAndThreeQuartersInches = 3.75_in2bf;
			cout << "binary Float Cm2:= " << binaryFloatCm.count() << newl;
			cout << "converted To Dec:= " << convertedToDec.count() << newl;
			cout << "3 3/4 in2:= " << threeAndThreeQuartersInches.count() << " in2." << newl;
			static_assert(is_bin_to_dec_float_v<long double, standarddec>);
			centimeters2 twentyThousandSquareCentimeters = centimeters2(20'000);
			auto squareMeters = area_cast<meters2>(twentyThousandSquareCentimeters);
			centimeters2_dec someCentimeters = centimeters2_bf(10'233.01L);
			cout << "someCentimeters2 := " << someCentimeters.count() << " cm2." << newl;
			auto castToInches = area_cast<inches2_dec>(someCentimeters);
			cout << "someCentimeters2 := " << castToInches.count() << " in2." << newl;
			auto backToCm = area_cast<centimeters2_dec>(castToInches);
			cout << "back to centimters:= " << backToCm.count() << " cm2." << newl;
			auto backAgain = area_cast<centimeters2_dec>(backToCm);
			cjm_assert(backAgain == backToCm);
			cjm_assert(squareMeters == twentyThousandSquareCentimeters);
			centimeters oneHundredCentimeters = 100_cm;
			auto oneMeter = length_cast<meters>(oneHundredCentimeters);
			std::common_type_t<spatial_measure<long long, std::ratio<1, 1>, 1>, spatial_measure<
				                   long long, std::ratio<1, 100>, 1>> twoMeters = oneMeter + oneHundredCentimeters;
			cout << "two meters ratio: " << decltype(twoMeters)::unit::num << " / " << decltype(twoMeters)::unit::den << "." << newl;
			cjm_assert(oneHundredCentimeters == oneMeter);
			cjm_assert(twoMeters == 200_cm);
			inches twelveInches = 1_ft;
			feet twoFeet = length_cast<feet>(twelveInches) + 1_ft;
			cjm_assert(twoFeet == twelveInches * 2);
			auto twoMetersAndTwoFeet = twoMeters + twoFeet;
			cjm_assert(twoMetersAndTwoFeet > twoFeet && twoMetersAndTwoFeet > twoMeters);
			auto twoMetersAndTwoFeetDec = length_cast<meters_dec>(twoMetersAndTwoFeet);
			meters_dec expectedResult = 2.6096_mbf;
			cout << "expected result:= " << expectedResult.count() << newl;
			cjm_assert(twoMetersAndTwoFeet == twoMetersAndTwoFeetDec && twoMetersAndTwoFeetDec == twoMetersAndTwoFeet);
			cout << "Two meters and two feet is " << twoMetersAndTwoFeetDec.count() << " meters." << newl;
		}
		catch (const cjm::testing::testing_failure& ex)
		{
			std::cerr << "The do_measure_test FAILED because a testing_failure exception was thrown: " << ex.what() << newl;
			throw;
		}
		catch (const std::exception& ex)
		{
			std::cerr << "The do_measure_test FAILED because the following std::exception was thrown: " << ex.what() << newl;
			throw;
		}
		catch (...)
		{
			std::cerr << "The do_measure_test FAILED because an unexpected exception was thrown." << newl;
			throw;
		}
		cout << "do_measure_test PASSED." << newl;
	}*/

	void do_google_literal_test()
	{
		static constexpr uint128 bigAssUint128 = uint128::MakeUint128(0x1234'5678'9ABC'DEF0ull, 0x1234'5678'9ABC'DEF0ull);
		using namespace cjm::numerics::uint128_literals;
		auto x = 0x123a_u128;
		static constexpr auto z = 0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u128;
		cjm_assert(x == 0x123aull);
		static_assert(z == bigAssUint128, "The uint128 literal or equality operator must be fubarred.");
		static_assert(z == 0X1234'5678'9abc'def0'1234'5678'9abc'def0_u128);
		static_assert(z == 0x123456789ABCDEF0123456789ABCDEF0_u128);
		static_assert(z == 0x1'23'456'789ABCDEF012345678'9AB'C'DEF'0_u128);
		cout << "The biggest possible uint128 in decimal is: " << std::dec << std::numeric_limits<uint128>::max() << newl;
		x = 1'234_u128;
		cjm_assert(x == 1234ull);
		x = 24197857203266734864793317670504947440_u128;
		cjm_assert(x == bigAssUint128);
		static_assert(340282366920938463463374607431768211455_u128 == std::numeric_limits<uint128>::max());
		static_assert(340282366920938463463374607431768211455_u128 == 0xFFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF'FFFF_u128);
		static_assert(340282366920938463463374607431768211455_u128 + 1 == 0);
		static_assert(340282366920938463463374607431768211455_u128 == 340'282'366'920'938'463'463'374'607'431'768'211'455_u128);
		static_assert(340282366920938463463374607431768211455_u128 == 340'282'366'920'938'463'463'374'607'431'768'211'455_u128);
		static_assert(3'4028236'692093846346337460743176821145'5_u128 == 340'282'366'920'938'463'463'374'607'431'768'211'455_u128);
		//The following line should compile.
		constexpr auto big_as_possible = 340282366920938463463374607431768211455_u128;
		std::wstring wbig_ass_string = L"340282366920938463463374607431768211455";
		std::string big_ass_string = "340282366920938463463374607431768211455";
		std::string big_ass_string_with_commas_n_spaces = " 340,282,366,920,938,463,463,374,607,431,768,211,455   ";
		std::wstring wbig_ass_string_with_commas_n_spaces = L" 340,282,366,920,938,463,463,374,607,431,768,211,455   ";
		std::string too_big = "340282366920938463463374607431768211456";
		std::wstring wtoo_big = L"340282366920938463463374607431768211456";
		std::string empty = "";
		std::string octal = "01234";
		std::string ok = "000";
		std::string alsoOk = "0";
		std::string yetAgainOk = "1234";
		std::string text = "This is the winter of our discontent.";

		auto result = uint128::make_from_string(big_ass_string);
		cjm_assert(result == big_as_possible);
		result = uint128::make_from_string(big_ass_string_with_commas_n_spaces);
		cjm_assert(result == big_as_possible);
		result = uint128::make_from_string(wbig_ass_string);
		cjm_assert(result == big_as_possible);
		result = uint128::make_from_string(wbig_ass_string_with_commas_n_spaces);
		cjm_assert(result == big_as_possible);

		cjm_assert_throws<std::overflow_error>([=]() -> void
		{
			auto temp = uint128::make_from_string(too_big);
			std::cerr << "You should not see this line: " << temp << newl;
		});

		cjm_assert_throws<std::overflow_error>([=]() -> void
		{
			auto temp = uint128::make_from_string(wtoo_big);
			std::cerr << "You should not see this line: " << temp << newl;
		});

		cjm_assert_throws<std::invalid_argument>([=]() -> void
		{
			auto temp = uint128::make_from_string(empty);
			std::cerr << "You should not see this line: " << temp << newl;
		});

		cjm_assert_throws<std::invalid_argument>([=]() -> void
		{
			auto temp = uint128::make_from_string(octal);
			std::cerr << "You should not see this line: " << temp << newl;
		});

		cjm_assert_throws<std::invalid_argument>([=]() -> void
		{
			auto temp = uint128::make_from_string(text);
			std::cerr << "You should not see this line: " << temp << newl;
		});

		cjm_assert(uint128::make_from_string(ok) == 0_u128);
		cjm_assert(uint128::make_from_string(alsoOk) == 0_u128);
		cjm_assert(uint128::make_from_string(yetAgainOk) == 1234_u128);

		static_assert(big_as_possible == std::numeric_limits<uint128>::max());
		static_assert(std::numeric_limits<uint128>::min() - 1 == std::numeric_limits<uint128>::max() && 
			std::numeric_limits<uint128>::max() + 1 == std::numeric_limits<uint128>::min());
		//The following line SHOULD NOT compile
		//constexpr auto too_big = 340282366920938463463374607431768211456_u128d;
		static_assert(0x0F == 0x0000F_u128);
		//The following line will either throw during execution or fail to compile:
		cjm::testing::cjm_assert_throws<std::domain_error>([]() -> void
		{
			auto a_bit_too_big = 340282366920938463463374607431768211456_u128;
			cerr << a_bit_too_big << "You should never see this line in the test results.";
		});
		static_assert(0x0 == 0x0_u128);
		//The following line should not compile
		//static_assert(0x0 == 0x_u128);
		cjm_assert(0x00 == 0_u128);
		static_assert(0 == 00000000_u128);
		static_assert(0x0 == 0x0'00'00_u128);
		static_assert(0x0 == 0'000_u128);
		static_assert(0x0 == 0'0000'0'0_u128);
		//The following might or might not compile.  If it doesn't compile it is ok to comment it out.
		//The code inside the lambda, IF IT COMPILES, should throw a std::domain_error exception.
		cjm::testing::cjm_assert_throws<std::domain_error>([]()-> void
		{
			auto whatever = 0001_u128;
			cerr << "You should not see this line ... " << whatever << newl;
		});

		//The following line should cause a compilation failure:
		//constexpr uint128 shouldNotCompile = 001_u128;

		cout << "google literal test PASSED." << newl;
	}
}
int main()
{
	std::ios_base::sync_with_stdio(false);
	return test_program::main();
}

