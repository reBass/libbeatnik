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
#include <cstdlib>

#include <array>
#include <iterator>
#include <utility>

#include <gsl/span>

#include "../common/math.hpp"
#include "../common/Element.hpp"

namespace reBass {
template <typename T, std::ptrdiff_t N>
class Viterbi
{
public:
    Viterbi()
    noexcept {
        delta.fill(initial_delta);
        encache();
    }

    int
    decode(gsl::span<T const, N> input)
    noexcept {
        calculate_transition(input);
        math::normalize(gsl::span<T, N>(delta));

        return psi[
            Element<T>::max(
                std::cbegin(delta),
                std::cend(delta)
            ).index
        ];
    }
private:
    void
    calculate_transition (gsl::span<T const, N> input)
    noexcept {
        auto delta_it = std::begin(delta);
        auto psi_it = std::begin(psi);
        auto input_it = std::cbegin(input);

        for (auto i = 0; i < 3 * radius; i++) {
            auto m = i % radius;
            if (i >= radius) {
                *(delta_it++) = *(input_it++) * temp[m];
            }
            if (i < N) {
                auto max_el = max_delta_element(i);
                *(psi_it++) = max_el.first; // index
                temp[m] = max_el.second; // value
            }
        }
    }

    std::pair<int, T>
    max_delta_element(int permutation)
    const noexcept {
        T max_value = delta[0] * at(permutation, 0);
        int max_index = 0;
        for (auto i = 0; i < N; i++) {
            auto product = delta[i] * at(i, permutation);
            if (product > max_value) {
                max_index = i;
                max_value = product;
            }
        }
        return std::make_pair(max_index, max_value);
    }

    T
    at(int i, int j)
    const noexcept {
        auto distance = math::abs_difference(i, j);
        if (distance >= radius) {
            return 0.f;
        }
        return cache[distance];
    }

    void
    encache()
    noexcept {
        auto sigma_squared = powf(radius / 4.f, 2.f);

        for (auto i = 0u; i < radius; i++) {
            T mu_squared = i * i;
            cache[i] = std::exp(-1 * mu_squared / (2 * sigma_squared));
        }
    }

    static constexpr T initial_delta = 1;
    static constexpr int radius = static_cast<int>(N/2);
    std::array<T, radius> cache;
    std::array<T, radius> temp;
    std::array<T, N> delta;
    std::array<std::ptrdiff_t, N> psi;
};
}

