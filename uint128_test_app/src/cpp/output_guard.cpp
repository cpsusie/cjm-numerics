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
#include <cassert>
#include "output_guard.hpp"

namespace
{
	constexpr auto newl = '\n';
	
	//testing purposes only
	std::shared_ptr<cjm::testing::output_guard::dedicated_output>
		dedicated_output_manager;  // NOLINT(clang-diagnostic-exit-time-destructors)
	std::atomic<bool> m_start_flag;
	using namespace std::chrono_literals;

	template<typename T>
	std::shared_ptr<T> lock_or_clear(std::weak_ptr<T>& weak)
	{
		auto shared = weak.lock();
		if (!shared)
		{
			weak.reset();
		}
		return shared;
	}

	void write_hi()
	{
		assert(dedicated_output_manager);
		auto cout = dedicated_output_manager->cout_proxy();
		auto cerr = dedicated_output_manager->cerr_proxy();
		auto my_thread_id = std::this_thread::get_id();
		// ReSharper disable once CppPossiblyErroneousEmptyStatements (INTENTIONAL!)
		while (!m_start_flag.load());
		cout << "cout proxy-- thread# [" << my_thread_id << "] BEGIN!" << newl;
		for (auto i = 1; i <= 10; ++i)
		{
			cout << "Hello standard output #[" << i << "] from thread [" << my_thread_id << "]." << newl;
			std::this_thread::yield();
		}
		for (auto i = 1; i <= 10; ++i)
		{
			cerr << "Hello standard error #[" << i << "] from thread [" << my_thread_id << "]." << newl;
			std::this_thread::yield();
		}
		cout << "Standard output: Thread #[" << my_thread_id << "] OUT!" << newl;
		cerr << "Standard error: Thread #[" << my_thread_id << "] OUT!" << newl;
	}
}

std::shared_ptr<cjm::testing::output_guard::dedicated_output> cjm::testing::output_guard::dedicated_output::
make_dedicated_output(std::basic_ostream<char>& wrapped_std_out, std::basic_ostream<char>& wrapped_std_err, std::optional<std::filesystem::path> file_output_path)
{
	auto ret = std::shared_ptr<dedicated_output>(new dedicated_output(&wrapped_std_out, &wrapped_std_err, std::move(file_output_path)));
	ret->m_output_thread = std::jthread{ start_loop, ret };
	ret->m_cts = ret->m_output_thread.get_stop_source();
	return ret;
}

bool cjm::testing::output_guard::dedicated_output::has_file_out_proxy() const 
{
	auto lock = std::unique_lock(m_mutex);
	return m_wrapped_filestream.has_value();
}

bool cjm::testing::output_guard::dedicated_output::is_running() const
{
	auto lock = std::unique_lock(m_mutex);
	return !m_paused;
}

std::optional<std::filesystem::path> cjm::testing::output_guard::dedicated_output::get_file_out_proxy_path() const
{
	auto lck = std::unique_lock{ m_mutex };
	if (m_wrapped_filestream.has_value())
	{
		return m_wrapped_filestream->first;
	}
	return std::nullopt;
}

cjm::testing::output_guard::output_proxy cjm::testing::output_guard::dedicated_output::cout_proxy()
{
	auto this_ptr = shared_from_this();
	return output_proxy{ dedicated_output_target::std_out, this_ptr };
}

cjm::testing::output_guard::output_proxy cjm::testing::output_guard::dedicated_output::cerr_proxy()
{
	auto this_ptr = shared_from_this();
	return output_proxy{ dedicated_output_target::std_out, this_ptr };
}

cjm::testing::output_guard::output_proxy cjm::testing::output_guard::dedicated_output::file_out_proxy()
{
	if (!m_wrapped_filestream.has_value())
		throw no_file_out_proxy{};
	return unsafe_get_file_output_proxy();
}

std::optional<cjm::testing::output_guard::output_proxy> cjm::testing::output_guard::dedicated_output::
	try_get_output_proxy()
{
	if (!m_wrapped_filestream.has_value())
		return std::nullopt;
	return std::make_optional(unsafe_get_file_output_proxy());
}

cjm::testing::output_guard::dedicated_output::dedicated_output(std::basic_ostream<char>* wrapped_std_out,
                                                               std::basic_ostream<char>* wrapped_std_err, std::optional<std::filesystem::path> file_output_path) :
	m_wrapped_out(wrapped_std_out), m_wrapped_err(wrapped_std_err)
{
	if (m_wrapped_out == nullptr) throw std::invalid_argument{ "wrapped std output may not be a nullptr." };
	if (m_wrapped_err == nullptr) throw std::invalid_argument{ "wrapped std error may not be a nullptr." };
	if (file_output_path.has_value())
	{
		m_wrapped_filestream = std::make_optional(std::make_pair(*file_output_path, std::basic_ofstream<char>{*file_output_path}));
	}
}

