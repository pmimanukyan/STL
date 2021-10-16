#pragma once

#include <type_traits>
#include <utility>

/// not ebo
template <typename T, std::size_t I, bool Empty = std::is_empty_v<T> && !std::is_final_v<T>>
struct CompressedPairElement {
    T value_{};

    template <typename Q>
    CompressedPairElement(Q&& first) : value_(std::forward<Q>(first)){};
    CompressedPairElement() = default;

    T& Get() {
        return value_;
    }

    const T& Get() const {
        return value_;
    }
};

/// ebo
template <typename T, std::size_t I>
struct CompressedPairElement<T, I, true> : public T {
    template <typename Q>
    CompressedPairElement(Q&& first) {};
    CompressedPairElement() = default;

    T& Get() {
        return *this;
    }

    const T& Get() const {
        return *this;
    }
};

template <typename F, typename S>
class CompressedPair : private CompressedPairElement<F, 0>, private CompressedPairElement<S, 1> {
public:
    template <typename Q, typename U>
    CompressedPair(Q&& first, U&& second)
        : First(std::forward<Q>(first)), Second(std::forward<U>(second)) {
    }

    CompressedPair() = default;

    const F& GetFirst() const {
        return First::Get();
    }
    
    const S& GetSecond() const {
        return Second::Get();
    }
    
    F& GetFirst() {
        return First::Get();
    }
    
    S& GetSecond() {
        return Second::Get();
    }

private:
    using First = CompressedPairElement<F, 0>;
    using Second = CompressedPairElement<S, 1>;
};
