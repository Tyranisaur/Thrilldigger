#include "solver.h"
#include "problemparameters.h"

Solver::Solver(ProblemParameters * params)
{
    boardHeight = params->height;
    boardWidth = params->width;
    bombs = params->bombs;
    rupoors = params->rupoors;
    board = new DugType::DugType*[boardHeight];
    for(int y = 0; y < boardHeight; y++)
    {
        board[y] = new DugType::DugType[boardWidth];
        for(int x = 0; x < boardWidth; x++)
        {
            board[y][x] = DugType::DugType::undug;
        }
    }
}

Solver::~Solver()
{
    delete[] board;
}

void Solver::setCell(int x, int y, DugType::DugType type){
    board[y][x] = type;
}

double ** Solver::calculate()
{
    double ** ret = new double*[boardHeight];
    for(int y = 0; y < boardHeight; y++)
    {
        ret[y] = new double[boardWidth];
        for(int x = 0; x < boardWidth; x++)
        {
            ret[y][x] = 0.0;
        }
    }
    return ret;
}
