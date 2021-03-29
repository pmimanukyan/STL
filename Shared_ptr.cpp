#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>

template<typename T>
class SharedPtr {
private:
    T* ptr;
    size_t* k;

    void dec() noexcept {
        if (nullptr == ptr) {
            return;
        }
        if ((*k) == 1) {
            delete k;
            delete ptr;
            return;
        }
        --(*k);
    }

    void inc() {
        if (nullptr != ptr) {
            if (nullptr == k) {
                k = new size_t(0);
            }
            ++(*k);
        }
    }

public:
    SharedPtr() : ptr(nullptr), k(nullptr) {}

    SharedPtr(T* ptr) : ptr(ptr), k(nullptr) {
        inc();
    }

    SharedPtr(const SharedPtr& rhs) : ptr(rhs.ptr), k(rhs.k) {
        inc();
    }

    SharedPtr(SharedPtr&& rhs) : ptr(rhs.ptr), k(rhs.k) {
        rhs.ptr = nullptr;
        rhs.k = nullptr;
    }

    SharedPtr& operator= (const SharedPtr& rhs) {
        if (ptr != rhs.ptr) {
            dec();
            k = rhs.k;
            ptr = rhs.ptr;
            inc();
        }
        return *this;
    }

    SharedPtr& operator= (SharedPtr&& rhs) {
        if (ptr != rhs.ptr) {
            dec();
            k = rhs.k;
            ptr = rhs.ptr;
            rhs.ptr = nullptr;
            rhs.k = nullptr;
        }
        return *this;
    }

    T& operator*() {
        return *ptr;
    }

    const T& operator*() const {
        return *ptr;
    }

    const T* operator->() const {
        return ptr;
    }

    T* get() const {
        return ptr;
    }

    operator bool() const noexcept {
        return (nullptr != ptr);
    }

    void reset(T* p) {
        if (ptr != p) {
            dec();
            ptr = p;
            k = nullptr;
            inc();
        }
    }

    void swap(SharedPtr& other) noexcept {
        std::swap(ptr, other.ptr);
        std::swap(k, other.k);
    }

    ~SharedPtr() {
        dec();
    }
};
