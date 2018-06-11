#pragma once

#include <cmath>

#include <array>
#include <iterator>

#include <re/lib/math/element.hpp>

namespace re {
namespace beatnik {

template <typename T, int_t MinPeriod>
class skewed_window
{
public:
    skewed_window() noexcept
    {
        for (auto period = min_period; period < max_period; ++period) {
            cache[index_of(period)].fill(0);
            for (auto l = max_lag_at(period); l >= min_lag_at(period); --l) {
                cache[index_of(period)][l - 1] = window_value(period, l);
            }
        }
    };

    template <typename InputIt>
    math::element<T>
    find_max_score(int_t period, InputIt rfirst, InputIt rlast) const noexcept
    {
        auto d = static_cast<int_t>(std::distance(rfirst, rlast));
        d = std::min(max_lag_at(max_period), d);
        auto result = math::max_result(
            rfirst,
            rfirst + d,
            std::cbegin(cache[index_of(period)]),
            std::multiplies<T>()
        );
        return result;
    };

private:
    static constexpr int_t min_period = MinPeriod;
    static constexpr int_t max_period = 2*min_period;
    static constexpr int_t period_range = max_period - min_period;
    static constexpr int_t max_lag = 2 * max_period;
    static constexpr int_t row_size = max_lag;

    static constexpr int_t
    index_of(int_t period) {
        return period - min_period;
    };

    static constexpr int_t
    period_at(int_t index) {
        return index + min_period;
    };

    static constexpr int_t
    max_lag_at (int_t period) {
        return period * 2;
    };

    static constexpr int_t
    min_lag_at (int_t period) {
        return period / 2;
    };

    static T 
    window_value(int_t period, int_t lag) noexcept
    {
        auto x = static_cast<T>(lag) / static_cast<T>(period);
        return std::exp(-.5f * std::pow(TIGHTNESS * std::log(2 - x), 2.f));
    };

    static constexpr T TIGHTNESS = 5;
    std::array<std::array<T, row_size>, period_range> cache;
};

}
}
