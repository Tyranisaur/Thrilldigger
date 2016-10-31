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
            QList<Partition*>* sunkenPartitions,
            int numBadSpots);

    ~PartitionIterator();
    bool hasNext();
    void iterate(double * weight);

private:
    double choose(int n, int k);
    QList<Partition*>* partitionList;
    double weight;
    int * indexArray;
    int indexArrayLength;
    int sumBadSpots;
    int totalBadness;
    int listLength;
    bool started;
    QList<int> maxAmountsPerPartition;
    QList<int> minAmountsPerPartition;
    bool ** badSpots;
};

#endif // PARTITIONITERATOR_H
