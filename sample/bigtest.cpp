#include <iostream>
#include <thread>
#include <chrono>
#include "../include/sample.hpp"

// 生成随机字符串
std::string generateRandomString(size_t length) {
    const char charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);
    std::string result;
    result.resize(length);
    for (size_t i = 0; i < length; ++i) {
        result[i] = charset[dist(rng)];
    }
    return result;
}

// 压力测试函数
void stressTest(cache<std::string, std::string>& cache) {
    const int test_size = 1000000;
    std::vector<std::string> keys;
    keys.reserve(test_size);

    // 生成测试数据
    for (int i = 0; i < test_size; ++i) {
        keys.push_back(generateRandomString(8));
    }

    // 插入数据
    auto start_time = std::chrono::high_resolution_clock::now();
    for (const auto& key : keys) {
        cache.put(key, generateRandomString(16));
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> insert_duration = end_time - start_time;
    std::cout << "Insertion completed in " << insert_duration.count() << " seconds.\n";

    // 随机读取数据
    start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < test_size; ++i) {
        cache.get(keys[rand() % test_size]);
    }
    end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> read_duration = end_time - start_time;
    std::cout << "Random reads completed in " << read_duration.count() << " seconds.\n";
}

int main() {
    cache<std::string, std::string> myCache;

    // 单线程测试
    stressTest(myCache);

    // // 多线程测试
    // const int num_threads = 4;
    // std::vector<std::thread> threads;
    // for (int i = 0; i < num_threads; ++i) {
    //     threads.emplace_back(stressTest, std::ref(myCache));
    // }
    // for (auto& t : threads) {
    //     t.join();
    // }

    return 0;
}
