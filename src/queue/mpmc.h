//
// Created by focus on 4/3/23.
//

#pragma once

#include <atomic>
#include <unistd.h>
#include <vector>

namespace sync_cpp::queue {

template<typename T>
class MPMCUnboundedQueue final {
public:
	explicit MPMCUnboundedQueue(const size_t size)
	: data_(size), max_size_(size) {
		for(size_t i=0; i<max_size_; i++) {
			data_[i].generation.store(i, std::memory_order_relaxed);
		}
	}

	bool Enqueue(const T& value) {
		for(;;) {
			auto insert_generation = insert_generation_.load(std::memory_order_relaxed);
			Block & block = data_[insert_generation & (max_size_ - 1)];

			const int64_t diff = block.generation - insert_generation;

			if (diff == 0) {
				if (insert_generation_.compare_exchange_weak(insert_generation, insert_generation + 1)) {
					block.value = value;
					block.generation.store(insert_generation + 1);
					return true;
				}
			} else if (diff < 0) {
				// buffer full
				return false;
			}
		}
	}

	bool Dequeue(T& data) {
		for(;;) {
			auto drop_generation = drop_generation_.load(std::memory_order_relaxed);
			Block & block = data_[drop_generation & (max_size_ - 1)];

			const int64_t diff = block.generation - (drop_generation + 1);
			if (diff == 0) {
				if (drop_generation_.compare_exchange_weak(drop_generation, drop_generation + 1)) {
					data = std::move(block.value);
					block.generation.store(drop_generation + max_size_);
					return true;
				}
			} else if (diff < 0) {
				// buffer full
				return false;
			}
		}
	}
private:
	struct alignas(64) Block {
		T value;
		std::atomic<size_t> generation;
	};

private:
	std::vector<Block> data_;
	const size_t max_size_;
	std::atomic<size_t> insert_generation_{0};
	std::atomic<size_t> drop_generation_{0};
};

} // namespace sync_cpp::queue