#include "solver.h"
#include "problemparameters.h"
#include <QSet>
#include <QSetIterator>
#include <QThread>
#include "hole.h"
#include "constraint.h"
#include "configurationiterator.h"
#include "partitioniterator.h"
#include "partition.h"
#include <iostream>

Solver::Solver(ProblemParameters * params, QThread * thread)
{
    this->thread = thread;
    boardHeight = params->height;
    boardWidth = params->width;
    bombs = params->bombs;
    rupoors = params->rupoors;
    partitionList = new QList<Partition*>;
    board = new DugType::DugType*[boardHeight];
    constrainedUnopenedHoles = new QSet<Hole*>;
    unconstrainedUnopenedHoles = new QSet<Hole*>;
    knownBadSpots = new QSet<Hole*>;
    knownSafeSpots = new QSet<Hole*>;
    holes = new Hole*[boardHeight];
    badSpots = new bool*[boardHeight];
    constraints = new Constraint**[boardHeight];
    imposingConstraints = new QSet<Constraint*>**[boardHeight];

    probabilities = new double*[boardHeight];
    for(int y = 0; y < boardHeight; y++)
    {
        board[y] = new DugType::DugType[boardWidth];
        holes[y] = new Hole[boardWidth];
        badSpots[y] = new bool[boardWidth];
        constraints[y] = new Constraint*[boardWidth];
        imposingConstraints[y] = new QSet<Constraint*>*[boardWidth];
        probabilities[y] = new double[boardWidth];
        std::fill(probabilities[y], probabilities[y] + boardWidth, 0.0);
        std::fill(imposingConstraints[y], imposingConstraints[y] + boardWidth, nullptr);
        std::fill(constraints[y], constraints[y] + boardWidth, nullptr);
        std::fill(badSpots[y], badSpots[y] + boardWidth, false);
        std::fill(board[y], board[y] + boardWidth, DugType::DugType::undug);
        for(int x = 0; x < boardWidth; x++)
        {
            holes[y][x] = {x, y};
            unconstrainedUnopenedHoles->insert( &holes[y][x] );
        }
    }
    std::cout << "Total iterations\tLegal iterations\tPartitions\tConstrained holes" << std::endl;
}

Solver::~Solver()
{
    delete knownBadSpots;
    delete knownSafeSpots;
    delete partitionList;
    delete[] imposingConstraints;
    delete[] board;
    delete [] badSpots;
    delete [] holes;
    delete [] constraints;
    delete constrainedUnopenedHoles;
    delete unconstrainedUnopenedHoles;
    delete[] probabilities;
}

