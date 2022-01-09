#pragma once

#include "problemparameters.h"

namespace difficulty
{
enum class Difficulty {
    BEGINNER,
    INTERMEDIATE,
    EXPERT,
};


namespace beginner
{
constexpr int width = 5;
constexpr int height = 4;
constexpr int bombs = 4;
constexpr int ruppors = 0;
} // namespace beginner

namespace intermediate
{
constexpr int width = 6;
constexpr int height = 5;
constexpr int bombs = 4;
constexpr int ruppors = 4;
} // namespace intermediate

namespace expert
{
constexpr int width = 8;
constexpr int height = 5;
constexpr int bombs = 8;
constexpr int ruppors = 8;
} // namespace expert

constexpr ProblemParameters beginnerParams{
    beginner::width, beginner::height, beginner::bombs, beginner::ruppors};

constexpr ProblemParameters intermediateParams{intermediate::width,
                                               intermediate::height,
                                               intermediate::bombs,
                                               intermediate::ruppors};

constexpr ProblemParameters expertParams{
    expert::width, expert::height, expert::bombs, expert::ruppors};

constexpr ProblemParameters difficultyParameters(const Difficulty difficultyLevel) {
    switch (difficultyLevel) {
    case Difficulty::BEGINNER:
        return beginnerParams;
    case Difficulty::INTERMEDIATE:
        return intermediateParams;
    case Difficulty::EXPERT:
        return expertParams;
    }
    return {};
}

} // namespace difficulty
