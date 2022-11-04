#include "SnakeDirection.h"

DirectionChangeMap SnakeDirection::changeMap{
    {SnakeDirection::up, std::vector<int>{-1, 0}},
    {SnakeDirection::down, std::vector<int>{1, 0}},
    {SnakeDirection::left, std::vector<int>{0, -1}},
    {SnakeDirection::right, std::vector<int>{0, 1}}};