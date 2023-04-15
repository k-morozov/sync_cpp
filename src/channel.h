//
// Created by focus on 4/1/23.
//

#pragma once

#include <detail/channel_base.h>
#include <detail/input_channel.h>
#include <detail/output_channel.h>
#include <policy/buffered_policy.h>
#include <policy/unbuffered_policy.h>
#include <policy/buffered_mpmc_policy.h>

namespace sync_cpp {

template<typename T>
using buffered_input_channel = detail::InputChannel<T, detail::ChannelBase<T, detail::BufferedPolicy>>;

template<typename T>
using buffered_output_channel = detail::OutputChannel<T, detail::ChannelBase<T, detail::BufferedPolicy>>;

template<typename T>
using buffered_channel = detail::ChannelBase<T, detail::BufferedPolicy>;

template<typename T>
using unbuffered_input_channel = detail::InputChannel<T, detail::ChannelBase<T, detail::UnbufferedPolicy>>;

template<typename T>
using unbuffered_output_channel = detail::OutputChannel<T, detail::ChannelBase<T, detail::UnbufferedPolicy>>;

template<typename T>
using unbuffered_channel = detail::ChannelBase<T, detail::UnbufferedPolicy>;

template<typename T>
using buffered_mpmc_input_channel = detail::InputChannel<T, detail::ChannelBase<T, detail::BufferedMPMCPolicy>>;

template<typename T>
using buffered_mpmc_output_channel = detail::OutputChannel<T, detail::ChannelBase<T, detail::BufferedMPMCPolicy>>;

template<typename T>
using buffered_mpmc_channel = detail::ChannelBase<T, detail::BufferedMPMCPolicy>;

} // namespace sync_cpp
