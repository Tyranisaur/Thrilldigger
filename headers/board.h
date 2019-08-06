#pragma once
#include "dugtype.h"

#include "problemparameters.h"
#include "vector2d.h"

class Board
{

public:
    explicit Board(const ProblemParameters &params);

    DugType::DugType getCell(int x, int y) &;
    bool hasWon() const &;

    void reload() &;

private:
    ProblemParameters problemParams_;
    Vector2d<bool> opened_;
    Vector2d<DugType::DugType> boardRep_;
};