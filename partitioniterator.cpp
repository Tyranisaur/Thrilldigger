#include "partitioniterator.h"
#include <QList>
#include "partition.h"
#include "hole.h"
#include <algorithm>

PartitionIterator::PartitionIterator(QList<Partition*>* partitionList, bool ** badSpots)
{
    this->partitionList = partitionList;
    arrayLength = partitionList->size();
    maxAmountsPerPartition = new int[arrayLength];
    this->badSpots = badSpots;
    for(int i = 0; i < arrayLength; i++)
    {
        maxAmountsPerPartition[i] = partitionList->at(i)->holes->size();
        partitionList->at(i)->badness = 0;
        for(int j = 0; j < partitionList->at(i)->holes->size(); j++)
        {
            badSpots[partitionList->at(i)->holes->at(j)->y][partitionList->at(i)->holes->at(j)->x] = false;
        }
    }
    started = false;
    sumBadSpots = 0;
    weight = 1;
}

PartitionIterator::~PartitionIterator()
{
    delete maxAmountsPerPartition;
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
       *iterationWeight = 1;
       *badness = 0;
       return;
    }
    Hole * hole;
    for(int i = arrayLength - 1; i >= 0; i--)
    {
        if(partitionList->at(i)->badness < maxAmountsPerPartition[i])
        {
            hole = partitionList->at(i)->holes->at(partitionList->at(i)->badness);
            weight /= choose(maxAmountsPerPartition[i], partitionList->at(i)->badness);
            partitionList->at(i)->badness++;
            badSpots[hole->y][hole->x] = true;
            sumBadSpots++;
            weight *= choose(maxAmountsPerPartition[i], partitionList->at(i)->badness);
            for( i += 1; i < arrayLength; i++)
            {
                weight /= choose(maxAmountsPerPartition[i], partitionList->at(i)->badness);
                sumBadSpots -= partitionList->at(i)->badness;
                partitionList->at(i)->badness = 0;
                for(int j = 0; j < partitionList->at(i)->holes->size(); j++)
                {
                    hole = partitionList->at(i)->holes->at(j);
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
