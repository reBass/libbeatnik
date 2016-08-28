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
#include <iterator>
#include <numeric>
#include <type_traits>
#include <gsl/span>

namespace reBass
{
/**
 * Static helper functions for simple mathematical calculations.
 */
namespace math
{
template<typename T> constexpr T PI = T(3.141592653589793238462643L);

template <typename T>
T abs_difference (const T a, const T b) noexcept {
    return (a > b) ? (a - b) : (b - a);
}

template <typename I>
static constexpr unsigned abs_distance (I first, I last)
noexcept {
    auto d = std::distance(first, last);
    assert(d >= 0);
    return static_cast<unsigned>(d);
}

template <typename T>
T mean(gsl::span<T const> input)
noexcept {
    assert(std::size(input) > 0);
    return std::accumulate(
        std::cbegin(input),
        std::cend(input),
        static_cast<T>(0)
    ) / std::size(input);
}

template <typename I>
auto mean(I first, I last)
noexcept {
    assert(std::distance(first, last) > 0);
    return std::accumulate(first, last, 0) / std::distance(first, last);
};

template <typename T>
void adaptive_threshold(
    gsl::span<T const> in,
    gsl::span<T> out,
    const unsigned radius
) noexcept {
    T moving_sum = 0;

    const auto in_first = std::cbegin(in);
    const auto in_last = std::cend(in);
    auto moving_back = in_first;
    auto moving_center = in_first;
    auto moving_front = in_first;

    auto out_it = std::begin(out);

    do {
        if (std::distance(moving_back, moving_center) > radius) {
            moving_sum -= *(moving_back++);
        }
        if (abs_distance(in_first, moving_front) >= radius) {
            *(out_it++) = std::fdim(
                *(moving_center++),
                moving_sum / std::distance(moving_back, moving_front)
            );
        }
        if (std::distance(moving_front, in_last) > 0) {
            moving_sum += *(moving_front++);
        }
    } while (moving_center != in_last);
}

template <typename T, std::ptrdiff_t N>
void adaptive_threshold2(
        gsl::span<T const, N> in,
        gsl::span<T, N> out,
        const std::ptrdiff_t radius
) noexcept {
    std::array<T, N> thresh;

    for (std::ptrdiff_t i = 0; i < N; ++i) {
        auto offset = std::max(i - radius, std::ptrdiff_t{0});
        auto count = std::min(N - offset, 2*radius);
        count = std::min(count, i + radius);
        thresh[i] = mean(in.subspan(offset, count));
    }

    std::transform(
            std::cbegin(in),
            std::cend(in),
            std::cbegin(thresh),
            std::begin(out),
            [] (auto value, auto threshold) {
                return std::fdim(value, threshold);
            }
    );
}

template <typename T>
void normalize(gsl::span<T const> in, gsl::span<T> out)
noexcept {
    auto sum = std::accumulate(
        std::cbegin(in),
        std::cend(in),
        static_cast<T>(0),
        [] (const auto result, const auto value) {
            return result + std::fmax(value, 0);
        }
    );

    assert(sum >= 0);
    std::transform(
        std::cbegin(in),
        std::cend(in),
        std::begin(out),
        [sum] (auto v) {
            return v / sum;
        }
    );
}

template <typename T>
void comb_filter(gsl::span<T const> in, gsl::span<T> out)
noexcept {
    assert(in.size() % out.size() == 0);
    auto stage = in.size() / out.size();

    if (stage > 1) {
        comb_filter(in.subspan(0, (stage - 1) * out.size()), out);
    } else {
        std::fill(std::begin(out), std::end(out), 0);
    }

    for (auto i = 1; i < out.size(); ++i) {
        out[i] += mean(in.subspan(stage * (i - 1) + 1, 2*stage - 1));
    }
}

}
}
