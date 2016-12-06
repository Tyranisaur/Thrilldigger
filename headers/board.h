#ifndef BOARD_H
#define BOARD_H
#include "DugType.h"



struct ProblemParameters;


class Board
{

public:
    Board(ProblemParameters * params);
    ~Board();

    DugType::DugType getCell(int x, int y);
    bool hasWon();

private:
    int height;
    int width;
    bool ** opened;
    DugType::DugType ** boardRep;
};



#endif // BOARD_H
