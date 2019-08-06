#include "headers/partitioniterator.h"

#include "headers/constraint.h"
#include "headers/partition.h"
#include <QList>
#include <QSetIterator>
#include <algorithm>

PartitionIterator::PartitionIterator(std::vector<Partition *> *partitionList,
                                     std::vector<bool> &badSpots,
                                     std::vector<Partition *> *sunkenPartitions,
                                     int numBadSpots)
    : partitionList{*partitionList}, badSpots{badSpots}
{
    weight = 1.0;
    Constraint *constraint;
    Partition *partition;
    int hole;
    int sunkenBadness = 0;
    int maxAmount;
    int minAmount;
    started = false;
    indexArrayLength = numBadSpots;
    int sumMax = 0;
    int sumMin = 0;
    bool unconstrainedPartition =
        !partitionList->empty() && partitionList->at(0)->constraints.empty();
    int lowestIndex = unconstrainedPartition ? 1 : 0;
    for (int i = int(partitionList->size() - 1); i >= lowestIndex; i--) {
        partition = partitionList->at(i);
        maxAmount = int(partition->holes.size());

        minAmount = 0;
        for (auto it = partition->constraints.begin();
             it != partition->constraints.end();
             ++it) {
            constraint = *it;
            maxAmount = std::min(maxAmount, constraint->maxBadness);
            minAmount = std::max(
                minAmount,
                constraint->maxBadness - 1 -
                    int(constraint->holes.size() - partition->holes.size()));
        }
        for (int j = 0; j < minAmount; j++) {
            badSpots[partition->holes.at(j)] = true;
        }
        for (int j = minAmount; j < int(partition->holes.size()); j++) {
            badSpots[partition->holes.at(j)] = false;
        }
        sumMin += minAmount;
        sumMax += maxAmount;
        partition->badness = minAmount;
        indexArrayLength -= minAmount;
        if (maxAmount == minAmount) {
            sunkenBadness += partition->badness;
            partitionList->erase(std::remove(partitionList->begin(),
                                             partitionList->end(),
                                             partition),
                                 partitionList->end());
            sunkenPartitions->push_back(partition);
            continue;
        }
        weight *= choose(int(partition->holes.size()), int(partition->badness));
        minAmountsPerPartition.insert(minAmountsPerPartition.begin(),
                                      minAmount);
        maxAmountsPerPartition.insert(maxAmountsPerPartition.begin(),
                                      maxAmount);
    }
    if (unconstrainedPartition) {

        partition = partitionList->at(0);
        maxAmount =
            std::min(numBadSpots - sumMin, int(partition->holes.size()));
        minAmount = std::max(numBadSpots - sumMax, 0);
        indexArrayLength -= minAmount;

        partition->badness = minAmount;
        for (int j = 0; j < minAmount; j++) {
            badSpots[partition->holes.at(j)] = true;
        }
        for (int j = minAmount; j < int(partition->holes.size()); j++) {
            badSpots[partition->holes.at(j)] = false;
        }
        if (maxAmount == minAmount) {
            sunkenBadness += partition->badness;
            partitionList->erase(std::remove(partitionList->begin(),
                                             partitionList->end(),
                                             partition),
                                 partitionList->end());
            sunkenPartitions->push_back(partition);

        } else {
            minAmountsPerPartition.insert(minAmountsPerPartition.begin(),
                                          minAmount);
            maxAmountsPerPartition.insert(maxAmountsPerPartition.begin(),
                                          maxAmount);
            weight *=
                choose(int(partition->holes.size()), int(partition->badness));
        }
    }
    listLength = int(partitionList->size());
    indexArray.resize(indexArrayLength);
    int k = 0;
    int index = 0;
    while (k < indexArrayLength) {
        partition = partitionList->at(index);
        if (partition->badness < maxAmountsPerPartition.at(index)) {
            indexArray[k] = index;
            weight /= choose(int(partition->holes.size()), partition->badness);
            hole = partition->holes.at(partition->badness);
            badSpots[hole] = true;
            partition->badness++;
            weight *= choose(int(partition->holes.size()), partition->badness);
            k++;
        } else {
            index++;
        }
    }
}

bool PartitionIterator::hasNext()
{
    Partition *partition;
    Partition *givingPartition;
    Partition *receivingPartition;
    int hole;
    int badnessAccumulator = 0;
    int givingIndex;
    int receivingIndex;
    for (int i = listLength - 1; i >= 0; i--) {
        partition = partitionList[i];
        badnessAccumulator += partition->badness - minAmountsPerPartition.at(i);
        if (partition->badness < maxAmountsPerPartition.at(i) &&
            badnessAccumulator < indexArrayLength) {

            givingIndex = indexArrayLength - badnessAccumulator - 1;
            givingPartition = partitionList[indexArray[givingIndex]];
            weight /= choose(int(givingPartition->holes.size()),
                             givingPartition->badness);
            givingPartition->badness--;
            weight *= choose(int(givingPartition->holes.size()),
                             givingPartition->badness);
            hole = givingPartition->holes.at(givingPartition->badness);
            badSpots[hole] = false;

            receivingIndex = indexArray[givingIndex] + 1;

            receivingPartition = partitionList[receivingIndex];
            hole = receivingPartition->holes.at(receivingPartition->badness);
            badSpots[hole] = true;
            weight /= choose(int(receivingPartition->holes.size()),
                             receivingPartition->badness);
            receivingPartition->badness++;
            weight *= choose(int(receivingPartition->holes.size()),
                             receivingPartition->badness);
            indexArray[givingIndex] = receivingIndex;

            int index = givingIndex + 1;
            int partitionIndex = receivingIndex;
            receivingPartition = partitionList[partitionIndex];

            while (index < indexArrayLength && partitionIndex < listLength) {
                givingPartition = partitionList[indexArray[index]];
                receivingPartition = partitionList[partitionIndex];
                if (receivingPartition->badness ==
                    maxAmountsPerPartition.at(partitionIndex)) {
                    partitionIndex++;
                    continue;
                }
                if (givingPartition != receivingPartition) {
                    weight /= choose(int(givingPartition->holes.size()),
                                     givingPartition->badness);
                    givingPartition->badness--;
                    weight *= choose(int(givingPartition->holes.size()),
                                     givingPartition->badness);
                    hole = givingPartition->holes.at(givingPartition->badness);
                    badSpots[hole] = false;

                    hole = receivingPartition->holes.at(
                        receivingPartition->badness);
                    badSpots[hole] = true;
                    weight /= choose(int(receivingPartition->holes.size()),
                                     receivingPartition->badness);
                    receivingPartition->badness++;
                    weight *= choose(int(receivingPartition->holes.size()),
                                     receivingPartition->badness);
                    indexArray[index] = partitionIndex;
                }
                index++;
            }
            return true;
        }

        if (badnessAccumulator == indexArrayLength) {
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
    if (k > n) {
        return 0.0;
    }
    double r = 1.0;
    for (int d = 1; d <= k; ++d) {
        r *= n--;
        r /= d;
    }
    return r;
}
