#include "partitioniterator.h"
#include <QList>
#include "partition.h"
#include "hole.h"
#include <algorithm>
#include <QSetIterator>
#include "constraint.h"

PartitionIterator::PartitionIterator(
        QList<Partition*>* partitionList,
        bool ** badSpots,
        QList<Partition*>* sunkenPartitions)
{
    this->partitionList = partitionList;
    this->badSpots = badSpots;
    sumBadSpots = 0;
    QSetIterator<Constraint*> * it;
    Constraint * constraint;
    Partition * partition;
    int maxAmount;
    int minAmount;
    for(int i = partitionList->size() - 1; i >= 0; i--)
    {
        partition = partitionList->at(i);
        maxAmount = partition->holes->size();
        it = new QSetIterator<Constraint*>(*(partition->constraints));
        minAmount = 0;
        while(it->hasNext())
        {
            constraint = it->next();
            maxAmount =
                    std::min(maxAmount,
                             constraint->maxBadness);
            minAmount =
                    std::max(minAmount,
                             constraint->maxBadness - 1
                             - (constraint->holes.size() - partition->holes->size()));
        }
        delete it;
        for(int j = 0; j < minAmount; j++)
        {
            badSpots[partition->holes->at(j)->y][partition->holes->at(j)->x] = true;
        }
        for(int j = minAmount; j < partition->holes->size(); j++)
        {
            badSpots[partition->holes->at(j)->y][partition->holes->at(j)->x] = false;
        }
        sumBadSpots += minAmount;
        partition->badness = minAmount;
        if(maxAmount == minAmount)
        {
            partitionList->removeOne(partition);
            sunkenPartitions->append(partition);
            continue;
        }
        weights.prepend(choose(partition->holes->size(), partition->badness));
        minAmountsPerPartition.prepend(minAmount);
        maxAmountsPerPartition.prepend(maxAmount);

    }
    listLength = partitionList->size();
}

PartitionIterator::~PartitionIterator()
{

}

bool PartitionIterator::hasNext()
{
    Hole * hole;
    Partition * partition;
    for(int i = listLength - 1; i >= 0; i--)
    {
        partition = partitionList->at(i);
        if(partition->badness < maxAmountsPerPartition.at(i))
        {
            hole = partition->holes->at(partition->badness);
            partition->badness++;
            badSpots[hole->y][hole->x] = true;
            sumBadSpots++;
            weights[i] = choose(partition->holes->size(), partition->badness);
            for( i += 1; i < listLength; i++)
            {
                partition = partitionList->at(i);
                sumBadSpots -= (partition->badness - minAmountsPerPartition.at(i));
                partition->badness = minAmountsPerPartition.at(i);
                weights[i] = choose(partition->holes->size(), partition->badness);

                for(int j = minAmountsPerPartition.at(i); j < maxAmountsPerPartition.at(i); j++)
                {
                    hole = partition->holes->at(j);
                    badSpots[hole->y][hole->x] = false;
                }
            }
            return true;
        }
    }
    return false;
}

void PartitionIterator::iterate(uint64_t* iterationWeight, int* badness)
{
    *iterationWeight = 1;
    for(int i = 0; i < listLength; i++)
    {
        *iterationWeight *= weights.at(i);
    }
    *badness = sumBadSpots;

}

int PartitionIterator::choose(int n, int k) {
    if (k > n ) {
        return 0;
    }
    int r = 1;
    for (int d = 1; d <= k; ++d) {
        r *= n--;
        r /= d;
    }
    return r;
}
