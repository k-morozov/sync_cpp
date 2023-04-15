//
// Created by focus on 4/2/23.
//

#pragma once

namespace sync_cpp::detail {

template<typename T, class Derived>
class InputChannel {
public:
	template<typename U>
	Derived &operator<<(U value) {
		self()->Send(std::move(value));
		return *self();
	}

private:
	Derived *self() { return static_cast<Derived *>(this); }
};

} // namespace sync_cpp::detail
