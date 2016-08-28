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

#include <array>

namespace reBass {
template <typename T, unsigned N>
class Onset_detector
{
public:
    Onset_detector (T power_rise = DEFAULT_POWER_RISE)
    noexcept :
        power_rise{power_rise} {
        std::fill(previous.begin(), previous.end(), 0);
    };

    T process (gsl::span<T const, N> magnitudes)
    noexcept {
        auto result = static_cast<T>(0.1);
        for (auto i = 0u; i < N; i++) {
            if (magnitudes[i] > previous[i] * power_rise) {
                result++;
            }
            previous[i] = magnitudes[i];
        }
        return result / N;
    };
private:
    static constexpr T DEFAULT_POWER_RISE = 2;
    const T power_rise;
    std::array<T, N> previous;
};
}
