#include "headers/solver.h"
#include "headers/problemparameters.h"
#include <QSet>
#include <QSetIterator>
#include "headers/hole.h"
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
    board = new DugType::DugType*[boardHeight];
    holes = new Hole*[boardHeight];
    badSpots = new bool*[boardHeight];
    constraints = new Constraint*[boardHeight];
    imposingConstraints = new QSet<Constraint*>*[boardHeight];

    probabilities = new double*[boardHeight];
    for(uint8_t y = 0; y < boardHeight; y++)
    {
        board[y] = new DugType::DugType[boardWidth];
        holes[y] = new Hole[boardWidth];
        badSpots[y] = new bool[boardWidth];
        constraints[y] = new Constraint[boardWidth];
        imposingConstraints[y] = new QSet<Constraint*>[boardWidth];
        probabilities[y] = new double[boardWidth];
        std::fill(probabilities[y], probabilities[y] + boardWidth, 0.0);
        std::fill(badSpots[y], badSpots[y] + boardWidth, false);
        std::fill(board[y], board[y] + boardWidth, DugType::DugType::undug);
        for(uint8_t x = 0; x < boardWidth; x++)
        {
            holes[y][x] = {x, y};
            unconstrainedUnopenedHoles.insert( &holes[y][x] );
            constraints[y][x].maxBadness = -1;
        }
    }
    //std::cout << "True number of configurations\tTotal iterations\tLegal iterations\tPartitions\tSunken Partitions\tConstrained holes" << std::endl;
}

Solver::~Solver()
{

    qDeleteAll(partitionList);
    qDeleteAll(sunkenPartitions);
    for(int y = 0; y < boardHeight; y++)
    {

        delete[] probabilities[y];
        delete[] constraints[y];
        delete[] board[y];
        delete[] badSpots[y];
        delete[] holes[y];
    }
    delete[] probabilities;
    delete[] constraints;
    delete[] imposingConstraints;
    delete[] board;
    delete[] badSpots;
    delete[] holes;

}

void Solver::setCell(int x, int y, DugType::DugType type){
    if(board[y][x] != DugType::DugType::undug &&
            board[y][x] != type)
    {
        board[y][x] = DugType::DugType::undug;
        resetBoard();
    }
    board[y][x] = type;
    if(type >= 0)
    {
        Constraint * constraint = &constraints[y][x];
        constraint->maxBadness = type;

        constrainedUnopenedHoles.remove(&holes[y][x]);

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
                            if(knownBadSpots.contains(&holes[filterY][filterX]))
                            {
                                constraint->maxBadness--;
                            }
                            else if(board[filterY][filterX] == DugType::DugType::undug)
                            {

                                imposingConstraints[filterY][filterX].insert(constraint);

                                if(!knownSafeSpots.contains(&holes[filterY][filterX]))
                                {
                                    constraint->holes.append(&holes[filterY][filterX]);
                                    constrainedUnopenedHoles.insert(&holes[filterY][filterX]);
                                }
                                unconstrainedUnopenedHoles.remove(&holes[filterY][filterX]);


                            }

                        }
                    }
                }
            }
        }
        setKnownSafeSpot(x, y);
        if(constraint->maxBadness > 0)
        {
            constraintList.append(constraint);
        }
        if(constraint->maxBadness == 0)
        {
            for(int i = 0; i < constraint->holes.size(); i++)
            {
                setKnownSafeSpot(constraint->holes.at(i)->x, constraint->holes.at(i)->y);
            }
        }
    }
    else if(type >= -2)
    {
        setKnownBadSpot(x, y);
    }
}

