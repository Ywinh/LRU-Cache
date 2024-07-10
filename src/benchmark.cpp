#include <benchmark/benchmark.h>
#include <vector>
#include <string>
#include <random>
#include <cstdint>
#include "../include/lru_cache.hpp"  // 你实现的LRU缓存类

// 生成测试数据的函数
// key is 8 bytes, value is 16 bytes
// num_entries is number of k-v pair
std::vector<std::pair<uint64_t, std::string>> generate_test_data(size_t num_entries) {
    std::vector<std::pair<uint64_t, std::string>> data;
    data.reserve(num_entries); // preallocate
    
    std::mt19937_64 rng; // 一个 64 位的梅森旋转算法随机数生成器
    std::uniform_int_distribution<uint64_t> dist; // 生成均匀分布的 uint64_t 类型随机数
    
    for (size_t i = 0; i < num_entries; ++i) {
        uint64_t key = dist(rng); // 生成一个随机的 64 位无符号整数作为键
        std::string value(16, 'x');  // 16字节的值, 内容为全 x
        data.emplace_back(key, value); // insert to vector
    }


    return data;
}

// put 基准测试
static void BM_LRUCache_Insert(benchmark::State& state) {
    auto test_data = generate_test_data(1000000);  // 生成测试数据
    lru_cache<uint64_t, std::string> cache(1000000);  // 创建缓存

    for (auto _ : state) {
        for (const auto& kv : test_data) {
            // std::cout<<kv.first<<std::endl;
            cache.put(kv.first, kv.second);
        }
    }
}
BENCHMARK(BM_LRUCache_Insert);

// get 基准测试
static void BM_LRUCache_Get(benchmark::State& state) {
    auto test_data = generate_test_data(1000000);  // 生成测试数据
    lru_cache<uint64_t, std::string> cache(1000000);  // 创建缓存

    for (const auto& kv : test_data) {
        cache.put(kv.first, kv.second);
    }

    for (auto _ : state) {
        for (const auto& kv : test_data) {
            auto value = cache.get(kv.first);
            // if(value == ""){
            //     cache.put(kv.first,kv.second);
            // }
            benchmark::DoNotOptimize(value);
        }
    }
}
BENCHMARK(BM_LRUCache_Get);

BENCHMARK_MAIN();
