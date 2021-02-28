#ifndef CJM_UMULT_HPP_
#define CJM_UMULT_HPP_
#include <cjm/numerics/uint128.hpp>
#include <utility>
#include <limits>
#include <array>
#include <cstdint>

namespace cjm::numerics::internal
{
	constexpr std::array<std::uint64_t, 2u> umult(std::uint64_t first_factor, std::uint64_t second_factor) noexcept;
}

constexpr std::array<std::uint64_t, 2u> cjm::numerics::internal::umult(std::uint64_t first_factor, std::uint64_t second_factor) noexcept
{
	constexpr std::uint64_t high_32_mask = 0xffff'ffff'0000'0000ull;
	
	constexpr auto base_shift_amount = std::numeric_limits<std::uint32_t>::digits;
	auto left_high = static_cast<std::uint32_t>(first_factor >> base_shift_amount);
	auto left_low = static_cast<std::uint32_t>(first_factor);

	auto right_high = static_cast<std::uint32_t>(second_factor >> base_shift_amount);
	auto right_low = static_cast<std::uint32_t>(second_factor);

	auto llow_rlow_product_low = static_cast<std::uint64_t>(left_low) * static_cast<std::uint64_t>(right_low);
	std::uint64_t llow_rlow_product_high = 0ull;
	
	auto lhigh_rlow_product_low = static_cast<std::uint64_t>(left_high) * static_cast<std::uint64_t>(right_low);
	auto lhigh_rlow_product_high = (high_32_mask & lhigh_rlow_product_low) >> base_shift_amount;
	lhigh_rlow_product_low <<= base_shift_amount;

	std::uint64_t llow_righ_product_low = 0;
	auto llow_rhigh_product_high = static_cast<std::uint64_t>(left_low) * static_cast<std::uint64_t>(left_high);

	auto lhigh_rhigh_product_high = (static_cast<std::uint64_t>(left_high) * static_cast<std::uint64_t>(right_high)) << base_shift_amount;
	std::uint64_t lhigh_rhigh_product_low = 0;

	std::uint64_t high_sum = llow_rlow_product_high;
	unsigned char carry_out = 0;
	auto low_sum = 
		add_with_carry_u64(llow_rlow_product_low, lhigh_rlow_product_low, 0, carry_out);
	if (carry_out)
		++high_sum;
	high_sum += lhigh_rlow_product_high;
	low_sum = add_with_carry_u64(low_sum, llow_righ_product_low, 0, carry_out);
	if (carry_out)
		++high_sum;
	high_sum += llow_rhigh_product_high;
	low_sum = add_with_carry_u64(low_sum, lhigh_rhigh_product_low, 0, carry_out);
	if (carry_out)
		++high_sum;
	high_sum += lhigh_rhigh_product_high;

	return std::array<std::uint64_t, 2u>{low_sum, high_sum};
}


#endif
