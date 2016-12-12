#include "headers/solver.h"
#include "headers/problemparameters.h"
#include <QSet>
#include <QSetIterator>
#include "headers/constraint.h"
#include "headers/configurationiterator.h"
#include "headers/partitioniterator.h"
#include "headers/partition.h"
#include <iostream>

Solver::Solver(ProblemParameters * params)
{
    boardHeight = params->height;
    boardWidth = params->width;
    bombs = params->bombs;
    rupoors = params->rupoors;
    numHoles = boardWidth * boardHeight;
    board = new DugType::DugType[numHoles];
    badSpots = new bool[numHoles];
    constraints = new Constraint[numHoles];
    imposingConstraints = new QSet<Constraint*>[numHoles];
    partitions = new Partition[numHoles];

    probabilities = new double[numHoles];
    std::fill(probabilities, probabilities + numHoles, 0.0);
    std::fill(badSpots, badSpots + numHoles, false);
    std::fill(board, board + numHoles, DugType::DugType::undug);
    for(int i = 0; i < numHoles; i++)
    {

        unconstrainedUnopenedHoles.insert( i );
        constraints[i].maxBadness = -1;

    }
    //std::cout << "True number of configurations\tTotal iterations\tLegal iterations\tPartitions\tSunken Partitions\tConstrained holes" << std::endl;
}

Solver::~Solver()
{

    delete[] probabilities;
    delete[] constraints;
    delete[] imposingConstraints;
    delete[] board;
    delete[] badSpots;
    delete[] partitions;

}

