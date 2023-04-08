//
// Created by focus on 4/8/23.
//

#pragma once

#include <memory>

namespace sync_cpp {

class Mutex {
public:
	Mutex();
	~Mutex();

	void lock();
	void unlock();
private:
	class mutex_impl;
	std::unique_ptr<mutex_impl> impl_;
};

} // namespace sync_cpp
