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

#include "../common/Ring_array.hpp"
#include "../odf/Balanced_ACF.hpp"
#include "../odf/RCF_processor.hpp"
#include "../tracker/Tracker.hpp"
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
    unsigned calculate_period (gsl::span<T const, WindowSize> odf)
    noexcept {
        rcf_processor.transform(odf, {rcf_row});
        auto period_offset = viterbi.decode({rcf_row});

        return period_offset + min_period;
    }
private:
    static constexpr unsigned decimated_size = WindowSize / DecimationFactor;
    static constexpr unsigned row_size = decimated_size / 2;
    static constexpr unsigned max_period = decimated_size;
    static constexpr unsigned min_period = max_period - row_size;

    std::array<T, row_size> rcf_row;

    RCF_processor <T, WindowSize, DecimationFactor> rcf_processor;
    Viterbi<T, row_size> viterbi;
};
}
