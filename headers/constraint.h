#pragma once
#include <vector>

struct Constraint {
    int maxBadness = 0;
    std::vector<int> holes;
};
