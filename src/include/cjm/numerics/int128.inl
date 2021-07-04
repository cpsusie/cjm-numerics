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
	namespace int128_literals
	{
		//todo insert literal operator definition here
	}

	//todo insert stream insertion and extraction operator definitions here

	//todo insert to and from string function definitions here

	//todo insert to and from byte array function definitions here

	//todo insert try_div_mod, unsafe_div_mod func defs here

	constexpr int128::int128() noexcept : m_unsigned_rep{} {}

	//todo insert div_mod func def here

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
#endif