#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <cassert>
#include "lru_cache.hpp"  // 假设这是您提供的头文件

const int num_threads = 4;  // 定义测试的线程数
const int operations_per_thread = 1000;  // 每个线程的操作次数

// 每个线程执行的函数
void thread_function(lru_cache<int, int>& cache, std::atomic<int>& counter) {
    int thread_id = counter.fetch_add(1);
    for (int i = thread_id * operations_per_thread; i < (thread_id + 1) * operations_per_thread; ++i) {
        cache.put(i, i);  // 插入到缓存
        assert(cache.get(i) == i);  // 从缓存中获取，并检查结果是否正确
    }
}

int main() {
    lru_cache<int, int> cache(1000);  // 初始化容量为 1000 的缓存
    std::atomic<int> counter(0);  // 原子计数器，用于分配给线程的键

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(thread_function, std::ref(cache), std::ref(counter));
    }

    // 等待所有线程执行完毕
    for (auto& thread : threads) {
        thread.join();
    }

    // 检查缓存的最终状态
    int size = cache.size();
    std::cout << "多线程操作后的缓存大小: " << size << std::endl;

    // 进行断言检查
    assert(size <= 1000);  // 缓存大小不应超过容量限制

    std::cout << "测试通过！" << std::endl;

    return 0;
}
