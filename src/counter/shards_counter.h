//
// Created by focus on 4/9/23.
//

#pragma once

#include <array>
#include <atomic>
#include <thread>

namespace sync_cpp {

template<size_t N>
class ShardsCounter final {
public:
	void Increment() {
		shards_[GetShard()].Increment();
	}

	[[nodiscard]]
	size_t Get() const {
		size_t result{};
		for(const auto& shard : shards_) {
			result += shard.Get();
		}
		return result;
	}

private:
	class alignas(64) Shard final {
	public:
		void Increment() {
			value.fetch_add(1);
		}

		[[nodiscard]]
		size_t Get() const {
			return value.load();
		}
	private:
		std::atomic<size_t> value{0};
	};

	std::array<Shard, N> shards_;

	static size_t GetShard() {
		static std::hash<std::thread::id> hasher;
		auto id = std::this_thread::get_id();
		return hasher(id) % N;
	}
};

}