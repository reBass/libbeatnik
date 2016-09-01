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
template <typename T, int MaxPeriod>
class Skewed_window
{
public:
    Skewed_window()
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
        int period,
        InputIt rfirst,
        InputIt rlast
    ) const noexcept {
        int d = abs_distance(rfirst, rlast);
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
    static constexpr int 
    index_of(int period) {
        return period - 1 - MaxPeriod / 2;
    };

    static constexpr int 
    period_at(int index) {
        return index + 1 + MaxPeriod / 2;
    };

    static constexpr int 
    max_lag_for(int period) {
        return period * 2;
    };

    static constexpr int 
    min_lag_for(int period) {
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
    window_value(int period, int lag) noexcept {
        auto x = static_cast<T>(lag) / static_cast<T>(period);

        return std::exp(-.5f * std::pow(TIGHTNESS * std::log(2 - x), 2.f));
    };

    static constexpr T TIGHTNESS = 5;
    std::array<std::array<T, 2 * MaxPeriod>, MaxPeriod / 2> cache;
};

}
