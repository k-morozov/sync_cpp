//
// Created by focus on 4/3/23.
//

#include "gtest/gtest.h"

#include <thread>

#include <mutex/mutex.h>

#include "event.h"

using namespace std::chrono_literals;

class TestMutex : public ::testing::Test {
public:
};

TEST_F(TestMutex, lock_unlock) {
	sync_cpp::Mutex m;

	m.lock();
	m.unlock();
}

TEST_F(TestMutex, lock_unlock_twice) {
	sync_cpp::Mutex m;

	m.lock();
	m.unlock();

	m.lock();
	m.unlock();
}

TEST_F(TestMutex, mutual_exclusion) {
	sync_cpp::Mutex m;
	int value = 100;

	std::jthread th([&m, &value] {
		m.lock();
		value = 750;
		std::this_thread::sleep_for(3s);
		value = 510;
		m.unlock();
	});

	std::this_thread::sleep_for(1s);
	m.lock();
	ASSERT_TRUE(value == 510);

	m.unlock();
}

TEST_F(TestMutex, dead_after_unlock) {
	auto event = new test_space::Event{};

	std::jthread th([event]{
		event->done();
	});

	event->wait();
	delete event;
}