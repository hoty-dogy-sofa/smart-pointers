#ifndef UNIQUE_PTR_HPP
#define UNIQUE_PTR_HPP

#include <utility>

namespace ptrs::unique {

template <typename T>
class default_deleter {
public:
    void operator()(T *example) const noexcept {
        delete example;
    }
};

template <typename T, typename Deleter = default_deleter<T>>
class unique_ptr {
private:
    T *ptr;
    Deleter deleter;

public:
    unique_ptr() noexcept : ptr(nullptr), deleter() {
    }

    explicit unique_ptr(T *new_ptr) noexcept : ptr(new_ptr), deleter() {
    }

    unique_ptr(T *new_ptr, Deleter new_deleter) noexcept
        : ptr(new_ptr), deleter(std::move(new_deleter)) {
    }

    explicit unique_ptr(std::nullptr_t) noexcept : ptr(nullptr), deleter() {
    }

    unique_ptr(const unique_ptr &) = delete;
    unique_ptr &operator=(const unique_ptr &) = delete;

    unique_ptr(unique_ptr &&other) noexcept
        : ptr(std::exchange(other.ptr, nullptr)),
          deleter(std::move(other.deleter)) {
    }

    unique_ptr &operator=(unique_ptr &&other) noexcept {
        if (this != &other) {
            auto old_ptr =
                std::exchange(ptr, std::exchange(other.ptr, nullptr));
            deleter = std::move(other.deleter);
            if (old_ptr) {
                deleter(old_ptr);
            }
        }
        return *this;
    }

    ~unique_ptr() {
        if (ptr) {
            deleter(ptr);
        }
    }

    explicit operator bool() const noexcept {
        return ptr != nullptr;
    }

    [[nodiscard]] T *get() const noexcept {
        return ptr;
    }

    [[nodiscard]] T *release() noexcept {
        return std::exchange(ptr, nullptr);
    }

    void reset() noexcept {
        auto old_ptr = std::exchange(ptr, nullptr);
        if (old_ptr) {
            deleter(old_ptr);
        }
    }

    void reset(T *new_ptr) noexcept {
        auto old_ptr = std::exchange(ptr, new_ptr);
        if (old_ptr) {
            deleter(old_ptr);
        }
    }

    [[nodiscard]] T *operator->() const noexcept {
        return ptr;
    }

    [[nodiscard]] T &operator*() const noexcept {
        return *ptr;
    }

    friend bool
    operator==(const unique_ptr &lhs, const unique_ptr &rhs) noexcept {
        return lhs.get() == rhs.get();
    }

    friend bool
    operator!=(const unique_ptr &lhs, const unique_ptr &rhs) noexcept {
        return !(lhs == rhs);
    }

    friend void swap(unique_ptr &lhs, unique_ptr &rhs) noexcept {
        std::swap(lhs.ptr, rhs.ptr);
        std::swap(lhs.deleter, rhs.deleter);
    }
};

}  // namespace ptrs::unique

#endif  // UNIQUE_PTR_HPP
