#ifndef PARTITIONITERATOR_H
#define PARTITIONITERATOR_H
#include <QList>

struct Partition;

class PartitionIterator
{
public:
    PartitionIterator(QList<Partition*>* partitionList, bool ** badspots);
    ~PartitionIterator();
    bool hasNext();
    void iterate(uint64_t * weight, int * badness);

private:
    int choose(int n, int k);
    QList<Partition*>* partitionList;
    int * weights;
    int sumBadSpots;
    int arrayLength;
    int * maxAmountsPerPartition;
    int * minAmountsPerPartition;
    bool ** badSpots;
};

#endif // PARTITIONITERATOR_H
