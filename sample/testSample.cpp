#include <iostream>
#include <cassert>
#include "../include/sample.hpp"

void testInsertAndGet() {
    cache<std::string, std::string> myCache(10);

    // 测试插入和获取
    myCache.put("key1", "value1");
    assert(myCache.get("key1") == "value1");

    myCache.put("key2", "value2");
    assert(myCache.get("key2") == "value2");

    // 测试不存在的键
    assert(myCache.get("key3") == "");
}

void testEviction() {
    cache<std::string, std::string> myCache(10);

    // 插入10个键值对
    for (int i = 1; i <= 10; ++i) {
        myCache.put("key" + std::to_string(i), "value" + std::to_string(i));
    }

    // 缓存应满
    assert(myCache.full());

    // 超过容量，插入第11个键值对
    myCache.put("key11", "value11");

    // 至少一个键应被淘汰，缓存应满
    assert(myCache.full());

    // 检查缓存是否仍然包含某些键值对（可能会因为随机性导致不同的键被淘汰）
    int existingKeys = 0;
    for (int i = 1; i <= 11; ++i) {
        existingKeys += (myCache.get("key" + std::to_string(i)) != "") ? 1 : 0;
    }
    assert(existingKeys == 10);
}

void testLRUProperty() {
    cache<std::string, std::string> myCache(10);

    // 插入10个键值对
    for (int i = 1; i <= 10; ++i) {
        myCache.put("key" + std::to_string(i), "value" + std::to_string(i));
    }

    // 访问 key1 和 key2 以更新其时间戳
    myCache.get("key1");
    myCache.get("key2");

    // 插入第11个键值对，最近最少使用的键应被淘汰
    myCache.put("key11", "value11");

    // 检查 key1 和 key2 是否仍然存在
    assert(myCache.get("key1") == "value1");
    assert(myCache.get("key2") == "value2");

    // 检查是否淘汰了某个键
    int evicted = 0;
    for (int i = 3; i <= 10; ++i) {
        if (myCache.get("key" + std::to_string(i)) == "") {
            evicted++;
        }
    }
    assert(evicted == 1); // 只有一个键应该被淘汰

    // 检查 key11 是否存在
    assert(myCache.get("key11") == "value11");
}

void testRandomEviction() {
    cache<std::string, std::string> myCache(10);

    // 插入20个键值对
    for (int i = 0; i < 20; ++i) {
        myCache.put("key" + std::to_string(i), "value" + std::to_string(i));
    }

    // 检查缓存是否包含的键数不超过容量
    int count = 0;
    for (int i = 0; i < 20; ++i) {
        if (myCache.get("key" + std::to_string(i)) != "") {
            ++count;
        }
    }
    std::cout<<count<<std::endl;
    assert(count <= 10);
}


int main(){
    cache<int,int> test(10);
    test.put(1,1);
    test.put(2,2);
    std::cout << test.gettime(1) << std::endl;
    std::cout << test.gettime(2) << std::endl;
    std::cout << test.gettime(2) << std::endl;
    // test.printPool();

    testInsertAndGet();
    testEviction();
    testLRUProperty();
    testRandomEviction();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}