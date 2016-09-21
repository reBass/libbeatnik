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

#include <re/lib/common.hpp>
#include <re/lib/container/ring_array.hpp>
#include <re/lib/math/element.hpp>

#include <re/beatnik/tracker/skewed_window.hpp>

namespace re {
namespace beatnik {

template <typename T, int_t MinPeriod, int_t BeatsCount = 8>
class tracker
{
public:
    static constexpr int_t min_period = MinPeriod;
    static constexpr int_t max_period = 2 * min_period;
    static constexpr int_t beats_count = BeatsCount;

    void
    set_period_guess(int_t period) noexcept
    {
        assert(is_valid_period(period));
        period_guess = period;
    }

    bool
    update_score(T odf_value) noexcept
    {
        if (!is_valid_period(period_guess)) {
            return false;
        }

        auto last_score = window.find_max_score(
            period_guess,
            cumulative_score.rbegin(),
            cumulative_score.rend()
        );

        auto new_score = (1 - alpha) * odf_value + alpha * last_score.value;
        cumulative_score.push_back(new_score);
        // a distance from the last beat
        backlink.push_back(static_cast<int_t>(last_score.index));

        ++counter;

        return new_estimate_expected();
    }

    bool
    new_estimate_expected() noexcept
    {
        return is_valid_period(period_guess)
               && counter > period_guess;
    }

    T
    estimate_period() noexcept
    {
        counter = 0;

        auto max_index = static_cast<int_t>(
            std::distance(
                std::crbegin(cumulative_score),
                std::max_element(
                    std::crbegin(cumulative_score),
                    std::crbegin(cumulative_score) + period_guess
                )
            )
        );
        auto last_beat = N - max_index - 1;

        T periods_sum = 0;
        T periods_count = 0;

        // jump in reverse from a beat to another
        while (last_beat > backlink[last_beat]) {
            if (backlink[last_beat] == 0) break; // JIC
            periods_sum += backlink[last_beat];
            periods_count++;
            last_beat -= backlink[last_beat];
        }

        return periods_sum / periods_count;
    };

    void
    clear() noexcept
    {
        period_guess = 0;
        counter = 0;
        cumulative_score.fill(0.f);
        backlink.fill(0);
    }

private:
    static constexpr bool
    is_valid_period(int_t period) noexcept
    {
        return period >= min_period
               && period < max_period;
    }

    static constexpr float alpha = 0.9f;
    static constexpr int_t N = min_period * beats_count;

    int_t period_guess;
    int_t counter;
    ring_array <T, N> cumulative_score;
    ring_array <int_t, N> backlink;
    skewed_window <T, min_period> window;
};

}
}
