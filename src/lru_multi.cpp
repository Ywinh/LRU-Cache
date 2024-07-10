#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <cassert>
#include "../include/lru_cache.hpp"  // 假设这是您提供的头文件

const int num_threads = 4;  // 定义测试的线程数
const int operations_per_thread = 1000;  // 每个线程的操作次数

// 每个线程执行的函数
void thread_function(lru_cache<int, int>& cache, std::atomic<int>& counter, std::atomic<bool>& stop_flag) {
    int thread_id = counter.fetch_add(1);
    for (int i = thread_id * operations_per_thread; i < (thread_id + 1) * operations_per_thread; ++i) {
        // 插入到缓存
        cache.put(i, i);
        assert(cache.get(i) == i);  // 从缓存中获取，并检查结果是否正确
    }
    
    // 继续进行一些随机操作，直到收到停止信号
    while (!stop_flag.load()) {
        int key = rand() % (num_threads * operations_per_thread);
        cache.put(key, key);
        assert(cache.get(key) == key);
    }
}

int test_multithread() {
    lru_cache<int, int> cache(1000);  // 初始化容量为 1000 的缓存
    std::atomic<int> counter(0);  // 原子计数器，用于分配给线程的键
    std::atomic<bool> stop_flag(false);  // 原子标志，用于通知线程停止

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(thread_function, std::ref(cache), std::ref(counter), std::ref(stop_flag));
    }

    // 让线程运行一段时间, 是否有问题？
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 通知线程停止
    stop_flag.store(true);

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

// int main() {
//     test_multithread();
//     return 0;
// }
