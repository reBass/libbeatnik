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

#include <cassert>
#include <cmath>

#include <algorithm>
#include <numeric>
#include <gsl/span>

#include "../common/Element.hpp"
#include "../common/math.hpp"
#include "../common/Ring_array.hpp"
#include "Skewed_window.hpp"

namespace reBass {
template <typename T, int MaxPeriod, int BeatsCount = 8>
class Tracker
{
public:
    void set_period_guess(int period)
    noexcept {
        assert(is_valid_period(period));
        period_guess = period;
    }

    bool update_score (T odf_value)
    noexcept {
        if (!is_valid_period(period_guess)) {
            return false;
        }

        auto last_score = window.find_max_score(
                period_guess,
                cumulative_score.rbegin(),
                cumulative_score.rend()
            );

        auto new_score = (1 - ALPHA) * odf_value + ALPHA * last_score.value;
        cumulative_score.push_back(new_score);
        backlink.push_back(static_cast<unsigned>(last_score.index));

        counter++;

        return new_estimate_expected();
    }

    bool new_estimate_expected()
    noexcept {
        return is_valid_period(period_guess)
               && counter > period_guess;
    }

    T estimate_period()
    noexcept {
        counter = 0;

        auto max_index = static_cast<int>(
            Element<T>::max(
                std::crbegin(cumulative_score),
                std::crbegin(cumulative_score) + period_guess
            ).index
        );
        auto last_beat = N - max_index - 1;

        T periods_sum = 0;
        T periods_count = 0;

        while (last_beat > backlink[last_beat]) {
            if (backlink[last_beat] == 0) break; // JIC
            periods_sum += backlink[last_beat];
            periods_count++;
            last_beat -= backlink[last_beat];
        }

        return periods_sum / periods_count;
    };

    void clear()
    noexcept {
        period_guess = 0;
        counter = 0;
        std::fill(
            std::begin(cumulative_score),
            std::end(cumulative_score),
            0
        );
        std::fill(
            std::begin(backlink),
            std::end(backlink),
            0
        );
    }

private:
    static constexpr bool
    is_valid_period (int period)
    noexcept {
        return period > MaxPeriod / 2
               && period <= MaxPeriod;
    }

    static constexpr float ALPHA = 0.9f;
    static constexpr int N = MaxPeriod * BeatsCount;

    int period_guess;
    int counter;
    Ring_array<T, N> cumulative_score;
    Ring_array<int, N> backlink;
    Skewed_window<T, MaxPeriod> window;
};
}
