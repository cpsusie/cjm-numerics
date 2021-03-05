#ifndef CJM_FIXED_UINT_CONTAINER_MATH_HPP_
#define CJM_FIXED_UINT_CONTAINER_MATH_HPP_
#include "fixed_uint_container.hpp"
#include <array>
namespace cjm::numerics::fixed_uint_container
{


	template<numerics::concepts::unsigned_integer Ui>
	using add_carry_result_t = std::array<std::remove_cvref_t<std::remove_const_t<Ui>>, 2>;
			
	namespace internal
	{

		inline add_carry_result_t<std::uint64_t> add_with_carry_compiler_intrin_little_endian_u64(std::uint64_t first_addend, std::uint64_t second_addend, unsigned char carry_in) noexcept
		{
			std::uint64_t sum = 0;
			std::uint64_t carry_out = CJM_ADDCARRY64(carry_in, first_addend, second_addend, &sum);
			return add_carry_result_t<std::uint64_t>{sum, carry_out};
		}		
	}


	template<numerics::concepts::unsigned_integer Ui>
	constexpr Ui get_sum(const add_carry_result_t<Ui>& ac_res) noexcept
	{
		if constexpr (std::endian::native != std::endian::big)
		{
			return ac_res[0];
		}
		else
		{
			return ac_res[1];
		}
	}

	template<numerics::concepts::unsigned_integer Ui>
	constexpr Ui get_carry_out(const add_carry_result_t<Ui>& ac_res) noexcept
	{
		if constexpr (std::endian::native != std::endian::big)
		{
			return ac_res[1];
		}
		else
		{
			return ac_res[0];
		}
	}
	
	template<numerics::concepts::builtin_unsigned_integer Ui, uint128_calc_mode CalculationMode = calculation_mode, bool LittleEndian = std::endian::native != std::endian::big>
	constexpr add_carry_result_t<Ui> add_with_carry(Ui first_addend, Ui second_addend, unsigned char carry_in) noexcept
	{
		if (std::is_constant_evaluated())
		{
			Ui sum = first_addend + second_addend;
			Ui carry_out = sum < first_addend;
			if constexpr (LittleEndian)
			{
				return add_carry_result_t<Ui>{sum, carry_out};
			}
			else
			{
				return add_carry_result_t<Ui>{carry_out, sum};
			}
		}
		else if constexpr (std::is_nothrow_convertible_v<Ui, std::uint64_t> && (CalculationMode == uint128_calc_mode::msvc_x64 || CalculationMode == uint128_calc_mode::msvc_x64_clang_or_intel_llvm) && LittleEndian)
		{
			return internal::add_with_carry_compiler_intrin_little_endian_u64(
				static_cast<std::uint64_t>(first_addend), static_cast<std::uint64_t>(second_addend), carry_in);
		}
		else if constexpr (std::is_nothrow_convertible_v<Ui, std::uint64_t> && internal::exits_builtin_u128)
		{
			bi_ui128_t sum = static_cast<bi_ui128_t>(first_addend) + static_cast<bi_ui128_t>(second_addend) + static_cast<bi_ui128_t>(carry_in);
			std::uint64_t carry_out = sum > std::numeric_limits<std::uint64_t>::max();
			if constexpr (LittleEndian)
			{
				return add_carry_result_t<Ui>{ static_cast<std::uint64_t>(sum), carry_out};
			}
			else
			{
				return add_carry_result_t<Ui>{ carry_out, static_cast<std::uint64_t>(sum)};
			}
		}
		else
		{
			Ui sum = first_addend + second_addend;
			Ui carry_out = sum < first_addend;
			if constexpr (LittleEndian)
			{
				return add_carry_result_t<Ui>{sum, carry_out};
			}
			else
			{
				return add_carry_result_t<Ui>{carry_out, sum};
			}
		}
	}
	
	
	
	
}

#endif
