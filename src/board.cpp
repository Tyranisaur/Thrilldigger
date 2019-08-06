#include "headers/board.h"

#include "headers/problemparameters.h"
#include <random>

Board::Board(const ProblemParameters &params)
    : problemParams_(params),
      opened_(params.height, params.width),
      boardRep_(params.height, params.width)
{
    int holes = params.height * params.width;
    for (int y = 0; y < params.height; y++) {
        for (int x = 0; x < params.width; x++) {
            opened_.set(x, y, false);
            boardRep_.ref(x, y) = DugType::DugType::green;
        }
    }
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, holes - 1);
    for (int b = 0; b < params.bombs; b++) {
        int index = dist(rng);
        int x = index % params.width;
        int y = index / params.width;

        if (boardRep_.at(x, y) == DugType::DugType::bomb) {
            b--;
            continue;
        }
        boardRep_.ref(x, y) = DugType::DugType::bomb;
    }
    for (int r = 0; r < params.rupoors; r++) {
        int index = dist(rng);
        int x = index % params.width;
        int y = index / params.width;

        if (boardRep_.at(x, y) != DugType::DugType::green) {
            r--;
            continue;
        }
        boardRep_.ref(x, y) = DugType::DugType::rupoor;
    }

    for (int y = 0; y < params.height; y++) {
        for (int x = 0; x < params.width; x++) {
            if (boardRep_.at(x, y) == DugType::DugType::green) {
                int badspots = 0;
                for (int filterY = y - 1; filterY < y + 2; filterY++) {
                    if (filterY >= 0 && filterY < params.height) {
                        for (int filterX = x - 1; filterX < x + 2; filterX++) {
                            if (filterX >= 0 && filterX < params.width) {
                                if (filterX != x || filterY != y) {
                                    if (boardRep_.at(filterX, filterY) < 0) {
                                        badspots++;
                                    }
                                }
                            }
                        }
                    }
                }
                switch (badspots) {
                case 0:
                    boardRep_.ref(x, y) = DugType::DugType::green;
                    break;
                case 1:
                case 2:
                    boardRep_.ref(x, y) = DugType::DugType::blue;
                    break;
                case 3:
                case 4:
                    boardRep_.ref(x, y) = DugType::DugType::red;
                    break;
                case 5:
                case 6:
                    boardRep_.ref(x, y) = DugType::DugType::silver;
                    break;
                case 7:
                case 8:
                    boardRep_.ref(x, y) = DugType::DugType::gold;
                    break;
                }
            }
        }
    }
}

void Board::reload() &
{
    for (int y = 0; y < problemParams_.height; y++) {
        for (int x = 0; x < problemParams_.width; x++) {
            opened_.set(x, y, false);
            boardRep_.ref(x, y) = DugType::DugType::green;
        }
    }
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(
        0, problemParams_.height * problemParams_.width - 1);
    for (int b = 0; b < problemParams_.bombs; b++) {
        int index = dist(rng);
        int x = index % problemParams_.width;
        int y = index / problemParams_.width;

        if (boardRep_.at(x, y) == DugType::DugType::bomb) {
            b--;
            continue;
        }
        boardRep_.ref(x, y) = DugType::DugType::bomb;
    }
    for (int r = 0; r < problemParams_.rupoors; r++) {
        int index = dist(rng);
        int x = index % problemParams_.width;
        int y = index / problemParams_.width;

        if (boardRep_.at(x, y) != DugType::DugType::green) {
            r--;
            continue;
        }
        boardRep_.ref(x, y) = DugType::DugType::rupoor;
    }

    int badspots;
    for (int y = 0; y < problemParams_.height; y++) {
        for (int x = 0; x < problemParams_.width; x++) {
            if (boardRep_.at(x, y) == DugType::DugType::green) {
                badspots = 0;
                for (int filterY = y - 1; filterY < y + 2; filterY++) {
                    if (filterY >= 0 && filterY < problemParams_.height) {
                        for (int filterX = x - 1; filterX < x + 2; filterX++) {
                            if (filterX >= 0 &&
                                filterX < problemParams_.width) {
                                if (filterX != x || filterY != y) {
                                    if (boardRep_.at(filterX, filterY) < 0) {
                                        badspots++;
                                    }
                                }
                            }
                        }
                    }
                }
                switch (badspots) {
                case 0:
                    boardRep_.ref(x, y) = DugType::DugType::green;
                    break;
                case 1:
                case 2:
                    boardRep_.ref(x, y) = DugType::DugType::blue;
                    break;
                case 3:
                case 4:
                    boardRep_.ref(x, y) = DugType::DugType::red;
                    break;
                case 5:
                case 6:
                    boardRep_.ref(x, y) = DugType::DugType::silver;
                    break;
                case 7:
                case 8:
                    boardRep_.ref(x, y) = DugType::DugType::gold;
                    break;
                }
            }
        }
    }
}

DugType::DugType Board::getCell(int x, int y) &
{
    opened_.set(x, y, true);
    return boardRep_.at(x, y);
}

bool Board::hasWon() const &
{
    for (int y = 0; y < problemParams_.height; y++) {
        for (int x = 0; x < problemParams_.width; x++) {
            if (!opened_.get(x, y) && boardRep_.at(x, y) >= 0) {
                return false;
            }
        }
    }
    return true;
}
