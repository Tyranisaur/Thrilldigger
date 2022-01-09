#include "headers/partitioniterator.h"

#include "headers/constraint.h"
#include "headers/partition.h"
#include <QList>
#include <QSetIterator>
#include <algorithm>

namespace
{
constexpr double choose(std::size_t n, const std::size_t k)
{
    if (k > n) {
        return 0.0;
    }
    double r = 1.0;
    for (std::size_t d = 1; d <= k; ++d) {
        r *= double(n--);
        r /= double(d);
    }
    return r;
}
} // namespace

PartitionIterator::PartitionIterator(std::vector<Partition *> *partitionList,
                                     std::vector<bool> &badSpots,
                                     std::vector<Partition *> *sunkenPartitions,
                                     std::size_t numBadSpots)
    : partitionList_{*partitionList},
      indexArrayLength_{numBadSpots},
      badSpots_{badSpots}
{
    std::size_t sumMax = 0;
    std::size_t sumMin = 0;
    const bool unconstrainedPartition =
        !partitionList->empty() && partitionList->at(0)->constraints.empty();
    const std::size_t lowestIndex = unconstrainedPartition ? 1 : 0;
    for (std::size_t i = partitionList->size(); i-- > lowestIndex;)
    {
        Partition *partition = partitionList->at(i);
        std::size_t maxAmount = partition->holes.size();

        std::size_t minAmount = 0;
        for (auto it = partition->constraints.begin();
             it != partition->constraints.end();
             ++it) {
            const Constraint *constraint = *it;
            assert(constraint->maxBadness > 0);
            maxAmount = std::min(maxAmount, constraint->maxBadness);
            const auto minBadness = constraint->maxBadness - 1;
            const auto holesOutsidePartition =
                constraint->holes.size() - partition->holes.size();
            const auto maxBadnessOutSidePartition =
                std::min(constraint->maxBadness, holesOutsidePartition);
            const auto minBadnessInsidePartition =
                maxBadnessOutSidePartition > minBadness
                    ? 0
                    : minBadness - maxBadnessOutSidePartition;
            minAmount = std::max(minAmount, minBadnessInsidePartition);
        }
        for (std::size_t j = 0; j != minAmount; ++j) {
            badSpots[partition->holes.at(j)] = true;
        }
        for (std::size_t j = minAmount; j != partition->holes.size(); ++j) {
            badSpots[partition->holes.at(j)] = false;
        }
        sumMin += minAmount;
        sumMax += maxAmount;
        partition->badness = minAmount;
        indexArrayLength_ -= minAmount;
        if (maxAmount == minAmount) {
            partitionList->erase(std::remove(partitionList->begin(),
                                             partitionList->end(),
                                             partition),
                                 partitionList->end());
            sunkenPartitions->push_back(partition);
            continue;
        }
        weight_ *= choose(partition->holes.size(), partition->badness);
        minAmountsPerPartition_.insert(minAmountsPerPartition_.begin(),
                                       minAmount);
        maxAmountsPerPartition_.insert(maxAmountsPerPartition_.begin(),
                                       maxAmount);
    }
    if (unconstrainedPartition) {

        Partition *partition = partitionList->at(0);
        std::size_t maxAmount =
            std::min(numBadSpots > sumMin ? numBadSpots - sumMin
                                          : partition->holes.size(),
                     partition->holes.size());
        std::size_t minAmount = numBadSpots > sumMax ? numBadSpots - sumMax : 0;
        indexArrayLength_ -= minAmount;

        partition->badness = minAmount;
        for (std::size_t j = 0; j != minAmount; ++j) {
            badSpots[partition->holes.at(j)] = true;
        }
        for (std::size_t j = minAmount; j != partition->holes.size(); ++j) {
            badSpots[partition->holes.at(j)] = false;
        }
        if (maxAmount == minAmount) {
            partitionList->erase(std::remove(partitionList->begin(),
                                             partitionList->end(),
                                             partition),
                                 partitionList->end());
            sunkenPartitions->push_back(partition);

        } else {
            minAmountsPerPartition_.insert(minAmountsPerPartition_.begin(),
                                           minAmount);
            maxAmountsPerPartition_.insert(maxAmountsPerPartition_.begin(),
                                           maxAmount);
            weight_ *= choose(partition->holes.size(), partition->badness);
        }
    }
    listLength_ = partitionList->size();
    indexArray_.resize(indexArrayLength_);
    std::size_t k = 0;
    std::size_t index = 0;
    while (k != indexArrayLength_)
    {
        Partition *partition = partitionList->at(index);
        if (partition->badness < maxAmountsPerPartition_.at(index))
        {
            indexArray_[k] = index;
            weight_ /= choose(partition->holes.size(), partition->badness);
            const std::size_t hole = partition->holes.at(partition->badness);
            badSpots[hole] = true;
            partition->badness++;
            weight_ *= choose(partition->holes.size(), partition->badness);
            k++;
        }
        else
        {
            index++;
        }
    }
}

