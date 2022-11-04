#pragma once

#include <fmt/core.h>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>

namespace uuid {
    std::string generate_uuid_v4();
};

namespace utility {

    namespace random {
        int generateRandomNumber(int low, int high);
        double generateRandomDouble(double low, double high);
    }; // namespace random

    namespace time {
        // https://stackoverflow.com/questions/42138599/how-to-format-stdchrono-durations
        template <class... Durations, class DurationIn>
        std::tuple<Durations...> break_down_durations(DurationIn d) {
            std::tuple<Durations...> retval;
            using discard = int[];
            (void)discard{0, (void((
                                  (std::get<Durations>(retval) = std::chrono::duration_cast<Durations>(d)),
                                  (d -= std::chrono::duration_cast<DurationIn>(std::get<Durations>(retval))))),
                              0)...};
            return retval;
        }

        void measure(const std::string &label, std::string &result, std::function<void()> const &lambda);

        void measure(std::chrono::time_point<std::chrono::high_resolution_clock> &start,
                     std::chrono::time_point<std::chrono::high_resolution_clock> &end,
                     std::function<void()> const &lambda);

        std::string formatToString(std::chrono::duration<double, std::milli> const &duration);

        class Timer {
        public:
            Timer(const std::string &label);
            ~Timer();

        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
            std::string m_label;
        };

    }; // namespace time

}; // namespace utility
