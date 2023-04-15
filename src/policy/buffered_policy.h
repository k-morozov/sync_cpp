//
// Created by focus on 4/1/23.
//

#pragma once

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <optional>
#include <queue>

#include <policy/channel_policy.h>

namespace sync_cpp::detail {

template <class T>
class BufferedPolicy : public ChannelPolicy<T> {
public:
	explicit BufferedPolicy(const size_t size)
		: max_size_(size) {}

	void Send(T&& value) final;

	std::optional<T> Recv() final;

	void Close() {
		is_closed_.store(true);
		full_data_.notify_all();
		empty_data_.notify_all();
	}

private:
	const size_t max_size_;
	std::queue<T> buffer_;

	std::mutex m_;
	std::condition_variable full_data_;
	std::condition_variable empty_data_;

	std::atomic<bool> is_closed_{false};
};

template<class T>
void BufferedPolicy<T>::Send(T &&value) {
	std::unique_lock lock(m_);

	while(buffer_.size() == max_size_) {
		full_data_.wait(lock);
	}
	assert(buffer_.size() < max_size_);

	if (is_closed_.load()) {
		throw std::runtime_error("failed write to closed channel");
	}

	buffer_.push(std::move(value));

	empty_data_.notify_one();
	full_data_.notify_one();
}

template<class T>
std::optional<T> BufferedPolicy<T>::Recv() {
	std::unique_lock lock(m_);

	while(buffer_.empty() && !is_closed_ ) {
		empty_data_.wait(lock);
	}
	if (!buffer_.empty()) {
		std::optional<T> element = std::move(buffer_.front());
		buffer_.pop();

		full_data_.notify_one();
		empty_data_.notify_one();

		return element;
	}

	return {};
}

} // sync_cpp::detail