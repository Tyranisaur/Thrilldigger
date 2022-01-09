#pragma once
#include "dugtype.h"

#include "problemparameters.h"
#include "vector2d.h"

class Board
{

public:
    explicit Board(const ProblemParameters &params);

    DugTypeEnum getCell(std::size_t x, std::size_t y) &;
    bool hasWon() const &;

    void reload() &;

private:
    ProblemParameters problemParams_;
    Vector2d<bool> opened_;
    Vector2d<DugTypeEnum> boardRep_;

    void placeBadSpots();
    void placeRupees();
};
