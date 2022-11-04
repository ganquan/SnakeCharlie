#pragma once

#include <string>

class AppRunMode {
public:
    enum Mode : int {
        human = 0,
        train = 1,
        ai = 2
    };

    AppRunMode() = default;
    AppRunMode(int amode) { mode = Mode(amode); }
    constexpr AppRunMode(Mode amode) : mode(amode) {}

    std::string description() {
        switch (mode) {
        case human:
            return "human";
        case ai:
            return "AI";
        case train:
            return "train";
        }
    }

    constexpr operator Mode() const { return mode; }
    explicit operator bool() const = delete;

    constexpr bool operator==(AppRunMode a) const { return mode == a.mode; }
    constexpr bool operator!=(AppRunMode a) const { return mode != a.mode; }

    constexpr bool isHumanMode() const { return mode == human; }
    constexpr bool isTrainMode() const { return mode == train; }
    constexpr bool isAIMode() const { return mode == ai; }

private:
    Mode mode;
};
