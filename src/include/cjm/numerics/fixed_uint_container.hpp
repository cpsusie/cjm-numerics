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
#ifndef CJM_FIXED_UINT_CONTAINER_HPP_
#define CJM_FIXED_UINT_CONTAINER_HPP_
#include <cjm/numerics/numerics.hpp>
#include <limits>
#include <type_traits>
#include <cstdint>
#include <bit>
#include <array>
namespace cjm::numerics::fixed_uint_container
{
	namespace internal
	{
		template<typename Ui>
		concept is_builtin_u128 =
#if (defined (CJM_DIV_ONLY_INTRINSIC_U128)) || (defined(CJM_USE_INTRINSIC_U128))
			(sizeof(Ui) == sizeof(std::uint64_t) * 2) &&
				std::is_same_v<unsigned __int128, std::remove_cvref_t<std::remove_const_t<Ui>>>;
#else
			false;
#endif

		template<typename Si>
		concept is_builtin_i128 =
#if (defined (CJM_DIV_ONLY_INTRINSIC_U128)) || (defined(CJM_USE_INTRINSIC_U128))
			(sizeof(Si) == sizeof(std::int64_t) * 2) &&
			std::is_same_v<__int128, std::remove_cvref_t<std::remove_const_t<Si>>>;
#else
			false;
#endif

		template<typename Ui>
		concept is_full_builtin_u128 = is_builtin_u128<Ui> && std::is_arithmetic_v<Ui> &&
			std::numeric_limits<Ui>::is_specialized;

		template<typename Si>
		concept is_full_builtin_i128 = is_builtin_i128<Si> && std::is_arithmetic_v<Si> &&
			std::numeric_limits<Si>::is_specialized;

		
		template<typename Ui>
		concept is_partial_builtin_u128 = is_builtin_u128<Ui> && !is_full_builtin_u128<Ui>;

		constexpr bool exits_builtin_u128 = is_builtin_u128<natuint128_t>;

		template<typename Si>
		concept is_partial_builtin_i128 = is_builtin_i128<Si> && !is_full_builtin_u128<Si>;
		
		template<typename Ui, size_t Digits>
		concept is_unsigned_int_with_digits = (cjm::numerics::concepts::builtin_unsigned_integer<Ui> && Digits >= 64u &&
			Digits == std::numeric_limits<Ui>::digits) || (is_builtin_u128<Ui> &&
				sizeof(Ui) == (sizeof(std::uint64_t) * 2) && Digits == 128);

		template<typename Si, size_t Digits>
		concept is_signed_int_with_digits = (cjm::numerics::concepts::builtin_signed_integer<Si> && Digits >= 64u &&
			Digits == std::numeric_limits<Si>::digits) || (is_builtin_u128<Si> &&
				sizeof(Si) == (sizeof(std::int64_t) * 2) && Digits == 128);

		constexpr size_t uint128_alignment = is_x64 ? alignof(std::uint64_t) * 2 : alignof(std::uint64_t);

		template<typename UnsignedInteger, bool LittleEndian>
			requires (is_unsigned_int_with_digits<UnsignedInteger, 64u> || is_builtin_u128<UnsignedInteger>)
		struct uint128_limb_container;

		
		static_assert(cjm::numerics::concepts::builtin_unsigned_integer<std::uint64_t>);
		static_assert(std::numeric_limits<std::uint64_t>::digits == 64u);
		static_assert(is_unsigned_int_with_digits<std::uint64_t, 64>);
		static_assert(!exits_builtin_u128 || is_unsigned_int_with_digits<natuint128_t, 128>);

		template<typename Ui128LimbContainer>
		concept ui128_limb_container_split = requires(Ui128LimbContainer x)
		{
			{x.m_high}	noexcept	->	concepts::nothrow_convertible<std::uint64_t>;
			{x.m_low}	noexcept	->	concepts::nothrow_convertible<std::uint64_t>;
		};

