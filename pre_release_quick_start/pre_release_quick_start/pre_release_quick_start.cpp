#include <iostream>
#include <iomanip>
#include <cjm/numerics/uint128.hpp>

int main()
{
	using uint128_t = cjm::numerics::uint128;
	using namespace cjm::numerics::literals;
	constexpr auto newl = '\n';
	constexpr uint128_t lhs = 123'456'789'012'345'678'901'234_u128;
	constexpr uint128_t rhs = 432'109'876'543'210'987'654'321_u128;
	std::cout << "Hello, uint128_t!" << newl;
	std::cout << "lhs [" << lhs << "] * rhs [" << rhs << "] == [" << (lhs * rhs) << "]." << newl;
	std::cout << "Goodbye!" << std::endl;
	return 0;
}


