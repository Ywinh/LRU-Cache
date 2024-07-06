#include <benchmark/benchmark.h>
#include <vector>
#include <string>
#include <random>
#include <thread>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "../include/lru_cache.hpp"  // 假设这是你提供的头文件

const size_t KEY_SIZE = 8;
const size_t VALUE_SIZE = 16;
std::string path = "/root/LRU_cache/src/train_2D.csv"; // 我这里使用绝对路径，不太好 TODO: 相对路径报错

// 从文件读取测试数据
std::vector<std::pair<std::string, std::string>> load_test_data(const std::string& file_path) {
    std::vector<std::pair<std::string, std::string>> data;
    std::ifstream infile(file_path);
    std::string line;
    // std::cout<< file_path <<std::endl;

    // 跳过第一行
    std::getline(infile, line);

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, ',') && std::getline(iss, value, ',')) {
            // 保留key的前两位小数，确保长度为8字节
            double key_val = std::stod(key);
            std::ostringstream key_stream;
            key_stream << std::fixed << std::setprecision(2) << key_val;
            std::string formatted_key = key_stream.str();
            formatted_key.resize(KEY_SIZE, '0');  // 填充或截断至8字节

            // 确保值长度为16字节
            value.resize(VALUE_SIZE, 'x');  // 填充或截断至16字节

            data.emplace_back(formatted_key, value);
        }
    }
    std::cout<<"data size is " << data.size() << std::endl;
    return data;
}

// 全局静态变量，确保数据只加载一次
static std::vector<std::pair<std::string, std::string>> data = load_test_data(path);

static void BM_LRUCache_Insert(benchmark::State& state) {
    lru_cache<std::string, std::string> cache(state.range(0));  // LRU缓存的构造函数接受缓存大小

    for (auto _ : state) {
        // 运行多线程插入
        std::vector<std::thread> threads;
        size_t num_threads = state.threads();
        size_t entries_per_thread = data.size() / num_threads;

        for (size_t t = 0; t < num_threads; ++t) {
            threads.emplace_back([&, t]() {
                for (size_t i = t * entries_per_thread; i < (t + 1) * entries_per_thread; ++i) {
                    cache.put(data[i].first, data[i].second);
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }
}

static void BM_LRUCache_Get(benchmark::State& state) {
    lru_cache<std::string, std::string> cache(state.range(0));  // 假设LRU缓存的构造函数接受缓存大小

    // 预先插入数据
    for (const auto& kv : data) {
        cache.put(kv.first, kv.second);
    }

    for (auto _ : state) {
        // 运行多线程获取
        std::vector<std::thread> threads;
        size_t num_threads = state.threads();
        size_t entries_per_thread = data.size() / num_threads;

        for (size_t t = 0; t < num_threads; ++t) {
            threads.emplace_back([&, t]() {
                for (size_t i = t * entries_per_thread; i < (t + 1) * entries_per_thread; ++i) {
                    cache.get(data[i].first);
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }
}

static void BM_LRUCache_Mixed(benchmark::State& state) {
    // static auto data = load_test_data(path);
    lru_cache<std::string, std::string> cache(state.range(0));  // LRU缓存的构造函数接受缓存大小

    // 预热阶段：插入一部分数据
    // 最多有8个线程，200000 / 8 / 10 = 2500
    size_t warmup_size = 2000;
    for (size_t i = 0; i < warmup_size; ++i) {
        cache.put(data[i].first, data[i].second);
    }

    for (auto _ : state) {
        // 运行多线程混合操作
        std::vector<std::thread> threads;
        size_t num_threads = state.threads();
        size_t entries_per_thread = data.size() / num_threads;

        for (size_t t = 0; t < num_threads; ++t) {
            threads.emplace_back([&, t]() {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, 1);  // 0: get, 1: put
                for (size_t i = t * entries_per_thread; i < (t + 1) * entries_per_thread; ++i) {
                    if (dis(gen) == 0) {
                        // get 操作
                        if (cache.get(data[i].first) != ""){
                            // std::cout << "hit!" << std::endl;
                        }else{
                            // std::cout << "miss!" << std::endl;
                        }
                        
                    } else {
                        // put 操作
                        cache.put(data[i].first, data[i].second);
                    }
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }
}



// 注册基准测试
BENCHMARK(BM_LRUCache_Insert)->Range(8, 8<<10)->Threads(1)->Threads(2)->Threads(4)->Threads(8);
BENCHMARK(BM_LRUCache_Get)->Range(8, 8<<10)->Threads(1)->Threads(2)->Threads(4)->Threads(8);
BENCHMARK(BM_LRUCache_Mixed)->Range(8, 8 << 10)->Threads(1)->Threads(2)->Threads(4)->Threads(8);

BENCHMARK_MAIN();