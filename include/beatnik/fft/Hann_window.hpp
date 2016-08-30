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

#include <algorithm>
#include <array>
#include <iterator>

#include "../common/math.hpp"

namespace reBass
{
template<typename T, std::size_t N>
class Hann_window {
public:
    Hann_window() {
        encache();
    };

    template <class InputIt, class OutputIt>
    void cut(InputIt in, OutputIt out)
    const noexcept {
        std::transform(
            std::cbegin(cache),
            std::cend(cache),
            in,
            out,
            std::multiplies<>()
        );
    }

    static constexpr T norm_correction() {
        return static_cast<T>(0.5);
    }
private:
    std::array<T, N> cache;

    void encache()
    noexcept {
        for (auto i = 0u; i < cache.size(); ++i) {
            cache[i] = window_function(i, cache.size());
        }
    }

    static constexpr T window_function(
        std::size_t position,
        std::size_t window_size
    ) noexcept {
        auto relative_position = static_cast<T>(position) / window_size;
        return (1 - std::cos(relative_position * 2 * math::PI<T>)) / 2;
    }
};

}
