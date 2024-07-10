#include <iostream>
#include <cassert>
#include "../include/sample.hpp" // Include your cache implementation header

using namespace std;

// Function to test cache operations
void testCacheOperations() {
    const int keySize = 8;
    const int valueSize = 16;

    // Initialize your cache
    cache<string, string> myCache(100); // Adjust capacity as needed

    // Test insertion and retrieval
    myCache.put("key1", "value1");
    assert(myCache.get("key1") == "value1");

    // Test eviction
    myCache.put("key2", "value2");
    // Add more entries to fill the cache
    for (int i = 0; i < 98; ++i) {
        string key = "key" + to_string(i + 3);
        string value = "value" + to_string(i + 3);
        myCache.put(key, value);
    }
    // Now cache is full, inserting another entry should evict the least recently used
    string s = myCache.put("key101", "value101");
    std::cout << s << std::endl;
    assert(myCache.get("key1") == ""); // key1 should have been evicted

    // Test update
    myCache.put("key2", "value2_updated");
    assert(myCache.get("key2") == "value2_updated");

    // Additional tests can be added for edge cases, concurrency (if applicable), etc.
    // Remember to test edge cases like cache overflow, updating existing entries, etc.
}

int main() {
    testCacheOperations();
    cout << "All assertions passed. Cache implementation is correct." << endl;
    return 0;
}
