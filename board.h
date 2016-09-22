#ifndef BOARD_H
#define BOARD_H


struct ProblemParameters{
    const int width;
    const int height;
    const int bombs;
    const int rupoors;
};

enum CellType{
    bomb = -2,
    rupoor = -1,
    green = 0,
    blue = 2,
    red = 4,
    silver = 6,
    gold = 8,
};


class Board
{

public:
    Board(ProblemParameters * params);
    ~Board();

    CellType getCell(int x, int y);

private:
    CellType ** boardRep;
};



#endif // BOARD_H
