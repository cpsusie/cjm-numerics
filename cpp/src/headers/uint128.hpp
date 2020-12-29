#ifndef CJM_UINT128_HPP
#define CJM_UINT128_HPP
#include<cstdint>
#include<type_traits>
#include<limits>
#include<cassert>
#include<optional>
#include "numerics.hpp"
#include "cjm_numeric_concepts.hpp"
#include "cjm_string.hpp"
// Copyright 2018 CJM Screws, LLC
// 
// This contents of this file (uint128.hpp) and its inline implementation file (uint128.inl)
// are a modified version of the code released by "The Abseil Authors" in the file "int128.h"
// and "int128.cc".  The Copyright to the original Abseil code may be found below in this file.
// A copy of the original, unaltered files are included in this project as abseil_int128_h.txt (for int128.h)
// and abseil_int128_cpp.txt (for int128.cc) for your reference.  CJM Screws, LLC asserts no copyright to the
// original work of The Abseil Authors but claims copyright to its changes to the work of The Abseil Authors 
// and all additions it has made thereto.
// 
// CJM Screws, LLC releases its copyrighted work to you under the following terms and conditions:
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation 
// files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, 
// merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions: 
// 
// A copy of this notice, including the verbatim description of CJM Screws, LLC's business and the link to http://www.mdtitaniumhobbies.com as
// well as the Copyright notice of The Abseil Authors  shall be included in all copies or substantial portions of the Software. 
// 
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
// FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
// OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// CJM Screws, LLC (a Maryland Limited Liability Company) trades as Maryland Titanium Hobbies and may be
// reached at http://www.mdtitaniumhobbies.com.  We resell titanium alloy screws and provide the value-added
// service of decorative color anodization of these screws.  These high-strength, light weight and corrosion-resistant
// screws can make a beautiful addition to computer cases, knives, guns, drones, 
// electronic instrumentation panels and other projects.  
//
// The below notice is the Copyright asserted by The Abseil Authors on the source code CJM Screws, LLC 
// used to produce this software (which CJM Screws, LLC has modified significantly):
//
// Copyright 2017 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -----------------------------------------------------------------------------
// File: int128.h
// -----------------------------------------------------------------------------

//If compiled on a Big Endian system, change "true" to "false"
#define CJM_NUMERICS_LITTLE_ENDIAN true

namespace cjm::numerics
{
    class uint128;
    template<typename LimbType>
    class fixed_uint;

    template<concepts::integer IntegerType>
    struct divmod_result;
    
	
    /// <summary>
    /// The calculation mode used when not constant evaluating uint128s.
    /// For GCC and Clang, if a built-in uint128_t is available, that logic is used.
    /// On microsoft x64 (Intel/AMD), compiler intrinsics are used
    /// Otherwise, the --presumably-- slower, constant-evaluation-capable
    /// algorithm is used.
    /// </summary>
    constexpr uint128_calc_mode calculation_mode = init_eval_mode();

    namespace internal
    {
        constexpr int fls_int_part(std::uint64_t n) noexcept;

        constexpr int fls_slow(std::uint64_t n) noexcept;

        template <typename T>
        constexpr void step(T& n, int& pos, int shift) noexcept;
    }

