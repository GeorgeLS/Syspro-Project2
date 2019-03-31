#ifndef EXERCISE_II_LIST_HPP
#define EXERCISE_II_LIST_HPP

#include <cstddef>
#include "Memory_Utils.hpp"

namespace Types {
    template<typename T>
    class List {
    private:
        struct Node {
            Node();

            Node(T &data);

            ~Node() = default;

            T data_;
            Node *next_;
        };

        struct ForwardIterator {
            ForwardIterator() = default;

            ForwardIterator(Node *node);

            ~ForwardIterator() = default;

            ForwardIterator &operator++();

            bool operator==(const ForwardIterator &other) const;

            bool operator!=(const ForwardIterator &other) const;

            T &operator*() const;

            T &operator->() const;

        private:
            Node *node_;
        };

    public:
        using iterator = ForwardIterator;

        List();

        List(const List<T> &list);

        List(List<T> &&other) noexcept;

        List<T> &operator=(const List<T> &other);

        List<T> &operator=(List<T> &&other) noexcept;

        ~List();

        inline iterator Begin();

        inline iterator End();

        void Add_Front(T &data);

        void Add_Back(T &data);

        inline T &First() const;

        inline T &Last() const;

        inline size_t Size() const;

    private:
        void Construct(const iterator &begin, const iterator &end);

        void Construct(const List<T> &other);

        void Move(List<T> &&other);

        void Destroy();

        Node *head_;
        Node *tail_;
        size_t size_;
    };

    template<typename T>
    List<T>::Node::Node()
            : data_{}, next_{nullptr} {}

    template<typename T>
    List<T>::Node::Node(T &data)
            : data_{data}, next_{nullptr} {}

    template<typename T>
    List<T>::ForwardIterator::ForwardIterator(Node *node)
            : node_{node} {}

    template<typename T>
    typename List<T>::ForwardIterator &List<T>::ForwardIterator::operator++() {
        node_ = node_->next_;
        return *this;
    }

    template<typename T>
    bool List<T>::ForwardIterator::operator==(const ForwardIterator &other) const {
        return node_ == other.node_;
    }

    template<typename T>
    bool List<T>::ForwardIterator::operator!=(const List::ForwardIterator &other) const {
        return node_ != other.node_;
    }

    template<typename T>
    T &List<T>::ForwardIterator::operator*() const {
        return node_->data_;
    }

    template<typename T>
    T &List<T>::ForwardIterator::operator->() const {
        return node_->data_;
    }

    template<typename T>
    List<T>::List()
            : head_{nullptr}, tail_{nullptr}, size_{0U} {}

    template<typename T>
    List<T>::~List() {
        Destroy();
    }

    template<typename T>
    void List<T>::Add_Front(T &data) {
        Node *node_ = Utils::Memory::malloc<Node>(1);
        new(node_) Node{data};
        if (size_ == 0U) {
            head_ = tail_ = node_;
        } else {
            node_->next_ = head_;
            head_ = node_;
        }
        ++size_;
    }

    template<typename T>
    void List<T>::Add_Back(T &data) {
        Node *node_ = Utils::Memory::malloc<Node>(1);
        new(node_) Node{data};
        if (size_ == 0U) {
            head_ = tail_ = node_;
        } else {
            tail_->next_ = node_;
            tail_ = node_;
        }
        ++size_;
    }

    template<typename T>
    size_t List<T>::Size() const {
        return size_;
    }

    template<typename T>
    typename List<T>::iterator List<T>::Begin() {
        return ForwardIterator{head_};
    }

    template<typename T>
    typename List<T>::iterator List<T>::End() {
        return ForwardIterator{nullptr};
    }

    template<typename T>
    void List<T>::Construct(const List<T> &other) {
        Construct(other.Begin(), other.End());
    }

    template<typename T>
    void List<T>::Construct(const List::iterator &begin, const List::iterator &end) {
        for (auto it = begin; it != end; ++it) {
            Add_Back(*it);
        }
    }

    template<typename T>
    List<T>::List(const List<T> &other) {
        Construct(other);
    }

    template<typename T>
    void List<T>::Destroy() {
        Node *curr = head_;
        while (curr != nullptr) {
            Node *tmp = curr;
            curr = curr->next_;
            tmp->data_.~T();
            free(tmp);
        }
    }

    template<typename T>
    List<T> &List<T>::operator=(const List<T> &other) {
        Destroy();
        Construct(other);
        return *this;
    }

    template<typename T>
    T &List<T>::First() const {
        return head_->data_;
    }

    template<typename T>
    T &List<T>::Last() const {
        return tail_->data_;
    }

    template<typename T>
    List<T>::List(List<T> &&other) noexcept
            : head_{nullptr}, tail_{nullptr}, size_{0U} {
        Move(other);
    }

    template<typename T>
    List<T> &List<T>::operator=(List<T> &&other) noexcept {
        Move(other);
        return *this;
    }

    template<typename T>
    void List<T>::Move(List<T> &&other) {
        std::swap(head_, other.head_);
        std::swap(tail_, other.tail_);
        std::swap(size_, other.size_);
    }
}

#endif //EXERCISE_II_LIST_HPP
