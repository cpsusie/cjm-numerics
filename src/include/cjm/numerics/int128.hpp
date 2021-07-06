// Copyright � 2021 CJM Screws, LLC
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// CJM Screws, LLC is a Maryland Limited Liability Company.
// No copyright claimed to unmodified original work of others.
// The original, unmodified work of others, to the extent included in this library,
// is licensed to you under the same terms under which it was licensed to CJM Screws, LLC.
// For information about copyright and licensing of the original work of others,
// see Notices file in cjm/ folder.
//
// This library is modeled on the abseil C++ numerics library.  MOST OF ABSEIL'S CODE HAS BEEN MODIFIED SIGNIFICANTLY BY THIS LIBRARY.
// Some verbatim reproduction may remain in places.  The following notice was found in abseil's uint128 source code and is reproduced here in
// accordance with the terms of the Apache 2.0 license, under which Abseil's code was licensed to CJM Screws, LLC:

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


#ifndef CJM_INT128_HPP_
#define CJM_INT128_HPP_
#include <compare>
#include <cjm/numerics/uint128.hpp>
#include <cjm/numerics/cjm_numeric_concepts.hpp>
namespace cjm::numerics
{
	class int128;

	namespace internal
	{
		constexpr int countl_zero(int128 n) noexcept;

		template<numerics::concepts::builtin_floating_point TFloat>
		int128 make_signed_from_floating_point(TFloat v) noexcept;

	}


	/// <summary>
	/// Stream insertion operator for int128
	/// </summary>
	/// <typeparam name="Char">The Character type</typeparam>
	/// <typeparam name="CharTraits">traits for character</typeparam>
	/// <typeparam name="Allocator">allocator for character</typeparam>
	/// <param name="os">the stream to insert</param>
	/// <param name="v">the value to stringify and insert into the stream</param>
	/// <returns>the stream with the value inserted.</returns>
	template <typename Char = char, typename CharTraits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
		requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
	std::basic_ostream<Char, CharTraits>& operator<<(std::basic_ostream<Char, CharTraits>& os, int128 v);

	/// <summary>
	/// Stream extraction operator for uint128
	/// </summary>
	/// <typeparam name="Char">The Character type</typeparam>
	/// <typeparam name="CharTraits">traits for character</typeparam>
	/// <typeparam name="Allocator">allocator for character</typeparam>
	/// <param name="is">the stream to insert</param>
	/// <param name="v">the value to stringify and insert into the stream</param>
	/// <returns>the stream with the value inserted.</returns>
	template <typename Char = char, typename CharTraits = std::char_traits<Char>>
		requires cjm::numerics::concepts::char_with_traits<Char, CharTraits>
	std::basic_istream<Char, CharTraits>& operator>>(std::basic_istream<Char, CharTraits>& is, int128& v);

	//will return a positive value if v is positive, a negative value if v is negative and zero value if v is 0
	constexpr int signum(int128 v) noexcept;
	constexpr bool is_negative(int128 v) noexcept;
	//Comparison operators
	constexpr std::strong_ordering operator <=>(int128 lhs, int128 rhs) noexcept;
	constexpr bool operator==(int128 lhs, int128 rhs) noexcept;
	constexpr bool operator!=(int128 lhs, int128 rhs) noexcept;
	constexpr bool operator>(int128 lhs, int128 rhs) noexcept;
	constexpr bool operator<(int128 lhs, int128 rhs) noexcept;
	constexpr bool operator>=(int128 lhs, int128 rhs) noexcept;
	constexpr bool operator<=(int128 lhs, int128 rhs) noexcept;

	//Unary operators
	constexpr int128 operator-(int128 operand) noexcept;
	constexpr int128 operator+(int128 operand) noexcept;
	constexpr int128 operator~(int128 operand) noexcept;
	constexpr bool operator!(int128 operand) noexcept;
	//Logical operators
	constexpr int128 operator&(int128 lhs, int128 rhs) noexcept;
	constexpr int128 operator|(int128 lhs, int128 rhs) noexcept;
	constexpr int128 operator^(int128 lhs, int128 rhs) noexcept;
	//bit shift operators
	constexpr int128 operator>>(int128 lhs, int amount) noexcept;
	constexpr int128 operator<<(int128 lhs, int amount) noexcept;
	constexpr int128 operator>>(int128 lhs, int128 amount) noexcept;
	constexpr int128 operator<<(int128 lhs, int128 amount) noexcept;
	//arithmetic operators
	constexpr int128 operator+(int128 lhs, int128 rhs) noexcept;
	constexpr int128 operator-(int128 lhs, int128 rhs) noexcept;
	constexpr int128 operator*(int128 lhs, int128 rhs) noexcept;

