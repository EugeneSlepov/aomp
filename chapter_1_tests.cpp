#include <gtest/gtest.h>

#include "universe.h"

TEST(chapter_1_tests, exc_1) {
    using namespace std;

    struct table_slot {
        using chopstick_type = shared_ptr<mutex>;

        table_slot(const chopstick_type& left, const chopstick_type& right) :
            left_{left}, right_{right} {}

        [[nodiscard]] bool acquire() const {
            sync_print("Attempt to lock by philosopher ", this_thread::get_id(), "\n");
            unique_lock g1{*left_, defer_lock_t{}};
            if (g1.try_lock()) {
                unique_lock g2{*right_, defer_lock_t{}};
                if (g2.try_lock()) {
                    sync_print("Philosopher ", this_thread::get_id(), " successfully locked the slot\n");
                    return true;
                }
            }
            return false;
        }
    private:
        chopstick_type left_;
        chopstick_type right_;
    };

    auto chopstick_1 = make_shared<mutex>(), chopstick_2 = make_shared<mutex>(), chopstick_3 = make_shared<mutex>(),
         chopstick_4 = make_shared<mutex>(), chopstick_5 = make_shared<mutex>();
    auto constexpr table_size = 5;
    array<table_slot, table_size> table = {table_slot{chopstick_1, chopstick_2}, table_slot{chopstick_3, chopstick_1},
                                           table_slot{chopstick_4, chopstick_3}, table_slot{chopstick_5, chopstick_4},
                                           table_slot{chopstick_2, chopstick_5}};

    struct philosophers_stat {
        unsigned dinning_count_ = 0u;
    };
    array<thread, table_size> philosophers;
    array<philosophers_stat, table_size> stats;
    atomic_bool keep_going = true;
    for (auto i = 0; i < table_size; ++i) {
        philosophers[i] = thread{[i, &keep_going, &table, &stats] {
            while (keep_going) {
                this_thread::sleep_for(chrono::milliseconds{random(1, 100)});
                if (table[i].acquire()) {
                    ++stats[i].dinning_count_;
                }
            }
            atomic_thread_fence(memory_order_release);
        }};
    }

    for (auto intervals = 0; intervals < 10; ++intervals) {
        this_thread::sleep_for(chrono::milliseconds{1000});
    }

    keep_going = false;
    for (auto i = 0; i < table_size; ++i) {
        philosophers[i].join();
    }

    atomic_thread_fence(memory_order_acquire);
    auto total = 0u;
    for (auto& stat : stats) {
        total += stat.dinning_count_;
    }
    auto average = total / table_size;
    print("Average is ", average, "\n");

    auto max_dev = 0u;
    for (auto& stat : stats) {
        auto candidate_max_dev = static_cast<unsigned>(fabs(double(stat.dinning_count_) - average) / average * 100);
        if (candidate_max_dev > max_dev) {
            max_dev = candidate_max_dev;
        }
    }
    print("The highest deviation is ", max_dev, "%\n");
}
