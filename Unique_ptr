#include <algorithm>
#include <cstddef>
#include <memory>
#include <tuple>

template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr {
    std::tuple<T*, Deleter> data;

public:
    UniquePtr() noexcept {
        std::get<0>(data) = nullptr;
    }

    UniquePtr(T* ptr) noexcept {
        std::get<0>(data) = ptr;
    }

    UniquePtr(T* ptr, Deleter deleter) noexcept {
        std::get<0>(data) = ptr;
        std::get<1>(data) = deleter;
    }

    UniquePtr(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& that) noexcept {
        swap(that);
    }

    UniquePtr& operator=(std::nullptr_t) noexcept {
        std::get<1>(data)(std::get<0>(data));
        std::get<0>(data) = nullptr;
        return *this;
    }

    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr& operator=(UniquePtr&& rhs) noexcept {
        swap(rhs);
        return *this;
    }

    T& operator*() const noexcept {
        return *std::get<0>(data);
    }

    T* operator->() const noexcept {
        return std::get<0>(data);
    }

    T* release() noexcept {
        T* ptr = nullptr;
        std::swap(std::get<0>(data), ptr);
        return ptr;
    }

    void reset(T* ptr) noexcept {
        std::get<1>(data)(std::get<0>(data));
        std::swap(std::get<0>(data), ptr);
    }

    void swap(UniquePtr& rhs) noexcept {
        std::swap(std::get<0>(data), std::get<0>(rhs.data));
    }

    T* get() const noexcept {
        return std::get<0>(data);
    }

    explicit operator bool() const noexcept {
        return std::get<0>(data) != nullptr;
    }

    Deleter& get_deleter() noexcept {
        return std::get<1>(data);
    }

    const Deleter& get_deleter() const noexcept {
        return std::get<1>(data);
    }

    ~UniquePtr() noexcept {
        std::get<1>(data)(std::get<0>(data));
    }
};