	struct i128_str_format;
}

namespace std
{

	template<>
	struct hash<cjm::numerics::i128_str_format> final
	{
		constexpr hash() noexcept = default;
		constexpr size_t operator()(cjm::numerics::i128_str_format key) const noexcept;
	};
	
	/************************************************************************/
	/* Defines the default hash-code generator struct
	* so uint128's can be used in unordered_maps, unordered_sets,
	* or any other use for which std::hash is used by default	   */
	/************************************************************************/
	template<>
	struct hash<cjm::numerics::int128> final
	{
		constexpr hash() noexcept = default;
		constexpr size_t operator()(const cjm::numerics::int128 & keyVal) const noexcept;
	};
}

namespace cjm::numerics
{
	struct i128_str_format final
	{
	public:
		[[nodiscard]] constexpr bool is_negative() const noexcept { return m_neg_sign; }
		[[nodiscard]] constexpr u128_str_format format() const noexcept { return m_format; }

		constexpr i128_str_format() noexcept : m_neg_sign{}, m_format{}{}
		constexpr i128_str_format(u128_str_format format, bool is_neg) noexcept : m_neg_sign{ is_neg }, m_format{ format } {}
		constexpr i128_str_format(const i128_str_format& other) noexcept = default;
		constexpr i128_str_format(i128_str_format&& other) noexcept = default;
		constexpr i128_str_format& operator=(const i128_str_format& other) noexcept = default;
		constexpr i128_str_format& operator=(i128_str_format&& other) noexcept = default;
		constexpr ~i128_str_format() = default;


		friend constexpr std::strong_ordering  operator<=>(i128_str_format lhs, i128_str_format rhs) noexcept
		{
			using enum_underly_t = std::underlying_type_t<u128_str_format>;
			auto ret = std::strong_ordering::equal;
			const std::strong_ordering neg_order = lhs.m_neg_sign <=> rhs.m_neg_sign;
			if (neg_order == std::strong_ordering::equal)
			{
				ret = static_cast<enum_underly_t>(lhs.m_format) <=> static_cast<enum_underly_t>(rhs.m_format);
			}
			else
			{
				ret = neg_order;
			}
			return ret;
		}

		friend constexpr bool operator==(i128_str_format lhs, i128_str_format rhs) noexcept = default;
		friend constexpr bool operator!=(i128_str_format lhs, i128_str_format rhs) noexcept = default;
		friend constexpr bool operator<(i128_str_format lhs, i128_str_format rhs) noexcept = default;
		friend constexpr bool operator>(i128_str_format lhs, i128_str_format rhs) noexcept = default;
		friend constexpr bool operator<=(i128_str_format lhs, i128_str_format rhs) noexcept = default;
		friend constexpr bool operator>=(i128_str_format lhs, i128_str_format rhs) noexcept = default;

	private:
		bool m_neg_sign;
		u128_str_format m_format;
	};

	
	class alignas(uint128) int128 final
	{
	public:
		static constexpr size_t byte_array_size = uint128::byte_array_size;
		using byte_array = typename uint128::byte_array;
		using divmod_result_t = divmod_result<int128>;
		using int_part = typename std::make_signed_t<typename uint128::int_part>;
		/// <summary>
		/// Parses a string_view into an int128
		/// </summary>
		/// <typeparam name="Chars">Character type</typeparam>
		/// <typeparam name="CharTraits">Character traits type</typeparam>
		/// <param name="parse_me">the string_view to parse.</param>
		/// <returns>An int128 parsed from parse_me.</returns>
		/// <exception cref="std::invalid_argument">An int128 could not be parsed from
		/// the supplied value.</exception>
		template<typename Chars, typename CharTraits = std::char_traits<Chars>>
			requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
		static int128 make_from_string(std::basic_string_view<Chars, CharTraits>
				parse_me);

		/// <summary>
		/// Parses a string into an int128
		/// </summary>
		/// <typeparam name="Chars">Character type</typeparam>
		/// <typeparam name="CharTraits">Character traits type</typeparam>
		/// <param name="parse_me">the string to parse.</param>
		/// <returns>An int128 parsed from parse_me.</returns>
		/// <exception cref="std::invalid_argument">An int128 could not be parsed from
		/// the supplied value.</exception>
		template<typename Chars, typename CharTraits = std::char_traits<Chars>,
			typename Allocator = std::allocator<Chars>>
				requires cjm::numerics::concepts::char_with_traits_and_allocator<Chars,
					CharTraits, Allocator>
		static int128 make_from_string(const std::basic_string<Chars, CharTraits,
			Allocator>& parse_me);

