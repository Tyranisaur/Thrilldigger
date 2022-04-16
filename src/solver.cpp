#include "headers/solver.h"

#include "headers/constraint.h"
#include "headers/partition.h"
#include "headers/partitioniterator.h"
#include "headers/problemparameters.h"
#include <QSet>
#include <QSetIterator>
#include <iostream>

Solver::Solver(const ProblemParameters &params) : params_(params)
{

    for (std::size_t i = 0; i != numHoles; ++i)
    {

        unconstrainedUnopenedHoles.insert(i);
        constraints[i].maxBadness = std::numeric_limits<std::size_t>::max();
    }
    std::cout << "True number of configurations\tTotal iterations\tLegal "
              << "iterations\tPartitions\tSunken Partitions\tConstrained holes"
              << std::endl;
}

Solver::~Solver() = default;

void Solver::setCell(std::size_t x, std::size_t y, DugTypeEnum type)
{

    const std::size_t index = y * params_.width + x;
    if (board[index] != dugtype::undug && board[index] != type)
    {
        board[index] = dugtype::undug;
        resetBoard();
    }
    board[index] = type;
    if (type >= 0)
    {
        Constraint *constraint = &constraints[index];
        constraint->maxBadness = std::size_t(type);

        for (int filterY = int(y) - 1; filterY < int(y) + 2; filterY++)
        {
            if (filterY >= 0 && std::size_t(filterY) < params_.height)
            {
                for (int filterX = int(x) - 1; filterX < int(x) + 2; filterX++)
                {
                    if (filterX >= 0 && std::size_t(filterX) < params_.width)
                    {
                        if (filterX != int(x) || filterY != int(y))
                        {
                            const std::size_t filterIndex =
                                std::size_t(filterY) * params_.width +
                                std::size_t(filterX);
                            if (knownBadSpots.count(filterIndex) > 0)
                            {
                                constraint->maxBadness--;
                            }
                            else if (board[filterIndex] == dugtype::undug)
                            {

                                imposingConstraints[filterIndex].insert(
                                    constraint);

                                if (knownSafeSpots.count(filterIndex) == 0)
                                {
                                    constraint->holes.push_back(filterIndex);
                                    constrainedUnopenedHoles.insert(
                                        filterIndex);
                                }
                                unconstrainedUnopenedHoles.erase(filterIndex);
                            }
                        }
                    }
                }
            }
        }
        setKnownSafeSpot(index);
        if (constraint->maxBadness > 0)
        {
            constraintList.push_back(constraint);
        }
        if (constraint->maxBadness == 0)
        {
            for (const std::size_t hole : constraint->holes)
            {
                setKnownSafeSpot(hole);
            }
        }
    }
    else if (type >= -2)
    {
        setKnownBadSpot(index);
    }
}

void Solver::resetBoard()
{

    constraintList.clear();
    constrainedUnopenedHoles.clear();
    unconstrainedUnopenedHoles.clear();
    knownBadSpots.clear();
    knownSafeSpots.clear();
    std::fill(badSpots.begin(), badSpots.end(), false);

    for (std::size_t i = 0; i != numHoles; ++i)
    {

        constraints[i].maxBadness = std::numeric_limits<std::size_t>::max();
        constraints[i].holes.clear();
        unconstrainedUnopenedHoles.insert(i);
        imposingConstraints[i].clear();
    }

    for (std::size_t i = 0; i != numHoles; ++i)
    {

        if (board[i] != dugtype::undug)
        {
            const std::size_t x = i % params_.width;
            const std::size_t y = i / params_.width;
            setCell(x, y, board[i]);
        }
    }
}

void Solver::reload()
{
    constraintList.clear();
    constrainedUnopenedHoles.clear();
    unconstrainedUnopenedHoles.clear();
    knownBadSpots.clear();
    knownSafeSpots.clear();
    for (std::size_t i = 0; i != numHoles; ++i)
    {
        constraints[i].maxBadness = std::numeric_limits<std::size_t>::max();
        constraints[i].holes.clear();
        unconstrainedUnopenedHoles.insert(i);
        imposingConstraints[i].clear();
    }
    std::fill(badSpots.begin(), badSpots.end(), false);
    std::fill(board.begin(), board.end(), dugtype::undug);
}

