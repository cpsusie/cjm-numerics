#include <iostream>
#include <iomanip>
#include <cjm/numerics/uint128.hpp>

int main()  // NOLINT(bugprone-exception-escape) (it's a demo)
{
	using uint128_t = cjm::numerics::uint128;
	using namespace cjm::numerics::uint128_literals;
	constexpr auto newl = '\n';
	constexpr uint128_t lhs = 123'456'789'012'345'678'901'234_u128;
	constexpr uint128_t rhs = 432'109'876'543'210'987'654'321_u128;
	std::cout << "Hello, uint128_t!" << newl;
	std::cout << "lhs [" << lhs << "] * rhs [" << rhs << "] == [" << (lhs * rhs) << "]." << newl;

	//check if compiler intrinsics used:
	//should be available if using microsoft compiler targeting x64 Intel or AMD processor
	std::cout
		<< "Using msvc x64 intrinsics?: [" << std::boolalpha
		<< (cjm::numerics::calculation_mode == cjm::numerics::uint128_calc_mode::msvc_x64) << "]" << newl;

	//Check if using built-in unsigned int 128 for runtime math
	//This type is provided as a language extension by some compilers (NOT msvc)
	//it is usually only available for 64 bit targets on compilers that provide
	//this extension
	std::cout
		<< "Using built-in uint128 for runtime calculations?: [" << std::boolalpha
		<< (cjm::numerics::calculation_mode == cjm::numerics::uint128_calc_mode::intrinsic_u128)
		<< "]." << newl;
	
	//On msvc when targeting x64, enabling AVX2 extension will allow us to use compiler intrinsics
	//supplied by ADX and BMI2 (for add-with-carry and extended precision multiplication)
	//rather than the default compiler intrinsics for those operations.  Make sure your processor
	//target supports those extensions before trying this.
	std::cout
		<< "Has adx enabled: [" << std::boolalpha << cjm::numerics::intel_adx_available << "]." << newl;
	std::cout
		<< "Has bmi2 enabled: [" << std::boolalpha << cjm::numerics::intel_bmi2_available << "]." << newl;
	std::cout
		<< "Goodbye!" << std::endl;
	return 0;
}