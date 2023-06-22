//
// Created by focus on 4/3/23.
//

#pragma once

#include "gtest/gtest.h"

#include <algorithm>
#include <ranges>
#include <thread>

#include <channel.h>

template<typename T>
class UnbufferedExecutor final {
public:
	explicit UnbufferedExecutor(const size_t senders_count,
							  const size_t receivers_count) :
			senders_count_(senders_count),
			receivers_count_(receivers_count)
	{}
	~UnbufferedExecutor() { chan_.Close(); }

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
	sync_cpp::unbuffered_channel<T> chan_;
	std::vector<std::jthread> workers_;

	const size_t senders_count_;
	const size_t receivers_count_;

	std::mutex m_;
	std::atomic<bool> closed_ = false;
	std::atomic<size_t> counter_ = 0;

private:
	static std::vector<T> Join(const std::vector<std::vector<T>> & values) {
//		auto all = std::views::join(values);
		std::vector<T> all;
		for (const auto & value : values) {
			for (const auto & v : value) {
				all.push_back(v);
			}
		}
		return {all.begin(), all.end()};
	}

	void CheckResults(const std::vector<std::vector<T>> & senders,
					  const std::vector<std::vector<T>> & receivers) {
		auto all_senders = Join(senders);
		auto all_receivers = Join(receivers);

//		std::ranges::sort(all_senders);
//		std::ranges::sort(all_receivers);

		std::sort(all_senders.begin(), all_senders.end());
		std::sort(all_receivers.begin(), all_receivers.end());

		ASSERT_TRUE(!all_senders.empty());
		ASSERT_TRUE(all_senders.size() == all_receivers.size()) << " senders="
																<< all_senders.size() << ", receivers=" << all_receivers.size();


		for(size_t i=0; i<all_senders.size(); i++) {
			ASSERT_TRUE(all_senders[i] == all_receivers[i])
										<< " all_senders[" << i << "]=" << all_senders[i] << ", "
										<< " all_receivers[" << i << "]=" << all_receivers[i];
		}
	}

	void SendData(std::vector<T> & values, sync_cpp::unbuffered_input_channel<T> & chan) {
		while (true) {
			int value = counter_.fetch_add(1, std::memory_order::relaxed);
			try {
				chan << value;
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

	void RecvData(std::vector<T> & values, sync_cpp::unbuffered_output_channel<T> & chan) {
		while (true) {
			std::optional<T> value;
			if (chan >> value; value) {
				values.push_back(*value);
			} else {
				break;
			}
		}
	}
};