void Solver::setCell(int x, int y, DugType::DugType type){
    if(board[y][x] >= -2 && board[y][x] < 0)
    {
        knownBadSpots->remove(&holes[y][x]);
    }
    board[y][x] = type;
    if(type >= 0)
    {
        Constraint * constraint;
        if (constraints[y][x] != nullptr)
        {
            constraintList.removeOne(constraints[y][x]);
            delete constraints[y][x];
        }

        constraint = new Constraint;
        constraint->x = x;
        constraint->y = y;
        constraint->maxBadness = type;

        constrainedUnopenedHoles->remove(&holes[y][x]);
        badSpots[y][x] = false;

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
                            if(knownBadSpots->contains(&holes[filterY][filterX]))
                            {
                                constraint->maxBadness--;
                            }
                            else if(board[filterY][filterX] == DugType::DugType::undug)
                            {



                                if(imposingConstraints[filterY][filterX] == nullptr)
                                {
                                    imposingConstraints[filterY][filterX] = new QSet<Constraint*>;
                                }
                                imposingConstraints[filterY][filterX]->insert(constraint);

                                if(!knownSafeSpots->contains(&holes[filterY][filterX]))
                                {
                                    constraint->holes.append(&holes[filterY][filterX]);
                                    constrainedUnopenedHoles->insert(&holes[filterY][filterX]);
                                }
                                unconstrainedUnopenedHoles->remove(&holes[filterY][filterX]);


                            }

                        }
                    }
                }
            }
        }
        constraints[y][x] = constraint;
        setKnownSafeSpot(x, y);
        if(constraint->maxBadness >= 0)
        {
            constraintList.append(constraint);
        }
    }
    else if(type >= -2)
    {
        setKnownBadSpot(x, y);
    }
}
void Solver::standardCalculate()
{
    bool ** array = new bool*[constrainedUnopenedHoles->size()];
    int i = 0;
    QSetIterator<Hole*> setIter(*constrainedUnopenedHoles);
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
                std::max(bombs + rupoors - knownBadSpots->size() - unconstrainedUnopenedHoles->size(), 0),
                std::min(i, bombs + rupoors - knownBadSpots->size()));
    uint64_t configurationWeight;
    uint64_t totalWeight = 0;
    for(int y = 0; y < boardHeight; y++)
    {
        std::fill(probabilities[y], probabilities[y] + boardWidth, 0.0);
    }
    int totalIterations = 0;
    int legalIterations = 0;
    do
    {
        bombsAmongConstrainedHoles = it.iterate() + knownBadSpots->size();
        totalIterations++;
        if(!validateBoard())
        {
            continue;
        }
        legalIterations++;
        configurationWeight = choose(
                    unconstrainedUnopenedHoles->size(),
                    bombs + rupoors - bombsAmongConstrainedHoles);
        totalWeight += configurationWeight;
        for(int y = 0; y < boardHeight; y++)
        {
            for(int x = 0; x < boardWidth; x++)
            {
                probabilities[y][x] += badSpots[y][x] ? configurationWeight : 0.0;
            }
        }
        setIter = QSetIterator<Hole*>(*unconstrainedUnopenedHoles);
        while( setIter.hasNext())
        {
            hole = setIter.next();
            probabilities[hole->y][hole->x] +=
                    (double)(configurationWeight *
                             (bombs + rupoors - bombsAmongConstrainedHoles))/
                    unconstrainedUnopenedHoles->size();
        }

    }
    while(it.hasNext());

    std::cout << totalIterations << "\t" <<
                 legalIterations << "\t" <<
                 constrainedUnopenedHoles->size() << std::endl;
    for(int y = 0; y < boardHeight; y++)
    {
        for(int x = 0; x < boardWidth; x++)
        {
            if(probabilities[y][x] == totalWeight)
            {
                if(!knownBadSpots->contains(&holes[y][x]))
                {
                    setKnownBadSpot(x, y);
                    probabilities[y][x] = totalWeight;
                }
            }
            else
            {
                badSpots[y][x] = false;
            }
            probabilities[y][x] /= totalWeight;
        }
    }



    delete array;
    thread->quit();
}