		static constexpr int128 make_from_bytes_little_endian(byte_array bytes) noexcept;
		static constexpr int128 make_from_bytes_big_endian(byte_array bytes) noexcept;
		static constexpr std::optional<divmod_result_t> try_div_mod(int128 dividend,
			int128 divisor) noexcept;
		static constexpr divmod_result_t div_mod(int128 dividend, int128 divisor);
		static constexpr divmod_result_t unsafe_div_mod(int128 dividend,
			int128 divisor) noexcept; //NOLINT (bugprone-exception-escape)
		static inline void instrumented_div_mod(std::basic_ostream<char>& stream, int128 dividend,
			int128 divisor, int128* quotient_ret, uint128* remainder_ret);
		
		friend constexpr int signum(int128 v) noexcept;
		constexpr int128() noexcept;
		constexpr int128(const int128& other) noexcept = default;
		constexpr int128(int128 && other) noexcept = default;
		constexpr int128& operator=(const int128 & other) noexcept = default;
		constexpr int128& operator=(int128 && other) noexcept = default;
		constexpr ~int128() noexcept = default;
		// Constructors from arithmetic types
		constexpr int128(int v) noexcept;
		constexpr int128(unsigned int v) noexcept;
		constexpr int128(long v) noexcept;
		constexpr int128(unsigned long v) noexcept;
		constexpr int128(long long v) noexcept;
		constexpr int128(unsigned long long v) noexcept;
		constexpr explicit int128(uint128 v) noexcept;
		// Assignment operators from arithmetic types
		constexpr int128& operator=(int v) noexcept;
		constexpr int128& operator=(unsigned int v) noexcept;
		constexpr int128& operator=(long v) noexcept;
		constexpr int128& operator=(unsigned long v) noexcept;
		constexpr int128& operator=(long long v) noexcept;
		constexpr int128& operator=(unsigned long long v) noexcept;
		constexpr int128& operator=(uint128 v) noexcept;
		//converting ctors from floating point types
		inline explicit int128(float f) noexcept;
		inline explicit int128(double d) noexcept;
		inline explicit int128(long double d) noexcept;

		// Conversion operators to other arithmetic types
		constexpr explicit operator bool() const noexcept;
		constexpr explicit operator char() const noexcept;
		constexpr explicit operator signed char() const noexcept;
		constexpr explicit operator unsigned char() const noexcept;
		constexpr explicit operator char8_t() const noexcept;
		constexpr explicit operator char16_t() const noexcept;
		constexpr explicit operator char32_t() const noexcept;
		constexpr explicit operator wchar_t() const noexcept;
		constexpr explicit operator std::int16_t() const noexcept;
		constexpr explicit operator std::uint16_t() const noexcept;
		constexpr explicit operator std::int32_t() const noexcept;
		constexpr explicit operator std::uint32_t() const noexcept;
		constexpr explicit operator std::int64_t() const noexcept;
		constexpr explicit operator std::uint64_t() const noexcept;
		constexpr explicit operator uint128() const noexcept;
		inline explicit operator float() const;
		inline explicit operator double() const;
		inline explicit operator long double() const;

		//hash code function and comparison operators
		[[nodiscard]] constexpr size_t hash_code() const noexcept;

		friend constexpr int128 operator-(int128 operand) noexcept;
		friend constexpr int128 operator+(int128 operand) noexcept;
		friend constexpr int128 operator~(int128 operand) noexcept;
		friend constexpr bool operator!(int128 operand) noexcept;

