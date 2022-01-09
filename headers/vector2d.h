#pragma once
#include <vector>

template <class T> class Vector2d
{
public:
    Vector2d() = default;

    Vector2d(const std::size_t height, const std::size_t width, const T &value = T{})
        : width_(width), vector_(height * width, value)
    {
    }

    const T &at(const std::size_t x, const std::size_t y) const &
    {
        return vector_[std::size_t(y) * std::size_t(width_) + std::size_t(x)];
    }

    T get(const std::size_t x, const std::size_t y) const &
    {
        return vector_[std::size_t(y) * std::size_t(width_) + std::size_t(x)];
    }

    decltype(auto) ref(const std::size_t x, const std::size_t y) &
    {
        return vector_[std::size_t(y) * std::size_t(width_) + std::size_t(x)];
    }

    void set(const std::size_t x, const std::size_t y, const T &value) &
    {
        vector_[std::size_t(y) * std::size_t(width_) + std::size_t(x)] = value;
    }

    void reset(const T &value = T{})
    {
        std::fill(vector_.begin(), vector_.end(), value);
    }

    void reset(const std::size_t width, const std::size_t height, const T& value = T{}) {
        width_ = width;
        std::fill(vector_.begin(), vector_.end(), value);
        vector_.resize(width * height, value);
    }

    auto begin() { return vector_.begin(); }
    auto end() { return vector_.end(); }
    auto begin() const { return vector_.begin(); }
    auto end() const { return vector_.end(); }

    std::size_t width() const { return width_; }
    std::size_t height() const { return vector_.size() / width_; }

private:
    std::size_t width_ = 0;
    std::vector<T> vector_;
};
