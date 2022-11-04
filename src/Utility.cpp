#include "Utility.h"
#include <chrono>
#include <cstdlib>

namespace uuid {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    std::string generate_uuid_v4() {
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }

} // namespace uuid

namespace utility {
    namespace random {
        static std::random_device dev;
        //随机数发生器
        // mt19937 随机效果好，但是慢
        // minstd_rand 随机效果比不上mt19937，但是快很多
        static std::mt19937 mt19937_rng(dev());
        static std::minstd_rand minstd_rng(dev());

        int generateRandomNumber(int low, int high) {
#if 0
            std::uniform_int_distribution<std::mt19937::result_type> dist(low, high); // distribution in range [low, high]
            return dist(mt19937_rng);
#else
            std::uniform_int_distribution<std::minstd_rand::result_type> dist(low, high); // distribution in range [low, high]
            return dist(minstd_rng);
#endif
        }

        double generateRandomDouble(double low, double high) {
#if 0
            std::uniform_real_distribution<double> dist(low, high);
            return dist(mt19937_rng);
#else
            std::uniform_real_distribution<double> dist(low, high);
            return dist(minstd_rng);
#endif
        }

    } // namespace random

    namespace memory {

    }

    namespace time {
        void measure(const std::string &label, std::string &result, std::function<void()> const &lambda) {

            std::chrono::time_point<std::chrono::high_resolution_clock> start;
            std::chrono::time_point<std::chrono::high_resolution_clock> end;

            measure(start, end, lambda);

            std::chrono::duration<double, std::milli> elapsed = end - start;
            std::string durationStr = utility::time::formatToString(elapsed);

            result = fmt::format("{} - cost: {}", label, durationStr);
        }

        void measure(std::chrono::time_point<std::chrono::high_resolution_clock> &start,
                     std::chrono::time_point<std::chrono::high_resolution_clock> &end,
                     std::function<void()> const &lambda) {

            start = std::chrono::high_resolution_clock::now();
            lambda();
            end = std::chrono::high_resolution_clock::now();
        }

        std::string formatToString(std::chrono::duration<double, std::milli> const &duration) {
            auto formattedDuration = utility::time::break_down_durations<std::chrono::hours,
                                                                         std::chrono::minutes,
                                                                         std::chrono::seconds,
                                                                         std::chrono::milliseconds,
                                                                         std::chrono::microseconds>(duration);
            return fmt::format("{}h, {}min, {}sec, {}ms, {}us",
                               std::get<0>(formattedDuration).count(),
                               std::get<1>(formattedDuration).count(),
                               std::get<2>(formattedDuration).count(),
                               std::get<3>(formattedDuration).count(),
                               std::get<4>(formattedDuration).count());
        }

        Timer::Timer(const std::string &label) {
            m_start = std::chrono::high_resolution_clock::now();
            m_label = std::string(label);
        }

        Timer::~Timer() {
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double, std::milli> elapsed = end - m_start;
            std::string durationStr = utility::time::formatToString(elapsed);

            fmt::print("{} - cost: {}", m_label, durationStr);
        }

    } // namespace time

} // namespace utility
