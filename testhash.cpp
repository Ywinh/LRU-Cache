#include <iostream>
#include <vector>
#include "build/install/include/libcuckoo/cuckoohash_map.hh"

template<typename KeyType>
class test{
public:
    static std::vector<KeyType> v;

public:
    void insert(int x){
        v.push_back(x);
    }

    void pv(){
        for(auto t: v){
            std::cout << t << std::endl;
        }
    }

};

template<typename KeyType>
class evictionPoolEntry {
public:
    unsigned int time;    /* Object idle time (inverse frequency for LFU) */
    KeyType key;                   // 键名
    // int slot;                   /* Slot. */

    //使用缺省构造函数，后续自己来new
};

template<typename KeyType> 
static evictionPoolEntry<KeyType> *EvictionPoolLRU = nullptr;

template <typename KeyType> std::vector<KeyType> test<KeyType>::v;

int main() {

    test<int> a;
    a.insert(10);
    a.pv();

    // // 创建哈希表并插入一些元素
    // libcuckoo::cuckoohash_map<int, int> Table;
    // for (int i = 1; i <= 10; ++i) {
    //     Table.insert(i, i * 10);
    // }

    // // 多次遍历哈希表
    // const int num_iterations = 5; // 遍历次数
    // std::vector<std::vector<std::pair<int, int>>> iterations(num_iterations);


    // // for (int n = 0; n < num_iterations; ++n) {
    // //     for (const auto &item : Table.lock_table()) {
    // //         iterations[n].emplace_back(item.first, item.second);
    // //     }
    // // }
    
    // int iter = 0;
    // for(int i=0;i<3;i++){
    //     std::cout << "Iteration " << iter << ": ";
    //     Table.insert(iter+11,iter+11);
    //     for (const auto &item : Table.lock_table()){
    //         std::cout << "(" << item.first << ", " << item.second << ") ";
    //     }
    //     std::cout << std::endl;
    //     iter++;
    // }

    // auto tt = Table.lock_table();


    // int sz = Table.capacity();
    // std::cout << sz << std::endl;
    // // 打印每次遍历的顺序
    // for (int n = 0; n < num_iterations; ++n) {
    //     std::cout << "Iteration " << n + 1 << ": ";
    //     for (const auto &item : iterations[n]) {
    //         std::cout << "(" << item.first << ", " << item.second << ") ";
    //     }
    //     std::cout << std::endl;
    // }

    // // 比较每次遍历的顺序
    // bool is_random = false;
    // for (int i = 1; i < num_iterations; ++i) {
    //     if (iterations[i] != iterations[0]) {
    //         is_random = true;
    //         break;
    //     }
    // }

    // if (is_random) {
    //     std::cout << "The iteration order is random." << std::endl;
    // } else {
    //     std::cout << "The iteration order is not random." << std::endl;
    // }

    return 0;
}
