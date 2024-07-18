#ifndef _SAMPLE_HPP
#define _SAMPLE_HPP

#include <iostream>
#include <sys/time.h>
#include <vector>
#include <random>
#include <cstring>
#include <algorithm>
#include "../build/install/include/libcuckoo/cuckoohash_map.hh"
#define EVPOOL_SIZE 16
#define CACHE_LIMIT 1000
#define SAMPLES 5
#define INTERVAL_THERSHOLD 100


// pool only store key
template<typename KeyType>
class evictionPoolEntry {
public:
    long long time;    /* Object idle time (inverse frequency for LFU) */
    KeyType key;                   // 键名
    // int slot;                   /* Slot. */

    //使用缺省构造函数，后续自己来new
};

// evictionLRU 里面按照时间戳从大到小排序，淘汰从后往前
template<typename KeyType>
bool comparePool(const evictionPoolEntry<KeyType>& a, const evictionPoolEntry<KeyType>& b) {
    return a.time > b.time;
}

/* Return the UNIX time in microseconds */
static long long ustime(void) {
    struct timeval tv;
    long long ust;

    gettimeofday(&tv, NULL);
    ust = ((long long)tv.tv_sec) * 1000000;
    ust += tv.tv_usec;
    return ust;
}

/* Return the UNIX time in milliseconds */
// 是一个 syscall，因此会有开销
// redis 的做法是定期更新，减少 syscall 的开销
// Redis 会以 100ms/次 的频率记录 Unix 时间戳，然后除以 LRU 时钟单位 1000，即以秒为单位，保留低 24 位时间戳。
// 由于我们数据量比较小，因此定期更新多少合适呢，暂时没有做
static long long mstime(void) { return ustime() / 1000; }

static long long getLRUClock(void) {
    return ustime(); // 是否会因为 longlong 的截断导致错误？
}


template<typename KeyType, typename ValueType> class entry;

template<typename KeyType, typename ValueType>
class cache{
private:
    int capacity; //cache最大容量
    int samples; //采样数量
    int size; //cache当前size
    libcuckoo::cuckoohash_map<KeyType, entry<KeyType,ValueType>*> Table;
    evictionPoolEntry<KeyType> EvictionPoolLRU[EVPOOL_SIZE];
    std::vector<KeyType> existkey;
    int time_interval;
    bool firstSample;

    entry<KeyType,ValueType>* getEntry(const KeyType& key){
        entry<KeyType,ValueType>* et;
        if(Table.find(key,et)){
            return et;
        }else{
            return nullptr;
        }
    }

    void PoolAlloc();
    std::vector<KeyType> getRandomKey();

    void updateTimeInterval(){
        //倒序遍历
        for(int i=EVPOOL_SIZE-1;i>=0;i--){
            if(EvictionPoolLRU[i].key == KeyType()) continue;
            KeyType out = EvictionPoolLRU[i].key;
            entry<KeyType, ValueType>* entryToRemove = getEntry(out);
            entry<KeyType, ValueType>* firstEt = getEntry(EvictionPoolLRU[0].key);
            time_interval = abs(entryToRemove->time-firstEt->time);
            break;
        }
    }

    // 只有key不存在才会insert，因此不会出现key重复
    void insert(const KeyType& key, const ValueType& val){
        //entry 构造时会自动写入时间戳
        entry<KeyType,ValueType>* et = new entry<KeyType,ValueType>(key,val);
        Table.insert(key,et);
        // 同步insert到vector
        existkey.push_back(key);
        size++;
    }

