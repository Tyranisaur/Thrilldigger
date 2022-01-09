#pragma once
#include <vector>

struct Constraint {
    std::size_t maxBadness = 0;
    std::vector<std::size_t> holes;
};
