#pragma once

#include <cstddef>  // std::nullptr_t
#include <utility>
#include <algorithm>

class ControlBlockBase {
public:
    size_t ref_counter_{};

    virtual ~ControlBlockBase() = default;
};

template <typename U>
class ControlBlockPointer : public ControlBlockBase {
public:
    U* ptr_;
    ControlBlockPointer(U* ptr) : ptr_(ptr) {
    }
    ~ControlBlockPointer() {
        delete ptr_;
    }
};

template <typename U>
class ControlBlockHolder : public ControlBlockBase {
public:
    template <typename... Args>
    ControlBlockHolder(Args&&... args) {
        new (&storage_) U(std::forward<Args>(args)...);
        ++ref_counter_;
    }

    U* GetRawPtr() {
        return reinterpret_cast<U*>(&storage_);
    }

    ~ControlBlockHolder() {
        reinterpret_cast<U*>(&storage_)->~U();
    }

    std::aligned_storage_t<sizeof(U), alignof(U)> storage_;
};
// https://en.cppreference.com/w/cpp/memory/shared_ptr
template <typename T>
class SharedPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    SharedPtr() = default;

    SharedPtr(std::nullptr_t) : ptr_(nullptr), block_(nullptr) {
    }

    template <typename U>
    explicit SharedPtr(U* ptr) : ptr_(ptr), block_(new ControlBlockPointer<U>(ptr)) {
        Increasing();
    }

    SharedPtr(const SharedPtr& other) : ptr_(other.ptr_), block_(other.block_) {
        Increasing();
    }

    template <typename U>
    SharedPtr(const SharedPtr<U>& other) : ptr_(other.ptr_), block_(other.block_) {
        Increasing();
    }

    SharedPtr(SharedPtr&& other) : ptr_(other.ptr_), block_(other.block_) {
        other.ptr_ = nullptr;
        other.block_ = nullptr;
    }

    template <typename U>
    SharedPtr(SharedPtr<U>&& other) : ptr_(other.ptr_), block_(other.block_) {
        other.ptr_ = nullptr;
        other.block_ = nullptr;
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename U>
    SharedPtr(const SharedPtr<U>& other, T* ptr) : ptr_(ptr), block_(other.block_) {
        Increasing();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s
    SharedPtr& operator=(const SharedPtr& other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        Decrasing();
        ptr_ = other.ptr_;
        block_ = other.block_;
        Increasing();
        return *this;
    }

    template <typename U>
    SharedPtr& operator=(const SharedPtr<U>& other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        Decrasing();
        ptr_ = other.ptr_;
        block_ = other.block_;
        Increasing();
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        Decrasing();
        ptr_ = other.ptr_;
        block_ = other.block_;
        other.ptr_ = nullptr;
        other.block_ = nullptr;
        return *this;
    }

    template <typename U>
    SharedPtr& operator=(SharedPtr<U>&& other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        Decrasing();
        ptr_ = other.ptr_;
        block_ = other.block_;
        other.ptr_ = nullptr;
        other.block_ = nullptr;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() noexcept {
        Decrasing();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        Decrasing();
        ptr_ = nullptr;
        block_ = nullptr;
        Increasing();
    }

    void Reset(T* ptr) {
        Decrasing();
        ptr_ = ptr;
        block_ = new ControlBlockPointer<T>(ptr);
        Increasing();
    }

    template <typename U>
    void Reset(U* ptr) {
        Decrasing();
        ptr_ = ptr;
        block_ = new ControlBlockPointer<U>(ptr);
        Increasing();
    }

    void Swap(SharedPtr& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(block_, other.block_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return ptr_;
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

    size_t UseCount() const {
        return block_ == nullptr ? 0 : block_->ref_counter_;
    }

    explicit operator bool() const {
        if (ptr_) {
            return true;
        } else {
            return false;
        }
    }

    void Decrasing() {
        if (block_ != nullptr) {
            if (block_->ref_counter_ == 1) {
                delete block_;
                return;
            }
            --block_->ref_counter_;
        }
    }

    void Increasing() {
        if (ptr_ != nullptr || block_ != nullptr) {
            ++block_->ref_counter_;
        }
    }

private:
    T* ptr_ = nullptr;
    ControlBlockBase* block_ = nullptr;

    template <typename Y, typename... Args>
    friend SharedPtr<Y> MakeShared(Args&&... args);

    template <typename U>
    friend class SharedPtr;
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right);

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    SharedPtr<T> sp;
    auto block = new ControlBlockHolder<T>(std::forward<Args>(args)...);
    sp.block_ = block;
    sp.ptr_ = block->GetRawPtr();
    return sp;
}

// Look for usage examples in tests
template <typename T>
class EnableSharedFromThis {
public:
    SharedPtr<T> SharedFromThis();
    SharedPtr<const T> SharedFromThis() const;
};
