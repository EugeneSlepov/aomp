#include <gtest/gtest.h>

#include "universe.h"

TEST(chapter_1_tests, exc_1) {
    using namespace std;

    struct table_slot {
        using chopstick_type = shared_ptr<mutex>;

        table_slot(const chopstick_type& left, const chopstick_type& right) :
            left_{left}, right_{right} {}

        void eat() const {
            sync_print("Attempt to lock philosopher ", this_thread::get_id(), "\n");
            unique_lock g1{*left_, defer_lock_t{}};
            if (g1.try_lock()) {
                unique_lock g2{*right_, defer_lock_t{}};
                if (g2.try_lock()) {
                    sync_print("Philosopher ", this_thread::get_id(), " successfully locked\n");
                }
            }
        }
    private:
        mutable chopstick_type left_;
        mutable chopstick_type right_;
    };

    auto chopstick_1 = make_shared<mutex>(), chopstick_2 = make_shared<mutex>(), chopstick_3 = make_shared<mutex>(),
         chopstick_4 = make_shared<mutex>(), chopstick_5 = make_shared<mutex>();
    auto constexpr table_size = 5;
    array<table_slot, table_size> table = {table_slot{chopstick_1, chopstick_2}, table_slot{chopstick_3, chopstick_1},
                                           table_slot{chopstick_4, chopstick_3}, table_slot{chopstick_5, chopstick_4},
                                           table_slot{chopstick_2, chopstick_5}};
    array<thread, table_size> philosophers;
    atomic_bool keep_going = true;
    for (auto i = 0; i < table_size; ++i) {
        philosophers[i] = thread{[i, &keep_going, &table] {
            while (keep_going) {
                this_thread::sleep_for(chrono::milliseconds{random(5, 25)});
                table[i].eat();
            }
        }};
    }

    for (auto intervals = 0; intervals < 10; ++intervals) {
        this_thread::sleep_for(chrono::milliseconds{50});
    }

    keep_going = false;
    for (auto i = 0; i < table_size; ++i) {
        philosophers[i].join();
    }
}
