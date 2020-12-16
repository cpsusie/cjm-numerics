#include <concepts>
#include <iostream>
#include <iomanip>
#include <string_view>
#include <limits>
#include <cstdint>
#include "numerics.hpp"
#include "uint128.hpp"
namespace cjm
{
    constexpr auto newl = '\n';
    template<typename T>
    concept unsigned_integer = std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed;

    template<unsigned_integer TUInt>
    void print_spec(TUInt val)
    {
        std::cout << std::hex << "[0x" << val << "]." << newl;
    }

    template<typename T>
    void print_spec(T val)
    {
        std::cout << "Thing that isn't unsigned integer: [" << std::dec << val << "]." << newl;
    }
}

int main()
{
    std::ios::sync_with_stdio(false);
    using namespace std::string_view_literals;
    std::uint64_t ui = 0xc0de'd00d'fea2'b00bu;
    std::int64_t  i1 = 938'336;
    constexpr auto text = "Hi mom!"sv;

    cjm::print_spec(ui);
    cjm::print_spec(i1);
    cjm::print_spec(text);

    std::cout << "Bye now!" << std::endl;
}

