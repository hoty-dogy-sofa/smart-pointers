#ifndef SHARED_PTR_HPP
#define SHARED_PTR_HPP

#include <utility>

namespace ptrs::shared {

template <typename T>
class shared_ptr {
private:
    T *ptr;
    int *counter;

    void cleanup() {
        if (counter != nullptr) {
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuse-after-free"
#endif
            (*counter)--;
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
            if (*counter == 0) {
                delete ptr;
                delete counter;
            }
            ptr = nullptr;
            counter = nullptr;
        }
    }

public:
    shared_ptr() noexcept : ptr(nullptr), counter(nullptr) {
    }

    explicit shared_ptr(T *new_ptr)
        : ptr(new_ptr), counter(new_ptr ? new int(1) : nullptr) {
    }

    explicit shared_ptr(std::nullptr_t) noexcept
        : ptr(nullptr), counter(nullptr) {
    }

    shared_ptr(const shared_ptr &other) noexcept
        : ptr(other.ptr), counter(other.counter) {
        if (counter != nullptr) {
            ++(*counter);
        }
    }

    shared_ptr(shared_ptr &&other) noexcept
        : ptr(std::exchange(other.ptr, nullptr)),
          counter(std::exchange(other.counter, nullptr)) {
    }

    shared_ptr &operator=(const shared_ptr &other) noexcept {
        if (this != &other) {
            cleanup();
            ptr = other.ptr;
            counter = other.counter;
            if (counter != nullptr) {
                ++(*counter);
            }
        }
        return *this;
    }

    shared_ptr &operator=(shared_ptr &&other) noexcept {
        if (this != &other) {
            cleanup();
            ptr = std::exchange(other.ptr, nullptr);
            counter = std::exchange(other.counter, nullptr);
        }
        return *this;
    }

    ~shared_ptr() {
        cleanup();
    }

    explicit operator bool() const noexcept {
        return ptr != nullptr;
    }

    [[nodiscard]] T *get() const noexcept {
        return ptr;
    }

    [[nodiscard]] int use_count() const noexcept {
        return counter != nullptr ? *counter : 0;
    }

    void reset() noexcept {
        cleanup();
    }

    void reset(T *new_ptr) {
        cleanup();
        ptr = new_ptr;
        counter = new_ptr ? new int(1) : nullptr;
    }

    [[nodiscard]] T *operator->() const noexcept {
        return ptr;
    }

    [[nodiscard]] T &operator*() const noexcept {
        return *ptr;
    }

    friend bool
    operator==(const shared_ptr &left, const shared_ptr &right) noexcept {
        return left.get() == right.get();
    }

    friend bool
    operator!=(const shared_ptr &left, const shared_ptr &right) noexcept {
        return left.get() != right.get();
    }

    friend void swap(shared_ptr &left, shared_ptr &right) noexcept {
        std::swap(left.ptr, right.ptr);
        std::swap(left.counter, right.counter);
    }
};

}  // namespace ptrs::shared

#endif  // SHARED_PTR_HPP
