#ifndef BOARD_H
#define BOARD_H
#include "celltype.h"



struct ProblemParameters;


class Board
{

public:
    Board(ProblemParameters * params);
    ~Board();

    CellType::CellType getCell(int x, int y);
    bool hasWon();

private:
    int height;
    int width;
    bool ** opened;
    CellType::CellType ** boardRep;
};



#endif // BOARD_H
