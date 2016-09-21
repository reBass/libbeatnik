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

#include <re/lib/container/ring_array.hpp>
#include <re/lib/fft/acf.hpp>
#include <re/lib/math/adaptive_threshold.hpp>
#include <re/lib/math/comb_filter.hpp>
#include <re/lib/math/normalize.hpp>

#include <re/beatnik/decoder/viterbi.hpp>

namespace re {
namespace beatnik {

template <typename T, int WindowSize, int DecimationFactor = 4>
class decoder
{
public:
    static constexpr int window_size = WindowSize;

    int
    calculate_period(gsl::span<T const, window_size> input)
    noexcept
    {
        std::array<T, window_size> odf_frame;

        // subtract the baseline
        math::adaptive_threshold<threshold_range>(
            input,
            gsl::span<T, window_size>(odf_frame)
        );
        // calculate autocorrelation function -
        // - find the likelihood of periodicity
        acf(odf_frame);

        std::array<T, combed_size> combed_frame;
        // "echo" the acf at 1/4, 2/4 and 3/4 of the frame size
        // - effective downsampling retaining the resolution
        math::comb_filter(
            gsl::span<T const, window_size>(odf_frame),
            gsl::span<T, combed_size>(combed_frame)
        );
        // look for peaks
        math::adaptive_threshold<threshold_range>(
            gsl::span<T, combed_size>(combed_frame)
        );

        // a periodicity at T implies a periodicity at 2*T,
        // therefore the lower half of the output is redundant
        auto const viterbi_input = gsl::span<T, viterbi_size>(
            combed_frame.data() + viterbi_offset,
            viterbi_size
        );
        // make a transition from a previous frame to the current one
        // and find the maximum
        auto const period_offset = viterbi.decode(viterbi_input);

        return period_offset + min_period;
    }

private:
    static constexpr int combed_size = WindowSize / DecimationFactor;
    static constexpr int viterbi_size = combed_size / 2;
    static constexpr int viterbi_offset = combed_size - viterbi_size;
    static constexpr int max_period = combed_size;
    static constexpr int min_period = max_period - viterbi_size;
    static constexpr int threshold_range = 7;

    fft::acf<T, window_size> acf;
    viterbi<T, viterbi_size> viterbi;
};

}
}
