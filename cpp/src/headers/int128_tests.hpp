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

namespace cjm::uint128_tests
{
    using uint128_t = numerics::uint128;
    using namespace numerics::uint128_literals;
    using namespace std::string_literals;
    using namespace std::string_view_literals;
    using std::cout;
    constexpr auto newl = '\n';
    using cout_saver = boost::io::ios_flags_saver;

    template<typename Invocable>
    void execute_test(Invocable test, std::string_view test_name);

    void execute_uint128_tests();
    void execute_basic_test_one();
    void execute_string_parse_test();
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
