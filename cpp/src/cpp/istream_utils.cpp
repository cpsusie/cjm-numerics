#include "istream_utils.hpp"


std::wstring cjm::uint128_tests::widen(std::string_view convert_me)
{
    std::wstring ret;
    if (!convert_me.empty())
    {
        ret.reserve(convert_me.length());
        std::transform(convert_me.cbegin(), convert_me.cend(), std::back_inserter(ret), [](char c) -> wchar_t
            {
                return convert_char<char, wchar_t>(c);
            });
    }
    return ret;

}

std::string cjm::uint128_tests::narrow(std::wstring_view convert_me, char unknown)
{
    std::string ret;
    if (!convert_me.empty())
    {
        ret.reserve(convert_me.length());
        std::transform(convert_me.cbegin(), convert_me.cend(),
            std::back_inserter(ret),
            [=](wchar_t c) -> char { return convert_char<wchar_t, char>(c, unknown); });
    }
    return ret;
}

