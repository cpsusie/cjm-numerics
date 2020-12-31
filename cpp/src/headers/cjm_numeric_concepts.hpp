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
#include <optional>
#include <compare>

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
        concept castable_helper = requires (const From x)
        {
            {static_cast<To>(x)} -> nothrow_convertible<To>;
        
        };
		
		template<typename To, typename From>
        concept nothrow_castable_helper = requires (const From x)
        {
            {static_cast<To>(x)} noexcept -> nothrow_convertible<To>;
        };
	}

    template<typename To, typename From>
    concept castable = std::convertible_to<From, To> || internal::castable_helper<To, From>;
	
    template<typename To, typename From>
    concept nothrow_castable = (nothrow_convertible<To, From> || internal::nothrow_castable_helper<To, From>) && castable<To, From>;
	
    template<typename T>
    concept hashable = requires(T x)
    {
        {std::hash<T>{}(x)} -> nothrow_convertible<size_t>;
    };

	template<typename T>
    concept nothrow_hashable = hashable<T> && requires (T x)
    {
        {std::hash<T>{}(x)} noexcept -> nothrow_convertible<size_t>;
    };

    template<concepts::integer IntegerType>
    struct divmod_result;

    template<typename T>
    concept size_evenly_divisible_by_char_bit = (sizeof(T) % CHAR_BIT) == 0;
	
    template<typename T>
    concept cjm_unsigned_integer =
        //not a built-in type
        !builtin_integer<T> &&
        //is an unsigned integer
        unsigned_integer<T> &&
        // size evenly divides CHAR_BIT ... (CHAR_BIT == bits in byte on current platform)
        size_evenly_divisible_by_char_bit<T> &&
        //default constructible without exceptions
        std::is_nothrow_default_constructible_v<T> &&
        //defines a dependent type called int_part (limbs) are nothrow convertible to it
        std::is_nothrow_convertible_v<typename T::int_part, T> &&
        //it defines a proper dependent-type for divmod operations
        std::is_nothrow_convertible_v<typename T::divmod_result_t, numerics::divmod_result<T>> &&
        //trivially copyable
        std::is_trivially_copyable_v<T> &&
        //trivially move constructible
        std::is_trivially_move_constructible_v<T> &&
        //trivially copy assignable
        std::is_trivially_copy_assignable_v<T> &&
        //trivially move assignable
        std::is_trivially_move_assignable_v<T> &&
        //can be constructed explicitly from two int_parts
        std::is_constructible_v<T, typename T::int_part, typename T::int_part> &&
        //trivially destructible
        std::is_trivially_destructible_v<T> &&
        //has no virtual member functions and no v-table
        !std::is_polymorphic_v<T> &&
        //can't be used in a dynamic type-hierarchy
        std::is_final_v<T> &&
        //is a StandardLayout type
        std::is_standard_layout_v<T> &&
        //is regular (i.e. copy and move assignable) and equality comparable
        std::regular<T> &&
        //is totally ordered (offers ==, !=, >, <, >=, <=)
        std::totally_ordered<T> &&
        //its int_part dependent type (i.e. limb type) is an unsigned integer type
        unsigned_integer<typename T::int_part> &&
        //its int_part dependent type (i.e. limb type) is half its size
        matches_size<typename T::int_part, (sizeof(T) / 2)> && //its int_part_type 
        //defines dependent type byte_array as a std::array of unsigned char of length equal to sizeof(T)
        is_array_of_T_of_size<typename T::byte_array, unsigned char, sizeof(T)> &&
        //can be hashed without throwing any exception by using a specialization of std::hash
        nothrow_hashable<T> &&
        //implicit no-throw conversions exist to it from at least the following types:
        //bool, char, signed char, short, int, long, std::int64_t.
        //unsigned char, unsigned short, unsigned int, unsigned long, std::uint64_t
        //I do not include long long or unsigned long long because it is possible that
        //a compiler might want to make a built-in int128 defined as long long or unsigned long long
        //std::uint64_t and std::int64_t cover the case of integers that, by definition, must be 64-bit
        nothrow_convertible<T, bool> &&
        nothrow_convertible<T, char> &&
        nothrow_convertible<T, signed char> &&
        nothrow_convertible<T, short> &&
        nothrow_convertible<T, int> &&
        nothrow_convertible<T, long> &&
        nothrow_convertible<T, std::int64_t> &&
        nothrow_convertible<T, unsigned char> &&
        nothrow_convertible<T, unsigned short> &&
        nothrow_convertible<T, unsigned int> &&
        nothrow_convertible<T, unsigned long> &&
        nothrow_convertible<T, std::uint64_t> &&
        //must be explicitly or implicitly nothrow convertible to at least the following types:                
        //bool, char, signed char, unsigned char, char8_t, char16_t, char32_t, wchar_t,
        //short, int, long, long long, unsigned short, unsigned long, unsigned long long
        //std::int64_t, std::uint64_t
        //T::int_part
        nothrow_castable<bool, T> &&
        nothrow_castable<char, T> &&
        nothrow_castable<signed char, T> &&
        nothrow_castable<unsigned char, T> &&
        nothrow_castable<char8_t, T> &&
        nothrow_castable<char16_t, T> &&
        nothrow_castable<char32_t, T> &&
        nothrow_castable<wchar_t, T> &&
        nothrow_castable<short, T>&&
        nothrow_castable<int, T>&&
        nothrow_castable<long, T>&&
        nothrow_castable<long long, T> &&
        nothrow_castable<std::int64_t, T> && 
        nothrow_castable<unsigned short, T>&&
        nothrow_castable<unsigned int, T>&&
        nothrow_castable<unsigned long, T>&&
        nothrow_castable<unsigned long long, T>&&
        nothrow_castable<std::uint64_t, T> &&
        nothrow_castable<typename T::int_part, T> &&
        //must be explicitly castable to the following types (exceptions permitted):
        //float, double, long double
        castable<float, T> &&
        castable<double, T> &&
        castable<long double, T> &&
        requires (T x, T y, const T x_const, const T y_const, const int s)
    {
        //implements all binary arithmetic operators.  Only division and modulus may throw exception.
        {x_const + y_const}                 noexcept    ->  nothrow_convertible<T>;
        {x_const - y_const}                 noexcept    ->  nothrow_convertible<T>;
        {x_const* y_const}                  noexcept    ->  nothrow_convertible<T>;
        {x_const / y_const}                             ->  nothrow_convertible<T>;
        {x_const% y_const}                              ->  nothrow_convertible<T>;
        //implements binary bitwise operators all nothrow
        {x_const& y_const}                  noexcept    ->  nothrow_convertible<T>;
        {x_const | y_const}                 noexcept    ->  nothrow_convertible<T>;
        {x_const^ y_const}                  noexcept    ->  nothrow_convertible<T>;
        //implements binary bit-shift operators with both a T operand and and int operand none throw exceptions
        {x_const << y_const}                noexcept    ->  nothrow_convertible<T>;
        {x_const >> y_const}                noexcept    ->  nothrow_convertible<T>;
        {x_const << s}                      noexcept    ->  nothrow_convertible<T>;
        {x_const >> s}                      noexcept    ->  nothrow_convertible<T>;
        //implements unary +,-,~,! operators
        {+x_const}                          noexcept    ->  nothrow_convertible<T>;
        {-x_const}                          noexcept    ->  nothrow_convertible<T>;
        {~x_const}                          noexcept    ->  nothrow_convertible<T>;
        {!x_const}                          noexcept    ->  nothrow_convertible<bool>;
        //implements prefix and postfix increment and decrement operators, which do not throw exceptions
        {T{ ++x }}                          noexcept    ->  std::same_as<T>;
        {T{ --x }}                          noexcept    ->  std::same_as<T>;
        {T{ x-- }}                          noexcept    ->  std::same_as<T>;
        {T{ x++ }}                          noexcept    ->  std::same_as<T>;
        //implements the following compound assignment operators which may not throw exceptions:
        // +=(const T); -=(const T); *=(const T); &=(const T); |=(const T); ^=(const T);
        // <<=(const int), >>=(const int) --- AND TO BE ADDED: <<=(const T), >>=(const T)
        {y += x_const}                      noexcept;
        {y -= x_const}                      noexcept;
        {y *= x_const}                      noexcept;
        {y &= x_const}                      noexcept;
        {y |= x_const}                      noexcept;
        {y ^= x_const}                      noexcept;
        //todo fixit implement
        //{y <<= x_const}                   noexcept;
        //{y >>= x_const}                   noexcept;
        {y <<= s}                           noexcept;
        {y >>= s}                           noexcept;
        //implements the following compound assignment operators which may throw exceptions:
        {y /= x_const};
        {y %= x_const};
        //has nothrow const functions that return int_part
        {x_const.low_part()}                noexcept    -> nothrow_convertible<typename T::int_part>;
        {x_const.high_part()}               noexcept    -> nothrow_convertible<typename T::int_part>;
        //has nothrow const functions that return byte_array
        {x_const.to_little_endian_arr()}    noexcept    -> nothrow_convertible<typename T::byte_array>;
        {x_const.to_big_endian_arr()}       noexcept    -> nothrow_convertible<typename T::byte_array>;
        //offers spaceship comparisons, which will not except:
        { x_const <=> y_const } noexcept    -> nothrow_convertible<std::strong_ordering>;
    };
	
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