    /// <summary>
    /// Stream insertion operator for uint128
    /// </summary>
    /// <typeparam name="Char">The Character type</typeparam>
    /// <typeparam name="CharTraits">traits for character</typeparam>
    /// <typeparam name="Allocator">allocator for character</typeparam>
    /// <param name="os">the stream to insert</param>
    /// <param name="v">the value to stringify and insert into the stream</param>
    /// <returns>the stream with the value inserted.</returns>
    template <typename Char = char, typename CharTraits=std::char_traits<Char>, typename Allocator = std::allocator<Char>>
            requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
    std::basic_ostream<Char, CharTraits>& operator<<(std::basic_ostream<Char, CharTraits>& os, uint128 v);
    //Comparison operators
    constexpr auto operator <=>(uint128 lhs, uint128 rhs) noexcept; 
    constexpr bool operator==(uint128 lhs, uint128 rhs) noexcept;
    constexpr bool operator!=(uint128 lhs, uint128 rhs) noexcept;
    constexpr bool operator>(uint128 lhs, uint128 rhs) noexcept;
    constexpr bool operator<(uint128 lhs, uint128 rhs) noexcept;
    constexpr bool operator>=(uint128 lhs, uint128 rhs) noexcept;
    constexpr bool operator<=(uint128 lhs, uint128 rhs) noexcept;
    //Unary operators
    constexpr uint128 operator-(uint128 operand) noexcept;
    constexpr uint128 operator+(uint128 operand) noexcept;
    constexpr uint128 operator~(uint128 operand) noexcept;
    constexpr bool operator!(uint128 operand) noexcept;
    //Logical operators
    constexpr uint128 operator&(uint128 lhs, uint128 rhs) noexcept;
    constexpr uint128 operator|(uint128 lhs, uint128 rhs) noexcept;
    constexpr uint128 operator^(uint128 lhs, uint128 rhs) noexcept;
    //bit shift operators
    constexpr uint128 operator>>(uint128 lhs, int amount) noexcept;
    constexpr uint128 operator<<(uint128 lhs, int amount) noexcept;
    constexpr uint128 operator>>(uint128 lhs, uint128 amount) noexcept;
    constexpr uint128 operator<<(uint128 lhs, uint128 amount) noexcept;
    //arithmetic operators
    constexpr uint128 operator+(uint128 lhs, uint128 rhs) noexcept;
    constexpr uint128 operator-(uint128 lhs, uint128 rhs) noexcept;
    constexpr uint128 operator*(uint128 lhs, uint128 rhs) noexcept;
    //Division and modulus are friends declared within class

    namespace math_functions
    {
        template<>
        constexpr uint128 int_sign(uint128 val) noexcept;

        template<>
        constexpr uint128 int_gcd(uint128 first, uint128 second) noexcept;

        template<>
        constexpr uint128 int_lcm(uint128 first, uint128 second);
    }

    enum class u128_str_format
    {
        Illegal=0,
        Zero,
        Decimal,
        Hexadecimal
    };

    template<typename Chars, typename CharTraits = std::char_traits<Chars>>
              requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
    struct u128_parsing_helper
    {
        using char_t = Chars;

        template<typename Allocator = std::allocator<Chars>>
        requires cjm::numerics::concepts::char_with_traits_and_allocator<Chars, CharTraits, Allocator>
        using str128 = std::basic_string<Chars, CharTraits, Allocator>;

        using sv = std::basic_string_view<Chars, CharTraits>;


        static constexpr std::array<sv, 2> get_hex_tags();

        static constexpr sv non_decimal_separator();

        static constexpr sv decimal_separator();

        static constexpr std::uint8_t get_value_hex(char_t c);

        static constexpr std::uint8_t get_value_dec(char_t c);
        static constexpr bool is_legal_hex_char(char_t c);

        template<typename Allocator = std::allocator<char_t>>
            requires cjm::numerics::concepts::char_with_traits_and_allocator<Chars, CharTraits, Allocator>
        static constexpr u128_str_format get_format(const str128<Allocator>& string)
        {
            constexpr char zero = '0';
            constexpr char_t zero_cast = static_cast<char_t>(zero);
            auto length = string.length();
            if (length < 1)
                return u128_str_format::Illegal;
            bool allZero = std::all_of(string.cbegin(), string.cend(), [] (char_t c) -> bool
            {
                return c == zero_cast;
            });
            if (string[0] == zero_cast && (length == 1 || allZero))
                return u128_str_format::Zero;
            if (length < 3)
            {
                if (string[0] == zero_cast)
                    return u128_str_format::Illegal;
                return std::all_of(string.cbegin(), string.cend(),
                                   [](char_t c) -> bool { return static_cast<char>(c) >= 0x30 && static_cast<char>(c) <= 0x39; }) ?
                       u128_str_format::Decimal :
                       u128_str_format::Illegal;
            }
            std::basic_string_view<Chars, CharTraits>  orig_view = string;
            auto firstTwo = orig_view.substr(0, 2);
            auto hex_tags = get_hex_tags();
            bool hasHexTag = std::any_of(hex_tags.cbegin(), hex_tags.cend(), [=](sv tag) -> bool { return tag == firstTwo; });
            if (!hasHexTag)
            {
                return orig_view[0] != zero_cast && std::all_of(orig_view.cbegin(), orig_view.cend(),
                                                       [](char_t c) -> bool { return static_cast<char>(c) >= 0x30 && static_cast<char>(c) <= 0x39; })
                       ?	u128_str_format::Decimal :
                       u128_str_format::Illegal;
            }
            sv afterFirstTwo = orig_view.substr(2, string.length() - 2);
            return std::all_of(afterFirstTwo.cbegin(), afterFirstTwo.cend(), [](char_t c) -> bool {return is_legal_hex_char(c); })
                   ?	u128_str_format::Hexadecimal :
                   u128_str_format::Illegal;
        }


