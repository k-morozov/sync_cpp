//
// Created by focus on 4/2/23.
//

#pragma once

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <optional>
#include <queue>

namespace sync_cpp::detail {

template <typename T>
class ChannelPolicy {
public:
	virtual void Send(T&& value) = 0;
	virtual std::optional<T> Recv() = 0;
	virtual ~ChannelPolicy() = default;
};

} // sync_cpp::detail