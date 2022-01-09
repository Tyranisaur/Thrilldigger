#pragma once
#include <vector>

struct Partition;

class PartitionIterator
{
public:
    PartitionIterator(std::vector<Partition *> *partitionList,
                      std::vector<bool> &badspots,
                      std::vector<Partition *> *sunkenPartitions,
                      std::size_t numBadSpots);

    bool goToNextState();
    double stateWeight() const;

private:
    std::vector<Partition *> &partitionList_;
    double weight_ = 1.0;
    std::vector<std::size_t> indexArray_;
    std::size_t indexArrayLength_;
    std::size_t listLength_;
    bool started_ = false;
    std::vector<std::size_t> maxAmountsPerPartition_;
    std::vector<std::size_t> minAmountsPerPartition_;
    std::vector<bool> &badSpots_;
};