        template<typename Allocator = std::allocator<char_t>>
            requires cjm::numerics::concepts::char_with_traits_and_allocator<Chars, CharTraits, Allocator>
        static str128<Allocator> trim_and_strip(str128<Allocator> trim_and_strip_me)
        {
            auto trimmed = cjm::string::trim(std::move(trim_and_strip_me));
            trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), non_decimal_separator()[0]), trimmed.end());
            return trimmed;
        }

        static constexpr uint128 parse_decimal_str(sv decimal_str);

    };

}


namespace cjm::numerics
{
    /************************************************************************/
    /* An unsigned 128 bit-integer nearly identical algorithmically
     * and based almost entirely on the uint128 provided in abseil.io's int128.h
     * file.  Some stylistic changes have been made such as naming conventions, etc.
     * The string and stream operations have been modified to add support
     * for strings and streams that are wider than const char by turning them into templates.
     *
     * The essence of CJM's modifications  to abseil.io's code is:
     *    1- REMOVING ALL SUPPORT FOR C++ VERSIONS PRIOR TO C++20  THIS CODE WILL NOT WORK
     *    ON A C++17 OR EARLIER (OR ... SHUDDERS ... EARLIER) ENVIRONMENT.
     *    This code makes VERY heavy use of C++17 and C++20 facilities.
     *    2- making every mathematical operation (exception conversions to binary floating
     *    point representations) constexpr-enabled -- i.e. capable of being performed
     *    at compile time (note that stream insertion and string conversion operations
     *    ARE NOT constexpr-enabled)
     *    3- making nearly every mathematical operation specify noexcept.
     *     The operations that MAY throw exceptions are division and modulus,
     *     string and stream output, and conversions to and from binary floating
     *     point representations.
     *    4- Instead of being undefined behavior, division or modulus by 0 with
     *     the operators provided herein  WILL THROW a std::domain_error exception.
     *    5- A custom literal has been added.  You will need to use
     *    the cjm::numerics::uint128_literals namespace to make use of it.
     *    The literal suffix for uint128 is _u128.  It currently supports decimal
     *    and hexadecimal literals.  It supports character separator ( ' ).  OCTAL
     *    is not supported and there are no plans to support it.  Support for binary
     *    literals may be added later (but I can't imagine wanting to out all 128 bits in 1s and
     *    zeros...).  For numbers that will fit within a std::uint64_t, you are free
     *    to use those literals if you desire octal or binary.
     *    6- numeric limits and type traits information have been added to namespace std
     *    to support interoperation with this type in a metaprogramming context.
     *    7- where available (e.g. 64-bit versions of GCC and Clang), this type serves
     *       as a thin wrapper around the native support those compilers add for unsigned __int128.
     *       Using this type still provides advantages over direct use of the native version:
     *          a. Literal support (constexpr of course!)
     *          b. stream insertion and extraction support / string conversion and parsing support
     *          c. uses built-in type for arithmetic anyway
     *          d. portable to compilers that DO NOT offer such a type
     *          e. full constexpr support (except string / float ops) even if those compilers do not treat unsigned __int128
     *             as a completely constexpr-enabled type
     *    8- if using microsoft's compiler targeting windows with an Intel or AMD 64 bit processor,
     *       compiler intrinsics will be used for heavy arithmetic computation in lieu of fallback arithmetic
     *       used when calculations are PERFORMED AT RUNTIME.  This means you get the full-benefit of constexpr
     *       for compile-time evaluation but (presumably) more efficient compiler intrinsics for runtime evaluation.
     *   9- if using microsoft's compiler targeting windows with an Intel or AMD 64 bit processor the RUNTIME division algorithm
     *      is based on Clang-LLVM's division algorithm for the native unsigned __int128.  The paths in that algorithm
     *      that make use of inline assembly in Clang make use of x64 compiler intrinsics for this adaptation.  NOTE:
     *      constant-evaluated division uses the adapted fallback algorithm from abseil/google.
     *    Example literals
     *    Hex examples:
     *		constexpr uint128 x =  0x123456789ABCDEF0123456789ABCDEF0_u128;
     *		constexpr auto x =  0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u128;
     *		constexpr auto x =  0x1234'5678'9abc'def0'1234'5678'9abc'def0_u128;
     *		constexpr auto x = 0X1234'5678'9abc'def0'1234'5678'9abc'def0_u128;
     *		constexpr auto x =  0X1234'5678'9abc'def0'1234'5678'9abc'def0_u128; (capital X ok too)
     *		constexpr auto x =  0x1'23'456'789ABCDEF012345678'9AB'C'DEF'0_u128; //stupid but legal
     *		constexpr auto x = 0x0_u128;
     *	  Decimal examples:
     *	    constexpr uint128 x = 340282366920938463463374607431768211455_u128; (i.e. max value)
     *	    constexpr auto x = 340'282'366'920'938'463'463'374'607'431'768'211'455_u128; //thousands separators
     *	    constexpr auto x = 0_u128; //0 is ok too
     *	    constexpr auto x = 00_u128; //even multiple leading zeros if all are 0
     *	  Illegal examples:
     *		constexpr auto x = 001_u128; //Illegal: non-zero literal other than with hex (0x)
     *		                               prefix represents octal and octal is not supported.
     *	   	constexpr auto x = 340'282'366'920'938'463'463'374'607'431'768'211'456_u128;  //(too big by one)
     *     */
    /************************************************************************/
    class alignas(uint128_align) uint128 final
    {
    public:
        static constexpr size_t byte_array_size{ (128 / CHAR_BIT) / sizeof(unsigned char) };

