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
    bool started;
    uint64_t choose(uint64_t n, uint64_t k);
    QList<Partition*>* partitionList;
    uint64_t weight;
    int sumBadSpots;
    int arrayLength;
    int * maxAmountsPerPartition;
    bool ** badSpots;
};

#endif // PARTITIONITERATOR_H
