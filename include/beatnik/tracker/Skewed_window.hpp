#pragma once

#include <cmath>

#include <array>
#include <iterator>

#include "../common/Element.hpp"

namespace reBass {
template <typename T, unsigned MaxPeriod>
class Skewed_window
{
public:
    Skewed_window ()
    noexcept {
        for (auto period = period_at(0); period <= MaxPeriod; ++period) {
            cache[index_of(period)].fill(0);
            for (auto lag = max_lag_for(period);
                 lag >= min_lag_for(period); --lag
                ) {
                cache[index_of(period)][lag - 1] = window_value(period, lag);
            }
        }
    };

    template <typename InputIt>
    Element<T>
    find_max_score(
        unsigned period,
        InputIt rfirst,
        InputIt rlast
    ) const {
        unsigned d = abs_distance(rfirst, rlast);
        d = std::min(max_lag_for(MaxPeriod), d);
        auto result = max_result(
            rfirst,
            rfirst + d,
            std::cbegin(cache[index_of(period)]),
            std::multiplies<T>()
        );
        return result;
    };

private:
    static constexpr unsigned index_of (unsigned period) {
        return period - 1 - MaxPeriod / 2;
    };

    static constexpr unsigned period_at (unsigned index) {
        return index + 1 + MaxPeriod / 2;
    };

    static constexpr unsigned max_lag_for (unsigned period) {
        return period * 2;
    };

    static constexpr unsigned min_lag_for (unsigned period) {
        return period / 2;
    };

    template <typename I>
    static constexpr unsigned abs_distance (I first, I last)
    noexcept {
        auto d = std::distance(first, last);
        assert(d >= 0);
        return static_cast<unsigned>(d);
    }

    static T window_value (unsigned period, unsigned lag) noexcept {
        auto x = static_cast<T>(lag) / static_cast<T>(period);

        return std::exp(-.5f * std::pow(TIGHTNESS * std::log(2 - x), 2.f));
    };

    static constexpr T TIGHTNESS = 5;
    std::array<std::array<T, 8 * MaxPeriod>, MaxPeriod / 2> cache;
};

}