    	friend class fixed_uint<uint128>;
            	
        using int_part = std::uint64_t;      
        using byte_array = std::array<unsigned char, byte_array_size>;
        using divmod_result_t = divmod_result<uint128>;
    	
        template<typename Chars, typename CharTraits = std::char_traits<Chars>>
                requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
        static uint128 make_from_string(std::basic_string_view<Chars, CharTraits> parseMe);
        static constexpr uint128 make_from_bytes_little_endian(byte_array bytes) noexcept;
        //todo fixit implement:
    	//static constexpr uint128 make_from_bytes_big_endian(byte_array bytes) noexcept;
        static constexpr uint128 MakeUint128(std::uint64_t high, std::uint64_t low) noexcept;
        static constexpr std::optional<divmod_result_t> try_div_mod(uint128 dividend, uint128 divisor) noexcept;
    	static constexpr divmod_result_t div_mod(uint128 dividend, uint128 divisor);
        static constexpr divmod_result_t unsafe_div_mod(uint128 dividend, uint128 divisor) noexcept;
        static void instrumented_div_mod(std::basic_ostream<char>& stream, uint128 dividend, uint128 divisor,
            uint128* quotient_ret, uint128* remainder_ret);
        static constexpr int_part int_part_bits{ sizeof(int_part) * CHAR_BIT };
        static constexpr int_part int_part_bottom_half_bits{ int_part_bits / 2 };
        static constexpr int_part int_part_bottom_half_bitmask{std::numeric_limits<int_part>::max() >> int_part_bottom_half_bits};
        constexpr uint128() noexcept;
        constexpr uint128(const uint128& other) noexcept = default;
        constexpr uint128(uint128&& other) noexcept = default;
        constexpr uint128& operator=(const uint128& other) noexcept = default;
        constexpr uint128& operator=(uint128&& other) noexcept = default;
        ~uint128() noexcept = default;
        // Constructors from arithmetic types
        constexpr uint128(int v) noexcept;
        constexpr uint128(unsigned int v) noexcept;
        constexpr uint128(long v) noexcept;
        constexpr uint128(unsigned long v) noexcept;
        constexpr uint128(long long v) noexcept;
        constexpr uint128(unsigned long long v) noexcept;
        // Assignment operators from arithmetic types
        constexpr uint128& operator=(int v) noexcept;
        constexpr uint128& operator=(unsigned int v) noexcept;
        constexpr uint128& operator=(long v) noexcept;
        constexpr uint128& operator=(unsigned long v) noexcept;
        constexpr uint128& operator=(long long v) noexcept;
        constexpr uint128& operator=(unsigned long long v) noexcept;