		template<typename Ui128LimbContainer>
		concept ui128_limb_container_builtin = !ui128_limb_container_split<Ui128LimbContainer>
			&& exits_builtin_u128 && ((sizeof(Ui128LimbContainer) * 2u) == (sizeof(std::uint64_t) * 2u))
			&& requires 
		(Ui128LimbContainer x)
		{
			{x.m_value}	noexcept	->	concepts::nothrow_convertible<natuint128_t>;
		};

		template<typename Ui128LimbContainer>
		concept ui128_limb_container = ui128_limb_container_split<Ui128LimbContainer> ||
			ui128_limb_container_builtin<Ui128LimbContainer>;
		
		template<ui128_limb_container_split UnsignedInteger, bool LittleEndian>
		constexpr std::uint64_t get_low(const uint128_limb_container<UnsignedInteger, 
			LittleEndian>& val) noexcept;

		template<ui128_limb_container_split UnsignedInteger, bool LittleEndian>
		constexpr std::uint64_t get_high(const uint128_limb_container<UnsignedInteger, 
			LittleEndian>& val) noexcept;

		template<ui128_limb_container_builtin UnsignedInteger, bool LittleEndian>
		constexpr std::uint64_t get_low(const uint128_limb_container<UnsignedInteger, 
			LittleEndian>& val) noexcept;

		template<ui128_limb_container_builtin UnsignedInteger, bool LittleEndian>
		constexpr std::uint64_t get_high(const uint128_limb_container<UnsignedInteger, 
			LittleEndian>& val) noexcept;
		
		template<>
		struct alignas(uint128_alignment) uint128_limb_container<std::uint64_t, true> final
		{
			std::uint64_t m_low{};
			std::uint64_t m_high{};
			constexpr uint128_limb_container() noexcept = default;
			constexpr uint128_limb_container(const uint128_limb_container & other) noexcept = default;
			constexpr uint128_limb_container(uint128_limb_container && other) noexcept = default;
			constexpr uint128_limb_container& operator=(const uint128_limb_container & other) noexcept = default;
			constexpr uint128_limb_container& operator=(uint128_limb_container && other) noexcept = default;
			constexpr uint128_limb_container(std::uint64_t val) noexcept : m_low{ val }, m_high{} {}
			constexpr uint128_limb_container(std::uint32_t val) noexcept : m_low{ val }, m_high{} {}
			constexpr uint128_limb_container(std::uint64_t high, std::uint64_t low) noexcept : m_low{ low }, m_high{ high } {}
			constexpr uint128_limb_container(std::int64_t v) noexcept :
				m_low{static_cast<uint64_t>(v)}, m_high{(v < 0 ? std::numeric_limits<std::uint64_t>::max() : 0u)}{}
			constexpr uint128_limb_container(std::array<std::uint64_t, 2u> native_byte_order_arr) noexcept
			{
				m_low = native_byte_order_arr[0];
				m_high = native_byte_order_arr[1];
			}
			constexpr uint128_limb_container& operator=(std::uint64_t val) noexcept
			{
				m_high = 0;
				m_low = val;
				return *this;
			}
			constexpr uint128_limb_container& operator=(std::uint32_t val) noexcept
			{
				m_high = 0;
				m_low = val;
				return *this;
			}
			constexpr explicit operator std::array<std::uint64_t, 2u>() const noexcept
			{
				return std::array<std::uint64_t, 2u>{m_low, m_high};
			}
			constexpr explicit operator std::uint64_t() const noexcept
			{
				return m_low;
			}
			constexpr explicit operator std::uint32_t() const noexcept
			{
				return static_cast<std::uint32_t>(m_low);
			}
			constexpr ~uint128_limb_container() = default;
		};
		
