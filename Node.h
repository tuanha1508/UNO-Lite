#ifndef NODE_H
#define NODE_H

/**
 * @brief Generic node for a singly linked list.
 * @tparam T The data type stored in the node.
 * @author Khang
 */
template <typename T>
struct Node {
    T data;
    Node<T>* next;

    explicit Node(T data) : data(data), next(nullptr) {}
};

#endif // NODE_H
