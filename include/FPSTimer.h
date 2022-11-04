#pragma once
#include <chrono>
#include <functional>

typedef std::function<void(double)> FPSUpdateFunc;

class FPSTimer {
public:
    FPSTimer();
    ~FPSTimer();

    void reset();
    void tick();
    void setUpdateHandle(const FPSUpdateFunc &update);

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    int m_frameCounter;
    double m_fps;

    FPSUpdateFunc m_update;
};
