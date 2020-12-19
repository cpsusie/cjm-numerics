//
// Created by cpsusie on 12/17/20.
//
#include <limits>
#include <type_traits>
#include <concepts>
#include <cstdint>
#include <string>
#include <iostream>
#include <sstream>
#include <string>

#ifndef INT128_CJM_NUMERIC_CONCEPTS_HPP
#define INT128_CJM_NUMERIC_CONCEPTS_HPP

namespace cjm::numerics::concepts
{
    template<typename T>
    concept number = std::numeric_limits<T>::is_specialized;

    template<typename T>
    concept integer = number<T> && std::numeric_limits<T>::is_integer;

    template<typename T>
    concept builtin_integer = integer<T> && std::is_integral_v<T>;

    template<typename T>
    concept unsigned_integer = integer<T> && !std::numeric_limits<T>::is_signed;

    template<typename T>
    concept builtin_unsigned_integer = builtin_integer<T> && unsigned_integer<T>;

    template<typename T>
    concept character = std::is_nothrow_convertible_v<T, char> ||
            std::is_nothrow_convertible_v<T, char8_t> ||
                    std::is_nothrow_convertible_v<T, wchar_t> ||
                            std::is_nothrow_convertible_v<T, char16_t> ||
                                    std::is_nothrow_convertible_v<T, char32_t>;

    template<typename Char, typename CharTraits>
    concept char_with_traits = character<Char> && std::is_nothrow_convertible_v<CharTraits, std::char_traits<Char>>;

    template<typename Char, typename CharAllocator>
    concept char_with_allocator = character<Char> && std::is_nothrow_convertible_v<std::allocator<Char>, CharAllocator>;

    template<typename Char, typename CharTraits, typename CharAllocator>
    concept char_with_traits_and_allocator = character<Char> && char_with_traits<Char, CharTraits> && char_with_allocator<Char, CharAllocator>;

    template<typename Allocator>
    concept char_allocator = std::is_nothrow_convertible_v<std::allocator<char>, Allocator>;
    template<typename Allocator>
    concept wchar_allocator = std::is_nothrow_convertible_v<std::allocator<wchar_t>, Allocator>;
    template<typename Allocator>
    concept utf8_char_allocator = std::is_nothrow_convertible_v<std::allocator<char8_t>, Allocator>;
    template<typename Allocator>
    concept utf16_char_allocator = std::is_nothrow_convertible_v<std::allocator<char16_t>, Allocator>;
    template<typename Allocator>
    concept utf32_char_allocator = std::is_nothrow_convertible_v<std::allocator<char32_t>, Allocator>;




    template<typename Char, typename CharTraits> requires char_with_traits<Char, CharTraits>
    struct matching_str_data
    {
        using ostream_t = std::remove_cvref<std::basic_ostream<Char, CharTraits>>;
        using istream_t = std::remove_cvref<std::basic_istream<Char, CharTraits>>;
        using sv_t = std::remove_cvref<std::basic_string_view<Char, CharTraits>>;
        using char_t = std::remove_cvref<Char>;
        using traits_t =  std::remove_cvref<CharTraits>;
    };

    template<typename Char, typename CharTraits, typename CharAllocator> requires char_with_traits_and_allocator<Char, CharTraits, CharAllocator>
    struct matching_str_data_ex :  public matching_str_data<Char, CharTraits>
    {
        using string_t = std::remove_cvref<std::basic_string<Char, CharTraits, CharAllocator>>;
        using sstream_t = std::remove_cvref<std::basic_stringstream<Char, CharTraits, CharAllocator>>;
        using allocator_t = std::remove_cvref<CharAllocator>;
    };
}

#endif //INT128_CJM_NUMERIC_CONCEPTS_HPP
