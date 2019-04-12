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
            Node *previous_;
        };
		
        struct BidirectionalIterator {
            BidirectionalIterator() = default;
			
            BidirectionalIterator(Node *node);
			
            ~BidirectionalIterator() = default;
			
            BidirectionalIterator &operator++();
			
            BidirectionalIterator &operator--();
			
            bool operator==(const BidirectionalIterator &other) const;
			
            bool operator!=(const BidirectionalIterator &other) const;
			
            T &operator*() const;
			
            T &operator->() const;
			
            Node *node_;
        };
		
		public:
        using iterator = BidirectionalIterator;
		
        List();
		
        List(std::initializer_list<T> list);
		
        List(const List<T> &list);
		
        List(List<T> &&other) noexcept;
		
        List<T> &operator=(const List<T> &other);
		
        List<T> &operator=(List<T> &&other) noexcept;
		
        ~List();
		
        inline iterator Begin();
		
        inline const iterator Begin() const;
		
        inline const iterator End() const;
		
        inline iterator End();
		
        void Add_Front(T &data);
		
        void Add_Front(T &&data);
		
        void Add_Back(T &data);
		
        void Add_Back(T &&data);
		
        void Remove(const iterator &position);
		
        void Clear();
		
        inline T &First() const;
		
        inline T &Last() const;
		
        inline size_t Size() const;
		
		private:
        void Construct(const iterator &begin, const iterator &end);
		
        void Construct(const List<T> &other);
		
        void Move(List<T> &&other);
		
        void RemoveAt(iterator &position);
		
        void Destroy();
		
        Node *head_;
        Node *tail_;
        size_t size_;
    };
	
    template<typename T>
		List<T>::Node::Node()
		: data_{}, next_{nullptr}, previous_{nullptr} {}
	
    template<typename T>
		List<T>::Node::Node(T &data)
		: data_{data}, next_{nullptr}, previous_{nullptr} {}
	
    template<typename T>
		List<T>::BidirectionalIterator::BidirectionalIterator(Node *node)
		: node_{node} {}
	
    template<typename T>
		typename List<T>::BidirectionalIterator &List<T>::BidirectionalIterator::operator++() {
        node_ = node_->next_;
        return *this;
    }
	
    template<typename T>
		typename List<T>::BidirectionalIterator &List<T>::BidirectionalIterator::operator--() {
        node_ = node_->previous_;
        return *this;
    }
	
    template<typename T>
		bool List<T>::BidirectionalIterator::operator==(const BidirectionalIterator &other) const {
        return node_ == other.node_;
    }
	
    template<typename T>
		bool List<T>::BidirectionalIterator::operator!=(const List::BidirectionalIterator &other) const {
        return node_ != other.node_;
    }
	
    template<typename T>
		T &List<T>::BidirectionalIterator::operator*() const {
        return node_->data_;
    }
	
    template<typename T>
		T &List<T>::BidirectionalIterator::operator->() const {
        return node_->data_;
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
		void List<T>::Move(List<T> &&other) {
        std::swap(head_, other.head_);
        std::swap(tail_, other.tail_);
        std::swap(size_, other.size_);
    }
	
    template<typename T>
		void List<T>::Destroy() {
        Node *curr = head_;
        while (curr != nullptr) {
            Node *tmp = curr;
            curr = curr->next_;
            tmp->~Node();
            free(tmp);
        }
        head_ = tail_ = nullptr;
    }
	
    template<typename T>
		List<T>::List()
		: head_{nullptr}, tail_{nullptr}, size_{0U} {}
	
    template<typename T>
		List<T>::~List() {
        Destroy();
    }
	
    template<typename T>
		List<T>::List(std::initializer_list<T> list)
		:size_{0U} {
        for (auto it = list.begin(); it != list.end(); ++it) {
            Add_Back(const_cast<T &>(*it));
        }
    }
	
    template<typename T>
		List<T>::List(const List<T> &other)
		: size_{0U} {
        Construct(other);
    }
	
    template<typename T>
		List<T>::List(List<T> &&other) noexcept
		: head_{nullptr}, tail_{nullptr}, size_{0U} {
        Move(other);
    }
	
    template<typename T>
		List<T> &List<T>::operator=(const List<T> &other) {
        Destroy();
        Construct(other);
        return *this;
    }
	
    template<typename T>
		List<T> &List<T>::operator=(List<T> &&other) noexcept {
        Move(other);
        return *this;
    }
	
    template<typename T>
		typename List<T>::iterator List<T>::Begin() {
        return BidirectionalIterator{head_};
    }
	
    template<typename T>
		const typename List<T>::iterator List<T>::Begin() const {
        return BidirectionalIterator{head_};
    }
	
	
    template<typename T>
		typename List<T>::iterator List<T>::End() {
        return BidirectionalIterator{nullptr};
    }
	
    template<typename T>
		const typename List<T>::iterator List<T>::End() const {
        return BidirectionalIterator{nullptr};
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
		size_t List<T>::Size() const {
        return size_;
    }
	
    template<typename T>
		void List<T>::Add_Front(T &data) {
        Node *node_ = Utils::Memory::malloc<Node>(1);
        new(node_) Node{data};
        if (size_) {
            node_->next_ = head_;
            head_->previous_ = node_;
            head_ = node_;
        } else {
            head_ = tail_ = node_;
        }
        ++size_;
    }
	
    template<typename T>
		void List<T>::Add_Front(T &&data) {
        Node *node_ = Utils::Memory::malloc<Node>(1);
        new(node_) Node{data};
        if (size_) {
            node_->next_ = head_;
            head_->previous_ = node_;
            head_ = node_;
        } else {
            head_ = tail_ = node_;
        }
        ++size_;
    }
	
    template<typename T>
		void List<T>::Add_Back(T &data) {
        Node *node_ = Utils::Memory::malloc<Node>(1);
        new(node_) Node{data};
        if (size_) {
            node_->previous_ = tail_;
            tail_->next_ = node_;
            tail_ = node_;
        } else {
            head_ = tail_ = node_;
        }
        ++size_;
    }
	
    template<typename T>
		void List<T>::Add_Back(T &&data) {
        Node *node_ = Utils::Memory::malloc<Node>(1);
        new(node_) Node{data};
        if (size_) {
            node_->previous_ = tail_;
            tail_->next_ = node_;
            tail_ = node_;
        } else {
            head_ = tail_ = node_;
        }
        ++size_;
    }
	
    template<typename T>
		void List<T>::Remove(const List::iterator &position) {
        RemoveAt(const_cast<List::iterator &>(position));
    }
	
    template<typename T>
		void List<T>::Clear() {
        Destroy();
    }
	
    template<typename T>
		void List<T>::RemoveAt(iterator &position) {
        Node *node = position.node_;
        if (size_ == 1) {
            head_ = tail_ = nullptr;
        } else if (position == Begin()) {
            node->next_->previous_ = nullptr;
            head_ = node->next_;
        } else if (position == End()) {
            node->previous_->next_ = nullptr;
            tail_ = node->previous_;
        } else {
            node->previous_->next_ = node->next_;
            node->next_->previous_ = node->previous_;
        }
        --size_;
        node->~Node();
        free(node);
    }
}

#endif //EXERCISE_II_LIST_HPP
