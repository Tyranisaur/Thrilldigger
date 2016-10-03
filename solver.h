#ifndef SOLVER_H
#define SOLVER_H
#include "dugtype.h"
#include <QList>
#include <QSet>

struct ProblemParameters;
struct Hole;
struct Constraint;

class Solver
{
public:
    Solver(ProblemParameters * param);
    ~Solver();

    void setCell(int x, int y, DugType::DugType type);
    double ** calculate();


private:
    double ** probabilities;
    QList<Constraint*> constraintList;
    Constraint *** constraints;
    bool ** badSpots;
    Hole ** holes;
    QSet<Hole*> * constrainedUnopenedHoles;
    QSet<Hole*> * unconstrainedUnopenedHoles;

    int bombsAmongConstrainedHoles;
    int knownBadSpots;
    int boardHeight;
    int boardWidth;
    int bombs;
    int rupoors;
    long long ** badSpotWeights;
    DugType::DugType ** board;
    bool validateBoard();
    unsigned long long choose(unsigned long long n, unsigned long long k);
};

#endif // SOLVER_H
