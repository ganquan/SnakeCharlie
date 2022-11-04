#pragma once

class SnakeState {
public:
    enum State : int {
        die = 0,
        alive = 1,
        freeze = 2
    };

    SnakeState() = default;
    SnakeState(int astate) { state = State(astate); }
    constexpr SnakeState(State astate) : state(astate) {}

    bool isAlive() { return state == alive; }
    bool isDie() { return state == die; }
    bool isFreeze() { return state == freeze; }

private:
    State state;
};