#pragma once

#include <atomic>
#include <exception>
#include <memory>
#include <thread>

namespace sync_cpp {

static constexpr size_t MaxHazardPointers = 100;
struct HazardPtr
{
    std::atomic<std::thread::id> id;
    std::atomic<void *> ptr;
};

HazardPtr hazard_ptrs[MaxHazardPointers];

template<typename T>
void do_delete(void* p) {
    delete static_cast<T*>(p);
}


class hp_owner {
    public:
        hp_owner() : hp(nullptr) {
            for(size_t i=0; i<MaxHazardPointers; i++) {
                std::thread::id old_id;

                if (hazard_ptrs[i].id.compare_exchange_strong(old_id, std::this_thread::get_id())) {
                    hp = &hazard_ptrs[i];
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


    private:
        HazardPtr* hp;
};


} // namespace sync_cpp