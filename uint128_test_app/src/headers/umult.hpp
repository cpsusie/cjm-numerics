#ifndef CJM_UMULT_HPP_
#define CJM_UMULT_HPP_
#include <cjm/numerics/uint128.hpp>
#include <cjm/numerics/cjm_numeric_concepts.hpp>
#include <utility>
#include <limits>
#include <array>
#include <cstdint>
#include <concepts>
#include <type_traits>
#include <bit>
#include <algorithm>
namespace cjm::numerics::internal
{
	namespace concepts
	{
		template<typename UInt16Or32>
		concept is_uint_16_or_32_t = cjm::numerics::concepts::builtin_unsigned_integer<UInt16Or32> && 
				((sizeof(UInt16Or32) == sizeof(std::uint32_t) && std::numeric_limits<UInt16Or32>::digits == std::numeric_limits<std::uint32_t>::digits) || 
					(sizeof(UInt16Or32) == sizeof(std::uint16_t) && std::numeric_limits<UInt16Or32>::digits == std::numeric_limits<std::uint16_t>::digits));

		template<typename SomeUInt128>
		concept is_real_builtin_u128 =
#ifdef CJM_USE_INTRINSIC_U128
			std::is_same_v<unsigned __int128, std::remove_cvref_t<std::remove_const_t<SomeUInt128>>;
#else
			false;
#endif
		
	}

	template<cjm::numerics::concepts::unsigned_integer UnsignedInteger>
	struct uint_ext_mult_helper
	{

	};

	namespace x64_mult_helper_impl
	{
		template<bool LittleEndian>
		struct uint64_t_mult_helper;

		template<concepts::is_real_builtin_u128 BuiltInU128, bool LittleEndian>
		struct builtin_u128_mult_helper;
		
		template<>
		struct alignas(uint128_align) uint64_t_mult_helper<true> final
		{
			std::uint64_t m_low{};
			std::uint64_t m_high{};

			constexpr uint64_t_mult_helper(const std::array<std::uint64_t, 2u>& arr ) noexcept : m_low{arr[0]}, m_high{arr[1]} {}
			constexpr uint64_t_mult_helper(std::uint64_t high, std::uint64_t low) noexcept : m_low{low}, m_high{high} {}
			constexpr uint64_t_mult_helper(std::uint64_t low) noexcept : m_low{low}, m_high{} {}
			constexpr uint64_t_mult_helper(std::uint32_t low) noexcept : m_low{ low }, m_high{} {}
			constexpr uint64_t_mult_helper() noexcept = default;
			constexpr uint64_t_mult_helper(const uint64_t_mult_helper& other) noexcept = default;
			constexpr uint64_t_mult_helper(uint64_t_mult_helper&& other) noexcept = default;
			constexpr uint64_t_mult_helper& operator=(uint64_t_mult_helper&& other) noexcept = default;
			constexpr uint64_t_mult_helper& operator=(const uint64_t_mult_helper& other) noexcept = default;
			~uint64_t_mult_helper() = default;
			constexpr operator std::array<std::uint64_t, 2u>() noexcept
			{
				return std::array<std::uint64_t, 2u>{m_low, m_high};
			}
		};

		template<>
		struct alignas(uint128_align) uint64_t_mult_helper<false> final
		{

			std::uint64_t m_high{};
			std::uint64_t m_low{};

			constexpr uint64_t_mult_helper(const std::array<std::uint64_t, 2u>& arr ) noexcept :  m_high{arr[0]}, m_low{arr[1]} {}
			constexpr uint64_t_mult_helper(std::uint64_t high, std::uint64_t low) noexcept :  m_high{high}, m_low{low} {}
			constexpr uint64_t_mult_helper(std::uint64_t low) noexcept : m_high{}, m_low{low} {}
			constexpr uint64_t_mult_helper(std::uint32_t low) noexcept : m_high{}, m_low{ low } {}
			constexpr uint64_t_mult_helper() noexcept = default;
			constexpr uint64_t_mult_helper(const uint64_t_mult_helper& other) noexcept = default;
			constexpr uint64_t_mult_helper(uint64_t_mult_helper&& other) noexcept = default;
			constexpr uint64_t_mult_helper& operator=(uint64_t_mult_helper&& other) noexcept = default;
			constexpr uint64_t_mult_helper& operator=(const uint64_t_mult_helper& other) noexcept = default;
			~uint64_t_mult_helper() = default;
			constexpr operator std::array<std::uint64_t, 2u>() noexcept
			{
				return std::array<std::uint64_t, 2u>{m_high, m_low};
			}
		};

	}


