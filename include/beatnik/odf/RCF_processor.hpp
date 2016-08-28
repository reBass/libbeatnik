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
#include <iterator>

#include "../common/math.hpp"
#include "Balanced_ACF.hpp"

namespace reBass {
template <typename T, unsigned N, unsigned Decimation_factor = 4>
class RCF_processor
{
    static constexpr unsigned output_size = N / 2 / Decimation_factor;
public:
    void
    transform (gsl::span<T const, N> input, gsl::span<T, output_size> output)
    noexcept {
        math::adaptive_threshold2(
            input,
            gsl::span<T, N>{odf_frame},
            threshold_range
        );
        acf.perform_ACF({odf_frame}, {odf_frame});
        math::comb_filter(
            gsl::span<T const>{odf_frame},
            gsl::span<T>{comb_filter_output}
        );
        math::adaptive_threshold2(
            gsl::span<T const, N / Decimation_factor>{comb_filter_output},
            gsl::span<T, N / Decimation_factor>{comb_filter_output},
            threshold_range
        );

        // a periodicity at T implies a periodicity at 2*T,
        // therefore the lower half of the output is redundant
        auto from = std::cend(comb_filter_output) - output_size;
        auto to = std::cend(comb_filter_output);

        std::copy(from, to, std::begin(output));
    };
private:
    static constexpr unsigned threshold_range = 8;

    static_assert(std::is_floating_point<T>::value);
    static_assert((N / (Decimation_factor * 2) == output_size));

    std::array<T, N> odf_frame;
    std::array<T, N / Decimation_factor> comb_filter_output;
    Balanced_ACF<T, N> acf;
};
}
