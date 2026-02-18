#ifndef CIRCULARLINKEDLIST_H
#define CIRCULARLINKEDLIST_H

#include "Node.h"
#include <iostream>

/**
 * @brief Circular singly linked list with traversal direction support.
 *
 * Supports forward/backward traversal via a direction flag,
 * making it suitable for turn-based games like UNO.
 *
 * @tparam T The data type stored in each node.
 * @author Khang
 */
template <typename T>
class CircularLinkedList {
private:
    Node<T>* head;
    Node<T>* tail;
    Node<T>* current;
    int count;
    bool forward;

public:
    CircularLinkedList()
        : head(nullptr), tail(nullptr), current(nullptr),
          count(0), forward(true) {}

    ~CircularLinkedList() {
        if (head == nullptr) return;

        tail->next = nullptr;
        Node<T>* curr = head;
        while (curr != nullptr) {
            Node<T>* next = curr->next;
            delete curr;
            curr = next;
        }
        head = nullptr;
        tail = nullptr;
        current = nullptr;
        count = 0;
    }

    // Prevent shallow copies (pointers would be shared)
    CircularLinkedList(const CircularLinkedList&) = delete;
    CircularLinkedList& operator=(const CircularLinkedList&) = delete;

    // --- Insertion ---

    void insertBack(T value) {
        Node<T>* newNode = new Node<T>(value);
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
            newNode->next = head;
            current = head;
        } else {
            newNode->next = head;
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }

    void insertFront(T value) {
        Node<T>* newNode = new Node<T>(value);
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
            newNode->next = head;
            current = head;
        } else {
            newNode->next = head;
            head = newNode;
            tail->next = head;
        }
        count++;
    }

    void insertAt(int index, T value) {
        if (index < 0 || index > count) {
            std::cerr << "insertAt: index " << index << " out of range" << std::endl;
            return;
        }
        if (index == 0) { insertFront(value); return; }
        if (index == count) { insertBack(value); return; }

        Node<T>* prev = head;
        for (int i = 0; i < index - 1; i++) {
            prev = prev->next;
        }
        Node<T>* newNode = new Node<T>(value);
        newNode->next = prev->next;
        prev->next = newNode;
        count++;
    }

    // --- Removal ---

    void removeFront() {
        if (head == nullptr) return;

        if (count == 1) {
            if (current == head) current = nullptr;
            delete head;
            head = nullptr;
            tail = nullptr;
            count = 0;
            return;
        }

        Node<T>* toDelete = head;
        head = head->next;
        tail->next = head;
        if (current == toDelete) current = head;
        delete toDelete;
        count--;
    }

    void removeAt(int index) {
        if (index < 0 || index >= count) {
            std::cerr << "removeAt: index " << index << " out of range" << std::endl;
            return;
        }
        if (index == 0) { removeFront(); return; }

        Node<T>* prev = head;
        for (int i = 0; i < index - 1; i++) {
            prev = prev->next;
        }
        Node<T>* toDelete = prev->next;
        prev->next = toDelete->next;

        if (toDelete == tail) {
            tail = prev;
            tail->next = head;
        }
        if (toDelete == current) {
            current = prev->next;
        }

        delete toDelete;
        count--;

        if (count == 0) {
            head = nullptr;
            tail = nullptr;
            current = nullptr;
        }
    }

    void removeByValue(T value) {
        if (head == nullptr) return;

        if (head->data == value) { removeFront(); return; }

        Node<T>* prev = head;
        Node<T>* curr = head->next;
        for (int i = 1; i < count; i++) {
            if (curr->data == value) {
                prev->next = curr->next;
                if (curr == tail) {
                    tail = prev;
                    tail->next = head;
                }
                if (curr == current) {
                    current = prev->next;
                }
                delete curr;
                count--;
                return;
            }
            prev = curr;
            curr = curr->next;
        }
    }

    // --- Access ---

    T get(int index) const {
        if (index < 0 || index >= count) {
            std::cerr << "get: index " << index << " out of range" << std::endl;
            return T();
        }
        Node<T>* temp = head;
        for (int i = 0; i < index; i++) {
            temp = temp->next;
        }
        return temp->data;
    }

    int size() const { return count; }
    bool isEmpty() const { return count == 0; }

    void display() const {
        if (head == nullptr) {
            std::cout << "(empty)" << std::endl;
            return;
        }
        Node<T>* temp = head;
        for (int i = 0; i < count; i++) {
            std::cout << temp->data;
            if (i < count - 1) std::cout << ", ";
            temp = temp->next;
        }
        std::cout << std::endl;
    }

    // --- Traversal (UNO turn management) ---

    T getCurrent() const {
        if (current == nullptr) {
            std::cerr << "getCurrent: list is empty" << std::endl;
            return T();
        }
        return current->data;
    }

    void advance() {
        if (current == nullptr || count <= 1) return;

        if (forward) {
            current = current->next;
        } else {
            Node<T>* prev = current;
            while (prev->next != current) {
                prev = prev->next;
            }
            current = prev;
        }
    }

    void reverse() { forward = !forward; }

    void skipNext() {
        advance();
        advance();
    }
};

#endif // CIRCULARLINKEDLIST_H
