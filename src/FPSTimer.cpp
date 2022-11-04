#include "FPSTimer.h"

FPSTimer::FPSTimer() {
    m_frameCounter = 0;
    m_fps = 0;
}

void FPSTimer::setUpdateHandle(const FPSUpdateFunc &update) {
    this->m_update = update;
}

FPSTimer::~FPSTimer() {
}

void FPSTimer::reset() {
    m_start = std::chrono::high_resolution_clock::now();
    m_frameCounter = 0;
}

void FPSTimer::tick() {

    auto now = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapse = now - m_start;

    if (elapse.count() > 1000) {

        m_fps = double(m_frameCounter) / double(elapse.count() / 1000.f);

        m_update(m_fps);

        reset();

    } else {
        m_frameCounter++;
    }
}
