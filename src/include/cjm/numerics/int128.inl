// Copyright © 2021 CJM Screws, LLC
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

#ifndef CJM_INT128_INL_
#define CJM_INT128_INL_
#include "int128.hpp"

namespace cjm::numerics
{
	namespace uint128_literals
	{
		
		

		CJM_LIT_CONST std::array<char, std::numeric_limits<uint128>::digits10 + 1> max_pos_signed_dec_128_v = std::array<char, std::numeric_limits<uint128>::digits10 + 1>
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
		
		template<char... Chars>
		CJM_LIT_CONST bool lit_helper::validate_decimal_size_as_signed_uint128()
		{
			if (get_lit_type<Chars...>() != lit_type::Decimal)
				return false;
			constexpr std::optional<size_t> dec_digits = count_decimal_chars<Chars...>();
			constexpr size_t max_dec_digits = std::numeric_limits<uint128>::digits10 + 1;
			if (!dec_digits.has_value() || *dec_digits > max_dec_digits)
				return false;
			if (*dec_digits < max_dec_digits)
				return true;
			//the hard part, need if it has max digits
			constexpr auto arr = std::array<char, sizeof...(Chars)>{ Chars... };
			size_t max_idx = 0;
			for (char c : arr)
			{
				if (c == '\'') continue;
				char comparand = max_pos_signed_dec_128_v<Ui>[max_idx++];
				if (c < comparand)
					return true;
				if (c > comparand)
					return false;
			}
			return true;
			
		}
	}
	
	namespace int128_literals
	{
		template<char... Chars>
			requires (sizeof...(Chars) > 0)
		constexpr int128 operator""_i128()
		{
			using namespace uint128_literals;
			constexpr uint128 min_signed = uint128::make_uint128(0x8000'0000'0000'0000, 0x0000'0000'0000'0000);
			constexpr uint128 max_signed = min_signed - 1ull;
			constexpr std::optional<uint128> result = uint128_literals::lit_helper::parse_literal<uint128, Chars...>();
			static_assert(result.has_value(), "The literal is not a valid 128-bit decimal or hexadecimal integer.");
			static_assert(*result == min_signed || *result <= max_signed, 
				"The value is too large or to small to fit in a signed two's complement 128 bit integer.");
			return static_cast<int128>(*result);
		}
		
		
	}

	//todo insert stream insertion and extraction operator definitions here

	//todo insert to and from string function definitions here

	//todo insert to and from byte array function definitions here

	//todo insert try_div_mod, unsafe_div_mod func defs here

	constexpr int128::int128() noexcept : m_unsigned_rep{} {}

	//todo insert div_mod func def here

