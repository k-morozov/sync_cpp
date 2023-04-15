//
// Created by focus on 4/1/23.
//

#pragma once

#include <optional>
#include <type_traits>

#include <detail/input_channel.h>
#include <detail/output_channel.h>
#include <policy/fwd.h>

namespace sync_cpp::detail {

template <class T, template<class> class Policy>
requires std::derived_from<Policy<T>, ChannelPolicy<T>>
class ChannelBase final :
		public InputChannel<T, ChannelBase<T, Policy>>,
		public OutputChannel<T, ChannelBase<T, Policy>>,
		protected Policy<T> {
public:
	using Policy<T>::Close;
	friend InputChannel<T, ChannelBase<T, Policy>>;
	friend OutputChannel<T, ChannelBase<T, Policy>>;

	using input_channel_type = InputChannel<T, ChannelBase<T, Policy>>;
	using output_channel_type = OutputChannel<T, ChannelBase<T, Policy>>;

	template<typename U = T>
	requires std::is_same_v<Policy<U>, BufferedPolicy<U>> || std::is_same_v<Policy<U>, BufferedMPMCPolicy<U>>
	explicit ChannelBase(const size_t size)
		: Policy<T>(size) {}

	template<typename U = T>
	requires std::is_same_v<Policy<U>, UnbufferedPolicy<U>>
	ChannelBase() : Policy<T>() {}

	~ChannelBase() override {
		Policy<T>::Close();
	}
};

} // namespace sync_cpp::detail
