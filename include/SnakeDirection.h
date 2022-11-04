#pragma once

#include <glog/logging.h>
#include <map>
#include <string>
#include <vector>

typedef std::map<int8_t, std::vector<int>> DirectionChangeMap;

class SnakeDirection {
public:
    enum Direction : int8_t {
        invalid = 0,
        up = -1,
        down = 1,
        left = -2,
        right = 2
    };

    static DirectionChangeMap changeMap;

    SnakeDirection() = default;
    SnakeDirection(int adirection) { direction = Direction(adirection); }
    constexpr SnakeDirection(Direction adirection) : direction(adirection) {}

    constexpr bool operator==(SnakeDirection a) const { return direction == a.direction; }
    constexpr bool operator!=(SnakeDirection a) const { return direction != a.direction; }

    std::string toString() const {
        switch (direction) {
        case up:
            return "UP";
        case down:
            return "DOWN";
        case left:
            return "LEFT";
        case right:
            return "RIGHT";
        case invalid:
            return "INVALID";
        default:
            LOG(ERROR) << "SnakeDirection::toString Wrong direction.";
            return "Wrong direction";
        }
    }

    std::vector<double> toVector() {

        switch (direction) {
        case up:
            return std::vector<double>{1, 0, 0, 0};
        case right:
            return std::vector<double>{0, 1, 0, 0};
        case down:
            return std::vector<double>{0, 0, 1, 0};
        case left:
            return std::vector<double>{0, 0, 0, 1};

        default:
            LOG(ERROR) << "SnakeDirection::toVector Wrong direction.";
            return std::vector<double>{1, 1, 1, 1};
        }
    }

    int toVectorIndex() {
        switch (direction) {
        case up:
            return 0;
        case right:
            return 1;
        case down:
            return 2;
        case left:
            return 3;
        case invalid:
            return -1;

        default:
            LOG(ERROR) << "SnakeDirection::toVector Wrong direction.";
            return -1;
        }
    }

    int8_t rawValue() {
        return int8_t(direction);
    }
    constexpr int8_t operator+(const SnakeDirection &d) const { return direction + d.direction; }

private:
    Direction direction;
};