		// Arithmetic operators.
		friend constexpr int128 operator+(int128 lhs, int128 rhs) noexcept;
		friend constexpr int128 operator-(int128 lhs, int128 rhs) noexcept;
		constexpr int128& operator+=(int128 other) noexcept;
		constexpr int128& operator-=(int128 other) noexcept;
		constexpr int128& operator*=(int128 other) noexcept;
		// Long division/modulo for int128.
		constexpr int128& operator/=(int128 other);
		constexpr int128& operator%=(int128 other);
		constexpr int128 operator++(int) noexcept;
		constexpr int128 operator--(int) noexcept;
		constexpr int128& operator<<=(int amount) noexcept;
		constexpr int128& operator>>=(int amount) noexcept;
		constexpr int128& operator<<=(unsigned amount) noexcept;
		constexpr int128& operator>>=(unsigned amount) noexcept;
		constexpr int128& operator<<=(int128 amount) noexcept;
		constexpr int128& operator>>=(int128 amount) noexcept;
		constexpr int128& operator&=(int128 other) noexcept;
		constexpr int128& operator|=(int128 other) noexcept;
		constexpr int128& operator^=(int128 other) noexcept;
		constexpr int128& operator++() noexcept;
		constexpr int128& operator--() noexcept;
		friend constexpr int128 operator/(int128 lhs, int128 rhs);
		friend constexpr int128 operator%(int128 lhs, int128 rhs);
		friend constexpr int128 operator>>(int128 lhs, int amount) noexcept;
		friend constexpr int128 operator<<(int128 lhs, int amount) noexcept;
		friend constexpr int128 operator>>(int128 lhs, int128 amount) noexcept;
		friend constexpr int128 operator<<(int128 lhs, int128 amount) noexcept;
		//Logical operators
		friend constexpr int128 operator&(int128 lhs, int128 rhs) noexcept;
		friend constexpr int128 operator|(int128 lhs, int128 rhs) noexcept;
		friend constexpr int128 operator^(int128 lhs, int128 rhs) noexcept;
		//bit shift operators
		friend constexpr int128 operator>>(int128 lhs, int amount) noexcept;
		friend constexpr int128 operator<<(int128 lhs, int amount) noexcept;
		friend constexpr int128 operator>>(int128 lhs, int128 amount) noexcept;
		friend constexpr int128 operator<<(int128 lhs, int128 amount) noexcept;
		//arithmetic operators
		friend constexpr int128 operator+(int128 lhs, int128 rhs) noexcept;
		friend constexpr int128 operator-(int128 lhs, int128 rhs) noexcept;
		friend constexpr int128 operator*(int128 lhs, int128 rhs) noexcept;
		template<typename Char, typename CharTraits, typename Allocator>
			requires cjm::numerics::concepts::char_with_traits_and_allocator<Char,
				CharTraits, Allocator>
		friend std::basic_ostream<Char, CharTraits>& operator<< (std::basic_ostream<Char,
			CharTraits>& os, int128 v);

		//Accessor for sub-components
		[[nodiscard]] constexpr int_part low_part() const noexcept { return static_cast<int_part>(m_unsigned_rep.low_part()); }
		[[nodiscard]] constexpr int_part high_part() const noexcept  { return static_cast<int_part>(m_unsigned_rep.high_part()); } 

		[[nodiscard]] constexpr byte_array to_little_endian_arr() const noexcept; //NOLINT (bugprone-exception-escape)
		[[nodiscard]] constexpr byte_array to_big_endian_arr() const noexcept; //NOLINT (bugprone-exception-escape)

	private:
		uint128 m_unsigned_rep;
	};
}

namespace cjm::numerics::int128_literals
{
	using lit_type = uint128_literals::lit_type;
	using uint128_literals::operator ""_u128;
	using ulit_helper = uint128_literals::lit_helper;

	constexpr std::array<char, std::numeric_limits<uint128>::digits10 + 1> max_pos_signed_dec_128_v = std::array<char, std::numeric_limits<uint128>::digits10 + 1>
	{
			'1', '7', '0',
			'1', '4', '1',
			'1', '8', '3',
			'4', '6', '0',
			'4', '6', '9',
			'2', '3', '1',
			'7', '3', '1',
			'6', '8', '7',
			'3', '0', '3',
			'7', '1', '5',
			'8', '8', '4',
			'1', '0', '5',
			'7', '2', '7'
	};

	
	class lit_helper final
	{
		template<char... Chars>
			requires (sizeof...(Chars) > 0)
		friend constexpr int128 int128_literals::operator"" _i128();

		template<char... Chars>
			requires (sizeof...(Chars) > 0)
		static CJM_LIT_CONST std::optional<uint128> parse_literal_as_unsigned();

		template<char... Chars>
		static CJM_LIT_CONST bool validate_decimal_size_as_signed_uint128();
	public:
		
		template<char... Chars>
			requires(sizeof...(Chars) > 0)
		static CJM_LIT_CONST std::optional<int128> parse_literal_i128();

		lit_helper() = delete;
		~lit_helper() = delete;
		lit_helper(const lit_helper& other) = delete;
		lit_helper(lit_helper&& other) noexcept = delete;
		lit_helper& operator=(const lit_helper& other) = delete;
		lit_helper& operator=(lit_helper&& other) noexcept = delete;
		
	};

	
}

#include <cjm/numerics/int128.inl>
#endif