void Solver::partitionCalculate()
{
    generatePartitions();
    PartitionIterator it(&partitionList,
                         badSpots,
                         &sunkenPartitions,
                         constraintList,
                         params_.bombs + params_.rupoors -
                             knownBadSpots.size());

    bombsAmongConstrainedHoles = params_.bombs + params_.rupoors;
    totalWeight = 0.0;

    for (std::size_t i = 0; i != numHoles; i++)
    {

        if (constrainedUnopenedHoles.count(i) > 0 ||
            unconstrainedUnopenedHoles.count(i) > 0)
        {
            probabilities[i] = 0.0;
        }
    }
    totalIterations = 0;
    legalIterations = 0;
    do
    {
        const double configurationWeight = it.stateWeight();
        totalIterations++;
        if (!validateBoard())
        {
            continue;
        }
        legalIterations++;

        totalWeight += configurationWeight;

        for (Partition *partion : partitionList)
        {
            const double probability = configurationWeight *
                                       double(partion->badness) /
                                       double(partion->holes.size());
            for (const std::size_t hole : partion->holes)
            {
                probabilities[hole] += probability;
            }
        }

    } while (it.goToNextState());

    for (Partition *sunkenPartition : sunkenPartitions)
    {
        const double probability = totalWeight *
                                   double(sunkenPartition->badness) /
                                   double(sunkenPartition->holes.size());
        for (const std::size_t hole : sunkenPartition->holes)
        {
            probabilities[hole] += probability;
        }
    }
    numConstrained = constrainedUnopenedHoles.size();
    std::cout << totalWeight << "\t" << totalIterations << "\t"
              << legalIterations << "\t"
              << partitionList.size() + sunkenPartitions.size() << "\t"
              << sunkenPartitions.size() << "\t"
              << constrainedUnopenedHoles.size() << std::endl;
    for (std::size_t i = 0; i != numHoles; ++i)
    {
        if (constrainedUnopenedHoles.count(i) > 0 ||
            unconstrainedUnopenedHoles.count(i) > 0)
        {
            if (probabilities[i] == totalWeight)
            {
                setKnownBadSpot(i);
            }
            else if (probabilities[i] == 0.0)
            {
                setKnownSafeSpot(i);
            }
            else
            {
                badSpots[i] = false;
                probabilities[i] /= totalWeight;
            }
        }
    }

    emit done();
}

const std::vector<double> &Solver::getProbabilityArray() const
{
    return probabilities;
}

bool Solver::validateBoard() const
{
    if (bombsAmongConstrainedHoles > params_.bombs + params_.rupoors)
    {
        return false;
    }
    if (unconstrainedUnopenedHoles.size() + bombsAmongConstrainedHoles <
        params_.bombs + params_.rupoors)
    {
        return false;
    }
    for (Constraint *constraint : constraintList)
    {
        std::size_t badSpotsSeen = 0;
        for (std::size_t constrainedHole : constraint->holes)
        {
            if (badSpots[constrainedHole])
            {
                badSpotsSeen++;
            }
        }
        if (badSpotsSeen != constraint->maxBadness &&
            badSpotsSeen + 1 != constraint->maxBadness)
        {
            return false;
        }
    }
    return true;
}

