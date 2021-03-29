#include <iostream>
#include <cstddef>
#include <memory>
#include <utility>

template <typename T>
class Vector {
    struct VectorMemory {
        char *bytes;
        size_t capacity;
        VectorMemory();
        VectorMemory(size_t capacity);
        void construct(size_t start, size_t count);
        void copy(const VectorMemory &memory, size_t count);
        void move(const VectorMemory &memory, size_t count);
        void destroy(size_t start, size_t count);
        void swap(VectorMemory &memory);
        T& operator[](size_t index);
        const T& operator[](size_t index) const;
        T* begin();
        const T* begin() const;
        ~VectorMemory();
    };
    VectorMemory values;
    size_t count;

public:
    Vector();
    Vector(size_t size);
    Vector(const Vector& vector);
    Vector& operator=(const Vector& vector);
    void reserve(size_t size);
    void resize(size_t size);
    void clear();
    void push_back(const T &value);
    void push_back(T &&value);
    void pop_back();
    void swap(Vector<T> &vector);
    size_t size() const;
    size_t capacity() const;
    T* begin();
    const T* begin() const;
    T* end();
    const T* end() const;
    T& operator[](size_t index);
    const T& operator[](size_t index) const;
    ~Vector();
};

template <typename T>
Vector<T>::VectorMemory::VectorMemory() {
    bytes = nullptr;
    capacity = 0;
}

template <typename T>
Vector<T>::VectorMemory::VectorMemory(size_t capacity) {
    this->capacity = capacity;
    this->bytes = new char[capacity * sizeof(T)];
}

template <typename T>
void Vector<T>::VectorMemory::construct(size_t start, size_t count) {
    std::uninitialized_value_construct_n(reinterpret_cast<T*>(bytes) + start, count);
}

template <typename T>
void Vector<T>::VectorMemory::copy(const VectorMemory &memory, size_t count) {
    std::uninitialized_copy_n(reinterpret_cast<T*>(memory.bytes), count,
                              reinterpret_cast<T*>(bytes));
}

template <typename T>
void Vector<T>::VectorMemory::move(const VectorMemory &memory, size_t count) {
    std::uninitialized_move_n(reinterpret_cast<T*>(memory.bytes), count,
                              reinterpret_cast<T*>(bytes));
}

template <typename T>
void Vector<T>::VectorMemory::destroy(size_t start, size_t count) {
    if (count == 1) {
        std::destroy_at(reinterpret_cast<T*>(bytes) + start);
    } else {
        std::destroy_n(reinterpret_cast<T*>(bytes) + start, count);
    }
}

template <typename T>
void Vector<T>::VectorMemory::swap(VectorMemory &memory) {
    char *tmpBytes = bytes;
    bytes = memory.bytes;
    memory.bytes = tmpBytes;

    size_t tmpCapacity = capacity;
    capacity = memory.capacity;
    memory.capacity = tmpCapacity;
}

template <typename T>
T& Vector<T>::VectorMemory::operator[](size_t index) {
    return reinterpret_cast<T*>(bytes)[index];
}

template <typename T>
const T& Vector<T>::VectorMemory::operator[](size_t index) const {
    return reinterpret_cast<const T*>(bytes)[index];
}

template <typename T>
T* Vector<T>::VectorMemory::begin() {
    return reinterpret_cast<T*>(bytes);
}

template <typename T>
const T* Vector<T>::VectorMemory::begin() const {
    return reinterpret_cast<const T*>(bytes);
}

template <typename T>
Vector<T>::VectorMemory::~VectorMemory() {
    delete[] bytes;
}

template <typename T>
Vector<T>::Vector() {
    count = 0;
}

template <typename T>
Vector<T>::Vector(size_t size) : values(size) {
    values.construct(0, size);
    count = size;
}

template <typename T>
Vector<T>::Vector(const Vector& vector) : values(vector.count) {
    values.copy(vector.values, vector.count);
    count = vector.count;
}

template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& vector) {
    if (&vector == this) {
        return *this;
    }
    Vector<T> copy(vector);
    swap(copy);
    return *this;
}

template <typename T>
void Vector<T>::reserve(size_t size) {
    if (size <= values.capacity)
        return;

    VectorMemory tmp(size);
    tmp.move(values, count);
    values.destroy(0, count);
    tmp.swap(values);
}

template <typename T>

void Vector<T>::resize(size_t size) {
    reserve(size);

    if (size > count) {
        values.construct(count, size - count);
    } else if (size < count) {
        values.destroy(size, count - size);
    }

    count = size;
}

template <typename T>
void Vector<T>::clear() {
    values.destroy(0, count);
    count = 0;
}

template <typename T>
void Vector<T>::push_back(const T &value) {
    if (count == values.capacity) {
        reserve(count ? count * 2 : 1);
    }
    new (reinterpret_cast<T*>(values.bytes) + count) T(value);
    count++;
}

template <typename T>
void Vector<T>::push_back(T &&value) {
    if (count == values.capacity) {
        reserve(count ? count * 2 : 1);
    }

    new (reinterpret_cast<T*>(values.bytes) + count) T(std::move(value));
    count++;
}

template <typename T>
void Vector<T>::pop_back() {
    values.destroy(count - 1, 1);
    count--;
}

template <typename T>
void Vector<T>::swap(Vector<T> &vector) {
    values.swap(vector.values);

    size_t tmp = count;
    count = vector.count;
    vector.count = tmp;
}

template <typename T>
size_t Vector<T>::size() const {
    return count;
}

template <typename T>
size_t Vector<T>::capacity() const {
    return values.capacity;
}

template <typename T>
T* Vector<T>::begin() {
    return values.begin();
}

template <typename T>
const T*  Vector<T>::begin() const {
    return values.begin();
}

template <typename T>
T* Vector<T>::end() {
    return values.begin() + count;
}

template <typename T>
const T* Vector<T>::end() const {
    return values.begin() + count;
}

template <typename T>
T& Vector<T>::operator[](size_t index) {
    return values[index];
}

template <typename T>
const T& Vector<T>::operator[](size_t index) const {
    return values[index];
}

template <typename T>
Vector<T>::~Vector() {
    values.destroy(0, count);
}  // nice
