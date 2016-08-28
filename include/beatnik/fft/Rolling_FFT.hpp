#pragma once

#include <cassert>

#include <algorithm>
#include <array>
#include <complex>
#include <limits>
#include <gsl/span>

#include "../common/Ring_array.hpp"
#include "Hann_window.hpp"
#include "FFT.hpp"

namespace reBass {
template <typename T, unsigned N>
class Rolling_FFT final
{
public:
    static constexpr unsigned window_size = 2*N;

    void append_short(gsl::span<short const> input)
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

        compute_fft();
        compute_magnitudes();
    }

    void append(gsl::span<T const> input)
    noexcept {
        input_buffer.insert_at_end(
            std::cbegin(input),
            std::cend(input)
        );

        compute_fft();
        compute_magnitudes();
    };

    gsl::span<std::complex<T> const, N> get_fft_output()
    const noexcept {
        return {output.data(), N};
    }

    gsl::span<T const, N> get_magnitudes()
    const noexcept {
        return {magnitudes};
    }

private:
    constexpr T norm_factor() {
        return T{1} / (N * window.norm_correction());
    };

    void compute_fft()
    noexcept {
        window.cut(input_buffer.cend() - window_size, windowed_buffer.begin());
        fft.transform_real({windowed_buffer}, {output});
    };

    void compute_magnitudes()
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

    FFT<T, N, false> fft;
    const Hann_window<T, window_size> window;
    std::array<T, window_size> windowed_buffer;

    Ring_array<T, window_size> input_buffer;
    std::array<std::complex<T>, N + 1> output;
    std::array<T, N> magnitudes;
};

}
