#pragma once
#include <chrono>
#include <utility>

template<typename Func>
auto MeasureExecutionTimeWithResult(Func&& func)
{
    using clock = std::chrono::high_resolution_clock;

    auto start = clock::now();
    auto result = func();
    auto end = clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;
    return std::make_pair(result, elapsed.count());
}