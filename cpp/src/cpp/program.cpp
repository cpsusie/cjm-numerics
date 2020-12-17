#include <concepts>
#include <iostream>
#include <iomanip>
#include <string_view>
#include <limits>
#include <cstdint>
#include "numerics.hpp"
#include "uint128.hpp"
#include "int128_tests.hpp"
#include <boost/io/ios_state.hpp>

namespace cjm::base_test_program
{
    using cout_saver = cjm::uint128_tests::cout_saver;
    constexpr auto newl = '\n';
    template<typename T>
    concept unsigned_integer = std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed;

    template<unsigned_integer TUInt>
    void print_spec(TUInt val)
    {
        cout_saver{std::cout};
        std::cout << std::hex << "[0x" << val << "]." << newl;
    }

    template<typename T>
    void print_spec(T val)
    {
        cout_saver{std::cout};
        std::cout << "Thing that isn't unsigned integer: [" << std::dec << val << "]." << newl;
    }
}

int main()
{


    std::ios::sync_with_stdio(false);

    using namespace std::string_view_literals;
    std::uint64_t ui = 0xc0de'd00d'fea2'b00bu;
    std::int64_t  i1 = 938'336;
    using cjm::testing::cjm_assert;
    constexpr auto text = "Hi mom!"sv;
    using namespace cjm::base_test_program;
    using u128native_t = unsigned __int128;

    constexpr std::uint64_t low = 0xdead'beef'face'babe;
    constexpr std::uint64_t high = 0xc0de'd00d'fea2'b00b;
    u128native_t native = high;
    native <<= 64;
    native |= low;
    u128native_t copy_native = native;


    std::uint64_t rt_low = static_cast<std::uint64_t>(copy_native);
    copy_native >>= 64;
    std::uint64_t rt_high = static_cast<std::uint64_t>(copy_native);

    cjm_assert(low == rt_low && high == rt_high);

    cout_saver saver{std::cout};


    print_spec(ui);
    print_spec(i1);
    print_spec(text);
    cjm::uint128_tests::execute_uint128_tests();
    std::cout << "Bye now!" << std::endl;
}

