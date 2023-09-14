#pragma once

#include <atomic>
#include <exception>
#include <memory>
#include <thread>

#include "hp.h"

namespace sync_cpp {

class hp_owner {
	HazardPtr* hp;

    public:
        hp_owner() : hp(nullptr) {
            for(auto & hazard_ptr : hazard_ptrs) {
                std::thread::id old_id;

                if (hazard_ptr.id.compare_exchange_strong(old_id, std::this_thread::get_id())) {
                    hp = &hazard_ptr;
                    break;
                }
            }

            if (!hp) {
                throw std::runtime_error("No hazard pointers available");
            }
        };

        hp_owner(const hp_owner& p) = delete;
        hp_owner& operator=(const hp_owner& p) = delete;

        std::atomic<void*>& get_pointer() { return hp->ptr; }

        ~hp_owner() {
            hp->ptr.store(nullptr);
            hp->id.store(std::this_thread::get_id());
        }
};


} // namespace sync_cpp