	constexpr int signum(int128 v) noexcept
	{
		using namespace cjm::numerics::uint128_literals;
		if (v == 0)
			return 0;
		if ((v.m_unsigned_rep & 0x8000'0000'0000'0000'000'0000'0000'0000_u128)
			== 0x8000'0000'0000'0000'000'0000'0000'0000_u128)
			return -1;
		return 1;
	}
	constexpr bool is_negative(int128 v) noexcept
	{
		return signum(v) < 0;
	}
	constexpr bool operator==(int128 lhs, int128 rhs) noexcept
	{
		return static_cast<uint128>(lhs) == static_cast<uint128>(rhs);
	}

	constexpr int128::int128(int v) noexcept		
	{
		m_unsigned_rep = v < 0 ? 
			uint128::make_uint128(std::numeric_limits<typename uint128::int_part>::max(), 
				static_cast<typename uint128::int_part>(static_cast<int64_t>(v))) :
			uint128::make_uint128(std::numeric_limits<typename uint128::int_part>::min(), 
				static_cast<typename uint128::int_part>(static_cast<int64_t>(v)));		
	}

	constexpr int128::int128(unsigned int v) noexcept : m_unsigned_rep{v} {}
	constexpr int128::int128(long v) noexcept
	{
		m_unsigned_rep = v < 0 ?
			uint128::make_uint128(std::numeric_limits<typename uint128::int_part>::max(),
				static_cast<typename uint128::int_part>(static_cast<int64_t>(v))) :
			uint128::make_uint128(std::numeric_limits<typename uint128::int_part>::min(),
				static_cast<typename uint128::int_part>(static_cast<int64_t>(v)));
	}
	
	constexpr int128::int128(unsigned long v) noexcept : m_unsigned_rep{v}	{}

	constexpr int128::int128(long long v) noexcept
	{
		m_unsigned_rep = v < 0 ?
			uint128::make_uint128(std::numeric_limits<typename uint128::int_part>::max(),
				static_cast<typename uint128::int_part>(static_cast<int64_t>(v))) :
			uint128::make_uint128(std::numeric_limits<typename uint128::int_part>::min(),
				static_cast<typename uint128::int_part>(static_cast<int64_t>(v)));
	}

	constexpr int128::int128(unsigned long long v) noexcept : m_unsigned_rep{v}{}

	constexpr int128::int128(uint128 v) noexcept : m_unsigned_rep{v} {}
	
	//converting ctors from floating point types
	inline int128::int128(float f) noexcept
	{
		m_unsigned_rep = static_cast<uint128>(internal::make_signed_from_floating_point<float>(f));
	}

	inline int128::int128(double d) noexcept 
	{
		m_unsigned_rep = static_cast<uint128>(internal::make_signed_from_floating_point<double>(d));
	}

	inline int128::int128(long double d) noexcept 
	{
		m_unsigned_rep = static_cast<uint128>(internal::make_signed_from_floating_point<long double>(d));
	}
	constexpr int128::operator bool() const noexcept
	{
		return m_unsigned_rep.m_limbs.m_low || m_unsigned_rep.m_limbs.m_high;
	}
	constexpr int128::operator char() const noexcept
	{
		if constexpr (std::numeric_limits<char>::is_signed)
		{
			const auto temp = static_cast<int>(m_unsigned_rep.m_limbs.m_low);
			return static_cast<char>(temp);
		}
		else
		{
			return static_cast<char>(m_unsigned_rep.m_limbs.m_low);
		}
	}
	constexpr int128::operator signed char() const noexcept
	{
		const auto temp = static_cast<int>(m_unsigned_rep.m_limbs.m_low);
		return static_cast<signed char>(temp);
	}
	constexpr int128::operator unsigned char() const noexcept
	{
		return static_cast<unsigned char>(m_unsigned_rep.m_limbs.m_low);
	}

	constexpr int128::operator char8_t() const noexcept
	{
		if constexpr (std::numeric_limits<char8_t>::is_signed)
		{
			const auto temp = static_cast<int>(m_unsigned_rep.m_limbs.m_low);
			return static_cast<char8_t>(temp);
		}
		else
		{
			return static_cast<char8_t>(m_unsigned_rep.m_limbs.m_low);
		}
	}

	constexpr int128::operator char16_t() const noexcept
	{
		if constexpr (std::numeric_limits<char16_t>::is_signed)
		{
			const auto temp = static_cast<int>(m_unsigned_rep.m_limbs.m_low);
			return static_cast<char16_t>(temp);
		}
		else
		{
			return static_cast<char16_t>(m_unsigned_rep.m_limbs.m_low);
		}
	}
	constexpr int128::operator char32_t() const noexcept
	{
		if constexpr (std::numeric_limits<char32_t>::is_signed)
		{
			const auto temp = static_cast<std::int32_t>(m_unsigned_rep.m_limbs.m_low);
			return static_cast<char32_t>(temp);
		}
		else
		{
			return static_cast<char32_t>(m_unsigned_rep.m_limbs.m_low);
		}
	}
	constexpr int128::operator wchar_t() const noexcept
	{
		if constexpr (std::numeric_limits<wchar_t>::is_signed)
		{
			const auto temp = static_cast<std::int64_t>(m_unsigned_rep.m_limbs.m_low);
			return static_cast<wchar_t>(temp);
		}
		else
		{
			return static_cast<wchar_t>(m_unsigned_rep.m_limbs.m_low);
		}
	}
	constexpr int128::operator std::int16_t() const noexcept
	{
		return static_cast<std::int16_t>(m_unsigned_rep.m_limbs.m_low);
	}
	constexpr int128::operator std::uint16_t() const noexcept
	{
		return static_cast<std::uint16_t>(m_unsigned_rep.m_limbs.m_low);
	}
	constexpr int128::operator std::int32_t() const noexcept
	{
		return static_cast<std::int32_t>(m_unsigned_rep.m_limbs.m_low);
	}
	constexpr int128::operator std::uint32_t() const noexcept
	{
		return static_cast<std::uint32_t>(m_unsigned_rep.m_limbs.m_low);
	}
	constexpr int128::operator std::int64_t() const noexcept
	{
		return static_cast<std::int64_t>(m_unsigned_rep.m_limbs.m_low);
	}
	constexpr int128::operator std::uint64_t() const noexcept
	{
		return static_cast<std::uint64_t>(m_unsigned_rep.m_limbs.m_low);
	}
}
namespace cjm::numerics::internal
{
	template<numerics::concepts::builtin_floating_point TFloat>
	int128 make_signed_from_floating_point(TFloat v) noexcept
	{
		bool sign = v < 0;
		if (sign) v = -v;
		uint128 temp = make_from_floating_point<TFloat>(v);
		if (sign)
		{
			temp = -temp;
		}
		return static_cast<int128>(temp);		
	}
}

constexpr cjm::numerics::int128 std::numeric_limits<cjm::numerics::int128>::min() noexcept
{
	using namespace cjm::numerics;
	return static_cast<int128>(uint128::make_uint128(0x8000'0000'0000'0000, 0x0000'0000'0000'0000));
}

constexpr cjm::numerics::int128 std::numeric_limits<cjm::numerics::int128>::lowest() noexcept
{
	return min();
}

constexpr cjm::numerics::int128 std::numeric_limits<cjm::numerics::int128>::max() noexcept
{
	using namespace cjm::numerics;
	return static_cast<int128>(uint128::make_uint128(0x8000'0000'0000'0000, 0x0000'0000'0000'0000) - 1ull);
}

constexpr cjm::numerics::int128 std::numeric_limits<cjm::numerics::int128>::epsilon() noexcept
{
	return std::numeric_limits<int64_t>::epsilon();
}

constexpr cjm::numerics::int128 std::numeric_limits<cjm::numerics::int128>::round_error() noexcept
{
	return std::numeric_limits<int64_t>::round_error();
}

constexpr cjm::numerics::int128 std::numeric_limits<cjm::numerics::int128>::infinity() noexcept
{
	return std::numeric_limits<int64_t>::infinity();
}

constexpr cjm::numerics::int128 std::numeric_limits<cjm::numerics::int128>::quiet_NaN() noexcept
{
	return std::numeric_limits<int64_t>::quiet_NaN();
}

constexpr cjm::numerics::int128 std::numeric_limits<cjm::numerics::int128>::signaling_NaN() noexcept
{
	return std::numeric_limits<int64_t>::signaling_NaN();
}

constexpr cjm::numerics::int128 std::numeric_limits<cjm::numerics::int128>::denorm_min() noexcept
{
	return std::numeric_limits<int64_t>::denorm_min();
}
#endif