#ifndef SOLVER_H
#define SOLVER_H
#include "dugtype.h"
struct ProblemParameters;


class Solver
{
public:
    Solver(ProblemParameters * param);
    ~Solver();

    void setCell(int x, int y, DugType::DugType type);
    double ** calculate();


private:
    int boardHeight;
    int boardWidth;
    int bombs;
    int rupoors;
    DugType::DugType ** board;
};

#endif // SOLVER_H