void Solver::setCell(int x, int y, DugType::DugType type){

    int index = y * boardWidth + x;
    int filterIndex;
    if(board[index] != DugType::DugType::undug &&
            board[index] != type)
    {
        board[index] = DugType::DugType::undug;
        resetBoard();
    }
    board[index] = type;
    if(type >= 0)
    {
        Constraint * constraint = &constraints[index];
        constraint->maxBadness = type;

        constrainedUnopenedHoles.remove(index);

        for(int filterY = y - 1; filterY < y + 2; filterY++)
        {
            if(filterY >= 0 && filterY < boardHeight)
            {
                for(int filterX = x - 1; filterX < x + 2; filterX++)
                {
                    if(filterX >= 0 && filterX < boardWidth)
                    {
                        if(filterX != x || filterY != y)
                        {
                            filterIndex = filterY * boardWidth + filterX;
                            if(knownBadSpots.contains(filterIndex))
                            {
                                constraint->maxBadness--;
                            }
                            else if(board[filterIndex] == DugType::DugType::undug)
                            {

                                imposingConstraints[filterIndex].insert(constraint);

                                if(!knownSafeSpots.contains(filterIndex))
                                {
                                    constraint->holes.append(filterIndex);
                                    constrainedUnopenedHoles.insert(filterIndex);
                                }
                                unconstrainedUnopenedHoles.remove(filterIndex);


                            }

                        }
                    }
                }
            }
        }
        setKnownSafeSpot(index);
        if(constraint->maxBadness > 0)
        {
            constraintList.append(constraint);
        }
        if(constraint->maxBadness == 0)
        {
            for(int i = 0; i < constraint->holes.size(); i++)
            {
                setKnownSafeSpot(constraint->holes.at(i));
            }
        }
    }
    else if(type >= -2)
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
    std::fill(badSpots, badSpots + numHoles, false);
    for(int i = 0; i < numHoles; i++)
    {

        constraints[i].maxBadness = -1;
        constraints[i].holes.clear();
        unconstrainedUnopenedHoles.insert( i );
        imposingConstraints[i].clear();


    }
    int x;
    int y;
    for(int i = 0; i < numHoles; i++)
    {

        if(board[i] != DugType::DugType::undug)
        {
            x = i % boardWidth;
            y = i / boardHeight;
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
    for(int i = 0; i < numHoles; i++)
    {

        constraints[i].maxBadness = -1;
        constraints[i].holes.clear();
        unconstrainedUnopenedHoles.insert( i );
        imposingConstraints[i].clear();


    }
    std::fill(badSpots, badSpots + numHoles, false);
    std::fill(board, board + numHoles, DugType::DugType::undug);
}

void Solver::standardCalculate()
{
    bool ** array = new bool*[constrainedUnopenedHoles.size()];
    int i = 0;
    QSetIterator<int> setIter(constrainedUnopenedHoles);
    int hole;
    while(setIter.hasNext())
    {
        hole = setIter.next();
        array[i] = &badSpots[hole];
        i++;
    }
    ConfigurationIterator it(
                array,
                i,
                std::max(bombs + rupoors - knownBadSpots.size() - unconstrainedUnopenedHoles.size(), 0),
                std::min(i, bombs + rupoors - knownBadSpots.size()));
    double configurationWeight;
    totalWeight = 0;
    double probability;

    std::fill(probabilities, probabilities + numHoles, 0.0);

    totalIterations = 0;
    legalIterations = 0;
    do
    {
        bombsAmongConstrainedHoles = it.next() + knownBadSpots.size();
        totalIterations++;
        if(!validateBoard())
        {
            continue;
        }
        legalIterations++;
        configurationWeight = choose(
                    unconstrainedUnopenedHoles.size(),
                    bombs + rupoors - bombsAmongConstrainedHoles);
        totalWeight += configurationWeight;
        for(int i = 0; i < numHoles; i++)
        {

            probabilities[i] += badSpots[i] ? configurationWeight : 0.0;

        }
        setIter = QSetIterator<int>(unconstrainedUnopenedHoles);
        probability = (double)(configurationWeight *
                               (bombs + rupoors - bombsAmongConstrainedHoles))/
                unconstrainedUnopenedHoles.size();

        while( setIter.hasNext())
        {
            hole = setIter.next();
            probabilities[hole] += probability;
        }
    }
    while(it.hasNext());

    numConstrained = constrainedUnopenedHoles.size();
    //    std::cout <<
    //                 totalIterations << "\t" <<
    //                 legalIterations << "\t" <<
    //                 constrainedUnopenedHoles->size() << std::endl;
    for(int i = 0; i < numHoles; i++)
    {

        if(probabilities[i] == totalWeight)
        {
            if(!knownBadSpots.contains(i))
            {
                setKnownBadSpot(i);
            }
        }
        else if(probabilities[i] == 0.0)
        {
            setKnownSafeSpot(i);
        }
        else
        {
            badSpots[i] = false;
            probabilities[i] /= totalWeight;
        }

    }



    delete[] array;
    emit done();
}

void Solver::partitionCalculate()
{
    generatePartitions();
    PartitionIterator it(
                &partitionList,
                badSpots,
                &sunkenPartitions,
                bombs + rupoors - knownBadSpots.size());

    bombsAmongConstrainedHoles = bombs + rupoors;
    double configurationWeight;
    int hole;
    totalWeight = 0;
    double probability;
    for(int i = 0; i < numHoles; i++)
    {

        if(constrainedUnopenedHoles.contains(i) ||
                unconstrainedUnopenedHoles.contains(i))
        {
            probabilities[i] = 0.0;
        }

    }
    totalIterations = 0;
    legalIterations = 0;
    do
    {
        configurationWeight = it.iterate();
        totalIterations++;
        if(!validateBoard())
        {
            continue;
        }
        legalIterations++;

        totalWeight += configurationWeight;

        for(int i = 0; i < partitionList.size(); i++)
        {
            probability = configurationWeight *
                    partitionList.at(i)->badness / (double) partitionList.at(i)->holes.size();
            for(int j = 0; j < partitionList.at(i)->holes.size(); j++)
            {
                hole = partitionList.at(i)->holes.at(j);
                probabilities[hole] += probability;
            }
        }


    }
    while(it.hasNext());


    for(int i = 0; i < sunkenPartitions.size(); i++)
    {
        probability = totalWeight *
                sunkenPartitions.at(i)->badness /
                (double) sunkenPartitions.at(i)->holes.size();
        for(int j = 0; j < sunkenPartitions.at(i)->holes.size(); j++)
        {
            hole = sunkenPartitions.at(i)->holes.at(j);
            probabilities[hole] += probability;
        }
    }
    numConstrained = constrainedUnopenedHoles.size();
    //    std::cout << totalWeight << "\t" <<
    //                 totalIterations << "\t" <<
    //                 legalIterations << "\t" <<
    //                 partitionList->size() + sunkenPartitions.size() << "\t" <<
    //                 sunkenPartitions.size() << "\t" <<
    //                 constrainedUnopenedHoles->size() << std::endl;
    for(int i = 0; i < numHoles; i++)
    {
        if(constrainedUnopenedHoles.contains(i) ||
                unconstrainedUnopenedHoles.contains(i))
        {
            if(probabilities[i] == totalWeight)
            {
                setKnownBadSpot(i);
            }
            else if(probabilities[i] == 0.0)
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

double * Solver::getProbabilityArray()
{
    return probabilities;
}

bool Solver::validateBoard()
{
    if(bombsAmongConstrainedHoles > bombs + rupoors)
    {
        return false;
    }
    if(unconstrainedUnopenedHoles.size() + bombsAmongConstrainedHoles < bombs + rupoors)
    {
        return false;
    }
    int badSpotsSeen;
    Constraint* constraint;
    int constrainedHole;
    for(int j = constraintList.size() - 1; j >= 0; j--)
    {
        constraint = constraintList.at(j);
        badSpotsSeen = 0;
        for(int i = constraint->holes.size() - 1; i >= 0; i--)
        {
            constrainedHole = constraint->holes.at(i);
            if(badSpots[constrainedHole])
            {
                badSpotsSeen++;
            }
        }
        if(badSpotsSeen != constraint->maxBadness &&
                badSpotsSeen + 1 != constraint->maxBadness)
        {
            return false;
        }

    }
    return true;
}

double Solver::choose(uint64_t n, uint64_t k) {
    if (k > n ) {
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
    constrainedUnopenedHoles.remove(index);
    unconstrainedUnopenedHoles.remove(index);
    Constraint * constraint;
    int constrainedHole;
    int filterIndex;
    int unimportantHole;
    int y = index / boardHeight;
    int x = index % boardWidth;
    for(int filterY = y - 1; filterY < y + 2; filterY++)
    {
        if(filterY >= 0 && filterY < boardHeight)
        {
            for(int filterX = x - 1; filterX < x + 2; filterX++)
            {
                if ((filterX >= 0 && filterX < boardWidth)  &&
                        (filterX != x || filterY != y))
                {
                    filterIndex = filterY * boardWidth + filterX;
                    if (board[filterIndex] > 0)
                    {
                        constraint = &constraints[filterIndex];

                        if( constraint->maxBadness != -1 &&
                                constraint->holes.removeOne(index))
                        {
                            constraint->maxBadness--;
                            if(constraint->maxBadness == 0)
                            {
                                while(constraint->holes.size() > 0)
                                {
                                    constrainedHole = constraint->holes.takeFirst();
                                    setKnownSafeSpot(constrainedHole);
                                }


                                constraintList.removeOne(constraint);

                            }
                            else if(constraint->holes.size() == 1 &&
                                    constraint->maxBadness == 1)
                            {
                                unimportantHole = constraint->holes.at(0);
                                imposingConstraints[unimportantHole].remove(constraint);
                                constraintList.removeOne(constraint);
                                if(imposingConstraints[unimportantHole].size() == 0)
                                {
                                    constrainedUnopenedHoles.remove(unimportantHole);
                                    unconstrainedUnopenedHoles.insert(unimportantHole);
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
    constrainedUnopenedHoles.remove(index);
    unconstrainedUnopenedHoles.remove(index);
    probabilities[index] = 0.0;
    badSpots[index] = false;
    Constraint * constraint;
    int constrainedHole;
    int filterIndex;
    int unimportantHole;
    int y = index / boardHeight;
    int x = index % boardWidth;
    for(int filterY = y - 1; filterY < y + 2; filterY++)
    {
        if(filterY >= 0 && filterY < boardHeight)
        {
            for(int filterX = x - 1; filterX < x + 2; filterX++)
            {
                if ((filterX >= 0 && filterX < boardWidth)  &&
                        (filterX != x || filterY != y))
                {
                    filterIndex = filterY * boardWidth + filterX;
                    if(board[filterIndex] > 0)
                    {
                        constraint = &constraints[filterIndex];

                        if(constraint->maxBadness != -1 &&
                                constraint->holes.removeOne(index))
                        {
                            if(constraint->maxBadness - 1 == constraint->holes.size())
                            {
                                while(constraint->holes.size() > 0)
                                {
                                    constrainedHole = constraint->holes.takeFirst();
                                    setKnownBadSpot(constrainedHole);
                                }
                                if(constraintList.contains(constraint) )
                                {
                                    constraintList.removeOne(constraint);
                                }
                            }
                            else if(constraint->holes.size() == 1 &&
                                    constraint->maxBadness == 1)
                            {
                                unimportantHole = constraint->holes.takeFirst();
                                imposingConstraints[unimportantHole].remove(constraint);
                                constraintList.removeOne(constraint);
                                if(imposingConstraints[unimportantHole].size() == 0)
                                {
                                    constrainedUnopenedHoles.remove(unimportantHole);
                                    unconstrainedUnopenedHoles.insert(unimportantHole);
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
    QSetIterator<int> iter(constrainedUnopenedHoles);
    int constrainedHole;
    Partition * partition;
    bool present;
    int numpartitions = 0;
    while(iter.hasNext())
    {
        constrainedHole = iter.next();
        partition = &partitions[numpartitions];
        partition->constraints = &imposingConstraints[constrainedHole];
        partition->holes.clear();
        present = false;
        for(int i = 0; i < partitionList.size(); i++)
        {
            if(*(partitionList.at(i)) == *partition)
            {

                partition = partitionList.at(i);
                present = true;
                break;
            }
        }
        partition->holes.append(constrainedHole);

        if(!present)
        {
            partitionList.append(partition);
            numpartitions++;
        }
    }
    if(unconstrainedUnopenedHoles.size() > 0)
    {
        partition = &partitions[numpartitions];
        partition->constraints = &emptySet;
        partition->holes = unconstrainedUnopenedHoles.toList();
        partitionList.prepend(partition);
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
    return partitionList.size() + sunkenPartitions.size();
}

