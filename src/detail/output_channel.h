//
// Created by focus on 4/2/23.
//

#pragma once

#include <optional>

namespace sync_cpp::detail {

template<typename T, class Derived>
class OutputChannel {
public:
	Derived & operator>>(std::optional<T>& value) {
		value = self()->Recv();
		return *self();
	}

private:
	Derived *self() { return static_cast<Derived *>(this); }
};

} // namespace sync_cpp::detail