/*
    SPDX-License-Identifier: MPL-2.0
    --------------------------------
    This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
    If a copy of the MPL was not distributed with this file,
    You can obtain one at https://mozilla.org/MPL/2.0/.

    Provided “as is”, without warranty of any kind.

    Copyright © 2026 Alar Akilbekov. All rights reserved.
 */

#include <iostream>
#include <string>
#include <iterator> // bidirectional_iterator_tag, reverse_iterator
#include <cstddef> // ptrdiff_t
#include <type_traits> // enable_if_t
#include <stdexcept> // runtime_error
#include <algorithm> // swap
#include <new> // bad_alloc

// --- LinkedList ---

template <typename T>
class LinkedList { // Doubly-Linked List
public:
    template <typename R>
    class Node {
    public:
        R data;
        Node<R> * next;
        Node<R> * prev; // Doubly-Linked List + 8 bytes
        /*
            LinkedList::Node единственный кто будет хранить lvalue значение data,
            При этом копия будет создаваться только если передать lvalue data.
            В общем, тут хитрая оптимизация хранение объекта в Node.
            Node должен хранить lvalue значение объекта, не reference объекта, и не pointer на объект.

            Простым решением было бы сделать конструктор с pass-by-value и всегда создавать копию:
            Node(R data) : data(data), next(nullptr), prev(nullptr) {}

            Но, тут, получается, единственный конструктор принимает rvalue и крадет данные, не созадавая лишних копий
            Это нужно учитывать, и это учитывается в `add` функциях.
        */
        Node() = delete; // Когда может понадобиться запоздалый assignment data?
        Node(R & data) = delete; // Передавать reference точно плохая идея (извне могут удалить объект), лучше Node будет полным обладателем data

        // Node(R && rvalue) : data(std::move(rvalue)), next(nullptr), prev(nullptr) {
        Node(R && rvalue) {
            // std::cout << "Node rvalue Constructor" << std::endl;
            data = std::move(rvalue); // R's Move Assignment Operator
            next = nullptr;
            prev = nullptr;
        }

        // Rule of 5
        ~Node() = default; // можно было бы сделать цепное удаление next, но пусть Node будет максимально простым
        Node(Node & other) = delete; // можно было бы разрешить копирование, но зачем копировать Node и data лишний раз
        Node & operator = (Node & other) = delete;
        Node(Node && other) = delete; // move semantics для Node никогда не понадобится
        Node & operator = (Node && other) = delete;
    };

private:
    Node<T> * _head;
    Node<T> * _tail;
    int _size;

public:
    explicit LinkedList() : _head(nullptr), _tail(nullptr), _size(0) {}

    ~LinkedList(){
        /*
         * Можно было бы реализовать цепочную реакцию удаления:
         * if(_head != nullptr){ delete _head; _head = nullptr; _tail = nullptr; }
         */
        Node<T> * node = _head;
        Node<T> * next;
        // for(int i = 0; i < _size; i++){
        while(node != nullptr){
            // std::cout << "deallocate Node{" << typeid(node->data).name() << "}" << std::endl;
            next = node->next; // memorize
            delete node; node = nullptr;
            node = next;
        }
    }

    int size(){
        // std::cout << "size()" << std::endl;
        return _size;
    }


// --- Append ---

    // --- addLast ---
private:
    // addLast: O(1)
    T& addLast(Node<T> * new_node){
        /*
         * Normally it should not throw an exception, only on Memory Allocation fails
         * OutOfMemoryError in Java
         * in C++ 'new' throws std::bad_alloc if memory allocation fails, use try{}catch(const std::bad_alloc& e){}
         * in C malloc from <stdlib.h> returns NULL, so check if returned value is NULL in C
         */
        // Node<T> * new_node = new Node(item);

        if(_head == nullptr){
            // std::cout << "initial head node" << std::endl;
            _head = new_node;
            _tail = _head;
        }
        else {
            new_node->prev = _tail;
            _tail->next = new_node; // old tail
            _tail = new_node;
            // _tail->next = nullptr
        }
        ++_size;

        return _tail->data;
    }
public:
    T& addLast(T & lvalue){ // 1 copy
        // std::cout << "addLast(lvalue) (1)" << std::endl;
        T item = lvalue; // copy
        Node<T> * new_node = new Node(std::move(item));
        // std::cout << "addLast(lvalue) (2)" << std::endl;
        return addLast(new_node);
    }
    T& addLast(T && rvalue){ // 0 copy, 1 rvalue object
        /*
            ll.addLast(String("A"));
                -> String Constructor(A)
                -> Node rvalue Constructor
                -> String Move Assignment Operator
                -> ~String {nullstr0}
        */
        // std::cout << "addLast(rvalue) (1)" << std::endl;
        // T item = std::move(rvalue); // 1 object which is stealed
        Node<T> * new_node = new Node(std::move(rvalue)); // 0 copy object
        // std::cout << "addLast(rvalue) (2)" << std::endl;
        return addLast(new_node);
    }

    // --- addFirst ---
private:
    // addFirst: O(1)
    T& addFirst(Node<T> * new_node){
        // Node<T> * new_node = new Node(item);
        if(_head == nullptr){
            // std::cout << "initial head node" << std::endl;
            _head = new_node;
            _tail = _head;
        }
        else {
            new_node->next = _head;
            _head->prev = new_node; // old tail
            _head = new_node;
            // _head->prev = nullptr
        }
        ++_size;
        return _head->data;
    }

public:
    T& addFirst(T & lvalue){
        T item = lvalue; // copy
        Node<T> * new_node = new Node(std::move(item));
        return addFirst(new_node);
    }
    T& addFirst(T && rvalue){
        Node<T> * new_node = new Node(std::move(rvalue));
        return addFirst(new_node);
    }
    // ------


// --- Remove ---

