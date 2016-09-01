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
T abs_difference (const T a, const T b)
noexcept {
    return (a > b) ? (a - b) : (b - a);
}

template <typename T, std::ptrdiff_t N>
T
mean(gsl::span<T const, N> input)
noexcept {
    assert(std::size(input) > 0);
    return std::accumulate(
        std::cbegin(input),
        std::cend(input),
        static_cast<T>(0)
    ) / std::size(input);
}

template <
    typename T,
    std::ptrdiff_t N_in,
    std::ptrdiff_t N_out,
    std::ptrdiff_t N_thresh
>
void threshold_reference(
    gsl::span<T const, N_in> in,
    gsl::span<T, N_out> out,
    gsl::span<T, N_thresh> thresh,
    std::ptrdiff_t radius
) noexcept {
    assert(
        std::size(in) == std::size(out)
        && std::size(out) == std::size(thresh)
    );
    auto const n = std::size(in);

    for (std::ptrdiff_t i = 0; i < n; ++i) {
        auto offset = std::max(i - radius, std::ptrdiff_t{0});
        auto count = std::min(n - offset, 2*radius);
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

template <typename T, std::ptrdiff_t N_in, std::ptrdiff_t N_out>
void
adaptive_threshold(
    gsl::span<T const, N_in> in,
    gsl::span<T, N_out> out,
    std::ptrdiff_t radius
) noexcept {
    assert(std::size(in) == std::size(out));
    auto const n = std::size(in);

    assert(n >= 2*radius);
    T sum = 0;
    std::ptrdiff_t count = 0;

    for (auto i = 0; i < n + radius; ++i) {
        if (i < n) {
            sum += in[i];
            ++count;
        }
        if (i >= radius) {
            out[i - radius] = std::fdim(in[i - radius], sum / count);
            if (i > 2*radius) {
                sum -= in[i - 2*radius];
                --count;
            }
        }
    }
}

template <typename T, std::ptrdiff_t N>
void
adaptive_threshold(gsl::span<T, N> data, std::ptrdiff_t radius)
noexcept {
    adaptive_threshold<T, N>(data, data, radius);
}

template <typename T, std::ptrdiff_t N_in, std::ptrdiff_t N_out>
void
normalize(gsl::span<T const, N_in> in, gsl::span<T, N_out> out)
noexcept {
    assert(std::size(in) == std::size(out));
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

template <typename T, std::ptrdiff_t N>
void
normalize(gsl::span<T, N> data)
noexcept {
    normalize(gsl::span<T const, N>(data), data);
}

template <
    typename T,
    std::ptrdiff_t N_in,
    std::ptrdiff_t N_out,
    std::ptrdiff_t N_new = (N_in > N_out) ? N_in - N_out : 0,
    std::ptrdiff_t Stage = N_in / N_out
>
void
comb_filter(gsl::span<T const, N_in> in, gsl::span<T, N_out> out)
noexcept {
    static_assert(N_in >= 0 && N_out >= 0);
    static_assert(N_in % N_out == 0);

    if (Stage > 1) {
        comb_filter(gsl::span<T const, N_new>(in.data(), N_new), out);
    } else {
        std::fill(std::begin(out), std::end(out), 0);
    }

    for (auto i = 1; i < N_out; ++i) {
        out[i] += mean(in.subspan(Stage * (i - 1) + 1, 2*Stage - 1));
    }
}

}
}
