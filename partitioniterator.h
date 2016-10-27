#ifndef PARTITIONITERATOR_H
#define PARTITIONITERATOR_H
#include <QList>

struct Partition;

class PartitionIterator
{
public:
    PartitionIterator(
            QList<Partition*>* partitionList,
            bool ** badspots,
            QList<Partition*>* sunkenPartitions);

    ~PartitionIterator();
    bool hasNext();
    void iterate(uint64_t * weight, int * badness);

private:
    int choose(int n, int k);
    QList<Partition*>* partitionList;
    QList<int> weights;
    int sumBadSpots;
    int listLength;
    QList<int> maxAmountsPerPartition;
    QList<int> minAmountsPerPartition;
    bool ** badSpots;
};

#endif // PARTITIONITERATOR_H