    // --- removeLast ---
public:
    // remove: O(1)
    /*
        Тут тоже оптимизация.
        Мы должны удалить память выделенную для Node.
        Как тогда вернуть значение, да так, чтобы оно потом само удалилось?
        Мы освобождаем память, но возвращаем stealed объект in Stack Memory
        + тут RVO, из-за чего не должна создаваться копия при возврате.
    */
    T removeLast(){
        // std::cout << "remove last, _size = (" << _size  << " -> " << _size - 1 << ")" << std::endl;

        if(_size == 0){
            throw std::runtime_error("Error: cannot remove last, linked list is empty");
        }

        Node<T> * tail = _tail;
        T data = std::move(tail->data); // In Stack Memory, so it will free automatically

        if(_size == 1){
            _head = nullptr;
            _tail = nullptr;
        }
        else {
            _tail = tail->prev;
            _tail->next = nullptr; // до этого tail.prev.next = tail

            delete tail; // Delete Memory Allocated for Node
            tail = nullptr;
        }

        --_size;

        return data; // RVO
    }

    // --- removeFirst ---
public:
    // remove: O(1)
    T removeFirst(){
        // std::cout << "remove first, _size = (" << _size  << " -> " << _size - 1 << ")" << std::endl;

        if(_size == 0){
            throw std::runtime_error("Error: cannot remove first, linked list is empty");
        }

        Node<T> * head = _head;
        T data = std::move(head->data);

        if(_size == 1){
            _head = nullptr;
            _tail = nullptr;
        }
        else {
            _head = head->next;
            _head->prev = nullptr; // до этого tail.prev.next = tail

            delete head;
            head = nullptr;
        }

        --_size;

        return data;
    }

    // ------


// --- Get ---
public:
    // get: O(N)
    T& get(int index){
        if(index < 0 || index >= _size /*_size == 0)*/){
            // Normal behaviour is to throw an error, not to return NULL
            throw std::out_of_range("Error: index out of range");
        }
        Node<T> * node = _head;
        // for(int i = 0; i < index; ++i, node = node->next){}
        while(index != 0){
            node = node->next;
            --index;
        }
        return node->data;
    }

    T& getFirst(){
        if(_head == nullptr){
            throw std::runtime_error("Error: can't get first, linked list is empty");
        }
        return _head->data;
    }
    T& getLast(){
        if(_tail == nullptr){
            throw std::runtime_error("Error: can't get last, linked list is empty");
        }
        return _tail->data;
    }

public:
    int find(const T item){
        Node<T> * node = _head;
        for(int i = 0; /*i < _size*/ node != nullptr; ++i){
            if(node->data == item){
                return i;
            }
            node = node->next;
        }
        return -1;
    }

public:
    void reverse(){
        Node<T> * node = _head;
        _head = _tail;
        _tail = node;
        while(node != nullptr){
            Node<T> * tmp = node->prev;
            node->prev = node->next;
            node->next = tmp;
            node = node->prev; // it is like next
        }
    }

    void clean(){
        for(int n = _size, i = 0; i < n; i++){ // 0 1 2 3 4 5
            removeLast();
        }
    }

};


class String {
public:
    std::string value;
    ~String(){
        // std::cout << "~String {" << value << "}" << std::endl;
    }

    String() = default;
    String(std::string value) : value(value) {
        // std::cout << "String Constructor(" << value << ")" << std::endl;
    }
    String(String & other){
        // std::cout << "String Copy Constructor" << std::endl;
        value = other.value;
    }
    String & operator = (String & other){
        // std::cout << "String Copy Assignment Operator" << std::endl;
        if(this != &other){
            value = other.value;
        }
        return *this;
    }
    String(String && other){
        // std::cout << "String Move Constructor" << std::endl;
        value = other.value;
        other.value = "nullstr0";
    }
    String & operator = (String && other){
        // std::cout << "String Move Assignment Operator" << std::endl;
        if(this != &other){
            value = other.value;
            other.value = "nullstr0";
        }
        return *this;
    }

    bool operator==(const std::string& other) const noexcept {
        return value == other;
    }
    bool operator==(const String& other) const noexcept {
        return value == other.value;
    }
    bool operator!=(const String& other) const noexcept {
        return value != other.value;
    }
    bool operator!=(const std::string& other) const noexcept {
        return value != value;
    }

    // Stream insertion operation <<
    friend std::ostream& operator<<(std::ostream& os, const String& s);
};
std::ostream& operator<<(std::ostream& os, const String& s) {
    os << s.value;
    return os;
}

int main(){
    std::cout << "start ------------" << std::endl;
    LinkedList<String> ll;
    ll.addLast(String("A"));
    ll.addLast(String("B"));
    ll.addLast(String("C"));

    std::cout << (ll.get(0) == String("A")) << std::endl;
    std::cout << (ll.get(0) == "A") << std::endl;
    std::cout << (ll.get(0) == String("B")) << std::endl;
    std::cout << (ll.get(0) == "B") << std::endl;

    std::cout << ll.get(1) << std::endl;
    std::cout << ll.get(2) << std::endl;

    std::cout << "end ------------" << std::endl;
}
