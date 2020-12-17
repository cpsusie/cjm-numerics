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
#include "cjm_numeric_concepts.hpp"

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
    using namespace cjm::numerics::uint128_literals;
    using uint128_t = cjm::numerics::uint128;
    using cjm::testing::cjm_assert;
    constexpr auto text = "Hi mom!"sv;
    using namespace cjm::base_test_program;
    using u128native_t = unsigned __int128;
    constexpr u128native_t is_constexpr = 0xdead'beef'cafe'babe;
    constexpr std::uint64_t low = 0xdead'beef'face'babe;
    constexpr std::uint64_t high = 0xc0de'd00d'fea2'b00b;
    constexpr auto my_literal = 0xc0de'd00d'fea2'b00b'dead'beef'face'babe_u128;
    constexpr auto my_second_literal = 0xc0de'd00d'fea2'cafe'babe'b00b'face'dad0_u128;
    u128native_t native = high;
    native <<= 64;
    native |= low;
    u128native_t copy_native = native;
    uint128_t from_native = static_cast<uint128_t>(native);


    static_assert(cjm::numerics::concepts::unsigned_integer<uint128_t>);

    std::uint64_t rt_low = static_cast<std::uint64_t>(copy_native);
    copy_native >>= 64;
    std::uint64_t rt_high = static_cast<std::uint64_t>(copy_native);

    cjm_assert(low == rt_low && high == rt_high);
    static_assert(is_constexpr == 0xdead'beef'cafe'babe);
    cout_saver saver{std::cout};
    cjm_assert(from_native == my_literal);

    print_spec(ui);
    print_spec(i1);
    print_spec(text);
    cjm::uint128_tests::execute_uint128_tests();
    std::cout << "Bye now!" << std::endl;
    std::cout << "from native: [0x" << std::hex << from_native << "]." << newl;
    std::cout << "Another cute literal: [0x" << std::hex << my_second_literal << "]." << newl;
    std::cout << "Another cute literal as dec: [" << std::dec << my_second_literal << "]." << newl;
}

