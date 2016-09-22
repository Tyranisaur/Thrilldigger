#include "board.h"
#include <random>

Board::Board(ProblemParameters * params)
{
    int holes = params->height * params->width;
    boardRep = new CellType*[params->height];
    for(int i = 0; i < params->height; i++)
    {
        boardRep[i] = new CellType[params->width];
        for(int j = 0; j < params->width; j++)
        {
            boardRep[i][j] = CellType::green;
        }
    }
    int index;
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(0, holes - 1);
    int x, y;
    for(int b = 0; b < params->bombs; b++)
    {
        index = distr(eng);
        x = index % params->width;
        y = index / params->width;

        if(boardRep[y][x] == CellType::bomb)
        {
            b--;
            continue;
        }
        boardRep[y][x] = CellType::bomb;

    }
    for(int r = 0; r < params->rupoors; r++)
    {
        index = distr(eng);
        x = index % params->width;
        y = index / params->width;

        if(boardRep[y][x] != CellType::green)
        {
            r--;
            continue;
        }
        boardRep[y][x] = CellType::rupoor;

    }


    int badspots;
    for(int y = 0; y < params->height; y++)
    {
        for(int x = 0; x < params->width; x++)
        {
            if(boardRep[y][x] == CellType::green)
            {
                badspots = 0;
                for(int filterY = y - 1; filterY < y + 2; filterY++)
                {
                    if(filterY >= 0 && filterY < params->height)
                    {
                        for(int filterX = x - 1; filterX < x + 2; filterX++)
                        {
                            if(filterX >= 0 && filterX < params->width)
                            {
                                if(filterX != x || filterY != y)
                                {
                                    if(boardRep[filterY][filterX] < 0)
                                    {
                                        badspots++;
                                    }
                                }
                            }
                        }
                    }
                }
                switch(badspots)
                {
                case 0:
                    boardRep[y][x] = CellType::green;
                    break;
                case 1:
                case 2:
                    boardRep[y][x] = CellType::blue;
                    break;
                case 3:
                case 4:
                    boardRep[y][x] = CellType::red;
                    break;
                case 5:
                case 6:
                    boardRep[y][x] = CellType::silver;
                    break;
                case 7:
                case 8:
                    boardRep[y][x] = CellType::gold;
                    break;
                }
            }
        }
    }

}
Board::~Board()
{
    delete[] boardRep;
}

CellType Board::getCell(int x, int y)
{
    return boardRep[y][x];
}
