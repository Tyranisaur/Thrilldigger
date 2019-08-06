#pragma once
#include <vector>

struct Partition;

class PartitionIterator
{
public:
    PartitionIterator(std::vector<Partition *> *partitionList,
                      std::vector<bool> &badspots,
                      std::vector<Partition *> *sunkenPartitions,
                      int numBadSpots);

    bool hasNext();
    double iterate();

private:
    double choose(int n, int k);
    std::vector<Partition *> &partitionList;
    double weight;
    std::vector<int> indexArray;
    int indexArrayLength;
    int listLength;
    bool started;
    std::vector<int> maxAmountsPerPartition;
    std::vector<int> minAmountsPerPartition;
    std::vector<bool> &badSpots;
};
