#pragma once

#include "compressed_pair.h"
#include <utility>
#include <functional>
#include <cstddef>  // std::nullptr_t

template <typename T>
struct Slug {
    void operator()(T* ptr) {
        delete ptr;
    }
};

template <typename Y>
struct Slug<Y[]> {
    void operator()(Y* ptr) {
        delete[] ptr;
    }
};
// Primary template
template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) {
        pair_.GetFirst() = ptr;
    }

    template <typename Deleter1>
    UniquePtr(T* ptr, Deleter1&& deleter) noexcept : pair_(ptr, std::forward<Deleter1>(deleter)) {
    }

    template <typename S, typename SDeleter>
    UniquePtr(UniquePtr<S, SDeleter>&& other) noexcept
        : pair_(other.pair_.GetFirst(), std::forward<SDeleter>(other.pair_.GetSecond())) {
        other.pair_.GetFirst() = nullptr;
    }

    UniquePtr(UniquePtr&& other) noexcept {
        pair_.GetFirst() = other.pair_.GetFirst();
        pair_.GetSecond() = std::forward<Deleter>(other.pair_.GetSecond());
        other.pair_.GetFirst() = nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (other.pair_.GetFirst() == pair_.GetFirst()) {
            return *this;
        } else {
            pair_.GetSecond()(pair_.GetFirst());
            pair_.GetFirst() = nullptr;
            std::swap(pair_.GetFirst(), other.pair_.GetFirst());
            pair_.GetSecond() = std::forward<Deleter>(other.pair_.GetSecond());
            return *this;
        }
    }

    UniquePtr& operator=(std::nullptr_t other) {
        delete pair_.GetFirst();
        pair_.GetFirst() = other;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        pair_.GetSecond()(pair_.GetFirst());
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* ptr = pair_.GetFirst();
        pair_.GetFirst() = nullptr;
        return ptr;
    }
    
    void Reset(T* ptr = nullptr) {
        UniquePtr tmp(pair_.GetFirst());
        pair_.GetFirst() = ptr;
    }

    void Swap(UniquePtr& other) {
        std::swap(pair_.GetFirst(), other.pair_.GetFirst());
        std::swap(pair_.GetSecond(), other.pair_.GetSecond());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return pair_.GetFirst();
    }

    T*& GetPtr() {
        return pair_.GetFirst();
    }

    T* GetPtr() const {
        return pair_.GetFirst();
    }
    
    Deleter& GetDeleter() {
        return pair_.GetSecond();
    }
    
    const Deleter& GetDeleter() const {
        return pair_.GetSecond();
    }
    
    explicit operator bool() const {
        if (pair_.GetFirst() == nullptr) {
            return false;
        } else {
            return true;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    typename std::add_lvalue_reference<T>::type operator*() const {
        return *pair_.GetFirst();
    }
    
    T* operator->() const {
        return pair_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> pair_;
    template <typename U, typename Q>
    friend class UniquePtr;
};

//// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) {
        pair_.GetFirst() = ptr;
    }

    template <typename Deleter1>
    UniquePtr(T* ptr, Deleter1&& deleter) noexcept : pair_(ptr, std::forward<Deleter1>(deleter)) {
    }

    template <typename S, typename SDeleter>
    UniquePtr(UniquePtr<S, SDeleter>&& other) noexcept {
        if (pair_.GetFirst() != other.pair_.GetFirst()) {
            pair_.GetFirst() = other.pair_.GetFirst();
            pair_.GetSecond() = std::forward<SDeleter>(other.pair_.GetSecond());
            other.Release();
        }
    }

    UniquePtr(UniquePtr&& other) noexcept {
        pair_.GetFirst() = other.pair_.GetFirst();
        pair_.GetSecond() = std::forward<Deleter>(other.pair_.GetSecond());
        other.pair_.GetFirst() = nullptr;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (other.pair_.GetFirst() == pair_.GetFirst()) {
            return *this;
        } else {
            pair_.GetSecond()(pair_.GetFirst());
            pair_.GetFirst() = nullptr;
            std::swap(pair_.GetFirst(), other.pair_.GetFirst());
            pair_.GetSecond() = std::forward<Deleter>(other.pair_.GetSecond());
            return *this;
        }
    }

    UniquePtr& operator=(std::nullptr_t other) {
        delete pair_.GetFirst();
        pair_.GetFirst() = other;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        pair_.GetSecond()(pair_.GetFirst());
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* ptr = pair_.GetFirst();
        pair_.GetFirst() = nullptr;
        return ptr;
    }
    
    void Reset(T* ptr = nullptr) {
        UniquePtr tmp(pair_.GetFirst());
        pair_.GetFirst() = ptr;
    }

    void Swap(UniquePtr& other) {
        std::swap(pair_.GetFirst(), other.pair_.GetFirst());
        std::swap(pair_.GetSecond(), other.pair_.GetSecond());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return pair_.GetFirst();
    }

    T*& GetPtr() {
        return pair_.GetFirst();
    }

    T* GetPtr() const {
        return pair_.GetFirst();
    }
    Deleter& GetDeleter() {
        return pair_.GetSecond();
    }

    const Deleter& GetDeleter() const {
        return pair_.GetSecond();
    }

    explicit operator bool() const {
        if (pair_.GetFirst() == nullptr) {
            return false;
        } else {
            return true;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    typename std::add_lvalue_reference<T>::type operator*() const {
        return *pair_.GetFirst();
    }

    T* operator->() const {
        return pair_.GetFirst();
    }

    T& operator[](size_t i) {
        return pair_.GetFirst()[i];
    }

private:
    CompressedPair<T*, Deleter> pair_;
    template <typename U, typename Q>
    friend class UniquePtr;
};