    // 更新时从键空间随机选择N个key，分别计算它们的空闲时间idle
    // key只会在pool不满或者空闲时间大于pool里最小的时，才会进入pool，然后从pool中选择空闲时间最大的key淘汰掉
    // 注意选择并且最终进入了pool的key，需要从existkey删除
    int evictionPoolPopulate(){

        // evictionPoolEntry<KeyType>* pool[SAMPLES];
        std::vector<KeyType> sample = getRandomKey();
        int count = sample.size(); //这是现在exsitkey的前count个，如果哪个加入了淘汰池，则从existkey中删除
        if(firstSample){
            firstSample = false;
            for(int i=0;i<count;i++){
                entry<KeyType,ValueType>* et = getEntry(sample[i]);
                EvictionPoolLRU[i].key = et->key;
                EvictionPoolLRU[i].time = et->time;
                // existkey.erase(existkey.begin());
            }
            std::sort(EvictionPoolLRU, EvictionPoolLRU + count,comparePool<KeyType>);
            return count;
        }

        // if(count < SAMPLES && EvictionPoolLRU[0].key!=KeyType()){
        //     return count;
        // }

        // index 位置对应为1表示该下标被删除
        int deletindex[5] = {-1,-1,-1,-1,-1};
        for(int i=0;i<count;i++){
            entry<KeyType,ValueType>* et = getEntry(sample[i]);
            int k = 0;

            //find the location to insert, 时间越小的应该放在越后面，越早淘汰
            while(k < EVPOOL_SIZE && EvictionPoolLRU[k].key!=KeyType() && et->time < EvictionPoolLRU[k].time ) k++;
            
            if(sample[i] == EvictionPoolLRU[k].key) continue; //有重复

            //setup before insert
            if (k == 0 && EvictionPoolLRU[EVPOOL_SIZE-1].key != KeyType()) {
                /* Can't insert if the element is < the worst element we have
                * or there are no empty buckets. */
                continue;
            } else if (k < EVPOOL_SIZE && EvictionPoolLRU[k].key == KeyType() ) {
                /* Inserting into empty position. No setup needed before insert. */
            } else {
                /* Inserting in the middle. Now k points to the first element
                * greater than the element to insert.  */
                if (EvictionPoolLRU[EVPOOL_SIZE-1].key == KeyType()) {
                    /* Free space on the right? Insert at k shifting
                    * all the elements from k to end to the right. */

                   //为啥这个memmove会导致key的重复呢
                    // memmove(EvictionPoolLRU+k+1,EvictionPoolLRU+k,sizeof(EvictionPoolLRU[0])*(EVPOOL_SIZE-k-1));
                    for(int j=EVPOOL_SIZE-1;j>k;j--){
                        if(EvictionPoolLRU[j-1].key == KeyType()) continue;
                        EvictionPoolLRU[j].key = EvictionPoolLRU[j-1].key;
                        EvictionPoolLRU[j].time = EvictionPoolLRU[j-1].time;
                    }
                } else {
                    /* No free space on right? k = EVPOOL_SIZE, Insert at k-1 */
                    k--;
                    /* Shift all elements on the left of k (included) to the
                    * left, so we discard the element with smaller idle time. */
                    memmove(EvictionPoolLRU,EvictionPoolLRU+1,sizeof(EvictionPoolLRU[0])*k);
                }
            }

        /*insert*/
        EvictionPoolLRU[k].key = et->key;
        EvictionPoolLRU[k].time = et->time;
        // deletindex[k] = 1;
    }

        // 移除删除的键
        // for(int i=0;i<5;i++){
        //     if(deletindex[i]==1){
        //         existkey.erase(existkey.begin() + i);
        //     }
        // }
        return count;
    }

public:
    cache(int capacity): capacity(capacity), samples(SAMPLES), size(0), firstSample(true),time_interval(0) {
        for(int i=0;i<EVPOOL_SIZE;i++){
            EvictionPoolLRU[i].time = 0;
        }
     }
    cache():capacity(CACHE_LIMIT),samples(SAMPLES),size(0), firstSample(true) {
        for(int i=0;i<EVPOOL_SIZE;i++){
            EvictionPoolLRU[i].time = 0;
        }
    }

    ValueType get(const KeyType& key){
        entry<KeyType,ValueType>* et = getEntry(key);
        if(et){
            et->time = getLRUClock();
            return et->val;
        }else{
            return ValueType();
        }
    }

    // if evict, return the evict key
    ValueType put(const KeyType& key, const ValueType& val){
        entry<KeyType,ValueType>* et = getEntry(key);
        if(et){
            // key存在，更新时间戳+设置新值
            et->val = val;
            et->time = getLRUClock();
            return ValueType();
        }

        // key 不存在，分缓存已满和缓存未满讨论
        KeyType s;
        if(!full()){
            // 缓存未满，插入
            insert(key,val);
        }else{
            //缓存已满, evict
            // 当pool内部时间差大于一个阈值时，说明有些过时，此时才重新采样
            if(time_interval == 0 || time_interval>INTERVAL_THERSHOLD || EvictionPoolLRU[0].key==KeyType()){
                evictionPoolPopulate();
                updateTimeInterval();
            }

            
            //倒序遍历
            for(int i=EVPOOL_SIZE-1;i>=0;i--){
                if(EvictionPoolLRU[i].key == KeyType()) continue;
                KeyType out = EvictionPoolLRU[i].key;
                entry<KeyType, ValueType>* entryToRemove = getEntry(out);
                s = out;

                // 按理说淘汰池里的一定能找到
                EvictionPoolLRU[i].key = KeyType();
                EvictionPoolLRU[i].time = 0;
                Table.erase(out);
                existkey.erase(std::remove(existkey.begin(), existkey.end(), out), existkey.end());
                delete entryToRemove;
                // std::cout << "evict " << out<< std::endl;
                size--;

                break;
            }
            //insert
            insert(key,val);
        }
        return s;
    }


    bool full(){
        return size >= capacity;
    }

    /* function for test */
    long long gettime(const KeyType& key){
        return getEntry(key)->time;
    }

    void printPool(){
        for(int i=0;i<EVPOOL_SIZE;i++){
            std::cout << EvictionPoolLRU[i].key <<  " " << EvictionPoolLRU[i].time << std::endl;
        }
    }

};

/*从existkey中获取SAMPLE个 */
template<typename KeyType, typename ValueType>
std::vector<KeyType> cache<KeyType,ValueType>::getRandomKey(){
    int count = existkey.size();
    int n = std::min(count,SAMPLES);

    // 随机设备和随机数生成器
    std::random_device rnd_dev;
    std::mt19937_64 rnd_gen(rnd_dev());

    // 随机打乱 key 列表
    std::shuffle(existkey.begin(), existkey.end(), rnd_gen);

    // 返回前 n 个 key
    return std::vector<KeyType>(existkey.begin(), existkey.begin() + n);
}

template<typename KeyType, typename ValueType>
class entry{
public:
    long long time;
    KeyType key;
    ValueType val;

    // 构造时初始化 time 时间戳
    entry(const KeyType& key, const ValueType& val): time(getLRUClock()), key(key), val(val) {}
};

#endif