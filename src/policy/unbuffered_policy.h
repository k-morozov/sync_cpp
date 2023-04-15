//
// Created by focus on 4/3/23.
//

#pragma once

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

#include <policy/channel_policy.h>

namespace sync_cpp::detail {

template <class T>
class UnbufferedPolicy : public ChannelPolicy<T> {
public:
	UnbufferedPolicy() = default;

	void Send(T&& value) final;

	std::optional<T> Recv() final;

	void Close() {
		closed_.store(true);
		has_value_.notify_all();
		no_value_.notify_all();
	}

private:
	std::optional<T> value_;
	std::mutex m_;
	std::condition_variable has_value_;
	std::condition_variable no_value_;

	std::atomic<bool> closed_{};
};

template<class T>
void UnbufferedPolicy<T>::Send(T && value) {
	std::unique_lock lock(m_);

	while (value_.has_value()) {
		no_value_.wait(lock);
	}
	if (closed_.load()) {
		throw std::runtime_error("failed write to closed channel");
	}

	value_ = std::move(value);
	has_value_.notify_one();
}

template<class T>
std::optional<T> UnbufferedPolicy<T>::Recv() {
	std::unique_lock lock(m_);

	while (!value_.has_value() && !closed_) {
		has_value_.wait(lock);
	}

	std::optional<T> result;
	if (value_.has_value()) {
		result = std::move(value_);
		value_.reset();
	}
	no_value_.notify_one();
	has_value_.notify_one();

	return result;
}

} // sync_cpp::detail
