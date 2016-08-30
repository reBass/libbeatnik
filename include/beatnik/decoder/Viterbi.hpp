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
#include <numeric>
#include <gsl/span>

#include "../common/math.hpp"
#include "Transition_window.hpp"

namespace reBass {
template <typename T, unsigned N>
class Viterbi
{
public:
    Viterbi()
    noexcept {
        delta.fill(INITIAL_DELTA);
    }

    unsigned decode(gsl::span<T const, N> input)
    noexcept {
        window.make_transition(
            std::cbegin(input),
            std::cbegin(delta),
            std::begin(delta),
            std::begin(psi)
        );
        math::normalize<T>(delta);

        return psi[
            Element<T>::max(
                std::cbegin(delta), 
                std::cend(delta)
            ).index
        ];
    }
private:
    static constexpr unsigned INITIAL_DELTA = 1u;

    std::array<T, N> delta;
    std::array<unsigned, N> psi;
    Transition_window<T, N> window;
};
}