        // Conversion operators to other arithmetic types
        constexpr explicit operator bool() const noexcept;
        constexpr explicit operator char() const noexcept;
        constexpr explicit operator signed char() const noexcept;
        constexpr explicit operator unsigned char() const noexcept;
        constexpr explicit operator char16_t() const noexcept;
        constexpr explicit operator char32_t() const noexcept;
        constexpr explicit operator wchar_t() const noexcept;
        constexpr explicit operator short() const noexcept;
        constexpr explicit operator unsigned short() const noexcept;
        constexpr explicit operator int() const noexcept;
        constexpr explicit operator unsigned int() const noexcept;
        constexpr explicit operator long() const noexcept;
        constexpr explicit operator unsigned long() const noexcept;
        constexpr explicit operator long long() const noexcept;
        constexpr explicit operator unsigned long long() const noexcept;
        explicit operator float() const;
        explicit operator double() const;
        explicit operator long double() const;

        //ctor, assign op and explicit to-conversion for intrinsic unsigned __int128
#ifdef CJM_HAVE_BUILTIN_128
        uint128(unsigned __int128 other) noexcept;
        uint128& operator=(unsigned __int128 other) noexcept;
        explicit operator unsigned __int128() const noexcept;    	
#endif
        //hash code function and comparison operators
        [[nodiscard]] constexpr size_t hash_code() const noexcept;
               
        // Arithmetic operators.
        constexpr uint128& operator+=(uint128 other) noexcept;
        constexpr uint128& operator-=(uint128 other) noexcept;
        constexpr uint128& operator*=(uint128 other) noexcept;
        // Long division/modulo for uint128.
        constexpr uint128& operator/=(uint128 other);
        constexpr uint128& operator%=(uint128 other);
        constexpr uint128 operator++(int) noexcept;
        constexpr uint128 operator--(int) noexcept;
        constexpr uint128& operator<<=(int amount) noexcept;
        constexpr uint128& operator>>=(int amount) noexcept;
        constexpr uint128& operator&=(uint128 other) noexcept;
        constexpr uint128& operator|=(uint128 other) noexcept;
        constexpr uint128& operator^=(uint128 other) noexcept;
        constexpr uint128& operator++() noexcept;
        constexpr uint128& operator--() noexcept;
        friend constexpr uint128 operator/(uint128 lhs, uint128 rhs);
        friend constexpr uint128 operator%(uint128 lhs, uint128 rhs);
        friend constexpr uint128 operator>>(uint128 lhs, int amount) noexcept;
        friend constexpr uint128 operator<<(uint128 lhs, int amount) noexcept;
        friend constexpr uint128 operator>>(uint128 lhs, uint128 amount) noexcept;
        friend constexpr uint128 operator<<(uint128 lhs, uint128 amount) noexcept;
        template<typename Char, typename CharTraits, typename Allocator>
            requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
        friend std::basic_ostream<Char, CharTraits>& operator<<(std::basic_ostream<Char, CharTraits>& os, uint128 v);

        //Accessor for sub-compenents
        [[nodiscard]] constexpr int_part low_part() const noexcept;
        [[nodiscard]] constexpr int_part high_part() const noexcept;

        [[nodiscard]] constexpr byte_array to_little_endian_arr() const noexcept;
        [[nodiscard]] constexpr byte_array to_big_endian_arr() const noexcept;

    private:
        constexpr uint128(int_part high, int_part low) noexcept;
        static constexpr size_t calculate_hash(int_part hi, int_part low) noexcept;
        static constexpr void hash_combine(size_t& seed, size_t newVal) noexcept;
        inline static uint128 lshift_msvc_x64(uint128 shift_me, int shift_amount) noexcept;
        inline static uint128 rshift_msvc_x64(uint128 shift_me, int shift_amount) noexcept;
        static void best_safe_div_mod(uint128 dividend, uint128 divisor, uint128 * quotient, uint128 * remainder);
        static constexpr void constexpr_div_mod_impl(uint128 dividend, uint128 divisor,
            uint128 * quotient_ret, uint128 * remainder_ret);
        static constexpr void unsafe_constexpr_div_mod_impl(uint128 dividend, uint128 divisor,
            uint128 * quotient_ret, uint128 * remainder_ret) noexcept;
        template<typename T>
        static constexpr void step(T& n, int& pos, int shift) noexcept;
        static constexpr int fls(uint128 n) noexcept;
        static void div_mod_msc_x64_impl(uint128 dividend, uint128 divisor,
            uint128 * quotient_ret, uint128 * remainder_ret) noexcept;
        template<typename Char, typename CharTraits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
        requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
        static std::basic_string<Char, CharTraits, Allocator> to_string(uint128 item, std::ios_base::fmtflags flags);


#if CJM_NUMERICS_LITTLE_ENDIAN
        int_part m_low;
        int_part m_high;
#else //BIG ENDIAN
        int_part m_high;
        int_part m_low;
#endif
    };

   

