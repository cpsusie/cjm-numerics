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
#ifndef CJM_OUTPUT_GUARD_HPP_
#define CJM_OUTPUT_GUARD_HPP_
#include <utility>
#include <chrono>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <deque>
#include <stop_token>
#include <memory>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <deque>
#include <optional>
#include <filesystem>
#include <fstream>

namespace cjm::testing::output_guard
{
	enum class dedicated_output_target
	{
		std_out = 0,
		std_err,
		file_out
	};

	void demo_dedicated_output();

	class output_proxy;
	

	class no_file_out_proxy : std::logic_error
	{
	public:
		no_file_out_proxy()
			: std::logic_error{
				"This dedicated output object does not have a file out proxy." } {}
	};
	
	class dedicated_output final : public std::enable_shared_from_this<dedicated_output>
	{
	public:
		static std::shared_ptr<dedicated_output> make_dedicated_output(std::basic_ostream<char>& wrapped_std_out,
			std::basic_ostream<char>& wrapped_std_err, std::optional<std::filesystem::path> file_output_path = std::nullopt);

		[[nodiscard]] bool has_file_out_proxy() const;
		[[nodiscard]] bool is_running() const;
		[[nodiscard]] std::optional<std::filesystem::path> get_file_out_proxy_path() const;
		
		[[nodiscard]] output_proxy cout_proxy();
		[[nodiscard]] output_proxy cerr_proxy();
		[[nodiscard]] output_proxy file_out_proxy();
		[[nodiscard]] std::optional<output_proxy> try_get_output_proxy();
		
		
		dedicated_output& operator = (dedicated_output&& other) noexcept = delete;
		dedicated_output& operator=(const dedicated_output& other) = delete;
		dedicated_output(const dedicated_output& other) = delete;
		dedicated_output(dedicated_output&& other) noexcept = delete;
		dedicated_output() = delete;
		~dedicated_output();

		void clear_queue_and_pause();
		void restart();
		void restart(std::optional<std::filesystem::path> file_path);
		
		void enqueue_output(std::string_view text);
		void enqueue_output(std::string&& text);
		void enqueue_output(std::stringstream&& text);

		void enqueue_file_output(std::string_view text);
		void enqueue_file_output(std::string&& text);
		void enqueue_file_output(std::stringstream&& text);

		void enqueue_error(std::string_view text);
		void enqueue_error(std::string&& text);
		void enqueue_error(std::stringstream&& text);

	private:
		friend class output_proxy;
		using pair_t = std::pair<dedicated_output_target, std::string>;
		using lock_t = std::unique_lock<std::mutex>;
		explicit dedicated_output(std::basic_ostream<char>* wrapped_std_out,
			std::basic_ostream<char>* wrapped_std_err, std::optional<std::filesystem::path> file_output_path);
		void enqueue_item(std::pair<dedicated_output_target, std::string>&& enqueue_me);
		output_proxy unsafe_get_file_output_proxy();

		static void start_loop(std::stop_token token, std::shared_ptr<dedicated_output> ptr);

		mutable std::mutex m_mutex;
		std::condition_variable m_not_empty_condition;
		std::condition_variable m_empty_condition;
		std::basic_ostream<char>* m_wrapped_out;
		std::basic_ostream<char>* m_wrapped_err;
		std::optional<std::pair<std::filesystem::path, std::basic_ofstream<char>>> m_wrapped_filestream;
		std::jthread m_output_thread;
		std::stop_source m_cts;
		std::deque<pair_t> m_queue;
		std::atomic<bool> m_suppress_enqueue;
		bool m_paused = false;
	};



	class output_proxy final : public std::stringstream
	{
	public:

		output_proxy(const output_proxy& other) = delete;
		output_proxy(output_proxy&& other) noexcept;
		output_proxy& operator=(const output_proxy& other) = delete;
		output_proxy& operator=(output_proxy&& other) noexcept;
		~output_proxy() override; // NOLINT(bugprone-exception-escape) 
	private:
		output_proxy();
		friend class dedicated_output;
		output_proxy(dedicated_output_target target, std::shared_ptr<dedicated_output> queue);
		std::weak_ptr<dedicated_output> m_output_queue;
		dedicated_output_target m_target;
	};
}

#endif