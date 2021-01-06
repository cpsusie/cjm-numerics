#ifndef CJM_ISTREAM_UTILS_HPP_
#define CJM_ISTREAM_UTILS_HPP_
#include <concepts>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <string_view>
#include <algorithm>
#include <optional>
#include <locale>
#include "cjm_string.hpp"
#include "numerics.hpp"
#include "cjm_numeric_concepts.hpp"
#include <utility>


namespace cjm::uint128_tests::istream_utils
{
    enum class char_classification
    {
        error,
        eof,
        alphanum,
        punctuation,
        space,
        other,
    };

    template<cjm::numerics::concepts::character Char>
    bool is_space(Char c, const std::locale& l);

    template<cjm::numerics::concepts::character Char>
    std::pair<char_classification, std::optional<Char>> extract_and_classify_next(std::basic_istream<Char, std::char_traits<Char>>& istream);




    /*template<numerics::concepts::utf_character Char>
    bool advance_next_alpha_numeric_string(std::basic_istringstream<Char, std::char_traits<Char>>& is)
    {
        if (is.fail() || is.bad() || is.eof() || is.peek() == std::char_traits<Char>::eof())
        {
            return false;
        }

        Char c;
        is.get(c);
        if (!is.good())
        {
            return false;
        }
        bool is_alpha =

    }*/
}

namespace cjm::uint128_tests
{
	using sv_t = std::string_view;
	using wsv_t = std::wstring_view;
	using u8sv_t = std::u8string_view;
	using u16sv_t = std::u16string_view;
	using u32sv_t = std::u32string_view;

    
	
	

	template<typename SourceChar, typename TargetChar>
	requires (!std::is_same_v<std::remove_cvref_t<std::remove_const_t<SourceChar>>, std::remove_cvref_t<std::remove_const_t<TargetChar>>>
		&& cjm::numerics::concepts::character<SourceChar>
		&& cjm::numerics::concepts::character<TargetChar>)
		TargetChar convert_char(SourceChar c, std::optional<TargetChar> unknown = std::nullopt);


	template<cjm::numerics::concepts::utf_character UtfChar>
	std::basic_string<UtfChar, std::char_traits<UtfChar>> convert_to_utf(sv_t source);

	template<cjm::numerics::concepts::utf_character UtfChar>
	std::string convert_from_utf(std::basic_string_view<UtfChar> convert_me, char unknown = '?');

	template<cjm::numerics::concepts::utf_character UtfChar>
	requires (std::numeric_limits<wchar_t>::is_signed)
		std::wstring convert_wide_from_utf(std::basic_string_view<UtfChar> convert_me, wchar_t unknown = L'?');

	template<cjm::numerics::concepts::utf_character UtfChar>
	requires ((!std::numeric_limits<wchar_t>::is_signed))
		std::wstring convert_wide_from_utf(std::basic_string_view<UtfChar> convert_me, wchar_t unknown = L'?');

	std::wstring widen(std::string_view convert_me);

	std::string narrow(std::wstring_view convert_me, char unknown = '?');

	

}



template<cjm::numerics::concepts::character Char>
std::pair<cjm::uint128_tests::istream_utils::char_classification, std::optional<Char>> cjm::uint128_tests::istream_utils::extract_and_classify_next(std::basic_istream<Char, std::char_traits<Char>>& istream)
{
    using char_t = std::remove_cvref_t<std::remove_const_t<Char>>;
    using traits_t = std::remove_cvref_t<std::remove_const_t<std::char_traits<char_t>>>;
    auto local = std::locale("");
    if (istream.eof())
    {
        return std::make_pair(char_classification::eof, std::nullopt);
    }
    if (istream.bad() || istream.fail())
    {
        return std::make_pair(char_classification::error, std::nullopt);
    }
    auto peeked = istream.peek();
    if (istream.good() && peeked != traits_t::eof())
    {
        auto c = char_t{};
        istream.get(c);
        if (istream.bad() || istream.fail() || istream.eof())
			return std::make_pair(char_classification::error, c);
        
        char cast = convert_char<char_t, char>(c);
        const auto& x = std::use_facet<std::ctype<char>>(local);
        // ReSharper disable once CppEqualOperandsInBinaryExpression -- might not be identical everywhere!
        if (x.is(std::ctype<char>::space | std::ctype<char>::blank, cast))
			return std::make_pair(char_classification::space, c);
        if (x.is(std::ctype<char>::alpha | std::ctype<char>::digit | std::ctype<char>::xdigit, cast))        
	        return std::make_pair(char_classification::alphanum, c);
        if (x.is(std::ctype<char>::punct | std::ctype<char>::cntrl, c))        
	        return std::make_pair(char_classification::punctuation, c);
        return std::make_pair(char_classification::other, c);
    }
    if (peeked == traits_t::eof())
    {
	    return std::make_pair(char_classification::eof, std::nullopt);
    }
    return std::make_pair(char_classification::error, std::nullopt);
}

