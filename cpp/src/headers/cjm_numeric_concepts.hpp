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

    static_assert(builtin_unsigned_integer <std::uint64_t>);
    static_assert(builtin_unsigned_integer <std::uint8_t>);
}

namespace cjm::numerics
{
    template<concepts::integer IntegerType>
    struct divmod_result;
    	
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

        constexpr std::strong_ordering operator<=>(const divmod_result& other) const noexcept
        {
            if (other.quotient == quotient)
            {
                if (other.remainder == remainder)
                    return std::strong_ordering::equal;
                return other.remainder > remainder ? std::strong_ordering::less : std::strong_ordering::greater;
            }
            return other.quotient > quotient ? std::strong_ordering::less : std::strong_ordering::greater;;
        }

        friend constexpr bool operator==(const divmod_result& l, const divmod_result& r) noexcept = default;
        friend constexpr bool operator!=(const divmod_result& l, const divmod_result& r) noexcept = default;
        friend constexpr bool operator<(const divmod_result& l, const divmod_result& r) noexcept = default;
        friend constexpr bool operator>(const divmod_result & l, const divmod_result & r) noexcept = default;
        friend constexpr bool operator>=(const divmod_result& l, const divmod_result& r) noexcept = default;
        friend constexpr bool operator<=(const divmod_result& l, const divmod_result& r) noexcept = default;
    };
}

namespace std
{
	template<cjm::numerics::concepts::integer Integer>
	struct hash<cjm::numerics::divmod_result<Integer>>
	{
        constexpr hash() noexcept = default;
        constexpr size_t operator()(const cjm::numerics::divmod_result<Integer>& hash_me) const noexcept;
	};

	template <cjm::numerics::concepts::integer Integer>
	constexpr size_t hash<cjm::numerics::divmod_result<Integer>>::operator()(
		const cjm::numerics::divmod_result<Integer>& hash_me) const noexcept
	{
        static_assert(sizeof(size_t) == 8 || sizeof(size_t) == 4, "Only 32 and 64 bit architecture supported.");
        auto hash = size_t{ 0 };
		size_t hash_quot = std::hash<Integer>{}(hash_me.quotient);
		size_t hash_rem = std::hash<Integer>{}(hash_me.remainder);
		hash = hash_quot + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash = hash_rem + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
		
	}
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



    template<typename DivModResType, typename IntegerType>
    concept division_modulus_result =   integer<IntegerType> &&
										std::is_nothrow_default_constructible_v<DivModResType> &&
										std::is_trivially_copyable_v<DivModResType> &&
										std::constructible_from<IntegerType, IntegerType> &&
										std::is_standard_layout_v<DivModResType> &&
										nothrow_hashable<IntegerType> &&
										nothrow_hashable<DivModResType> &&
										std::totally_ordered<DivModResType> && requires (const DivModResType dmres, IntegerType it)
    {
        {it = dmres.quotient}    noexcept;
        {it = dmres.remainder}   noexcept;
        {DivModResType{ it, it }}  noexcept;
    };
	
    static_assert(division_modulus_result<divmod_result<std::uint64_t>, std::uint64_t>);
    static_assert(integer<std::uint64_t>);
    static_assert(std::is_standard_layout_v<divmod_result<std::uint64_t>>);
    static_assert(std::totally_ordered < divmod_result<std::uint64_t>>);
    template<typename T>
    concept size_evenly_divisible_by_char_bit = (sizeof(T) % CHAR_BIT) == 0;

    namespace internal
    {
        template<typename T>
        concept has_static_fls_mem_func = unsigned_integer<T> && requires (const T value)
        {
            {T::most_sign_set_bit(value)} noexcept -> nothrow_convertible<int>;
        };
    }

