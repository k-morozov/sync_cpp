//
// Created by focus on 4/2/23.
//

#pragma once

#include <concepts>
#include <optional>

namespace sync_cpp::detail {

template <class Self, typename T>
concept ChannelConcept = requires(Self self)
{
	{ self.Send(std::move(T{})) } -> std::same_as<void>;
	{ self.Recv() } -> std::same_as<std::optional<T>>;
	{ self.Close() } -> std::same_as<void>;
};


} // sync_cpp::detail