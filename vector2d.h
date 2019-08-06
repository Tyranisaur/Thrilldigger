#pragma once
#include <vector>

template <class T> class Vector2d
{
public:
    Vector2d(size_t height, size_t width, const T &value = T())
        : width(int(width)), vector_(height * width, value)
    {
    }
    const T &at(size_t x, size_t y) const & { return vector_[y * width + x]; }
    T get(size_t x, size_t y) const & { return vector_[y * width + x]; }
    T &ref(size_t x, size_t y) & { return vector_[y * width + x]; }
    void set(size_t x, size_t y, const T &value) &
    {
        vector_[y * width + x] = value;
    }

    auto begin() { return vector_.begin(); }
    auto end() { return vector_.end(); }
    auto begin() const { return vector_.begin(); }
    auto end() const { return vector_.end(); }

private:
    const int width = 0;
    std::vector<T> vector_;
};