    ///<summary>
    ///A type complies with this constraint by being a built-in unsigned integer or by being
    ///a user-defined unsigned integer type that has the following non-throwing static member function
    ///that accepts a value of that type and returns the bitpos of the most significant set-bit:
    /// int T::most_sign_set_bit([const] T value).
    /// If a type fulfills this concept, you can use the following function template to get the bitpos
    /// of the value's most significant set bit:
    /// (in namespace cjm::numerics) ---
    /// template<cjm::numerics::concepts::can_find_most_significant_set_bitpos UI>
    /// constexpr int most_sign_set_bit(UI value) noexcept;
    /// </summary>
    template<typename T>
    concept can_find_most_significant_set_bitpos = builtin_unsigned_integer<T> || internal::has_static_fls_mem_func<T>;

    template<typename T>
    concept cjm_unsigned_integer =
        //you can find the bitpos of the most significant set bit in the value using ( in namespace cjm numerics)
        // the following template function:
        // template<cjm::numerics::concepts::can_find_most_significant_set_bitpos UI>
        // constexpr int most_sign_set_bit(UI value) noexcept;
        can_find_most_significant_set_bitpos<T> &&
        //limb tye is half size of type
        (sizeof(typename T::int_part) == sizeof(T) /2) &&
        //has static constexpr member called int_parts_bits equal to number of bits in int_part
		(T::int_part_bits == std::numeric_limits<typename T::int_part>::digits) &&
        //has static constexpr member called int_part_bottom_half_bits that equals half of int_part_bits
        (T::int_part_bottom_half_bits == T::int_part_bits / 2) &&
        //has static constexpr member called int_part_bottom_half_bitmask equal to the maximum value of type int_part
        //right shifted by int_part_bottom_half_bits
        (T::int_part_bottom_half_bitmask == std::numeric_limits<typename T::int_part>::max() 
            >> T::int_part_bottom_half_bits )&&
        //not a built-in type
        !builtin_integer<T> &&
        //is an unsigned integer
        unsigned_integer<T> &&
        division_modulus_result<typename T::divmod_result_t, T> &&
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
        requires (T x, T y, const T x_const, const T y_const, const int s, const typename T::byte_array arr)
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
        //offers spaceship comparisons, which will not throw and will return a std::strong_ordering:
        { x_const <=> y_const }             noexcept    -> nothrow_convertible<std::strong_ordering>;
        //has nothrow static factories that create type when passed T::byte_arr
    	{T::make_from_bytes_little_endian(arr)}
											noexcept    -> nothrow_convertible<T>;
		{T::make_from_bytes_big_endian(arr)}
                                            noexcept    -> nothrow_convertible<T>;
		//offers a static nothrow unsafe div_mod method that returns T::divmod_result_t (undefined behavior if divisor == 0
    	{T::unsafe_div_mod(x_const, y_const)}
											noexcept    -> nothrow_convertible<typename T::divmod_result_t>;
        //offers a static nothrow  try_div_mod method that returns std::optional<T::divmod_result_t> returns
    	//std::nullopt if and only if divisor == 0
        {T::try_div_mod(x_const, y_const)}
                                            noexcept    -> nothrow_convertible<std::optional<typename T::divmod_result_t>>;
        //offers a div_mod method that returns T::divmod_result_t (throws std::domain_error if and only if divisor == 0)
        {T::div_mod(x_const, y_const)}
													    -> nothrow_convertible<typename T::divmod_result_t>;
    };
    template<typename Integer>
    concept builtin_128bit_integer = builtin_integer<Integer> && std::numeric_limits<Integer>::digits == 128;

    template<typename Integer>
    concept builtin_128bit_unsigned_integer = unsigned_integer<Integer> && builtin_integer<Integer>;
	
    template<typename T>
    concept character = std::is_same_v<T, char> ||
            std::is_same_v<T, char8_t> ||
                    std::is_same_v<T, wchar_t> ||
                            std::is_same_v<T, char16_t> ||
                                    std::is_same_v<T, char32_t>;

    template<typename T>
    concept utf_character = character<T> && (std::is_same_v<T, char8_t> ||
                                             std::is_same_v<T, char16_t> ||
                                             std::is_same_v<T, char32_t>);

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
