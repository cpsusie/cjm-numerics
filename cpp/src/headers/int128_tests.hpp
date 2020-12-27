#ifndef INT128_INT128_TESTS_HPP
#define INT128_INT128_TESTS_HPP
#include "numerics.hpp"
#include "uint128.hpp"
#include "cjm_string.hpp"
#include "testing.hpp"
#include <iostream>
#include <iomanip>
#include <string_view>
#include <string>
#include <sstream>
#include <limits>
#include <type_traits>
#include <cstdint>
#include <boost/io/ios_state.hpp>
#include <exception>
#include <stdexcept>
#include <boost/multiprecision/cpp_int.hpp>

namespace cjm::uint128_tests
{
    using ctrl_uint128_t = boost::multiprecision::uint128_t;
    using uint128_t = numerics::uint128;
    using namespace numerics::uint128_literals;
    using namespace numerics::literals;
    using namespace std::string_literals;
    using namespace std::string_view_literals;
    using testing::cjm_assert;
    using testing::cjm_deny;
    using std::cout;
    constexpr auto newl = '\n';
    using cout_saver = boost::io::ios_flags_saver;

    template<typename Invocable>
    void execute_test(Invocable test, std::string_view test_name);

    ctrl_uint128_t to_ctrl(uint128_t convert);
    uint128_t to_test(const ctrl_uint128_t& convert);

    constexpr size_t pow_2_arr_size = 63;
    constexpr std::array<std::uint64_t, pow_2_arr_size> get_pow2_arr()
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
    constexpr std::array<int, pow_2_arr_size> get_pow2_res_arr()
    {
        using namespace cjm::numerics::literals;
        constexpr std::array<std::uint64_t, pow_2_arr_size> work_on_me = get_pow2_arr();
        std::array<int, pow_2_arr_size> ret{};
    	for (auto i = 0_szt; i < work_on_me.size(); ++i)
    	{
            ret[i] = cjm::numerics::internal::fls_slow(work_on_me[i]);
    	}
        return ret;
    }	
	
	
    void execute_uint128_tests();
    void execute_basic_test_one();
    void execute_string_parse_test();
    void execute_basic_multiplication_test();
    void test_fls();

    void test_interconversion(const ctrl_uint128_t& control, uint128_t test);

	
}

template<typename Invocable>
void cjm::uint128_tests::execute_test(Invocable test, std::string_view test_name)
{
    cout_saver o_saver{cout};
    cout_saver e_saver{std::cerr};

    cout << "Beginning test: [" << test_name << "]:" << newl;
    try
    {
        test();
        cout << "Test [" << test_name << "] PASSED" << newl << newl;
    }
    catch (const std::exception& ex)
    {
        std::stringstream ss;
        ss << "Test [" << test_name << "] failed with exception message: [" << ex.what() << "]." << newl;
        std::cerr << ss.str();
        throw;
    }
}


#endif //INT128_INT128_TESTS_HPP
