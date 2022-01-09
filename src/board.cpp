#include "headers/board.h"

#include "headers/problemparameters.h"

#include <algorithm>
#include <array>
#include <functional>
#include <numeric>
#include <random>

namespace
{
std::mt19937 rng()
{
    std::random_device dev;
    std::array<std::seed_seq::result_type, std::mt19937::state_size> data{};
    std::generate(data.begin(), data.end(), std::ref(dev));

    std::seed_seq prng_seed(data.begin(), data.end());

    return std::mt19937(prng_seed);
}

std::size_t badSpotsAround(const std::size_t x,
                           const std::size_t y,
                           const Vector2d<DugTypeEnum> &boardRep)
{
    std::size_t badspots = 0;
    for (int filterY = int(y) - 1; filterY < int(y) + 2; filterY++) {
        if (filterY >= 0 && std::size_t(filterY) < boardRep.height()) {
            for (int filterX = int(x) - 1; filterX < int(x) + 2; filterX++) {
                if (filterX >= 0 && std::size_t(filterX) < boardRep.width()) {
                    if (filterX != int(x) || filterY != int(y)) {
                        if (boardRep.at(std::size_t(filterX), std::size_t(filterY)) < 0) {
                            badspots++;
                        }
                    }
                }
            }
        }
    }
    return badspots;
}
}


Board::Board(const ProblemParameters &params)
    : problemParams_(params),
      opened_(params.height, params.width),
      boardRep_(params.height, params.width)
{
    reload();
}

void Board::reload() &
{
    opened_.reset(false);
    boardRep_.reset(dugtype::green);

    placeBadSpots();

    placeRupees();
}

void Board::placeBadSpots()
{
    std::vector indices = std::vector<std::size_t>(std::size_t(problemParams_.height) *
                                                   std::size_t(problemParams_.width));
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), rng());

    std::size_t badSpotIndex = 0;
    for (std::size_t b = 0; b != problemParams_.bombs; ++b) {
        const std::size_t index = indices[badSpotIndex];
        const std::size_t x = index % problemParams_.width;
        const std::size_t y = index / problemParams_.width;

        boardRep_.ref(x, y) = dugtype::bomb;
        ++badSpotIndex;
    }
    for (std::size_t r = 0; r != problemParams_.rupoors; ++r) {
        const std::size_t index = indices[badSpotIndex];
        const std::size_t x = index % problemParams_.width;
        const std::size_t y = index / problemParams_.width;

        boardRep_.ref(x, y) = dugtype::rupoor;
        ++badSpotIndex;
    }
}

void Board::placeRupees()
{
    for (std::size_t y = 0; y != problemParams_.height; ++y) {
        for (std::size_t x = 0; x != problemParams_.width; ++x) {
            if (boardRep_.at(x, y) == dugtype::green) {
                const std::size_t badspots = badSpotsAround(x, y, boardRep_);
                switch (badspots) {
                case 0:
                    boardRep_.ref(x, y) = dugtype::green;
                    break;
                case 1:
                case 2:
                    boardRep_.ref(x, y) = dugtype::blue;
                    break;
                case 3:
                case 4:
                    boardRep_.ref(x, y) = dugtype::red;
                    break;
                case 5:
                case 6:
                    boardRep_.ref(x, y) = dugtype::silver;
                    break;
                case 7:
                case 8:
                    boardRep_.ref(x, y) = dugtype::gold;
                    break;
                default:
                    break;
                }
            }
        }
    }
}

DugTypeEnum Board::getCell(const std::size_t x, const std::size_t y) &
{
    opened_.set(x, y, true);
    return boardRep_.at(x, y);
}

bool Board::hasWon() const &
{
    for (std::size_t y = 0; y != problemParams_.height; ++y) {
        for (std::size_t x = 0; x != problemParams_.width; ++x) {
            if (!opened_.get(x, y) && boardRep_.at(x, y) >= 0) {
                return false;
            }
        }
    }
    return true;
}
