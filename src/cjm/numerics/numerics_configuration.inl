#ifndef CJM_NUMERICS_CONFIGURATION_INL_
#define CJM_NUMERICS_CONFIGURATION_INL_
#include <cjm/numerics/numerics_configuration.hpp>

namespace cjm::numerics
{
	
	namespace internal
	{
		using enum_type_t = std::underlying_type_t<compiler_used>;
		using namespace std::string_view_literals;
		constexpr auto compiler_name_lookup = std::array<std::string_view, static_cast<enum_type_t>(max_value) + 1>
		{
				"Other"sv,
				"Microsoft"sv,
				"Microsoft-Clang"sv,
				"Microsoft-Intel-Classic"sv,
				"Microsoft-Intel-LLVM"sv,
				"Clang-GCC"sv,
				"GCC"sv,
				"Clang"sv
			
		};

		constexpr auto compiler_name_lookup_wide = std::array<std::wstring_view, static_cast<enum_type_t>(max_value) + 1>
		{
				L"Other"sv,
				L"Microsoft"sv,
				L"Microsoft-Clang"sv,
				L"Microsoft-Intel-Classic"sv,
				L"Microsoft-Intel-LLVM"sv,
				L"Clang-GCC"sv,
				L"GCC"sv,
				L"Clang"sv

		};
	}

	

	constexpr bool is_value_defined(compiler_used v) noexcept
	{
		using enum_type_t = std::underlying_type_t<compiler_used>;
		const enum_type_t min = static_cast<enum_type_t>(min_value);
		const enum_type_t max = static_cast<enum_type_t>(max_value);
		const enum_type_t val = static_cast<enum_type_t>(v);
		return val >= min && val <= max;
	}

	constexpr compiler_used value_or_other_ifndef(compiler_used v) noexcept
	{
		return is_value_defined(v) ? v : compiler_used::other;
	}

	constexpr std::string_view get_text_narrow(compiler_used v) noexcept
	{
		return internal::compiler_name_lookup[(static_cast<internal::enum_type_t>(value_or_other_ifndef(v)))];
	}
	constexpr std::wstring_view get_text_wide(compiler_used v) noexcept
	{
		return internal::compiler_name_lookup_wide[(static_cast<internal::enum_type_t>(value_or_other_ifndef(v)))];		
	}
	
	#pragma warning(push)
	#pragma warning (disable:4068) 
	#pragma clang diagnostic push
	#pragma ide diagnostic ignored "UnreachableCode"
	constexpr uint128_calc_mode init_eval_mode() noexcept
	{
		if constexpr (cjm::numerics::has_intrinsic_u128)
		{
			return cjm::numerics::uint128_calc_mode::intrinsic_u128;
		}
		else if constexpr (cjm::numerics::is_microsoft_windows_x64)
		{
			return cjm::numerics::uint128_calc_mode::msvc_x64;
		}
		else if constexpr (cjm::numerics::is_clang_or_intel_llvm_msvc_x64)
		{
			return uint128_calc_mode::msvc_x64_clang_or_intel_llvm;
		}
		else
		{
			return cjm::numerics::uint128_calc_mode::default_eval;
		}
	}
	#pragma clang diagnostic pop
	#pragma warning(pop)
}

#endif