void cjm::testing::output_guard::dedicated_output::enqueue_item(
	std::pair<dedicated_output_target, std::string>&& enqueue_me)
{
	if (!m_suppress_enqueue.load())
	{
		bool enqueued = false;
		{
			auto my_lock = lock_t{ m_mutex };
			if (!m_suppress_enqueue.load())
			{
				m_queue.emplace_back(std::move(enqueue_me));
				enqueued = true;
			}
		}
		if (enqueued)
		{
			m_not_empty_condition.notify_one();
		}
	}
}

cjm::testing::output_guard::output_proxy cjm::testing::output_guard::dedicated_output::unsafe_get_file_output_proxy()
{
	auto ptr = shared_from_this();
	return output_proxy{ dedicated_output_target::file_out, ptr };
}

cjm::testing::output_guard::dedicated_output::~dedicated_output()
{
	clear_queue_and_pause();
}

void cjm::testing::output_guard::dedicated_output::clear_queue_and_pause()
{
	
	{
		auto lock = std::unique_lock{ m_mutex };
		m_suppress_enqueue.store(true);
		if (m_paused)
		{
			assert(m_queue.empty());
			return;
		}
		if (!m_queue.empty())
		{
			m_empty_condition.wait(lock, [&]() -> bool {return m_queue.empty(); });
			m_paused = true;
			m_output_thread.request_stop();
		}
		assert(m_queue.empty());
		m_paused = true;
		if (m_wrapped_filestream.has_value())
		{
			m_wrapped_filestream = std::nullopt;
		}

	}
	m_output_thread.request_stop();
	m_not_empty_condition.notify_all();
	if (m_output_thread.joinable())
		m_output_thread.join();

}

void cjm::testing::output_guard::dedicated_output::restart()
{
	std::optional<std::filesystem::path> path;
	{
		auto lck = std::unique_lock{ m_mutex };
		path = m_wrapped_filestream.has_value() ? std::make_optional(m_wrapped_filestream->first) : std::nullopt;
	}
	restart(std::move(path));
}


void cjm::testing::output_guard::dedicated_output::restart(std::optional<std::filesystem::path> file_path)
{

	{
		auto lock = std::unique_lock{ m_mutex };
		if (!m_paused)
			throw std::logic_error{ "The dedicated output queue is not paused!" };
		if (file_path == std::nullopt)
		{
			m_wrapped_filestream = std::nullopt;
		}
		else
		{
			m_wrapped_filestream = std::make_optional(std::make_pair(*file_path, std::basic_ofstream<char>{*file_path}));
		}
		auto this_ptr = shared_from_this();
		m_output_thread = std::jthread{ start_loop, this_ptr };
		m_cts = m_output_thread.get_stop_source();
		m_paused = false;
		m_suppress_enqueue.store(false);
	}	
}

void cjm::testing::output_guard::dedicated_output::enqueue_output(std::string_view text)
{
	auto payload =
		std::make_pair<dedicated_output_target, std::string>(dedicated_output_target::std_out,
			static_cast<std::string>(text));
	enqueue_item(std::move(payload));
}

void cjm::testing::output_guard::dedicated_output::enqueue_output(std::string&& text)
{
	enqueue_item(std::make_pair(dedicated_output_target::std_out, std::move(text)));
}

void cjm::testing::output_guard::dedicated_output::enqueue_output(std::stringstream&& text)
{
	enqueue_item(std::make_pair(dedicated_output_target::std_out, std::move(*(text.rdbuf())).str()));
}

void cjm::testing::output_guard::dedicated_output::enqueue_file_output(std::string_view text)
{
	auto payload =
		std::make_pair<dedicated_output_target, std::string>(dedicated_output_target::file_out,
			static_cast<std::string>(text));
	enqueue_item(std::move(payload));
}
void cjm::testing::output_guard::dedicated_output::enqueue_file_output(std::string&& text)
{
	enqueue_item(std::make_pair(dedicated_output_target::file_out, std::move(text)));
}
void cjm::testing::output_guard::dedicated_output::enqueue_file_output(std::stringstream&& text)
{
	enqueue_item(std::make_pair(dedicated_output_target::file_out, std::move(*(text.rdbuf())).str()));
}

void cjm::testing::output_guard::dedicated_output::enqueue_error(std::string_view text)
{
	auto payload =
		std::make_pair<dedicated_output_target, std::string>(dedicated_output_target::std_err,
			static_cast<std::string>(text));
	enqueue_item(std::move(payload));
}

void cjm::testing::output_guard::dedicated_output::enqueue_error(std::string&& text)
{
	enqueue_item(std::make_pair(dedicated_output_target::std_err, std::move(text)));
}

