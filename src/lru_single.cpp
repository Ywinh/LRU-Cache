#include <iostream>
#include <cassert>
#include "../include/lru_cache.hpp"

void test_basic_operations() {
    lru_cache<int, std::string> cache(3);

    // 插入一些键值对
    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");

    // 检查缓存中的值
    assert(cache.get(1) == "one");
    assert(cache.get(2) == "two");
    assert(cache.get(3) == "three");

    // 插入第四个键值对，应该驱逐第一个键值对
    cache.put(4, "four");

    // 检查第一个键值对是否被驱逐，返回默认值
    assert(cache.get(1) == ""); // 默认值是空字符串

    // 检查剩余的键值对
    assert(cache.get(2) == "two");
    assert(cache.get(3) == "three");
    assert(cache.get(4) == "four");

    // 再插入一个键值对
    cache.put(5, "five");

    // 检查第二个键值对是否被驱逐，返回默认值
    assert(cache.get(2) == ""); // 默认值是空字符串

    // 检查剩余的键值对
    assert(cache.get(3) == "three");
    assert(cache.get(4) == "four");
    assert(cache.get(5) == "five");

    std::cout << "Basic operations test passed!\n";
}

void test_update_value() {
    lru_cache<int, std::string> cache(2);

    // 插入一个键值对
    cache.put(1, "one");
    assert(cache.get(1) == "one");

    // 更新值
    cache.put(1, "uno");
    assert(cache.get(1) == "uno");

    std::cout << "Update value test passed!\n";
}

void test_edge_cases() {
    lru_cache<int, std::string> cache(1);

    // 插入一个键值对
    cache.put(1, "one");
    assert(cache.get(1) == "one");

    // 插入第二个键值对，应该驱逐第一个键值对
    cache.put(2, "two");
    assert(cache.get(1) == ""); // 默认值是空字符串
    assert(cache.get(2) == "two");

    std::cout << "Edge cases test passed!\n";
}

void test_large_number_of_operations() {
    lru_cache<int, std::string> cache(100);

    for (int i = 0; i < 1000; ++i) {
        cache.put(i, "value" + std::to_string(i));
    }

    // 检查最后100个键值对是否在缓存中
    for (int i = 900; i < 1000; ++i) {
        assert(cache.get(i) == "value" + std::to_string(i));
    }

    // 检查第一个900个键值对是否被驱逐
    for (int i = 0; i < 900; ++i) {
        assert(cache.get(i) == ""); // 默认值是空字符串
    }

    std::cout << "Large number of operations test passed!\n";
}


// int main() {
//     test_basic_operations();
//     test_update_value();
//     test_edge_cases();
//     test_large_number_of_operations();

//     std::cout << "All tests passed!\n";
//     return 0;
// }
