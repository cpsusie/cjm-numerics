// Copyright © 2020-2021 CJM Screws, LLC
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
#ifndef CJM_NUMERICS_INL_
#define CJM_NUMERICS_INL_
#include <cjm/numerics/numerics.hpp>
namespace cjm::numerics
{
		
	template<typename Char, typename CharTraits>
	requires (cjm::numerics::concepts::char_or_wchar_t_with_traits<Char, CharTraits>)
		std::basic_ostream<Char, CharTraits>& operator<<(std::basic_ostream<Char, CharTraits>& os, compiler_used comp)
	{
		using char_t = std::remove_const_t<Char>;
		if constexpr (std::is_same_v<char_t, char>)
		{
			os << get_text_narrow(comp);
		}
		else
		{
			os << get_text_wide(comp);
		}
		return os;
	}
}

namespace cjm::numerics::math_functions
{
	template<cjm::numerics::concepts::integer TInt>
	constexpr std::make_unsigned_t<TInt> int_abs(TInt val) noexcept
	{
		static_assert(std::is_integral_v<TInt> && !std::is_same_v<bool, TInt>, 
			"Function is only valid for non-boolean integers.");
		if constexpr (std::is_unsigned_v<TInt>)
		{	//unsigned
			return val;
		}
		else
		{	//signed
			 return (val == std::numeric_limits<TInt>::min()) ? 
				static_cast<std::make_unsigned_t<TInt>>(val) :
					static_cast<std::make_unsigned_t<TInt>>((val < 0 ? -val : val));
		}
	}

	template<concepts::integer TInt>
	constexpr TInt int_sign(TInt val) noexcept
	{
		static_assert(std::is_integral_v<TInt> && !std::is_same_v<bool, TInt>, "Function is only valid for non-boolean integers.");
		if constexpr (!std::numeric_limits<TInt>::is_signed)
			return TInt{ 1 };
		else
			return val < 0 ? TInt{ -1 } : TInt{ 1 };
	}
}

#ifdef __cpp_lib_bit_cast
template<typename To, typename From>
	requires cjm::numerics::concepts::bit_castable<To, From>
CJM_BIT_CAST_CONST To cjm::numerics::bit_cast(const From& f) noexcept
{
	return std::bit_cast<To, From>(f);
}
#else
template<typename To, typename From>
	requires cjm::numerics::concepts::bit_castable<To, From>
CJM_BIT_CAST_CONST To cjm::numerics::bit_cast(const From& f) noexcept
{
	//GCC seems to get all butt-hurt about private member variables even if type is trivial.
	//All c++ standard requires is trivially copyable and same size (and non-overlapping).  the concepts here
	//enforce MORE than that requirement (also require nothrow default constructible)
	//Suppressing because uint128 is a type that is trivially copyable-to.
	auto dst = To{};
#if  (defined(__GNUC__) && !defined(__clang__))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
	std::memcpy(&dst, &f, sizeof(To));  /* no diagnostic for this one */
#pragma GCC diagnostic pop
#else
	std::memcpy(&dst, &f, sizeof(To));
#endif
	return dst;
}
#endif


#endif //CJM_NUMERICS_INL_ 