void Solver::setKnownBadSpot(const std::size_t index)
{
    knownBadSpots.insert(index);
    badSpots[index] = true;
    probabilities[index] = 1.0;
    constrainedUnopenedHoles.erase(index);
    unconstrainedUnopenedHoles.erase(index);
    const std::size_t y = index / params_.width;
    const std::size_t x = index % params_.width;
    for (int filterY = int(y) - 1; filterY < int(y) + 2; filterY++)
    {
        if (filterY >= 0 && std::size_t(filterY) < params_.height)
        {
            for (int filterX = int(x) - 1; filterX < int(x) + 2; filterX++)
            {
                if ((filterX >= 0 && std::size_t(filterX) < params_.width) &&
                    (std::size_t(filterX) != x || std::size_t(filterY) != y))
                {
                    const std::size_t filterIndex =
                        std::size_t(filterY) * params_.width +
                        std::size_t(filterX);
                    if (board[filterIndex] > 0)
                    {
                        Constraint *constraint = &constraints[filterIndex];
                        auto it = std::find(constraint->holes.begin(),
                                            constraint->holes.end(),
                                            index);
                        if (it != constraint->holes.end())
                        {
                            constraint->holes.erase(it);
                            constraint->maxBadness--;
                            if (constraint->maxBadness == 0)
                            {
                                while (!constraint->holes.empty())
                                {
                                    const std::size_t constrainedHole =
                                        constraint->holes.back();
                                    constraint->holes.pop_back();
                                    setKnownSafeSpot(constrainedHole);
                                }

                                constraintList.erase(
                                    std::remove(constraintList.begin(),
                                                constraintList.end(),
                                                constraint),
                                    constraintList.end());
                            }
                            else if (constraint->holes.size() == 1 &&
                                     constraint->maxBadness == 1)
                            {
                                const std::size_t unimportantHole =
                                    constraint->holes.front();
                                imposingConstraints[unimportantHole].erase(
                                    constraint);
                                constraintList.erase(
                                    std::remove(constraintList.begin(),
                                                constraintList.end(),
                                                constraint),
                                    constraintList.end());
                                if (imposingConstraints[unimportantHole]
                                        .empty())
                                {
                                    constrainedUnopenedHoles.erase(
                                        unimportantHole);
                                    unconstrainedUnopenedHoles.insert(
                                        unimportantHole);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Solver::setKnownSafeSpot(const std::size_t index)
{
    knownSafeSpots.insert(index);
    constrainedUnopenedHoles.erase(index);
    unconstrainedUnopenedHoles.erase(index);
    probabilities[index] = 0.0;
    badSpots[index] = false;
    const std::size_t y = index / params_.width;
    const std::size_t x = index % params_.width;
    for (int filterY = int(y) - 1; filterY < int(y) + 2; filterY++)
    {
        if (filterY >= 0 && std::size_t(filterY) < params_.height)
        {
            for (int filterX = int(x) - 1; filterX < int(x) + 2; filterX++)
            {
                if ((filterX >= 0 && std::size_t(filterX) < params_.width) &&
                    (std::size_t(filterX) != x || std::size_t(filterY) != y))
                {
                    const std::size_t filterIndex =
                        std::size_t(filterY) * params_.width +
                        std::size_t(filterX);
                    if (board[filterIndex] > 0)
                    {
                        Constraint *constraint = &constraints[filterIndex];
                        auto constrainedHoleIt =
                            std::find(constraint->holes.begin(),
                                      constraint->holes.end(),
                                      index);
                        if (constrainedHoleIt != constraint->holes.end())
                        {
                            constraint->holes.erase(constrainedHoleIt);
                            if (constraint->maxBadness - 1 ==
                                constraint->holes.size())
                            {
                                while (!constraint->holes.empty())
                                {
                                    const std::size_t constrainedHole =
                                        constraint->holes.back();
                                    constraint->holes.pop_back();
                                    setKnownBadSpot(constrainedHole);
                                }
                                auto it = std::find(constraintList.begin(),
                                                    constraintList.end(),
                                                    constraint);
                                if (it != constraintList.end())
                                {
                                    constraintList.erase(it);
                                }
                            }
                            else if (constraint->holes.size() == 1 &&
                                     constraint->maxBadness == 1)
                            {
                                const std::size_t unimportantHole =
                                    constraint->holes.back();
                                constraint->holes.pop_back();
                                imposingConstraints[unimportantHole].erase(
                                    constraint);
                                auto trivialConstraintIt =
                                    std::find(constraintList.begin(),
                                              constraintList.end(),
                                              constraint);
                                if (trivialConstraintIt != constraintList.end())
                                {
                                    constraintList.erase(trivialConstraintIt);
                                }
                                if (imposingConstraints[unimportantHole]
                                        .empty())
                                {
                                    constrainedUnopenedHoles.erase(
                                        unimportantHole);
                                    unconstrainedUnopenedHoles.insert(
                                        unimportantHole);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Solver::generatePartitions()
{

    sunkenPartitions.clear();
    partitionList.clear();
    std::size_t numpartitions = 0;
    for (const std::size_t constrainedHole : constrainedUnopenedHoles)
    {
        Partition *partition = &partitions[numpartitions];
        partition->constraints = imposingConstraints[constrainedHole];
        partition->holes.clear();
        bool present = false;
        for (Partition *preExistingParition : partitionList)
        {
            if (*preExistingParition == *partition)
            {

                partition = preExistingParition;
                present = true;
                break;
            }
        }
        partition->holes.push_back(constrainedHole);

        if (!present)
        {
            partitionList.push_back(partition);
            numpartitions++;
        }
    }
    if (!unconstrainedUnopenedHoles.empty())
    {
        Partition *partition = &partitions[numpartitions];
        partition->constraints.clear();
        partition->holes = {unconstrainedUnopenedHoles.begin(),
                            unconstrainedUnopenedHoles.end()};
        partitionList.insert(partitionList.begin(), partition);
    }
}
std::size_t Solver::getConstrainedHoles() const
{
    return numConstrained;
}

double Solver::getTotalNumConfigurations() const
{
    return totalWeight;
}

std::uint64_t Solver::getIterations() const
{
    return totalIterations;
}

std::size_t Solver::getLegalIterations() const
{
    return legalIterations;
}

std::size_t Solver::getPartitions() const
{
    return partitionList.size() + sunkenPartitions.size();
}
