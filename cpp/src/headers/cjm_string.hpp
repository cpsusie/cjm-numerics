#ifndef  cjm_string_hpp
#define cjm_string_hpp
#include <string>
namespace cjm::string
{
	template<typename Char, typename CharTraits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
	std::basic_string<Char, CharTraits, Allocator> trim(std::basic_string<Char, CharTraits, Allocator>&& trimMe);

	template<typename Char, typename CharTraits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
	std::basic_string<Char, CharTraits, Allocator> trim(const std::basic_string<Char, CharTraits, Allocator>& trimMe);
	
	class string_helper
	{
	public:
		template<typename Char, typename CharTraits, typename Allocator>
		friend std::basic_string<Char, CharTraits, Allocator> trim(std::basic_string<Char, CharTraits, Allocator>&& trimMe);

		string_helper() = delete;
		~string_helper() = delete;
		string_helper(const string_helper& other) = delete;
		string_helper(string_helper&& other) noexcept = delete;
		string_helper& operator=(const string_helper& other) = delete;
		string_helper& operator=(string_helper&& other) noexcept = delete;
	private:
		
		template<typename Char, typename CharTraits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
		static void ltrim(std::basic_string<Char, CharTraits, Allocator>& toBeLTrimmed);
		template<typename Char, typename CharTraits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
		static void rtrim(std::basic_string<Char, CharTraits, Allocator>& toBeRTrimmed);

	};

	
}
#include "cjm_string.inl"
#endif // ! cjm_string_hpp