template <typename SourceChar, typename TargetChar>
requires (!std::is_same_v<std::remove_cvref_t<std::remove_const_t<SourceChar>>, std::remove_cvref_t<std::remove_const_t<TargetChar>>>
	&& cjm::numerics::concepts::character<SourceChar>
	&& cjm::numerics::concepts::character<TargetChar>)
	TargetChar cjm::uint128_tests::convert_char(SourceChar c, std::optional<TargetChar> unknown)
{
	TargetChar real_unknown = unknown.value_or(static_cast<TargetChar>('?'));
	if constexpr (std::is_signed_v<TargetChar> == std::is_signed_v<SourceChar>)
	{
		return c >= std::numeric_limits<TargetChar>::min() && c <= std::numeric_limits<TargetChar>::max() ? static_cast<TargetChar>(c) : real_unknown;
	}
	else if constexpr (std::is_signed_v<TargetChar>) //target signed source unsigned
	{
		return (c <= static_cast<std::int64_t>(std::numeric_limits<TargetChar>::max()) &&
			(c >= static_cast<std::int64_t>(std::numeric_limits<TargetChar>::min())) ? static_cast<TargetChar>(c) : real_unknown);
	}
	else //source signed target unsigned
	{
		using unsigned_source_t = std::make_unsigned_t<SourceChar>;
		return c > -1 && static_cast<unsigned_source_t>(c) <= std::numeric_limits<TargetChar>::max() ? static_cast<TargetChar>(c) : real_unknown;
	}
}

template <cjm::numerics::concepts::character Char>
bool cjm::uint128_tests::istream_utils::is_space(Char c, const std::locale& l)
{
    if constexpr (std::is_same_v<std::remove_const_t<Char>, char> || std::is_same_v<std::remove_const_t<Char>, wchar_t>)
    {
        return std::isspace<Char>(c, l);
    }
    else
    {
        const wchar_t cast = convert_char(c);
        return std::isspace(cast, l);
    }
}




template <cjm::numerics::concepts::utf_character UtfChar>
std::basic_string<UtfChar, std::char_traits<UtfChar>> cjm::uint128_tests::convert_to_utf(sv_t source)
{
    std::basic_string<UtfChar, std::char_traits<UtfChar>> ret;
    if (!source.empty())
    {
        ret.reserve(source.length());
        std::transform(source.cbegin(), source.cend(), std::back_inserter(ret), [](char c) -> UtfChar
            {
                return convert_char<char, UtfChar>(c);
            });
    }
    return ret;
}

template <cjm::numerics::concepts::utf_character UtfChar>
std::string cjm::uint128_tests::convert_from_utf(std::basic_string_view<UtfChar> convert_me, char unknown)
{
    std::string ret;
    if (!convert_me.empty())
    {
        ret.reserve(convert_me.size());
        std::transform(convert_me.cbegin(), convert_me.cend(), std::back_inserter(ret), [=](UtfChar c) -> char
            {
                return convert_char<UtfChar, char>(c, unknown);
            });
    }
    return ret;
}

template <cjm::numerics::concepts::utf_character UtfChar>
requires (std::numeric_limits<wchar_t>::is_signed)
std::wstring cjm::uint128_tests::convert_wide_from_utf(std::basic_string_view<UtfChar> convert_me, wchar_t unknown)
{
    std::wstring ret;
    if (!convert_me.empty())
    {
        ret.reserve(convert_me.size());
        std::transform(convert_me.cbegin(), convert_me.cend(), std::back_inserter(ret), [=](UtfChar c) -> wchar_t
            {
                return convert_char<UtfChar, wchar_t>(c, unknown);
            });
    }
    return ret;
}

template<cjm::numerics::concepts::utf_character UtfChar>
requires ((!std::numeric_limits<wchar_t>::is_signed))
std::wstring cjm::uint128_tests::convert_wide_from_utf(std::basic_string_view<UtfChar> convert_me, wchar_t unknown)
{
    std::wstring ret;
    if (!convert_me.empty())
    {
        ret.reserve(convert_me.size());
        std::transform(convert_me.cbegin(), convert_me.cend(), std::back_inserter(ret), [=](UtfChar c) -> wchar_t
            {
                return convert_char<UtfChar, wchar_t>(c, unknown);
            });
    }
    return ret;
}

#endif
