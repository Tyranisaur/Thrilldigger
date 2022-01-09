#pragma once
#include "dugtype.h"

namespace rupee
{
constexpr int green = 1;
constexpr int blue = 5;
constexpr int red = 20;
constexpr int silver = 100;
constexpr int gold = 300;
constexpr int rupoor = -10;

constexpr int value(const DugTypeEnum dugType) {
    switch (dugType) {
    case dugtype::green:
        return green;
    case dugtype::blue:
        return blue;
    case dugtype::red:
        return red;
    case dugtype::silver:
        return silver;
    case dugtype::gold:
        return gold;
    case dugtype::rupoor:
        return rupoor;
    case dugtype::undug:
    case dugtype::bomb:
        break;
    }
    return 0;
}
} // namespace rupee
