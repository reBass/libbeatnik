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

#include <algorithm>
#include <array>
#include <complex>
#include <limits>
#include <gsl/span>

#include "../common/Ring_array.hpp"
#include "../fft/Hann_window.hpp"
#include "../fft/Real_FFT.hpp"

namespace reBass {
template <typename T, int N>
class Onset_detector
{
public:
    static constexpr int fft_window_size = N;
    static constexpr int fft_output_size = N/2 + 1;
    static constexpr int magnitudes_size = N/2;

    /// Appends chunk of incomming audio to an internal buffer and assings it
    /// a scalar value representing the likelihood of an onset
    template <std::ptrdiff_t N_in>
    T
    process(gsl::span<T const, N_in> input)
    noexcept {
        input_buffer.append(input);
        compute_fft();
        compute_magnitudes();
        return estimate_power_rise();
    }

    gsl::span<std::complex<T> const, fft_output_size>
    get_fft_output()
    const noexcept {
        return output;
    }

    gsl::span<T const, magnitudes_size>
    get_magnitudes()
    const noexcept {
        return magnitudes;
    }

private:
    void
    append_short(gsl::span<short const> input)
    noexcept {
        std::transform(
            std::cbegin(input),
            std::cend(input),
            std::back_inserter(input_buffer),
            [] (short sample) {
                return
                    static_cast<T>(sample)
                    / static_cast<T>(std::numeric_limits<short>::max());
            }
        );
    }

    void
    compute_fft()
    noexcept {
        window.cut(input_buffer.cend() - N, windowed_buffer.begin());
        fft.transform_forward(windowed_buffer, output);
    }

    void
    compute_magnitudes()
    noexcept {
        std::transform(
            std::cbegin(output),
            std::cend(output) - 1,
            std::begin(magnitudes),
            [this] (const auto value) {
                return abs(value * norm_factor());
            }
        );
    }

    T
    estimate_power_rise()
    noexcept {
        auto result = 0.000001f;
        for (auto i = 0u; i < magnitudes_size; ++i) {
            if (magnitudes[i] > previous_magnitudes[i] * 2) {
                ++result;
            }
            previous_magnitudes[i] = magnitudes[i];
        }
        return result / magnitudes_size;
    }

    constexpr T
    norm_factor() {
        return T{1} / (N * window.norm_correction());
    }

    const Real_FFT<T, fft_window_size> fft;
    const Hann_window<T, fft_window_size> window;
    std::array<T, fft_window_size> windowed_buffer;
    Ring_array<T, fft_window_size> input_buffer;

    std::array<std::complex<T>, fft_output_size> output;
    std::array<T, magnitudes_size> previous_magnitudes;
    std::array<T, magnitudes_size> magnitudes;
};
}
