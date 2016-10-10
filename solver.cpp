#include "solver.h"
#include "problemparameters.h"
#include <QSet>
#include <QSetIterator>
#include <QThread>
#include "hole.h"
#include "constraint.h"
#include "configurationiterator.h"
#include <iostream>

Solver::Solver(ProblemParameters * params, QThread * thread)
{
    this->thread = thread;
    boardHeight = params->height;
    boardWidth = params->width;
    bombs = params->bombs;
    rupoors = params->rupoors;
    board = new DugType::DugType*[boardHeight];
    constrainedUnopenedHoles = new QSet<Hole*>;
    unconstrainedUnopenedHoles = new QSet<Hole*>;
    holes = new Hole*[boardHeight];
    badSpots = new bool*[boardHeight];
    constraints = new Constraint**[boardWidth];
    for(int y = 0; y < boardHeight; y++)
    {
        board[y] = new DugType::DugType[boardWidth];
        holes[y] = new Hole[boardWidth];
        badSpots[y] = new bool[boardWidth];
        constraints[y] = new Constraint*[boardWidth];
        for(int x = 0; x < boardWidth; x++)
        {
            constraints[y][x] = nullptr;
            badSpots[y][x] = false;
            holes[y][x] = {x, y};
            unconstrainedUnopenedHoles->insert( &holes[y][x] );
            board[y][x] = DugType::DugType::undug;
        }
    }
    probabilities = new double*[boardHeight];
    for(int y = 0; y < boardHeight; y++)
    {
        probabilities[y] = new double[boardWidth];
        for(int x = 0; x < boardWidth; x++)
        {
            probabilities[y][x] = 0.0;
        }
    }
    knownBadSpots = 0;
    std::cout << std::endl;
}

Solver::~Solver()
{
    delete[] board;
    delete [] badSpots;
    delete [] holes;
    delete [] constraints;
    delete constrainedUnopenedHoles;
    delete unconstrainedUnopenedHoles;
    delete[] probabilities;
}

void Solver::setCell(int x, int y, DugType::DugType type){
    unconstrainedUnopenedHoles->remove(&holes[y][x]);
    if(board[y][x] >= -2 && board[y][x] < 0)
    {
        knownBadSpots--;
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
                            if(badSpots[filterY][filterX])
                            {
                                constraint->maxBadness--;
                            }
                            else if(board[filterY][filterX] == DugType::DugType::undug)
                            {
                                constraint->holes.append(&holes[filterY][filterX]);
                                constrainedUnopenedHoles->insert(&holes[filterY][filterX]);
                                unconstrainedUnopenedHoles->remove(&holes[filterY][filterX]);
                            }

                        }
                    }
                }
            }
        }
        constraints[y][x] = constraint;
        if(constraint->maxBadness >= 0)
        {
            constraintList.append(constraint);
        }
    }
    else if(type >= -2)
    {
        knownBadSpots++;
        badSpots[y][x] = true;
        constrainedUnopenedHoles->remove(&holes[y][x]);
        constraintList.removeOne(constraints[y][x]);
    }
}
void Solver::calculate()
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
                std::max(bombs + rupoors - knownBadSpots - unconstrainedUnopenedHoles->size(), 0),
                std::min(i, bombs + rupoors - knownBadSpots));
    unsigned long long configurationWeight;
    unsigned long long totalWeight = 0;
    for(int y = 0; y < boardHeight; y++)
    {
        std::fill(probabilities[y], probabilities[y] + boardWidth, 0.0);
    }
    int totalIterations = 0;
    int legalIterations = 0;
    do
    {
        bombsAmongConstrainedHoles = it.iterate() + knownBadSpots;
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

    std::cout << totalIterations << " configurations evaluated, found  " << legalIterations  << " legal ones" << std::endl;
    for(int y = 0; y < boardHeight; y++)
    {
        for(int x = 0; x < boardWidth; x++)
        {
            if(probabilities[y][x] == totalWeight)
            {
                if(constrainedUnopenedHoles->contains(&holes[y][x]))
                {

                    knownBadSpots++;

                    constrainedUnopenedHoles->remove(&holes[y][x]);
                    badSpots[y][x] = true;
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
            if(board[constrainedHole->y][constrainedHole->x] >= -2)
            {
                constraint->holes.removeAt(i);
                if(board[constrainedHole->y][constrainedHole->x] < 0)
                {
                    constraint->maxBadness--;
                    badSpotsSeen--;
                }

            }
        }
        if(badSpotsSeen != constraint->maxBadness &&
                badSpotsSeen + 1 != constraint->maxBadness)
        {
            return false;
        }
        if(constraint->maxBadness == 0 && constraint->holes.length() == 0)
        {
            constraintList.removeAt(j);
        }

    }
    return true;
}

unsigned long long Solver::choose(unsigned long long n, unsigned long long k) {
    if (k > n ) {
        return 0;
    }
    unsigned long long r = 1;
    for (unsigned long long d = 1; d <= k; ++d) {
        r *= n--;
        r /= d;
    }
    return r;
}