	template<>
	struct uint_ext_mult_helper<std::uint64_t>
	{
		//default to little endian, only use big if actually set.
		using x64_mult_helper_t = x64_mult_helper_impl::uint64_t_mult_helper<std::endian::native != std::endian::big>;
		using multiplicand_t = std::uint64_t;
		static constexpr size_t max_straight_add_digits = std::numeric_limits<std::uint64_t>::digits;
		static constexpr size_t mult_size = std::numeric_limits<std::uint64_t>::digits;
		static constexpr size_t add_c_size = mult_size * 2;
		static constexpr size_t base_shift = mult_size / 2;
		static constexpr size_t full_shift = mult_size;
		static constexpr size_t low_index = std::endian::native != std::endian::big ? 0 : 1;
		static constexpr size_t high_index = std::endian::native != std::endian::big ? 1 : 0;
		static constexpr bool can_do_straight_addition = add_c_size <= max_straight_add_digits;
		using add_c_size_t = x64_mult_helper_t;
		using split_size_t = std::uint32_t;
		using split_array_t = std::array<multiplicand_t , 2u>;
		using split_product_array_t = std::array<add_c_size_t, 4u>;
		static constexpr size_t max_split_value = std::numeric_limits<split_size_t>::max();
		using return_val_t = add_c_size_t;
		static constexpr multiplicand_t low_half_bitmask = static_cast<multiplicand_t>(std::numeric_limits<split_size_t>::max());

		static constexpr multiplicand_t get_low(const split_array_t& arr) noexcept
		{
			return arr[low_index];
		}

		static constexpr multiplicand_t get_high(const split_array_t& arr) noexcept
		{
			return arr[high_index];
		}

		static constexpr split_array_t split_up(const multiplicand_t& split_me) noexcept
		{
			return fallback_split_up(split_me);
		}

		static constexpr add_c_size_t& lshift(add_c_size_t& val, size_t amount) noexcept
		{
			[[maybe_unused]] constexpr size_t max_shift = 64;
			assert(amount <= max_shift);
			if (amount == 64)
			{
				val.m_high = val.m_low;
				val.m_low = 0;
			}
			else if (amount > 0)
			{
				val.m_high <<= amount;
				const auto carryover_mask = (std::numeric_limits<std::uint64_t>::max() << (64 - amount));
				const auto remaining_low = (val.m_low & carryover_mask) >> (64 - amount);
				val.m_low <<= amount;
				val.m_high &= ~(carryover_mask >> (64 - amount));
				val.m_high |= remaining_low;
			}

			return val;
		}

		static constexpr std::uint64_t add_with_carry(std::uint64_t lhs, std::uint64_t rhs, unsigned char carry_in, unsigned char& carry_out) noexcept
		{
			carry_out = 0;
			auto sum = lhs + rhs + (static_cast<bool>(carry_in) ? 1 : 0);
			carry_out = sum < lhs;
			return sum;			
		}

		static constexpr x64_mult_helper_t& add(x64_mult_helper_t& lhs, const x64_mult_helper_t& rhs) noexcept
		{
			unsigned char carry_in = 0;
			unsigned char carry_out = 0;
			lhs.m_low = add_with_carry(lhs.m_low, rhs.m_low, carry_in, carry_out);
			carry_in = carry_out;
			lhs.m_high = add_with_carry(lhs.m_high, rhs.m_high, carry_in, carry_out);
			assert(carry_out == 0); //shouldn't be overflowing on high unless we did something wrong
			return lhs;
		}
		
