#include <cstddef>
class List {
    struct Node {
        int value;
        Node *prev, *next;
        Node(int value, Node *prev, Node *next) : value(value), prev(prev), next(next) {}
    };
    Node* head_;
    Node* tail_;
    size_t size_;

public:
    List() : head_(nullptr), tail_(nullptr), size_(0) {}

    List& operator= (const List &rhs) {
        while (size_ > 0) {
            pop_back();
        }
        for (auto i : rhs) {
            push_back(i);
        }
        return *this;
    }

    ~List() {
        while (size_ > 0) {
            pop_back();
        }
    }

    void push_back(int item) {
        if (size_ ==0) {
            head_ = tail_ = new Node(item, nullptr, nullptr);
        } else {
            tail_ = tail_->next = new Node(item, tail_, nullptr);
        }
        ++size_;
    }

    void push_front(int item) {
        if (size_ == 0) {
            head_ = tail_ = new Node(item, nullptr, nullptr);
        } else {
            head_ = head_->prev = new Node(item, nullptr, head_);
        }
        ++size_;
    }

    void pop_front() {
        Node* ptr = head_;
        head_ = head_->next;
        if (size_ == 1) {
            tail_ = nullptr;
        } else {
            head_->prev = nullptr;
        }
        delete ptr;
        --size_;
    }

    void pop_back() {
        Node* ptr = tail_;
        tail_ = tail_->prev;
        if (size_ == 1) {
            head_ = nullptr;
        } else {
            tail_->next = nullptr;
        }
        delete ptr;
        --size_;
    }

    size_t size() const {
        return size_;
    }

    class ConstIterator {
        const List* list_ptr;
        Node* i_;

    public:
        ConstIterator(const List* value, Node* i) : list_ptr(value), i_(i) {}

        ConstIterator& operator++ () {
            i_ = i_->next;
            return *this;
        }

        ConstIterator& operator-- () {
            i_ = (i_ != nullptr ? i_->prev : list_ptr->tail_);
            return *this;
        }

        int operator*() const {
            return i_-> value;
        }

        bool operator == (const ConstIterator &rhs) const {
            return ((i_ ==rhs.i_) && (list_ptr == rhs.list_ptr));
        }

        bool operator != (const ConstIterator &rhs) const {
            return ((i_ != rhs.i_) || (list_ptr != rhs.list_ptr));
        }
    };

    ConstIterator begin() const {
        return ConstIterator(this, head_);
    }

    ConstIterator end() const {
        return ConstIterator(this, tail_->next);
    }
};