		template<>
		struct alignas(uint128_alignment) uint128_limb_container<std::uint64_t, false> final
		{
			std::uint64_t m_high{};
			std::uint64_t m_low{};
			constexpr uint128_limb_container() noexcept = default;
			constexpr uint128_limb_container(const uint128_limb_container & other) noexcept = default;
			constexpr uint128_limb_container(uint128_limb_container && other) noexcept = default;
			constexpr uint128_limb_container& operator=(const uint128_limb_container & other) noexcept = default;
			constexpr uint128_limb_container& operator=(uint128_limb_container && other) noexcept = default;
			constexpr uint128_limb_container(std::uint64_t val) noexcept : m_high{}, m_low{ val } {}
			constexpr uint128_limb_container(std::uint32_t val) noexcept : m_high{}, m_low{ val } {}
			constexpr uint128_limb_container(std::uint64_t high, std::uint64_t low) noexcept
				: m_high{ high }, m_low{ low } {}
			constexpr uint128_limb_container(std::int64_t v) noexcept :
				m_high{ (v < 0 ? std::numeric_limits<std::uint64_t>::max() : 0u) },
					m_low{ static_cast<uint64_t>(v) } {}
			constexpr uint128_limb_container(std::array<std::uint64_t, 2u> native_byte_order_arr) noexcept
			{
				m_high = native_byte_order_arr[0];
				m_low = native_byte_order_arr[1];
			}
			constexpr uint128_limb_container& operator=(std::uint64_t val) noexcept
			{
				m_high = 0;
				m_low = val;
				return *this;
			}
			constexpr uint128_limb_container& operator=(std::uint32_t val) noexcept
			{
				m_high = 0;
				m_low = val;
				return *this;
			}
			constexpr explicit operator std::array<std::uint64_t, 2u>() const noexcept
			{
				return std::array<std::uint64_t, 2u>{m_high, m_low, };
			}
			constexpr explicit operator std::uint64_t() const noexcept
			{
				return m_low;
			}
			constexpr explicit operator std::uint32_t() const noexcept
			{
				return static_cast<std::uint32_t>(m_low);
			}
			constexpr ~uint128_limb_container() = default;
		};

		template<is_builtin_u128 UnsignedInteger, bool LittleEndian>
		constexpr std::array<std::uint64_t, 2u> to_native_array(UnsignedInteger val) noexcept
		{
			const auto low = static_cast<std::uint64_t>(val);
			const auto high = static_cast<std::uint64_t>(val >> 64);
			if constexpr (LittleEndian)
			{
				return std::array<std::uint64_t, 2u>{low, high};
			}
			else
			{
				return std::array<std::uint64_t, 2u>{high,low};
			}
		}
		
		template<is_builtin_u128 UnsignedInteger, bool LitteEndian>
		constexpr UnsignedInteger from_native_array(std::array<std::uint64_t, 2u> native_arr) noexcept
		{
			using uint_t = std::remove_cvref_t<std::remove_const_t<UnsignedInteger>>;
			if constexpr (LitteEndian)
			{
				uint_t ret = native_arr[1];
				ret <<= 64;
				ret |= native_arr[0];
				return ret;
			}
			else
			{
				uint_t ret = native_arr[0];
				ret <<= 64;
				ret |= native_arr[1];
				return ret;
			}			
		}
		
