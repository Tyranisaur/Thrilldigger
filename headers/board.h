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

    void reload();

private:
    int height;
    int width;
    int bombs;
    int rupoors;
    bool ** opened;
    DugType::DugType ** boardRep;
};



#endif // BOARD_H
