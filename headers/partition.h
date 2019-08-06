#pragma once
#include <unordered_set>
#include <vector>

struct Constraint;

struct Partition {
    std::unordered_set<Constraint *> constraints;
    std::vector<int> holes;
    int badness = 0;

    bool operator==(Partition &other) const
    {
        return this->constraints == other.constraints;
    }
};
