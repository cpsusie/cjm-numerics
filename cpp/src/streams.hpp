#pragma once
#ifndef CJM_STREAMS_HPP
#define CJM_STREAMS_HPP
#include<locale>
#include<iostream>
#include <boost/io/ios_state.hpp>

namespace cjm
{
	namespace streams
	{
		template<typename Char>
		class NumberFormatter : public std::numpunct<Char>
		{
		public:
			using char_return = typename std::numpunct<Char>::char_type;
			using str_return = typename std::numpunct<Char>::string_type;
			NumberFormatter() = default;
			NumberFormatter(const NumberFormatter& other) = default;
			NumberFormatter(NumberFormatter&& other) noexcept = default;
			NumberFormatter& operator=(const NumberFormatter& other) = default;
			NumberFormatter& operator=(NumberFormatter&& other) noexcept = default;
			~NumberFormatter() override = default;

		protected:

			char_return do_thousands_sep() const override
			{
				static_assert(std::is_same_v<char_return, char> || std::is_same_v<char_return, wchar_t>, "Only supports chars and wchars.");
				if constexpr (std::is_same_v<char_return, char>)
					return ',';
				else
					return L',';
			}

			str_return do_grouping() const override
			{
				static_assert(std::is_same_v<char_return, char> || std::is_same_v<char_return, wchar_t>, "Only supports chars and wchars.");
				static_assert(std::is_same_v < str_return,
					std::basic_string<Char, std::char_traits<Char>, std::allocator<Char>>>,
					"Only standard strings are supported.");
				if constexpr (std::is_same_v<char_return, char>)
					return "\03";
				else
					return L"\03";
			}
		};

		template<typename Char>
		struct cout_type_s;
		template<typename Char>
		struct ios_state_save_s;
#ifdef UNICODE
		constexpr bool Unicode = true;
#define CJM_OSTR std::wcout;
#else 
#define CJM_OSTR std::cout;
		constexpr bool Unicode = false;
#endif
		template<>
		struct cout_type_s<char>
		{
			using cout_type = std::basic_ostream<char, std::char_traits<char>>;
		};

		template<>
		struct cout_type_s<wchar_t>
		{
			using cout_type = std::basic_ostream<wchar_t, std::char_traits<wchar_t>>;
		};

		template<>
		struct ios_state_save_s<char>
		{
			using savertype = boost::io::basic_ios_all_saver<char, std::char_traits<char>>;
		};

		template<>
		struct ios_state_save_s<wchar_t>
		{
			using savertype = boost::io::basic_ios_all_saver<wchar_t, std::char_traits<wchar_t>>;
		};

		constexpr char nnewl = '\n';
		constexpr wchar_t wnewl = L'\n';
		
		using termin = std::conditional_t<Unicode, wchar_t, char>;
		using outstr = cout_type_s< std::conditional_t<Unicode, wchar_t, char>>::cout_type;
		using ios_state_saver = ios_state_save_s<std::conditional_t<Unicode, wchar_t, char>>::savertype;
		using nios_state_saver = boost::io::basic_ios_all_saver<char, std::char_traits<char>>;
		using wios_state_saver = boost::io::basic_ios_all_saver<char, std::char_traits<char>>;
		
		constexpr termin get_termin() noexcept
		{
			if constexpr(Unicode)
			{
				return wnewl;
			}
			else
			{
				return nnewl;
			}
		}

		inline outstr& get_cout() noexcept
		{
			if constexpr (Unicode)
			{
				//It's only an error if it isn't compiled 
				//(i.e. the constexpr value is false) so who cares,
				//safe to ignore.
				return std::wcout;
			}
			else
			{
				return std::cout;
			}			
		}

		inline outstr& get_cerr() noexcept
		{
			if constexpr (Unicode)
			{
				return std::wcerr;
			}
			else
			{
				return std::cerr;
			}
		}
		
		constexpr termin newl = get_termin();
		inline outstr& cout = get_cout();
		inline outstr& cerr = get_cerr();
						
	}
}
#ifdef CJM_OSTR
	#undef CJM_OSTR
#endif
#endif