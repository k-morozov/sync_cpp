//
// Created by focus on 4/3/23.
//

//
// Created by focus on 4/1/23.
//

#pragma once

#include "gtest/gtest.h"

#include <algorithm>
#include <ranges>
#include <thread>

#include <channel.h>

template<typename T>
class BufferedExecutor final {
public:
	explicit BufferedExecutor(const size_t senders_count,
							  const size_t receivers_count,
							  const int buffer_size) :
			chan_(buffer_size),
			senders_count_(senders_count),
			receivers_count_(receivers_count),
			free_buffer_slots_(buffer_size)
	{}
	~BufferedExecutor() { chan_.Close(); }

	void operator()() {
		std::vector<std::vector<T>> send_values(senders_count_);

		for(auto & values : send_values) {
			workers_.emplace_back([&](){
				SendData(values, chan_);
			});
		}

		std::vector<std::vector<T>> recv_values(receivers_count_);
		for (auto & values : recv_values) {
			workers_.emplace_back([&](){
				RecvData(values, chan_);
			});
		}
		std::this_thread::sleep_for(std::chrono::milliseconds{500});
		closed_.store(true);
		chan_.Close();
		workers_.clear();

		CheckResults(send_values, recv_values);
	}
private:
	sync_cpp::buffered_channel<T> chan_;
	std::vector<std::jthread> workers_;

	const size_t senders_count_;
	const size_t receivers_count_;

	std::atomic<int> free_buffer_slots_;

	std::mutex m_;
	std::atomic<bool> closed_ = false;
	std::atomic<size_t> counter_ = 0;

private:
	static std::vector<T> Join(const std::vector<std::vector<T>> & values) {
		auto all = std::views::join(values);
		return {all.begin(), all.end()};
	}

	void CheckResults(const std::vector<std::vector<T>> & senders,
					  const std::vector<std::vector<T>> & receivers) {
		auto all_senders = Join(senders);
		auto all_receivers = Join(receivers);

		std::ranges::sort(all_senders);
		std::ranges::sort(all_receivers);

		ASSERT_TRUE(!all_senders.empty());
		ASSERT_TRUE(all_senders.size() == all_receivers.size()) << " senders="
																<< all_senders.size() << ", receivers=" << all_receivers.size();


		for(size_t i=0; i<all_senders.size(); i++) {
			ASSERT_TRUE(all_senders[i] == all_receivers[i])
										<< " all_senders[" << i << "]=" << all_senders[i] << ", "
										<< " all_receivers[" << i << "]=" << all_receivers[i];
		}
//		ASSERT_TRUE(all_senders == all_receivers);
	}

	void SendData(std::vector<T> & values, sync_cpp::buffered_input_channel<T> & chan) {
		while (true) {
			int value = counter_.fetch_add(1, std::memory_order::relaxed);
			try {
				chan << value;
				{
					std::lock_guard lock(m_);
					ASSERT_TRUE(free_buffer_slots_.fetch_sub(1, std::memory_order_release) >= 0);
				}
				values.push_back(value);
			} catch (const std::runtime_error& ex) {
				{
					std::lock_guard lock(m_);
					ASSERT_TRUE(closed_);
				}
				break;
			}
		}
	}

	void RecvData(std::vector<T> & values, sync_cpp::buffered_output_channel<T> & chan) {
		while (true) {
			free_buffer_slots_.fetch_add(1, std::memory_order_relaxed);
			std::optional<T> value;
			if (chan >> value; value) {
				values.push_back(*value);
			} else {
				{
					assert(value == std::nullopt);
					std::lock_guard lock(m_);
					ASSERT_TRUE(closed_);
				}
				break;
			}
		}
	}
};
