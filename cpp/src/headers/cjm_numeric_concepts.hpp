#ifndef CJM_NUMERIC_CONCEPTS_HPP_
#define CJM_NUMERIC_CONCEPTS_HPP_
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
#include <functional>



namespace cjm::numerics::concepts
{

    /// <summary>
    /// Applies if the type is a number ... considers anything that specializes
    /// std::numeric_limits to be a number.
    /// </summary>
    template<typename T>
    concept number = std::numeric_limits<T>::is_specialized;

    /// <summary>
    /// Represents an integer, whether built-in or user defined
    /// </summary>
    /// <remarks>considers whether numeric_limits<T>::is_integer is true for that type.
    /// If not specialized, not considered an integer.</remarks>
    template<typename T>
    concept integer = number<T> && std::numeric_limits<T>::is_integer;

    /// <summary>
    /// True if an integer that is also a built-in type.
    /// </summary>
    template<typename T>
    concept builtin_integer = integer<T> && std::is_integral_v<T>;

    /// <summary>
    /// True for an unsigned integer, whether built-in or user-defined
    /// </summary>
    /// <remarks>bases determination off of specialization of std::numeric_limits for the
    /// type in question.</remarks>
    template<typename T>
    concept unsigned_integer = integer<T> && !std::numeric_limits<T>::is_signed;

    /// <summary>
    /// Applies to unsigned integers that are also built-in fundamental types
    /// </summary>
    template<typename T>
    concept builtin_unsigned_integer = builtin_integer<T> && unsigned_integer<T>;
}

namespace cjm::numerics
{
    template<concepts::integer IntegerType>
    struct divmod_result final
    {
        using int_t = IntegerType;
        int_t quotient;
        int_t remainder;
        constexpr divmod_result(int_t quot, int_t rem) noexcept : quotient{ quot }, remainder{ rem } {}
        constexpr divmod_result() noexcept : quotient{}, remainder{} {}
        constexpr divmod_result(const divmod_result& other) noexcept = default;
        constexpr divmod_result(divmod_result&& other) noexcept = default;
        constexpr divmod_result& operator=(const divmod_result& other) noexcept = default;
        constexpr divmod_result& operator=(divmod_result&& other) noexcept = default;
        ~divmod_result() = default;

        constexpr auto operator<=>(const divmod_result& other) const noexcept
        {
            if (other.quotient == quotient)
            {
                if (other.remainder == remainder)
                    return 0;
                return other.remainder > remainder ? -1 : 1;
            }
            return other.quotient > quotient ? -1 : 1;
        }
    };
}

namespace cjm::numerics::concepts
{
    template<typename T, size_t Size>
    concept matches_size = (sizeof(T) == Size);

	template<typename TArray, typename TElement, size_t Size>
    concept is_array_of_T_of_size = std::is_same_v<std::array<TElement, Size>, TArray>;

    template<typename To, typename From>
    concept nothrow_convertible = std::is_same_v<To, From> || std::is_nothrow_convertible_v<From, To>;

	namespace internal
	{
		template<typename To, typename From>
        concept nothrow_castable_helper = requires (From x)
        {
            {static_cast<To>(x)} noexcept -> nothrow_convertible<To>;
        };
	}

    template<typename To, typename From>
    concept nothrow_castable = nothrow_convertible<To, From> || internal::nothrow_castable_helper<To, From>;
	
    template<typename T>
    concept hashable = requires(T x)
    {
        {std::hash<T>{}(x)} noexcept -> nothrow_convertible<size_t>;
    };

    template<concepts::integer IntegerType>
    struct divmod_result;
	
    template<typename T>
    concept cjm_unsigned_integer = !builtin_integer<T> &&
        unsigned_integer<T> &&
        std::is_nothrow_default_constructible_v<T> &&
        std::is_nothrow_convertible_v<typename T::int_part, T> &&
        std::is_nothrow_convertible_v<typename T::divmod_result_t, numerics::divmod_result<T>> && 
        std::is_trivially_copyable_v<T> &&
        std::is_trivially_move_constructible_v<T> &&
        std::is_trivially_copy_assignable_v<T> &&
        std::is_trivially_move_assignable_v<T> &&
        std::is_trivially_destructible_v<T> &&
        !std::is_polymorphic_v<T> &&
        std::is_final_v<T> &&
        std::regular<T> &&
        unsigned_integer<typename T::int_part> &&
        matches_size<typename T::int_part, (sizeof(T) / 2)> &&
        is_array_of_T_of_size<typename T::byte_array, unsigned char, sizeof(T)> && 
        hashable<T>
	;/*&&
									requires (T x, T y, const T& x_const, const T& y_const, int s)
    {
        
    
    };*/
	
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

    /// <summary>
    /// Determines whether you can bitcast From to To:
    ///    to be bit_castable, the types must both:
    ///     1- have the same size and alignment as the other
    ///     2- be no-throw default constructible
    ///     3- be trivially copyable
    /// </summary>
    template<typename To, typename From>
    concept bit_castable = sizeof(To) == sizeof(From) 
        && std::is_trivially_copyable_v<To> && std::is_trivially_copyable_v<From> && !std::is_polymorphic_v<To> && !std::is_polymorphic_v<From> && std::is_nothrow_default_constructible_v<To> && std::is_nothrow_default_constructible_v<From>;


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
