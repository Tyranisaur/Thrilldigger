#include "partitioniterator.h"
#include <QList>
#include "partition.h"
#include "hole.h"
#include <algorithm>
#include <QSetIterator>
#include "constraint.h"

PartitionIterator::PartitionIterator(QList<Partition*>* partitionList, bool ** badSpots)
{
    this->partitionList = partitionList;
    arrayLength = partitionList->size();
    maxAmountsPerPartition = new int[arrayLength];
    minAmountsPerPartition = new int[arrayLength];
    this->badSpots = badSpots;
    sumBadSpots = 0;
    weight = 1;
    QSetIterator<Constraint*> * it;
    Constraint * constraint;
    Partition * partition;
    for(int i = 0; i < arrayLength; i++)
    {
        partition = partitionList->at(i);
        maxAmountsPerPartition[i] = partition->holes->size();
        it = new QSetIterator<Constraint*>(*(partition->constraints));
        minAmountsPerPartition[i] = 0;
        while(it->hasNext())
        {
            constraint = it->next();
            maxAmountsPerPartition[i] =
                    std::min(maxAmountsPerPartition[i],
                             constraint->maxBadness);
            minAmountsPerPartition[i] =
                    std::max(minAmountsPerPartition[i],
                             constraint->maxBadness - 1
                             - (constraint->holes.size() - partition->holes->size()));
        }
        delete it;
        partition->badness = minAmountsPerPartition[i];
        sumBadSpots += minAmountsPerPartition[i];
        weight *= choose(partition->holes->size(), partition->badness);
        for(int j = 0; j < minAmountsPerPartition[i]; j++)
        {
            badSpots[partition->holes->at(j)->y][partition->holes->at(j)->x] = true;
        }
        for(int j = minAmountsPerPartition[i]; j < partition->holes->size(); j++)
        {
            badSpots[partition->holes->at(j)->y][partition->holes->at(j)->x] = false;
        }
    }
    started = false;
}

PartitionIterator::~PartitionIterator()
{
    delete maxAmountsPerPartition;
    delete minAmountsPerPartition;
}

bool PartitionIterator::hasNext()
{
    for(int i = 0; i < arrayLength; i++)
    {
        if(maxAmountsPerPartition[i] > partitionList->at(i)->badness)
        {
            return true;
        }
    }
    return false;
}

void PartitionIterator::iterate(uint64_t* iterationWeight, int* badness)
{
    if(!started)
    {
        started = true;
        *iterationWeight = weight;
        *badness = sumBadSpots;
        return;
    }
    Hole * hole;
    Partition * partition;
    for(int i = arrayLength - 1; i >= 0; i--)
    {
        partition = partitionList->at(i);
        if(partition->badness < maxAmountsPerPartition[i])
        {
            hole = partition->holes->at(partition->badness);
            weight /= choose(partition->holes->size(), partition->badness);
            partition->badness++;
            badSpots[hole->y][hole->x] = true;
            sumBadSpots++;
            weight *= choose(partition->holes->size(), partition->badness);
            for( i += 1; i < arrayLength; i++)
            {
                partition = partitionList->at(i);
                weight /= choose(partition->holes->size(), partition->badness);
                sumBadSpots -= (partition->badness - minAmountsPerPartition[i]);

                partition->badness = minAmountsPerPartition[i];
                weight *= choose(partition->holes->size(), partition->badness);

                for(int j = minAmountsPerPartition[i]; j < partition->holes->size(); j++)
                {
                    hole = partition->holes->at(j);
                    badSpots[hole->y][hole->x] = false;
                }
            }
            *iterationWeight = weight;
            *badness = sumBadSpots;
            return;
        }
    }
    return;
}

uint64_t PartitionIterator::choose(uint64_t n, uint64_t k) {
    if (k > n ) {
        return 0;
    }
    uint64_t r = 1;
    for (uint64_t d = 1; d <= k; ++d) {
        r *= n--;
        r /= d;
    }
    return r;
}
