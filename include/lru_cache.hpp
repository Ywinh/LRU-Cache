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
    ~lru_cache(){
        // lru.~List();
        // Table.clear();
    }

    ValueType get(KeyType key){
        Node<KeyType,ValueType>* t = get_node(key);
        if(t){
            return t->value;
        }else{
            std::cout<<"Key "<<key<<" not found"<<std::endl;
            return ValueType();
            // throw std::runtime_error("Key not found");
        }
    }

    void put(KeyType key, ValueType value){
        // if key already in table, update value, then promote
        // if key not exsit, insert
        Node<KeyType,ValueType>* t = get_node(key);
        if(t){
            t->value = value;
            lru.move_to_front(t);
        }else{
            // check size
            if(lru.get_size()+1 > capacity){
                Node<KeyType,ValueType>* back_node = lru.end(); 
                Table.erase(back_node->key);
                lru.pop_back();
            }
            lru.push_front(key,value);
            Table.insert(key,lru.begin());
        }
    }

    size_t size(){
        return lru.get_size();
    }

private:
    size_t capacity;
    List<KeyType,ValueType> lru;
    libcuckoo::cuckoohash_map<KeyType, Node<KeyType,ValueType>*> Table;

    Node<KeyType,ValueType>* get_node(KeyType key){
        Node<KeyType,ValueType>* t;
        if(Table.find(key,t)){
            return t;
        }else{
            return nullptr;
        }
    }

    // just touch and move
    void touch(KeyType key){
        Node<KeyType,ValueType>*t = get_node(key);
        if(t == nullptr)
            return
        lru.move_to_front(t);
    }
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

    // need a clear?

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

    void pop_back(){
        Node<KeyType, ValueType>* node = dummy->prev;
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
        return dummy->prev;
    }

    Node<KeyType, ValueType>* dummy_() const {
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