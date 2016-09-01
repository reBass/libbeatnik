// Copyright (c) 2016 Roman Beránek. All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <cmath>

#include <array>
#include <iterator>

#include "../common/Element.hpp"

namespace reBass {
template <typename T, int MinPeriod>
class Skewed_window
{
public:
    Skewed_window()
    noexcept {
        for (auto period = min_period; period < max_period; ++period) {
            cache[index_of(period)].fill(0);
            for (auto l = max_lag_at(period); l >= min_lag_at(period); --l) {
                cache[index_of(period)][l - 1] = window_value(period, l);
            }
        }
    };

    template <typename InputIt>
    Element<T>
    find_max_score(
        int period,
        InputIt rfirst,
        InputIt rlast
    ) const noexcept {
        int d = abs_distance(rfirst, rlast);
        d = std::min(row_size, d);
        auto result = max_result(
            rfirst,
            rfirst + d,
            std::cbegin(cache[index_of(period)]),
            std::multiplies<T>()
        );
        return result;
    };

private:
    static constexpr int min_period = MinPeriod;
    static constexpr int max_period = 2*min_period;
    static constexpr int period_range = max_period - min_period;
    static constexpr int max_lag = 2 * max_period;
    static constexpr int row_size = max_lag;

    static constexpr int 
    index_of(int period) {
        return period - min_period;
    };

    static constexpr int 
    period_at(int index) {
        return index + min_period;
    };

    static constexpr int 
    max_lag_at (int period) {
        return period * 2;
    };

    static constexpr int 
    min_lag_at (int period) {
        return period / 2;
    };

    template <typename I>
    static constexpr int 
    abs_distance(I first, I last)
    noexcept {
        auto d = std::distance(first, last);
        assert(d >= 0);
        return static_cast<int>(d);
    }

    static T 
    window_value(int period, int lag)
    noexcept {
        auto x = static_cast<T>(lag) / static_cast<T>(period);
        return std::exp(-.5f * std::pow(TIGHTNESS * std::log(2 - x), 2.f));
    };

    static constexpr T TIGHTNESS = 5;
    std::array<std::array<T, row_size>, period_range> cache;
};

}