void cjm::testing::output_guard::dedicated_output::enqueue_error(std::stringstream&& text)
{
	enqueue_item(std::make_pair(dedicated_output_target::std_err, std::move(*(text.rdbuf())).str()));
}

void cjm::testing::output_guard::dedicated_output::start_loop(std::stop_token token, std::shared_ptr<dedicated_output> ptr)
{
	while (!token.stop_requested())
	{
		pair_t payload;
		if (token.stop_requested())
		{
			return;
		}
		bool empty_after_pop = false;
		{
			auto my_lock = lock_t{ ptr->m_mutex };
			ptr->m_not_empty_condition.wait(my_lock, [&]() -> bool {return !ptr->m_queue.empty() || token.stop_requested(); });
			if (!ptr->m_queue.empty())
			{
				payload = std::move(ptr->m_queue.front());
				ptr->m_queue.pop_front();
				empty_after_pop = ptr->m_queue.empty();
			}
			else if (token.stop_requested())
			{
				return;
			}
		}
		if (empty_after_pop) ptr->m_empty_condition.notify_all();

		if (token.stop_requested())
		{
			return;
		}

		if (!payload.second.empty())
		{
			switch (payload.first)
			{
			case dedicated_output_target::std_out: 
				(*(ptr->m_wrapped_out)) << payload.second;
				break;
			case dedicated_output_target::std_err: 
				(*(ptr->m_wrapped_err)) << payload.second;
				break;
			case dedicated_output_target::file_out: 
				if (ptr->m_wrapped_filestream.has_value())
				{
					((ptr->m_wrapped_filestream->second)) << payload.second;
				}
				else
				{
					(*(ptr->m_wrapped_out)) << payload.second;
				}
				break;
			}
		}
	}
}

cjm::testing::output_guard::output_proxy::output_proxy(output_proxy&& other) noexcept : std::stringstream{std::move(other)}, m_output_queue{}, m_target{dedicated_output_target::std_out}
{
	std::swap(m_output_queue, other.m_output_queue);
	std::swap(m_target, other.m_target);
}

cjm::testing::output_guard::output_proxy& cjm::testing::output_guard::output_proxy::operator=(
	output_proxy&& other) noexcept
{
	if (this != &other)
	{
		std::stringstream& this_ss = *this;
		std::stringstream& other_ss = other;
		std::swap(this_ss, other_ss);
		std::swap(m_output_queue, other.m_output_queue);
		std::swap(m_target, other.m_target);
	}
	return *this;
}

//if exception is thrown, it is ok that std::terminate be called ... things are totally effed and there
//is no chance of recovery anyway.
cjm::testing::output_guard::output_proxy::~output_proxy()  // NOLINT(bugprone-exception-escape) 
{
	auto shared = lock_or_clear(m_output_queue);
	if (shared)
	{
		switch (m_target)
		{
		case dedicated_output_target::std_out:
			shared->enqueue_output(std::move(*(rdbuf())).str());
			break;
		case dedicated_output_target::std_err:
			shared->enqueue_error(std::move(*(rdbuf())).str());
			break;
		case dedicated_output_target::file_out:
			shared->enqueue_file_output(std::move(*(rdbuf())).str());
			break;
		}
	}
}

cjm::testing::output_guard::output_proxy::output_proxy() : std::stringstream{}, m_output_queue{}, m_target{ dedicated_output_target::std_out } {}

cjm::testing::output_guard::output_proxy::output_proxy(dedicated_output_target target, std::shared_ptr<dedicated_output> queue)
	: m_target{ target }
{
	if (target != dedicated_output_target::std_err && target != dedicated_output_target::std_out && target != dedicated_output_target::file_out)
		throw std::invalid_argument{ "The dedicated output target supplied is not a defined value of the dedicated_output_target enumeration type." };
	if (!queue)
		throw std::invalid_argument{ "The queue pointer supplied is nullptr." };
	m_output_queue = queue;
}

void cjm::testing::output_guard::demo_dedicated_output()
{
	dedicated_output_manager = dedicated_output::make_dedicated_output(std::cout, std::cerr);
	try
	{
		{
			auto thread_vec = std::vector<std::thread>{};
			thread_vec.reserve(6);
			for (int i = 1; i <= 6; ++i)
			{
				thread_vec.emplace_back(std::thread{ write_hi });
			}
			m_start_flag.store(true);
			for (auto& thrd : thread_vec)
			{
				thrd.join();
			}
		}
		m_start_flag.store(false);
	}
	catch (...)
	{
		dedicated_output_manager->clear_queue_and_pause();
		m_start_flag.store(false);
	}
	dedicated_output_manager->clear_queue_and_pause();
	m_start_flag.store(false);
}