bool PartitionIterator::goToNextState()
{
    std::size_t badnessAccumulator = 0;
    for (std::size_t i = listLength_; i--;)
    {
        const Partition *partition = partitionList_[i];
        badnessAccumulator +=
            partition->badness - minAmountsPerPartition_.at(i);
        if (partition->badness < maxAmountsPerPartition_.at(i) &&
            badnessAccumulator < indexArrayLength_)
        {

            const std::size_t givingIndex =
                indexArrayLength_ - badnessAccumulator - 1;
            Partition *givingPartition =
                partitionList_[indexArray_[givingIndex]];
            weight_ /=
                choose(givingPartition->holes.size(), givingPartition->badness);
            givingPartition->badness--;
            weight_ *=
                choose(givingPartition->holes.size(), givingPartition->badness);
            const std::size_t givingHole = givingPartition->holes.at(givingPartition->badness);
            badSpots_[givingHole] = false;

            std::size_t receivingIndex = indexArray_[givingIndex] + 1;

            Partition *receivingPartition = partitionList_[receivingIndex];
            const std::size_t receivingHole = receivingPartition->holes.at(receivingPartition->badness);
            badSpots_[receivingHole] = true;
            weight_ /= choose(receivingPartition->holes.size(),
                              receivingPartition->badness);
            receivingPartition->badness++;
            weight_ *= choose(receivingPartition->holes.size(),
                              receivingPartition->badness);
            indexArray_[givingIndex] = receivingIndex;

            std::size_t index = givingIndex + 1;
            std::size_t partitionIndex = receivingIndex;

            while (index < indexArrayLength_ && partitionIndex < listLength_)
            {
                givingPartition = partitionList_[indexArray_[index]];
                receivingPartition = partitionList_[partitionIndex];
                if (receivingPartition->badness ==
                    maxAmountsPerPartition_.at(partitionIndex))
                {
                    partitionIndex++;
                    continue;
                }
                if (givingPartition != receivingPartition) {
                    weight_ /= choose(givingPartition->holes.size(),
                                      givingPartition->badness);
                    givingPartition->badness--;
                    weight_ *= choose(givingPartition->holes.size(),
                                      givingPartition->badness);
                    const std::size_t givingSpot = givingPartition->holes.at(givingPartition->badness);
                    badSpots_[givingSpot] = false;

                    const std::size_t receivingSpot = receivingPartition->holes.at(
                        receivingPartition->badness);
                    badSpots_[receivingSpot] = true;
                    weight_ /= choose(receivingPartition->holes.size(),
                                      receivingPartition->badness);
                    receivingPartition->badness++;
                    weight_ *= choose(receivingPartition->holes.size(),
                                      receivingPartition->badness);
                    indexArray_[index] = partitionIndex;
                }
                index++;
            }
            return true;
        }

        if (badnessAccumulator == indexArrayLength_)
        {
            return false;
        }
    }
    return false;
}

double PartitionIterator::stateWeight() const
{
    return weight_;
}


