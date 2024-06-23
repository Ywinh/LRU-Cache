#include <iostream>
#include "../include/lru_cache.hpp"

template<typename KeyType, typename ValueType>
void print_list(const List<KeyType, ValueType>& list) {
    Node<KeyType, ValueType>* current = list.begin();
    while (current != list.end()) {
        std::cout << "(" << current->key << ", " << current->value << ") ";
        current = current->next;
    }
    std::cout << std::endl;
}

int main() {
    List<int, std::string> list;

    // Test pushing elements
    list.push_front(1, "one");
    list.push_front(2, "two");
    list.push_front(3, "three");

    std::cout << "After pushing three elements:" << std::endl;
    print_list(list);

    // Test moving an element to front
    Node<int, std::string>* node = list.begin()->next; // Get second element
    list.move_to_front(node);

    std::cout << "After moving second element to front:" << std::endl;
    print_list(list);

    // Test popping the front element
    list.pop(list.begin()); // Remove first element

    std::cout << "After popping front element:" << std::endl;
    print_list(list);

    // Test pushing more elements
    list.push_front(4, "four");
    list.push_front(5, "five");

    std::cout << "After pushing two more elements:" << std::endl;
    print_list(list);

    // Test moving the last element to front
    Node<int, std::string>* last_node = list.end()->prev; // Get last element
    list.move_to_front(last_node);

    std::cout << "After moving last element to front:" << std::endl;
    print_list(list);

    // Test popping all elements
    while (list.get_size() > 0) {
        list.pop(list.begin());
    }

    std::cout << "After popping all elements:" << std::endl;
    print_list(list);

    // Test get_size after clearing
    std::cout << "Current size of the list: " << list.get_size() << std::endl;

    return 0;
}
