#ifndef CJM_STRING_INL_
#define CJM_STRING_INL_

#include "cjm_string.hpp"

template<typename Char, typename CharTraits, typename Allocator>
requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
std::basic_string<Char, CharTraits, Allocator>
cjm::string::trim(const std::basic_string<Char, CharTraits, Allocator>& trim_me)
{
	using sv = std::basic_string_view<Char, CharTraits>;
	sv ret = trim_me;
	string_helper::ltrim(ret);
	string_helper::rtrim(ret);
	return std::basic_string<Char, CharTraits, Allocator>{ret};
}


template<typename Char, typename CharTraits, typename Allocator>
	requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
std::basic_string<Char, CharTraits, Allocator> cjm::string::trim(std::basic_string<Char, CharTraits, Allocator>&& trim_me)
{
    using sv = std::basic_string_view<Char, CharTraits>;
    std::string here = std::move(trim_me);
    sv ret = here;
    string_helper::ltrim(ret);
    string_helper::rtrim(ret);
    auto return_me = std::string{ ret };
    return return_me;
}


template<typename Char, typename CharTraits>
requires cjm::numerics::concepts::char_with_traits<Char, CharTraits>
void cjm::string::string_helper::ltrim(std::basic_string_view<Char, CharTraits>& l_trim_me)
{
    using char_t = std::remove_const_t<Char>;
    using traits_t = std::remove_const_t<CharTraits>;
    if (!l_trim_me.empty())
    {
        auto locale = std::locale("");
        size_t leading_space = 0;
        for (char_t c : l_trim_me)
        {
            bool is_space;
        	
            if constexpr (!cjm::numerics::is_windows && cjm::numerics::concepts::utf_character<char_t>)
            {
                 is_space = cjm::uint128_tests::istream_utils::is_space<char_t>(c, locale);
            }
            else
            {
                is_space = std::isspace<char_t>(c, locale);
            }
            if (is_space)
            {
                ++leading_space;
            }
            else
            {
                break;
            }
        }
        if (leading_space >= l_trim_me.length())
        {
            l_trim_me = std::basic_string_view<char_t, traits_t>{};
        }
        else
        {
            l_trim_me = l_trim_me.substr(leading_space, l_trim_me.size() - leading_space);
        }
    }
}

template<typename Char, typename CharTraits>
    requires cjm::numerics::concepts::char_with_traits<Char, CharTraits>
void cjm::string::string_helper::rtrim(std::basic_string_view<Char, CharTraits>& r_trim_me)
{
    using char_t = std::remove_const_t<Char>;
    if (!r_trim_me.empty())
    {
        size_t trailing_space = 0;
        for (auto it = r_trim_me.rbegin(); it != r_trim_me.rend(); ++it)
        {
            auto locale = std::locale("");
            char_t c = *it;
            bool is_space;
            if constexpr (!cjm::numerics::is_windows && cjm::numerics::concepts::utf_character<char_t>)
            {
                is_space = cjm::uint128_tests::istream_utils::is_space<char_t>(c, locale);
            }
            else
            {
                is_space = std::isspace<char_t>(c, locale);
            }
            if (is_space)
            {
                ++trailing_space;
            }
            else
            {
                break;
            }
        }
        if (trailing_space >= r_trim_me.length())
        {
            r_trim_me = std::basic_string_view<Char, CharTraits>{};
        }
        else
        {
            r_trim_me = r_trim_me.substr(0, r_trim_me.size() - trailing_space);
        }
    }
}

template<typename Char, typename CharTraits>
requires cjm::numerics::concepts::char_with_traits<Char, CharTraits>
std::basic_string_view<Char, CharTraits> cjm::string::trim_as_sv(std::basic_string_view<Char, CharTraits> trim_me)
{
    auto view_copy = trim_me;
    cjm::string::string_helper::ltrim(view_copy);
    cjm::string::string_helper::rtrim(view_copy);
    return view_copy;
}

#endif
 