    static_assert(concepts::integer<uint128>, "Needs to be an integer.");
}
namespace std
{ //fixme todo --- get rid of the traits overloads -- undefined behavior
	template<>
	struct make_unsigned<cjm::numerics::uint128>
	{
		using type = cjm::numerics::uint128;
	};
	/************************************************************************/
	/* Defines the default hash-code generator struct
	* so uint128's can be used in unordered_maps, unordered_sets,
	* or any other use for which std::hash is used by default       */
	/************************************************************************/
	template<>
	struct hash<cjm::numerics::uint128>
	{
		constexpr hash() noexcept = default;
		constexpr size_t operator()(const cjm::numerics::uint128& keyVal) const noexcept 
		{
			return keyVal.hash_code();
		}
	};





}


namespace cjm
{
	namespace numerics
	{
		namespace literals
		{
			constexpr size_t operator"" _szt(unsigned long long val)
			{
                return static_cast<size_t>(val);
			}
		}
		namespace fixed_uint_literals
		{
			class fixed_uint_lit_helper;
		}
		namespace uint128_literals
		{
			/************************************************************************/
			/* This literal operator accepts numeric literals in decimal or hexadecimal
			 * format only.  Octal literals are not supported.  Binary literals 
			 * are not supported now but support might be added.
			 * 
			 * This literal operator supports character separators i.e. " ' "
			 * 
			 * Most of the common types of illegal literals should cause a compilation 
			 * failure.  A few of them, however, if not assigned in a constexpr context,
			 * will generate a domain error exception at run time.  
			 * 
			 * Calling this literal operator directly or using it other than to parse
			 * literals in source code is NOT RECOMMENDED AND NOT SUPPORTED.  
			 * DO NOT USE THIS LITERAL OPERATOR TO PARSE STRINGS
			 *                  */
			/************************************************************************/
			template<char... Chars>
			constexpr uint128 operator"" _u128();

			enum class lit_type
			{
				Illegal = 0,
				Decimal = 1,
				Hexadecimal = 2,
				Zero = 3,
			};

			/************************************************************************/
			/* NOT FOR EXTERNAL USE
			 * The functions provided
			 * for parsing a series of chars
			 * herein are ONLY useful in a context 
			 * of parsing a literal at compile time.
			 * The are not safe for or intended for general use.
			 * This class exists to facilitate the literal operator:
			 * constexpr uint128 operator "" _u128(const char* chars);				*/
			/************************************************************************/
			class uint128_lit_helper
			{
				friend class cjm::numerics::fixed_uint_literals::fixed_uint_lit_helper; 

				template<char... Chars>
				friend constexpr uint128 operator"" _u128();
				
				template<size_t Size>
				static constexpr lit_type get_lit_type(std::array<char, Size> arr);

				template<size_t Size>
				static constexpr bool are_all_chars_0(std::array<char, Size> arr);

				static constexpr uint8_t get_hex_value(char c);

				template<size_t Size>
				static constexpr std::pair<bool, size_t> scan_chars_dec(std::array<char, Size> arr);

				template<size_t Size>
				static constexpr std::pair<size_t, size_t> get_dec_val(std::array<char, Size> arr, size_t index);

				static constexpr char to_lower(char c) noexcept;

				template<size_t Size>
				static constexpr uint128 get_hex_literal(std::array<char, Size> arr);

				template<size_t Size>
				static constexpr uint128 get_decimal_literal(std::array<char, Size> arr);

				static constexpr bool is_legal_hex_char(char c) noexcept;

				template<size_t Size>
				static constexpr std::pair<uint8_t, size_t> get_byte(std::array<char, Size> arr, size_t index);

			public:

				template<size_t Size>
				static constexpr uint128 parse_from_char_array(std::array<char, Size> arr);

				template<char... Chars>
				static constexpr std::array<char, sizeof...(Chars)> get_array();

				uint128_lit_helper() = delete;
				~uint128_lit_helper() = delete;
				uint128_lit_helper(const uint128_lit_helper& other) = delete;
				uint128_lit_helper(uint128_lit_helper&& other) noexcept = delete;
				uint128_lit_helper& operator=(const uint128_lit_helper& other) = delete;
				uint128_lit_helper& operator=(uint128_lit_helper&& other) noexcept = delete;
			};
		}

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
}

#include "uint128.inl"
#endif