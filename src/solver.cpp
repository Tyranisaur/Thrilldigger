#include "headers/solver.h"

#include "headers/constraint.h"
#include "headers/partition.h"
#include "headers/partitioniterator.h"
#include "headers/problemparameters.h"
#include <QSet>
#include <QSetIterator>
#include <iostream>

Solver::Solver(const ProblemParameters &params)
    : params_(params),
      numHoles(params_.width * params_.height),
      probabilities(numHoles, 0.0),
      constraints(numHoles),
      partitions(numHoles),
      badSpots(numHoles, false),
      imposingConstraints(numHoles),
      board(numHoles, DugType::undug)
{

    for (int i = 0; i < numHoles; i++) {

        unconstrainedUnopenedHoles.insert(i);
        constraints[i].maxBadness = -1;
    }
    std::cout << "True number of configurations\tTotal iterations\tLegal "
              << "iterations\tPartitions\tSunken Partitions\tConstrained holes"
              << std::endl;
}

void Solver::setCell(int x, int y, DugType::DugType type)
{

    int index = y * params_.width + x;
    int filterIndex;
    if (board[index] != DugType::DugType::undug && board[index] != type) {
        board[index] = DugType::DugType::undug;
        resetBoard();
    }
    board[index] = type;
    if (type >= 0) {
        Constraint *constraint = &constraints[index];
        constraint->maxBadness = type;

        for (int filterY = y - 1; filterY < y + 2; filterY++) {
            if (filterY >= 0 && filterY < params_.height) {
                for (int filterX = x - 1; filterX < x + 2; filterX++) {
                    if (filterX >= 0 && filterX < params_.width) {
                        if (filterX != x || filterY != y) {
                            filterIndex = filterY * params_.width + filterX;
                            if (knownBadSpots.count(filterIndex)) {
                                constraint->maxBadness--;
                            } else if (board[filterIndex] ==
                                       DugType::DugType::undug) {

                                imposingConstraints[filterIndex].insert(
                                    constraint);

                                if (!knownSafeSpots.count(filterIndex)) {
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
        if (constraint->maxBadness > 0) {
            constraintList.push_back(constraint);
        }
        if (constraint->maxBadness == 0) {
            for (int hole : constraint->holes) {
                setKnownSafeSpot(hole);
            }
        }
    } else if (type >= -2) {
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
    for (int i = 0; i < numHoles; i++) {

        constraints[i].maxBadness = -1;
        constraints[i].holes.clear();
        unconstrainedUnopenedHoles.insert(i);
        imposingConstraints[i].clear();
    }
    int x;
    int y;
    for (int i = 0; i < numHoles; i++) {

        if (board[i] != DugType::DugType::undug) {
            x = i % params_.width;
            y = i / params_.width;
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
    for (int i = 0; i < numHoles; i++) {

        constraints[i].maxBadness = -1;
        constraints[i].holes.clear();
        unconstrainedUnopenedHoles.insert(i);
        imposingConstraints[i].clear();
    }
    std::fill(badSpots.begin(), badSpots.end(), false);
    std::fill(board.begin(), board.end(), DugType::DugType::undug);
}

void Solver::partitionCalculate()
{
    generatePartitions();
    PartitionIterator it(&partitionList,
                         badSpots,
                         &sunkenPartitions,
                         params_.bombs + params_.rupoors -
                             int(knownBadSpots.size()));

    bombsAmongConstrainedHoles = params_.bombs + params_.rupoors;
    double configurationWeight;
    totalWeight = 0.0;
    double probability;
    for (int i = 0; i < numHoles; i++) {

        if (constrainedUnopenedHoles.count(i) ||
            unconstrainedUnopenedHoles.count(i)) {
            probabilities[i] = 0.0;
        }
    }
    totalIterations = 0;
    legalIterations = 0;
    do {
        configurationWeight = it.iterate();
        totalIterations++;
        if (!validateBoard()) {
            continue;
        }
        legalIterations++;

        totalWeight += configurationWeight;

        for (auto i : partitionList) {
            probability =
                configurationWeight * i->badness / double(i->holes.size());
            for (int hole : i->holes) {
                probabilities[hole] += probability;
            }
        }

    } while (it.hasNext());

    for (auto sunkenPartition : sunkenPartitions) {
        probability = totalWeight * sunkenPartition->badness /
                      double(sunkenPartition->holes.size());
        for (int hole: sunkenPartition->holes) {
            probabilities[hole] += probability;
        }
    }
    numConstrained = int(constrainedUnopenedHoles.size());
    std::cout << totalWeight << "\t" << totalIterations << "\t"
              << legalIterations << "\t"
              << partitionList.size() + sunkenPartitions.size() << "\t"
              << sunkenPartitions.size() << "\t"
              << constrainedUnopenedHoles.size() << std::endl;
    for (int i = 0; i < numHoles; i++) {
        if (constrainedUnopenedHoles.count(i) ||
            unconstrainedUnopenedHoles.count(i)) {
            if (probabilities[i] == totalWeight) {
                setKnownBadSpot(i);
            } else if (probabilities[i] == 0.0) {
                setKnownSafeSpot(i);
            } else {
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

bool Solver::validateBoard()
{
    if (bombsAmongConstrainedHoles > params_.bombs + params_.rupoors) {
        return false;
    }
    if (int(unconstrainedUnopenedHoles.size()) + bombsAmongConstrainedHoles <
        params_.bombs + params_.rupoors) {
        return false;
    }
    for (Constraint *constraint : constraintList) {
        int badSpotsSeen = 0;
        for (int constrainedHole : constraint->holes) {
            if (badSpots[constrainedHole]) {
                badSpotsSeen++;
            }
        }
        if (badSpotsSeen != constraint->maxBadness &&
            badSpotsSeen + 1 != constraint->maxBadness) {
            return false;
        }
    }
    return true;
}

double Solver::choose(uint64_t n, uint64_t k)
{
    if (k > n) {
        return 0.0;
    }
    double r = 1.0;
    for (uint64_t d = 1; d <= k; ++d) {
        r *= n--;
        r /= d;
    }
    return r;
}

void Solver::setKnownBadSpot(int index)
{
    knownBadSpots.insert(index);
    badSpots[index] = true;
    probabilities[index] = 1.0;
    constrainedUnopenedHoles.erase(index);
    unconstrainedUnopenedHoles.erase(index);
    const int y = index / params_.width;
    const int x = index % params_.width;
    for (int filterY = y - 1; filterY < y + 2; filterY++) {
        if (filterY >= 0 && filterY < params_.height) {
            for (int filterX = x - 1; filterX < x + 2; filterX++) {
                if ((filterX >= 0 && filterX < params_.width) &&
                    (filterX != x || filterY != y)) {
                    const int filterIndex = filterY * params_.width + filterX;
                    if (board[filterIndex] > 0) {
                        Constraint *constraint = &constraints[filterIndex];
                        auto it = std::find(constraint->holes.begin(),
                                            constraint->holes.end(),
                                            index);
                        if (constraint->maxBadness != -1 &&
                            it != constraint->holes.end()) {
                            constraint->holes.erase(it);
                            constraint->maxBadness--;
                            if (constraint->maxBadness == 0) {
                                while (!constraint->holes.empty()) {
                                    const int constrainedHole =
                                        constraint->holes.back();
                                    constraint->holes.pop_back();
                                    setKnownSafeSpot(constrainedHole);
                                }

                                constraintList.erase(
                                    std::remove(constraintList.begin(),
                                                constraintList.end(),
                                                constraint),
                                    constraintList.end());

                            } else if (constraint->holes.size() == 1 &&
                                       constraint->maxBadness == 1) {
                                const int unimportantHole =
                                    constraint->holes.at(0);
                                imposingConstraints[unimportantHole].erase(
                                    constraint);
                                constraintList.erase(
                                    std::remove(constraintList.begin(),
                                                constraintList.end(),
                                                constraint),
                                    constraintList.end());
                                if (imposingConstraints[unimportantHole]
                                        .empty()) {
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

void Solver::setKnownSafeSpot(int index)
{
    knownSafeSpots.insert(index);
    constrainedUnopenedHoles.erase(index);
    unconstrainedUnopenedHoles.erase(index);
    probabilities[index] = 0.0;
    badSpots[index] = false;
    Constraint *constraint;
    int constrainedHole;
    int filterIndex;
    int unimportantHole;
    int y = index / params_.width;
    int x = index % params_.width;
    for (int filterY = y - 1; filterY < y + 2; filterY++) {
        if (filterY >= 0 && filterY < params_.height) {
            for (int filterX = x - 1; filterX < x + 2; filterX++) {
                if ((filterX >= 0 && filterX < params_.width) &&
                    (filterX != x || filterY != y)) {
                    filterIndex = filterY * params_.width + filterX;
                    if (board[filterIndex] > 0) {
                        constraint = &constraints[filterIndex];
                        auto constrainedHoleIt =
                            std::find(constraint->holes.begin(),
                                      constraint->holes.end(),
                                      index);
                        if (constraint->maxBadness != -1 &&
                            constrainedHoleIt != constraint->holes.end()) {
                            constraint->holes.erase(constrainedHoleIt);
                            if (constraint->maxBadness - 1 ==
                                int(constraint->holes.size())) {
                                while (!constraint->holes.empty()) {
                                    constrainedHole = constraint->holes.back();
                                    constraint->holes.pop_back();
                                    setKnownBadSpot(constrainedHole);
                                }
                                auto it = std::find(constraintList.begin(),
                                                    constraintList.end(),
                                                    constraint);
                                if (it != constraintList.end()) {
                                    constraintList.erase(it);
                                }
                            } else if (constraint->holes.size() == 1 &&
                                       constraint->maxBadness == 1) {
                                unimportantHole = constraint->holes.back();
                                constraint->holes.pop_back();
                                imposingConstraints[unimportantHole].erase(
                                    constraint);
                                auto trivialConstraintIt =
                                    std::find(constraintList.begin(),
                                              constraintList.end(),
                                              constraint);
                                if (trivialConstraintIt !=
                                    constraintList.end()) {
                                    constraintList.erase(trivialConstraintIt);
                                }
                                if (imposingConstraints[unimportantHole]
                                        .empty()) {
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
    Partition *partition;
    bool present;
    int numpartitions = 0;
    for (int constrainedHole : constrainedUnopenedHoles) {
        partition = &partitions[numpartitions];
        partition->constraints = imposingConstraints[constrainedHole];
        partition->holes.clear();
        present = false;
        for (auto i : partitionList) {
            if (*i == *partition) {

                partition = i;
                present = true;
                break;
            }
        }
        partition->holes.push_back(constrainedHole);

        if (!present) {
            partitionList.push_back(partition);
            numpartitions++;
        }
    }
    if (!unconstrainedUnopenedHoles.empty()) {
        partition = &partitions[numpartitions];
        partition->constraints = Solver::emptySet;
        partition->holes = {unconstrainedUnopenedHoles.begin(),
                            unconstrainedUnopenedHoles.end()};
        partitionList.insert(partitionList.begin(), partition);
    }
}
int Solver::getConstrainedHoles()
{
    return numConstrained;
}

double Solver::getTotalNumConfigurations()
{
    return totalWeight;
}

uint64_t Solver::getIterations()
{
    return totalIterations;
}

int Solver::getLegalIterations()
{
    return legalIterations;
}

int Solver::getPartitions()
{
    return int(partitionList.size() + sunkenPartitions.size());
}
