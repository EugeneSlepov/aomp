#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <random>
#include <sstream>
#include <thread>
#include <vector>

inline int random(int min = std::numeric_limits<int>::min(), int max = std::numeric_limits<int>::max()) {
    using namespace std;

    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution distribution{min, max};
    return distribution(gen);
}

template<typename... Ts>
void print(const Ts&... args) {
    (std::cout << ... << args);
}

template<typename... Ts>
void sync_print(const Ts&... args) {
    using namespace std;

    static mutex m;
    lock_guard g(m);
    print(args...);
}