		template<is_builtin_u128 UnsignedInteger, bool LittleEndian>
		struct alignas(uint128_alignment) uint128_limb_container<UnsignedInteger, LittleEndian> final
		{
			using uint_t = std::remove_cvref_t<std::remove_const_t<UnsignedInteger>>;
			uint_t m_value{};
			constexpr uint128_limb_container() noexcept = default;
			constexpr uint128_limb_container(const uint128_limb_container& other) noexcept = default;
			constexpr uint128_limb_container(uint128_limb_container&& other) noexcept = default;
			constexpr uint128_limb_container& operator=(const uint128_limb_container& other) noexcept = default;
			constexpr uint128_limb_container& operator=(uint128_limb_container&& other) noexcept = default;
			constexpr uint128_limb_container(uint_t val) noexcept : m_value{val} {}
			constexpr uint128_limb_container(std::int64_t v) noexcept : m_value{}
			{
				m_value = static_cast<uint_t>(v);
			}
			constexpr uint128_limb_container(std::uint64_t val) noexcept : m_value{ val } {}
			constexpr uint128_limb_container(std::uint32_t val) noexcept : m_value{val} {}
			constexpr uint128_limb_container(std::uint64_t high, std::uint64_t low) noexcept
			{
				m_value = high;
				m_value <<= 64;
				m_value |= low;
			}
			constexpr uint128_limb_container& operator=(uint_t val) noexcept
			{
				m_value = val;
				return *this;
			}

			
			constexpr uint128_limb_container(std::array<std::uint64_t, 2u> native_byte_order_arr) noexcept
			{
				if (std::is_constant_evaluated())
				{
					m_value = from_native_array<uint_t, LittleEndian>(native_byte_order_arr);
				}
				else
				{
					m_value = bit_cast<uint_t, std::array<std::uint64_t, 2u>>(native_byte_order_arr);
				}
			}
			constexpr uint128_limb_container& operator=(std::uint64_t val)
			{
				m_value = val;
				return *this;
			}
			constexpr uint128_limb_container& operator=(std::uint32_t val)
			{
				m_value = val;
				return *this;
			}
			constexpr operator uint_t() const noexcept
			{
				return m_value;
			}
			constexpr explicit operator std::array<std::uint64_t, 2u>() const noexcept
			{
				if (std::is_constant_evaluated())
				{
					return to_native_array<UnsignedInteger, LittleEndian>(m_value);
				}
				else
				{
					return bit_cast<std::array<std::uint64_t, 2u>>(m_value);
				}
			}
			constexpr explicit operator std::uint64_t() const noexcept
			{
				return static_cast<std::uint64_t>(m_value);
			}
			constexpr explicit operator std::uint32_t() const noexcept
			{
				return static_cast<std::uint32_t>(m_value);
			}
			constexpr ~uint128_limb_container() = default;
			
		};

		static constexpr bool is_little_endian = std::endian::native != std::endian::big;

		template<ui128_limb_container_split UnsignedInteger, bool LittleEndian>
		constexpr std::uint64_t get_low(const uint128_limb_container<UnsignedInteger, LittleEndian>& val) noexcept
		{
			return val.m_low;
		}

		template<ui128_limb_container_builtin UnsignedInteger, bool LittleEndian>
		constexpr std::uint64_t get_low(const uint128_limb_container<UnsignedInteger, LittleEndian>& val) noexcept
		{
			return static_cast<std::uint64_t>(val.m_value);
		}

		template<ui128_limb_container_split UnsignedInteger, bool LittleEndian>
		constexpr std::uint64_t get_high(const uint128_limb_container<UnsignedInteger, LittleEndian>& val) noexcept
		{
			return val.m_high;
		}

		template<ui128_limb_container_builtin UnsignedInteger, bool LittleEndian>
		constexpr std::uint64_t get_high
			(const uint128_limb_container<UnsignedInteger, LittleEndian>& val) noexcept
		{
			return static_cast<std::uint64_t>(val.m_value >> 64);
		}
		using ui128_split_limb = std::uint64_t;
		using ui128_limb = std::conditional_t<is_full_builtin_u128<natuint128_t>, natuint128_t, ui128_split_limb>;
		
		using ui128_partial_builtin_limb = std::conditional_t<is_partial_builtin_u128<natuint128_t> ||
			is_full_builtin_u128<natuint128_t>, natuint128_t, std::uint64_t>;
	}

	using uint128_limb_container_t = std::conditional_t<internal::is_full_builtin_u128<internal::ui128_limb>,
		internal::uint128_limb_container<internal::ui128_limb, internal::is_little_endian>,
		internal::uint128_limb_container<internal::ui128_split_limb, internal::is_little_endian>>;

	using uint128_always_split_container_t = internal::uint128_limb_container<internal::ui128_split_limb, internal::is_little_endian>;

	using uint128_limited_builtin_container_t = std::conditional_t<internal::is_partial_builtin_u128<internal::ui128_limb>,
		internal::uint128_limb_container<internal::ui128_partial_builtin_limb, internal::is_little_endian>, uint128_always_split_container_t>;

	using bi_ui128_t = std::conditional_t<internal::exits_builtin_u128, natuint128_t, std::uint64_t>;
	
	constexpr uint128_calc_mode calculation_mode = numerics::init_eval_mode();

	constexpr bool is_little_endian = std::endian::native != std::endian::big;
	
}

#endif //CJM_FIXED_UINT_CONTAINER_HPP_