void Solver::resetBoard()
{

    constraintList.clear();
    constrainedUnopenedHoles.clear();
    unconstrainedUnopenedHoles.clear();
    knownBadSpots.clear();
    knownSafeSpots.clear();
    for(int y = 0; y < boardHeight; y++)
    {
        for(int x = 0; x < boardWidth; x++)
        {
            constraints[y][x].maxBadness = -1;
            constraints[y][x].holes.clear();
            unconstrainedUnopenedHoles.insert( &holes[y][x] );
            imposingConstraints[y][x].clear();
        }
        std::fill(badSpots[y], badSpots[y] + boardWidth, false);

    }
    for(int y = 0; y < boardHeight; y++)
    {
        for(int x = 0; x < boardWidth; x++)
        {
            if(board[y][x] != DugType::DugType::undug)
            {
                setCell(x, y, board[y][x]);
            }
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
    for(int y = 0; y < boardHeight; y++)
    {
        for(int x = 0; x < boardWidth; x++)
        {
            constraints[y][x].maxBadness = -1;
            constraints[y][x].holes.clear();
            unconstrainedUnopenedHoles.insert( &holes[y][x] );
            imposingConstraints[y][x].clear();
        }
        std::fill(badSpots[y], badSpots[y] + boardWidth, false);
        std::fill(board[y], board[y] + boardWidth, DugType::DugType::undug);

    }
}

void Solver::standardCalculate()
{
    bool ** array = new bool*[constrainedUnopenedHoles.size()];
    int i = 0;
    QSetIterator<Hole*> setIter(constrainedUnopenedHoles);
    Hole * hole;
    while(setIter.hasNext())
    {
        hole = setIter.next();
        array[i] = &badSpots[hole->y][hole->x];
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
    for(int y = 0; y < boardHeight; y++)
    {
        std::fill(probabilities[y], probabilities[y] + boardWidth, 0.0);
    }
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
        for(int y = 0; y < boardHeight; y++)
        {
            for(int x = 0; x < boardWidth; x++)
            {
                probabilities[y][x] += badSpots[y][x] ? configurationWeight : 0.0;
            }
        }
        setIter = QSetIterator<Hole*>(unconstrainedUnopenedHoles);
        probability = (double)(configurationWeight *
                               (bombs + rupoors - bombsAmongConstrainedHoles))/
                unconstrainedUnopenedHoles.size();

        while( setIter.hasNext())
        {
            hole = setIter.next();
            probabilities[hole->y][hole->x] += probability;
        }
    }
    while(it.hasNext());

    numConstrained = constrainedUnopenedHoles.size();
//    std::cout <<
//                 totalIterations << "\t" <<
//                 legalIterations << "\t" <<
//                 constrainedUnopenedHoles->size() << std::endl;
    for(int y = 0; y < boardHeight; y++)
    {
        for(int x = 0; x < boardWidth; x++)
        {
            if(probabilities[y][x] == totalWeight)
            {
                if(!knownBadSpots.contains(&holes[y][x]))
                {
                    setKnownBadSpot(x, y);
                }
            }
            else if(probabilities[y][x] == 0.0)
            {
                setKnownSafeSpot(x, y);
            }
            else
            {
                badSpots[y][x] = false;
                probabilities[y][x] /= totalWeight;
            }
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
    Hole * hole;
    totalWeight = 0;
    double probability;
    for(int y = 0; y < boardHeight; y++)
    {
        for(int x = 0; x < boardWidth; x++)
        {
            if(constrainedUnopenedHoles.contains(&holes[y][x]) ||
                    unconstrainedUnopenedHoles.contains(&holes[y][x]))
            {
                probabilities[y][x] = 0.0;
            }
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
                probabilities[hole->y][hole->x] += probability;
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
            probabilities[hole->y][hole->x] += probability;
        }
    }
    numConstrained = constrainedUnopenedHoles.size();
//    std::cout << totalWeight << "\t" <<
//                 totalIterations << "\t" <<
//                 legalIterations << "\t" <<
//                 partitionList->size() + sunkenPartitions.size() << "\t" <<
//                 sunkenPartitions.size() << "\t" <<
//                 constrainedUnopenedHoles->size() << std::endl;
    for(int y = 0; y < boardHeight; y++)
    {
        for(int x = 0; x < boardWidth; x++)
        {
            if(constrainedUnopenedHoles.contains(&holes[y][x]) ||
                    unconstrainedUnopenedHoles.contains(&holes[y][x]))
            {
                if(probabilities[y][x] == totalWeight)
                {
                    setKnownBadSpot(x, y);
                }
                else if(probabilities[y][x] == 0.0)
                {
                    setKnownSafeSpot(x, y);
                }
                else
                {
                    badSpots[y][x] = false;
                    probabilities[y][x] /= totalWeight;
                }
            }
        }
    }

   emit done();
}

double ** Solver::getProbabilityArray()
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
    Hole * constrainedHole;
    for(int j = constraintList.size() - 1; j >= 0; j--)
    {
        constraint = constraintList.at(j);
        badSpotsSeen = 0;
        for(int i = constraint->holes.size() - 1; i >= 0; i--)
        {
            constrainedHole = constraint->holes.at(i);
            if(badSpots[constrainedHole->y][constrainedHole->x])
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

void Solver::setKnownBadSpot(int x, int y)
{
    knownBadSpots.insert(&holes[y][x]);
    badSpots[y][x] = true;
    probabilities[y][x] = 1.0;
    constrainedUnopenedHoles.remove(&holes[y][x]);
    unconstrainedUnopenedHoles.remove(&holes[y][x]);
    Constraint * constraint;
    Hole * constrainedHole;
    Hole * unimportantHole;
    for(int filterY = y - 1; filterY < y + 2; filterY++)
    {
        if(filterY >= 0 && filterY < boardHeight)
        {
            for(int filterX = x - 1; filterX < x + 2; filterX++)
            {
                if ((filterX >= 0 && filterX < boardWidth)  &&
                        (filterX != x || filterY != y)      &&
                        (board[filterY][filterX] > 0))
                {
                    constraint = &constraints[filterY][filterX];

                    if( constraint->maxBadness != -1 &&
                            constraint->holes.removeOne(&holes[y][x]))
                    {
                        constraint->maxBadness--;
                        if(constraint->maxBadness == 0)
                        {
                            while(constraint->holes.size() > 0)
                            {
                                constrainedHole = constraint->holes.takeFirst();
                                setKnownSafeSpot(constrainedHole->x, constrainedHole->y);
                            }

                            if(constraintList.contains(constraint))
                            {
                                constraintList.removeOne(constraint);
                            }
                        }
                        else if(constraint->holes.size() == 1 &&
                                constraint->maxBadness == 1)
                        {
                            unimportantHole = constraint->holes.at(0);
                            imposingConstraints[unimportantHole->y][unimportantHole->x].remove(constraint);
                            constraintList.removeOne(constraint);
                            if(imposingConstraints[unimportantHole->y][unimportantHole->x].size() == 0)
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

void Solver::setKnownSafeSpot(int x, int y)
{
    knownSafeSpots.insert(&holes[y][x]);
    constrainedUnopenedHoles.remove(&holes[y][x]);
    unconstrainedUnopenedHoles.remove(&holes[y][x]);
    probabilities[y][x] = 0.0;
    badSpots[y][x] = false;
    Constraint * constraint;
    Hole * constrainedHole;
    Hole * unimportantHole;
    for(int filterY = y - 1; filterY < y + 2; filterY++)
    {
        if(filterY >= 0 && filterY < boardHeight)
        {
            for(int filterX = x - 1; filterX < x + 2; filterX++)
            {
                if ((filterX >= 0 && filterX < boardWidth)  &&
                        (filterX != x || filterY != y)      &&
                        (board[filterY][filterX] > 0))
                {
                    constraint = &constraints[filterY][filterX];

                    if(constraint->maxBadness != -1 &&
                            constraint->holes.removeOne(&holes[y][x]))
                    {
                        if(constraint->maxBadness - 1 == constraint->holes.size())
                        {
                            while(constraint->holes.size() > 0)
                            {
                                constrainedHole = constraint->holes.takeFirst();
                                setKnownBadSpot(constrainedHole->x, constrainedHole->y);
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
                            imposingConstraints[unimportantHole->y][unimportantHole->x].remove(constraint);
                            constraintList.removeOne(constraint);
                            if(imposingConstraints[unimportantHole->y][unimportantHole->x].size() == 0)
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

void Solver::generatePartitions()
{
    qDeleteAll(partitionList);
    qDeleteAll(sunkenPartitions);
    sunkenPartitions.clear();
    partitionList.clear();
    QSetIterator<Hole*> iter(constrainedUnopenedHoles);
    Hole* constrainedHole;
    Partition * partition;
    bool present;
    while(iter.hasNext())
    {
        constrainedHole = iter.next();
        partition = new Partition;
        partition->constraints = &imposingConstraints[constrainedHole->y][constrainedHole->x];
        present = false;
        for(int i = 0; i < partitionList.size(); i++)
        {
            if(*(partitionList.at(i)) == *partition)
            {

                delete partition;
                partition = partitionList.at(i);
                present = true;
                break;
            }
        }
        partition->holes.append(constrainedHole);

        if(!present)
        {
            partitionList.append(partition);
        }
    }
    if(unconstrainedUnopenedHoles.size() > 0)
    {
        partition = new Partition;
        partition->constraints = &emptySet;
        partition->holes << unconstrainedUnopenedHoles.toList();
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