void Solver::partitionCalculate()
{
    generatePartitions();
    PartitionIterator it(partitionList, badSpots);
    QSetIterator<Hole*> setIter(*constrainedUnopenedHoles);
    uint64_t configurationWeight;
    uint64_t partitionWeight;
    int configurationBadness;
    Hole * hole;
    uint64_t totalWeight = 0;
    double probability;
    for(int y = 0; y < boardHeight; y++)
    {
        for(int x = 0; x < boardWidth; x++)
        {
            if(constrainedUnopenedHoles->contains(&holes[y][x]) ||
                    unconstrainedUnopenedHoles->contains(&holes[y][x]))
            {
                probabilities[y][x] = 0.0;
            }
        }
    }
    int totalIterations = 0;
    int legalIterations = 0;
    do
    {
        it.iterate(&partitionWeight, &configurationBadness);
        bombsAmongConstrainedHoles = configurationBadness + knownBadSpots->size();
        totalIterations++;
        if(!validateBoard())
        {
            continue;
        }
        legalIterations++;
        configurationWeight = partitionWeight * choose(
                    unconstrainedUnopenedHoles->size(),
                    bombs + rupoors - bombsAmongConstrainedHoles);
        totalWeight += configurationWeight;

        for(int i = 0; i < partitionList->size(); i++)
        {
            probability = configurationWeight *
                    partitionList->at(i)->badness / (double) partitionList->at(i)->holes->size();
            for(int j = 0; j < partitionList->at(i)->holes->size(); j++)
            {
                hole = partitionList->at(i)->holes->at(j);
                probabilities[hole->y][hole->x] += probability;
            }
        }

        setIter = QSetIterator<Hole*>(*unconstrainedUnopenedHoles);
        while( setIter.hasNext())
        {
            hole = setIter.next();
            probabilities[hole->y][hole->x] +=
                    (double)(configurationWeight *
                             (bombs + rupoors - bombsAmongConstrainedHoles))/
                    unconstrainedUnopenedHoles->size();
        }

    }
    while(it.hasNext());

    std::cout << totalIterations << "\t" <<
                 legalIterations << "\t" <<
                 partitionList->size() << "\t" <<
                 constrainedUnopenedHoles->size() << std::endl;
    for(int y = 0; y < boardHeight; y++)
    {
        for(int x = 0; x < boardWidth; x++)
        {
            if(constrainedUnopenedHoles->contains(&holes[y][x]) ||
                    unconstrainedUnopenedHoles->contains(&holes[y][x]))
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

    thread->quit();
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
    if(unconstrainedUnopenedHoles->size() + bombsAmongConstrainedHoles < bombs + rupoors)
    {
        return false;
    }
    int badSpotsSeen;
    Constraint* constraint;
    Hole * constrainedHole;
    for(int j = constraintList.length() - 1; j >= 0; j--)
    {
        constraint = constraintList.at(j);
        badSpotsSeen = 0;
        for(int i = constraint->holes.length() - 1; i >= 0; i--)
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

uint64_t Solver::choose(uint64_t n, uint64_t k) {
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

void Solver::setKnownBadSpot(int x, int y)
{
    std::cout << x << ", " << y << " Bad" << std::endl;
    knownBadSpots->insert(&holes[y][x]);
    badSpots[y][x] = true;
    probabilities[y][x] = 1.0;
    constrainedUnopenedHoles->remove(&holes[y][x]);
    unconstrainedUnopenedHoles->remove(&holes[y][x]);
    Constraint * constraint;
    Hole * constrainedHole;
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
                    constraint = constraints[filterY][filterX];
                    if(constraintList.contains(constraint))
                    {
                        if(constraint->holes.removeOne(&holes[y][x]))
                        {
                            constraint->maxBadness--;
                            if(constraint->maxBadness == 0)
                            {
                                while(constraint->holes.size() > 0)
                                {
                                    constrainedHole = constraint->holes.takeFirst();
                                    setKnownSafeSpot(constrainedHole->x, constrainedHole->y);
                                }

                                constraintList.removeOne(constraint);
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
    std::cout << x << ", " << y << " Safe" << std::endl;
    knownSafeSpots->insert(&holes[y][x]);
    constrainedUnopenedHoles->remove(&holes[y][x]);
    unconstrainedUnopenedHoles->remove(&holes[y][x]);
    probabilities[y][x] = 0.0;
    badSpots[y][x] = false;
    Constraint * constraint;
    Hole * constrainedHole;
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
                    constraint = constraints[filterY][filterX];
                    if ( (constraintList.contains(constraint) )    &&
                         (constraint->holes.removeOne(&holes[y][x]))    &&
                         (constraint->maxBadness - 1 == constraint->holes.length()) )
                    {
                        while(constraint->holes.size() > 0)
                        {
                            constrainedHole = constraint->holes.takeFirst();
                            setKnownBadSpot(constrainedHole->x, constrainedHole->y);
                        }

                        constraintList.removeOne(constraint);
                    }

                }
            }
        }
    }
}

void Solver::generatePartitions()
{
    for(int i = partitionList->size() - 1; i >= 0; i--)
    {
        delete partitionList->takeAt(i);
    }
    QSetIterator<Hole*> iter(*constrainedUnopenedHoles);
    Hole* constrainedHole;
    Partition * partition;
    bool present;
    while(iter.hasNext())
    {
        constrainedHole = iter.next();
        partition = new Partition;
        partition->constraints = imposingConstraints[constrainedHole->y][constrainedHole->x];
        partition->holes = new QList<Hole*>;
        present = false;
        for(int i = 0; i < partitionList->size(); i++)
        {
            if(*(partitionList->at(i)) == *partition)
            {
                delete partition->holes;
                delete partition;
                partition = partitionList->at(i);
                present = true;
                break;
            }
        }

        partition->holes->append(constrainedHole);
        if(!present)
        {
            partitionList->append(partition);
        }
    }
}
