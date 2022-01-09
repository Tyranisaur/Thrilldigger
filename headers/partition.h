#pragma once
#include <unordered_set>
#include <vector>

struct Constraint;

struct Partition {
    std::unordered_set<Constraint *> constraints;
    std::vector<std::size_t> holes;
    std::size_t badness = 0;

    friend bool operator==(const Partition &l, const Partition &r)
    {
        return l.constraints == r.constraints;
    }
};
