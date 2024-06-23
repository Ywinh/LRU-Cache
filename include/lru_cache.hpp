#ifndef _LRU_CACHE_HPP
#define _LRU_CACHE_HPP

#include<iostream>
#include "../build/install/include/libcuckoo/cuckoohash_map.hh"

// Forward declaration of List and Node
template<typename KeyType, typename ValueType>
class List;

template<typename KeyType, typename ValueType>
class Node;

template<typename KeyType, typename ValueType>
class lru_cache
{
public:
    lru_cache(size_t capacity):capacity(capacity),lru() {}
    ~lru_cache();

private:
    size_t capacity;
    List<KeyType,ValueType> lru;
    libcuckoo::cuckoohash_map<KeyType, ValueType> Table;
};


template<typename KeyType, typename ValueType>
class Node{
public:
    Node(const KeyType& k, const ValueType& v):key(k),value(v),prev(nullptr),next(nullptr) {}
    Node():prev(nullptr),next(nullptr) {} //for dummy creating

    KeyType key;
    ValueType value;
    Node* prev; // Equivalent to Node<KeyType, ValueType>* prev within the class scope
    Node* next;
};

template<typename KeyType, typename ValueType>
class List{
public:
    List():size(0){
        dummy = new Node<KeyType, ValueType>();
        dummy->prev = dummy;
        dummy->next = dummy;
    }
    ~List(){
        delete dummy;
    }

    void push_front(const KeyType& key, const ValueType& value){
        Node<KeyType, ValueType>* node = new Node<KeyType, ValueType>(key,value);
        insert_to_head(node);
        size ++;
    }

    void move_to_front(Node<KeyType, ValueType>* node){
        // remove from the list
        remove(node);
        // insert to head
        insert_to_head(node);
    }

    // permenantly remove and delete node
    void pop(Node<KeyType, ValueType>* node){
        remove(node);
        size --;
        delete node;
    }

    size_t get_size(){
        return size;
    }

    Node<KeyType, ValueType>* begin() const {
        return dummy->next;
    }

    Node<KeyType, ValueType>* end() const {
        return dummy;
    }

private:
    Node<KeyType,ValueType>* dummy;
    size_t size;

    // not maintain the size
    void insert_to_head(Node<KeyType, ValueType>* node){
        node->next = dummy->next;
        node->prev = dummy;
        dummy->next->prev = node;
        dummy->next = node;
    }

    // not maintain the size
    // not delete the node
    void remove(Node<KeyType, ValueType>* node){
        if(node == dummy){
            throw std::invalid_argument("Cannot remove sentinel nodes");
        }
        node->next->prev = node->prev;
        node->prev->next = node->next;
    }

};

#endif