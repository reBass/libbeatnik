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

#include <array>
#include <functional>
#include <gsl/span>

#include "../common/math.hpp"
#include "../common/Ring_array.hpp"
#include "../fft/ACF.hpp"
#include "Viterbi.hpp"

namespace reBass {
template <
    typename T = float,
    unsigned WindowSize = 512,
    unsigned DecimationFactor = 4
>
class Decoder
{
public:
    static constexpr unsigned window_size = WindowSize;

    unsigned calculate_period(gsl::span<T const, window_size> input)
    noexcept {
        math::adaptive_threshold<T, window_size>(
            input,
            odf_frame,
            threshold_range
        );
        acf.compute(odf_frame, odf_frame);
        math::comb_filter<T>(odf_frame, combed_frame);
        math::adaptive_threshold<T, combed_size>(combed_frame, threshold_range);

        // a periodicity at T implies a periodicity at 2*T,
        // therefore the lower half of the output is redundant
        auto const viterbi_input = gsl::span<T, viterbi_size>(
            combed_frame.data() + viterbi_offset,
            viterbi_size
        );
        auto const period_offset = viterbi.decode(viterbi_input);

        return period_offset + min_period;
    }
private:
    static constexpr unsigned combed_size = WindowSize / DecimationFactor;
    static constexpr unsigned viterbi_size = combed_size / 2;
    static constexpr unsigned viterbi_offset = combed_size - viterbi_size;
    static constexpr unsigned max_period = combed_size;
    static constexpr unsigned min_period = max_period - viterbi_size;
    static constexpr unsigned threshold_range = 8;

    std::array<T, window_size> odf_frame;
    std::array<T, combed_size> combed_frame;

    ACF<T, window_size> acf;
    Viterbi<T, viterbi_size> viterbi;
};
}
