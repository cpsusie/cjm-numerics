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
			sizeof(Ui) == 128 && std::is_same_v<unsigned __int128, std::remove_cvref_t<std::remove_const_t<Ui>>;
#else
			false;
#endif

		constexpr bool exits_builtin_u128 = is_builtin_u128<natuint128_t>;
		
		template<typename Ui, size_t Digits>
		concept is_unsigned_int_with_digits = (cjm::numerics::concepts::builtin_unsigned_integer<Ui> && Digits >= 64u && std::popcount(Digits) == 1u && Digits == std::numeric_limits<Ui>::digits) || (is_builtin_u128<Ui> && sizeof(Ui) == (sizeof(std::uint64_t) * 2) && Digits == 128);

		constexpr size_t uint128_alignment = is_x64 ? alignof(std::uint64_t) * 2 : alignof(std::uint64_t);

		template<typename UnsignedInteger, bool LittleEndian>
			requires (is_unsigned_int_with_digits<UnsignedInteger, 64u> || is_builtin_u128<UnsignedInteger>)
		struct uint128_limb_container;

		
		static_assert(cjm::numerics::concepts::builtin_unsigned_integer<std::uint64_t>);
		static_assert(std::numeric_limits<std::uint64_t>::digits == 64u);
		static_assert(is_unsigned_int_with_digits<std::uint64_t, 64>);
		static_assert(!exits_builtin_u128 || is_unsigned_int_with_digits<natuint128_t, 128>);

		template<typename UnsignedInteger, bool LittleEndian>
			requires (is_unsigned_int_with_digits<UnsignedInteger, 64u> || is_builtin_u128<UnsignedInteger>)
		constexpr std::uint64_t get_low(const uint128_limb_container<UnsignedInteger, LittleEndian>& val) noexcept;

		template<typename UnsignedInteger, bool LittleEndian>
			requires (is_unsigned_int_with_digits<UnsignedInteger, 64u> || is_builtin_u128<UnsignedInteger>)
		constexpr std::uint64_t get_high(const uint128_limb_container<UnsignedInteger, LittleEndian>& val) noexcept;
		
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
			~uint128_limb_container() = default;
		};
		
		template<>
		struct alignas(uint128_alignment) uint128_limb_container<std::uint64_t, false> final
		{
			std::uint64_t m_high;
			std::uint64_t m_low;
			constexpr uint128_limb_container() noexcept = default;
			constexpr uint128_limb_container(const uint128_limb_container & other) noexcept = default;
			constexpr uint128_limb_container(uint128_limb_container && other) noexcept = default;
			constexpr uint128_limb_container& operator=(const uint128_limb_container & other) noexcept = default;
			constexpr uint128_limb_container& operator=(uint128_limb_container && other) noexcept = default;
			constexpr uint128_limb_container(std::uint64_t val) noexcept : m_high{}, m_low{ val } {}
			constexpr uint128_limb_container(std::uint32_t val) noexcept : m_high{}, m_low{ val } {}
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
			~uint128_limb_container() = default;
		};

		template<is_builtin_u128 UnsignedInteger, bool LittleEndian>
		constexpr std::array<std::uint64_t, 2u> to_native_array(UnsignedInteger val) noexcept
		{
			using uint_t = std::remove_cvref_t<std::remove_const_t<UnsignedInteger>>;
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
			constexpr uint128_limb_container(std::uint64_t val) noexcept : m_value{ val } {}
			constexpr uint128_limb_container(std::uint32_t val) noexcept : m_value{val} {}
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
			~uint128_limb_container() = default;
			
		};

		static constexpr bool is_little_endian = std::endian::native != std::endian::big;

		template<typename UnsignedInteger, bool LittleEndian>
		requires (is_unsigned_int_with_digits<UnsignedInteger, 64u> || is_builtin_u128<UnsignedInteger>)
		constexpr std::uint64_t get_low(const uint128_limb_container<UnsignedInteger, LittleEndian>& val) noexcept
		{
			if constexpr (is_builtin_u128<UnsignedInteger>)
			{
				return static_cast<std::uint64_t>(val.m_value);
			}
			else
			{
				return val.m_low;
			}
		}

		template<typename UnsignedInteger, bool LittleEndian>
		requires (is_unsigned_int_with_digits<UnsignedInteger, 64u> || is_builtin_u128<UnsignedInteger>)
		constexpr std::uint64_t get_high(const uint128_limb_container<UnsignedInteger, LittleEndian>& val) noexcept
		{
			if constexpr (is_builtin_u128<UnsignedInteger>)
			{
				return static_cast<std::uint64_t>(val.m_value >> 64);				
			}
			else
			{
				return val.m_high;
			}
		}
	}
}

#endif