		static constexpr split_product_array_t get_adjusted_products(const split_array_t& lhs_arr, const split_array_t& rhs_arr) noexcept
		{
			auto factor_1_low = get_low(lhs_arr);
			auto factor_2_low = get_low(rhs_arr);
			auto factor_1_high = get_high(lhs_arr);
			auto factor_2_high = get_high(rhs_arr);

			assert(factor_1_low <= max_split_value);
			assert(factor_2_low <= max_split_value);
			assert(factor_1_high <= max_split_value);
			assert(factor_2_high <= max_split_value);

			const auto product_f1_low_f2_low = static_cast<add_c_size_t>(factor_1_low * factor_2_low);
			auto product_f1_high_f2_low = static_cast<add_c_size_t>(factor_1_high * factor_2_low);
			auto product_f1_low_f2_high = static_cast<add_c_size_t>(factor_1_low * factor_2_high);
			auto product_f1_high_f2_high = static_cast<add_c_size_t>(factor_1_high * factor_2_high);

			return split_product_array_t{product_f1_low_f2_low, lshift( product_f1_high_f2_low, base_shift), lshift(product_f1_low_f2_high, base_shift), lshift(product_f1_high_f2_high ,full_shift)};

//			let product_f1_low_f2_low := add_c_size_t(factor_1_low * factor_2_low) ( ((0x00a2 * 0x000f) -> add_c_size_t) == 0x0000_097e)
//			let product_f1_high_f2_low := add_c_size_t(factor_1_high * factor_2_low) ( ((0x00fe * 0x000f) -> add_c_size_t) == 0x0000_0ee2)
//			let product_f1_low_f2_high := add_c_size_t(factor_1_low * factor_2_high) ( ((0x00a2 * 0x00f0) -> add_c_size_t) == 0x0000_97e0)
//			let product_f1_high_f2_high := add_c_size_t(factor_1_high * factor_2_high) ( ((0x00fe * 0x00f0) -> add_c_size_t) == 0x0000_ee20)

		}

	private:
		static constexpr split_array_t fallback_split_up(const multiplicand_t& split_me) noexcept
		{
			auto high = static_cast<split_size_t>(split_me >> base_shift);
			auto low = static_cast<split_size_t>(split_me & low_half_bitmask);
			if constexpr (std::endian::native != std::endian::big)
			{
				return split_array_t {low, high};
			}
			else
			{
				return split_array_t {high, low};
			}
		}
	};

	template<concepts::is_uint_16_or_32_t UnsignedInteger>
	struct uint_ext_mult_helper<UnsignedInteger>
	{
		using multiplicand_t = std::remove_cvref_t<UnsignedInteger>;
		static constexpr size_t max_straight_add_digits = std::numeric_limits<std::uint64_t>::digits;
		static constexpr size_t mult_size = std::numeric_limits<UnsignedInteger>::digits;
		static constexpr size_t add_c_size = mult_size * 2;
		static constexpr size_t base_shift = mult_size / 2;
		static constexpr size_t full_shift = mult_size;
		static constexpr size_t low_index = std::endian::native != std::endian::big ? 0 : 1;
		static constexpr size_t high_index = std::endian::native != std::endian::big ? 1 : 0;
		static constexpr bool can_do_straight_addition = add_c_size <= max_straight_add_digits;
		using add_c_size_t = std::conditional_t<std::is_same_v<multiplicand_t, std::uint32_t>, std::uint64_t, std::uint32_t>;
		using split_size_t = std::conditional_t<std::is_same_v<multiplicand_t, std::uint32_t>, std::uint16_t, std::uint8_t>;
		using split_array_t = std::array<multiplicand_t , 2u>;
		using split_product_array_t = std::array<add_c_size_t, 4u>;
		static constexpr size_t max_split_value = std::numeric_limits<split_size_t>::max();
		using return_val_t = add_c_size_t;
		static constexpr multiplicand_t low_half_bitmask = static_cast<multiplicand_t>(std::numeric_limits<split_size_t>::max());

		static constexpr multiplicand_t get_low(const split_array_t arr) noexcept
		{
			return arr[low_index];
		}

		static constexpr multiplicand_t get_high(const split_array_t& arr) noexcept
		{
			return arr[high_index];
		}

		static constexpr split_array_t split_up(const multiplicand_t& split_me) noexcept
		{
			return fallback_split_up(split_me);
		}

