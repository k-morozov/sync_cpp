//
// Created by focus on 4/1/23.
//

#pragma once

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <optional>

#include <policy/channel_policy.h>
#include <queue/mpmc.h>

namespace sync_cpp::detail {

template <class T>
class BufferedMPMCPolicy : public ChannelPolicy<T> {
public:
	explicit BufferedMPMCPolicy(const size_t size)
		: max_size_(size), buffer_(max_size_) {
		assert(max_size_ % 2 == 0);
	}

	void Send(T&& value) final;

	std::optional<T> Recv() final;

	void Close() {
		is_closed_.store(true);
		full_data_.notify_all();
		empty_data_.notify_all();
	}

private:
	const size_t max_size_;
	queue::MPMCUnboundedQueue<T> buffer_;

	std::mutex m_;
	std::condition_variable full_data_;
	std::condition_variable empty_data_;

	std::atomic<bool> is_closed_{false};
};

template<class T>
void BufferedMPMCPolicy<T>::Send(T &&value) {
	if (is_closed_.load()) {
		throw std::runtime_error("failed write to closed channel");
	}
	while (!buffer_.Enqueue(value)) {
		std::unique_lock lock(m_);
		full_data_.wait(lock);
		if (is_closed_.load()) {
			throw std::runtime_error("failed write to closed channel");
		}
	}
	empty_data_.notify_one();
	full_data_.notify_one();
}

template<class T>
std::optional<T> BufferedMPMCPolicy<T>::Recv() {
	T val;
	bool status{};
	while ((status = buffer_.Dequeue(val)), !status && !is_closed_) {
		std::unique_lock lock(m_);
		empty_data_.wait(lock);
	}
	std::optional<T> result;
	if (status) {
		result.emplace(std::move(val));
		full_data_.notify_one();
		empty_data_.notify_one();
	}

	return result;
}

} // sync_cpp::detail