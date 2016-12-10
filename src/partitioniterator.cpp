#include "headers/partitioniterator.h"
#include <QList>
#include "headers/partition.h"
#include "headers/hole.h"
#include <algorithm>
#include <QSetIterator>
#include "headers/constraint.h"

PartitionIterator::PartitionIterator(
        QList<Partition*>* partitionList,
        bool ** badSpots,
        QList<Partition*>* sunkenPartitions,
        int numBadSpots)
{
    this->partitionList = partitionList;
    this->badSpots = badSpots;
    weight = 1.0;
    QSetIterator<Constraint*> * it;
    Constraint * constraint;
    Partition * partition;
    Hole * hole;
    int sunkenBadness = 0;
    int maxAmount;
    int minAmount;
    started = false;
    indexArrayLength = numBadSpots;
    int sumMax = 0;
    int sumMin = 0;
    bool unconstrainedPartition = partitionList->size() > 0 && partitionList->at(0)->constraints->size() == 0;
    int lowestIndex = unconstrainedPartition ? 1 : 0;
    for(int i = partitionList->size() - 1; i >= lowestIndex; i--)
    {
        partition = partitionList->at(i);
        maxAmount = partition->holes.size();
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
                             - (constraint->holes.size() - partition->holes.size()));
        }
        delete it;
        for(int j = 0; j < minAmount; j++)
        {
            badSpots[partition->holes.at(j)->y][partition->holes.at(j)->x] = true;
        }
        for(int j = minAmount; j < partition->holes.size(); j++)
        {
            badSpots[partition->holes.at(j)->y][partition->holes.at(j)->x] = false;
        }
        sumMin += minAmount;
        sumMax+= maxAmount;
        partition->badness = minAmount;
        indexArrayLength -= minAmount;
        if(maxAmount == minAmount)
        {
            sunkenBadness += partition->badness;
            partitionList->removeOne(partition);
            sunkenPartitions->append(partition);
            continue;
        }
        weight *= choose(partition->holes.size(), partition->badness);
        minAmountsPerPartition.prepend(minAmount);
        maxAmountsPerPartition.prepend(maxAmount);

    }
    if(unconstrainedPartition)
    {

        partition = partitionList->at(0);
        maxAmount = std::min(numBadSpots - sumMin, partition->holes.size());
        minAmount = std::max(numBadSpots - sumMax, 0);
        indexArrayLength -= minAmount;

        partition->badness = minAmount;
        for(int j = 0; j < minAmount; j++)
        {
            badSpots[partition->holes.at(j)->y][partition->holes.at(j)->x] = true;
        }
        for(int j = minAmount; j < partition->holes.size(); j++)
        {
            badSpots[partition->holes.at(j)->y][partition->holes.at(j)->x] = false;
        }
        if(maxAmount == minAmount)
        {
            sunkenBadness += partition->badness;
            partitionList->removeOne(partition);
            sunkenPartitions->append(partition);

        }
        else
        {
            maxAmountsPerPartition.prepend(maxAmount);
            minAmountsPerPartition.prepend(minAmount);
            weight *= choose(partition->holes.size(), partition->badness);

        }
    }
    listLength = partitionList->size();
    indexArray = new int[indexArrayLength];
    int k = 0;
    int index = 0;
    while( k < indexArrayLength)
    {
        partition = partitionList->at(index);
        if(partition->badness < maxAmountsPerPartition.at(index))
        {
            indexArray[k] = index;
            weight /= choose(partition->holes.size(), partition->badness);
            hole = partition->holes.at(partition->badness);
            badSpots[hole->y][hole->x] = true;
            partition->badness++;
            weight *= choose(partition->holes.size(), partition->badness);
            k++;
        }
        else
        {
            index++;
        }
    }


}

PartitionIterator::~PartitionIterator()
{
    delete[] indexArray;
}

bool PartitionIterator::hasNext()
{
    Partition * partition;
    Partition * givingPartition;
    Partition * receivingPartition;
    Hole * hole;
    int badnessAccumulator = 0;
    int givingIndex;
    int receivingIndex;
    for(int i = listLength - 1; i >= 0; i--)
    {
        partition = partitionList->at(i);
        badnessAccumulator += partition->badness - minAmountsPerPartition.at(i);
        if(partition->badness < maxAmountsPerPartition.at(i) &&
                badnessAccumulator < indexArrayLength)
        {

            givingIndex = indexArrayLength - badnessAccumulator - 1;
            givingPartition = partitionList->at(indexArray[givingIndex]);
            weight /= choose(givingPartition->holes.size(), givingPartition->badness);
            givingPartition->badness--;
            weight *= choose(givingPartition->holes.size(), givingPartition->badness);
            hole = givingPartition->holes.at(givingPartition->badness);
            badSpots[hole->y][hole->x] = false;

            receivingIndex = indexArray[givingIndex] + 1;

            receivingPartition = partitionList->at(receivingIndex);
            hole = receivingPartition->holes.at(receivingPartition->badness);
            badSpots[hole->y][hole->x] = true;
            weight /= choose(receivingPartition->holes.size(), receivingPartition->badness);
            receivingPartition->badness++;
            weight *= choose(receivingPartition->holes.size(), receivingPartition->badness);
            indexArray[givingIndex] = receivingIndex;


            int index = givingIndex + 1;
            int partitionIndex = receivingIndex;
            receivingPartition = partitionList->at(partitionIndex);

            while(index < indexArrayLength && partitionIndex < listLength)
            {
                givingPartition = partitionList->at(indexArray[index]);
                receivingPartition = partitionList->at(partitionIndex);
                if(receivingPartition->badness == maxAmountsPerPartition.at(partitionIndex))
                {
                    partitionIndex++;
                    continue;
                }
                if(givingPartition != receivingPartition)
                {
                    weight /= choose(givingPartition->holes.size(), givingPartition->badness);
                    givingPartition->badness--;
                    weight *= choose(givingPartition->holes.size(), givingPartition->badness);
                    hole = givingPartition->holes.at(givingPartition->badness);
                    badSpots[hole->y][hole->x] = false;


                    hole = receivingPartition->holes.at(receivingPartition->badness);
                    badSpots[hole->y][hole->x] = true;
                    weight /= choose(receivingPartition->holes.size(), receivingPartition->badness);
                    receivingPartition->badness++;
                    weight *= choose(receivingPartition->holes.size(), receivingPartition->badness);
                    indexArray[index] = partitionIndex;
                }
                index++;
            }
            return true;
        }

        else if(badnessAccumulator == indexArrayLength)
        {
            return false;
        }
    }
    return false;




}

double PartitionIterator::iterate()
{

    return weight;

}

double PartitionIterator::choose(int n, int k)
{
    if (k > n ) {
        return 0;
    }
    double r = 1;
    for (int d = 1; d <= k; ++d) {
        r *= n--;
        r /= d;
    }
    return r;
}