		static constexpr split_product_array_t get_adjusted_products(const split_array_t& lhs_arr, const split_array_t& rhs_arr) noexcept
		{
			auto factor_1_low = get_low(lhs_arr);
			auto factor_2_low = get_low(rhs_arr);
			auto factor_1_high = get_high(lhs_arr);
			auto factor_2_high = get_high(rhs_arr);

			assert(factor_1_low <= max_split_value);
			assert(factor_2_low <= max_split_value);
			assert(factor_1_high <= max_split_value);
			assert(factor_2_high <= max_split_value);

			auto product_f1_low_f2_low = static_cast<add_c_size_t>(factor_1_low * factor_2_low);
			auto product_f1_high_f2_low = static_cast<add_c_size_t>(factor_1_high * factor_2_low);
			auto product_f1_low_f2_high = static_cast<add_c_size_t>(factor_1_low * factor_2_high);
			auto product_f1_high_f2_high = static_cast<add_c_size_t>(factor_1_high * factor_2_high);

			return split_product_array_t{product_f1_low_f2_low, (product_f1_high_f2_low << base_shift), (product_f1_low_f2_high << base_shift), (product_f1_high_f2_high << full_shift)};

//			let product_f1_low_f2_low := add_c_size_t(factor_1_low * factor_2_low) ( ((0x00a2 * 0x000f) -> add_c_size_t) == 0x0000_097e)
//			let product_f1_high_f2_low := add_c_size_t(factor_1_high * factor_2_low) ( ((0x00fe * 0x000f) -> add_c_size_t) == 0x0000_0ee2)
//			let product_f1_low_f2_high := add_c_size_t(factor_1_low * factor_2_high) ( ((0x00a2 * 0x00f0) -> add_c_size_t) == 0x0000_97e0)
//			let product_f1_high_f2_high := add_c_size_t(factor_1_high * factor_2_high) ( ((0x00fe * 0x00f0) -> add_c_size_t) == 0x0000_ee20)

		}

	private:
		static constexpr split_array_t fallback_split_up(const multiplicand_t& split_me) noexcept
		{
			auto high = static_cast<split_size_t>(split_me >> base_shift);
			auto low = static_cast<split_size_t>(split_me & low_half_bitmask);
			if constexpr (std::endian::native != std::endian::big)
			{
				return split_array_t {low, high};
			}
			else
			{
				return split_array_t {high, low};
			}
		}
	};

	constexpr std::uint16_t umult(std::uint8_t lhs, std::uint8_t rhs) noexcept
	{
		return static_cast<std::uint16_t>((+lhs) * (+rhs));
	}

	template<concepts::is_uint_16_or_32_t UnsignedInteger>
	constexpr auto umult(const UnsignedInteger& lhs, const UnsignedInteger& rhs) noexcept -> typename uint_ext_mult_helper<UnsignedInteger>::return_val_t
	{
		using helper_t =  uint_ext_mult_helper<UnsignedInteger>;
		using result_t = typename helper_t::return_val_t;

		auto products = helper_t::get_adjusted_products(helper_t::split_up(lhs), helper_t::split_up(rhs));
		std::uint64_t sum = 0;
		for (const auto& p : products)
		{
			sum += static_cast<std::uint64_t>(p);
		}
		return static_cast<result_t>(sum);

	}
	constexpr uint_ext_mult_helper<std::uint64_t>::return_val_t umult(std::uint64_t first_factor, std::uint64_t second_factor) noexcept;
}

constexpr cjm::numerics::internal::uint_ext_mult_helper<std::uint64_t>::return_val_t cjm::numerics::internal::umult(std::uint64_t first_factor, std::uint64_t second_factor) noexcept
{
	using helper_t = uint_ext_mult_helper<std::uint64_t>;

	auto products = helper_t::get_adjusted_products(helper_t::split_up(first_factor), helper_t::split_up(second_factor));
	auto sum = products[0];
	sum = helper_t::add(sum, products[1]);
	sum = helper_t::add(sum, products[2]);
	sum = helper_t::add(sum, products[3]);
	return sum;
}


#endif
