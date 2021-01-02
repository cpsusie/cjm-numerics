#ifndef CJM_STRING_INL_
#define CJM_STRING_INL_

#include "cjm_string.hpp"

template<typename Char, typename CharTraits, typename Allocator>
requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
std::basic_string<Char, CharTraits, Allocator>
cjm::string::trim(std::basic_string<Char, CharTraits, Allocator>&& trimMe)
{
	using str = std::basic_string<Char, CharTraits, Allocator>;
	str toBeTrimmed(std::move(trimMe));
	string_helper::ltrim(toBeTrimmed);
	string_helper::rtrim(toBeTrimmed);
	return toBeTrimmed;
}

template <typename Char, typename CharTraits, typename Allocator>
requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
std::basic_string<Char, CharTraits, Allocator> cjm::string::
trim(const std::basic_string<Char, CharTraits, Allocator>& trimMe)
{
	using str = std::basic_string<Char, CharTraits, Allocator>;
	str copy = trimMe;
	return trim(std::move(copy));
}

template <typename Char, typename CharTraits, typename Allocator>
requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
void cjm::string::string_helper::ltrim(std::basic_string<Char, CharTraits, Allocator>& toBeLTrimmed)
{
	auto it = std::find_if(toBeLTrimmed.begin(), toBeLTrimmed.end(), [](Char c) -> bool
	{
	    if constexpr (!cjm::numerics::has_msc && cjm::numerics::concepts::utf_character<Char>)
        {
	        return !std::isspace<char>(static_cast<char>(c), std::locale(""));
        }
        else
        {
		    return !std::isspace<Char>(c, std::locale(""));
        }
	});
	toBeLTrimmed.erase(toBeLTrimmed.begin(), it);
}
template<typename Char, typename CharTraits, typename Allocator>
requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
void cjm::string::string_helper::rtrim(std::basic_string<Char, CharTraits, Allocator>& toBeRTrimmed)
{
	auto it = std::find_if(toBeRTrimmed.rbegin(), toBeRTrimmed.rend(), [](Char c) -> bool
	{
        if constexpr (!cjm::numerics::has_msc && cjm::numerics::concepts::utf_character<Char>)
        {
            return !std::isspace<char>(static_cast<char>(c), std::locale(""));
        }
        else
        {
            return !std::isspace<Char>(c, std::locale(""));
        }
	});
	toBeRTrimmed.erase(it.base(), toBeRTrimmed.end());
}
#endif 
 
