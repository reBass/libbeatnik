#pragma once

#include <cassert>

#include <algorithm>
#include <array>
#include <complex>
#include <limits>
#include <type_traits>
#include <gsl/span>

#include <re/lib/common.hpp>
#include <re/lib/container/ring_array.hpp>
#include <re/lib/fft/real_fft.hpp>
#include <re/lib/fft/hann_window.hpp>

namespace re {
namespace beatnik {

template <typename T, int_t N>
class onset_detector
{
    static_assert(std::is_floating_point<T>::value);
public:
    static constexpr int_t window_size = N;
    static constexpr int_t fft_output_size = N / 2 + 1;
    static constexpr int_t magnitudes_size = N / 2;

    /// Appends chunk of incomming audio to an internal buffer and assings it
    /// a scalar value representing the likelihood of an onset
    template <int_t N_in>
    T
    process(gsl::span<T const, N_in> input) noexcept
    {
        input_buffer.append(input);
        compute_fft();
        compute_magnitudes();
        return estimate_power_rise();
    }

    gsl::span<std::complex<T> const, fft_output_size>
    get_fft_output() const noexcept
    {
        return output;
    }

    gsl::span<T const, magnitudes_size>
    get_magnitudes() const noexcept
    {
        return magnitudes;
    }

private:
    void
    compute_fft() noexcept
    {
        std::array<T, window_size> windowed_buffer;
        window.cut(input_buffer.linearize().cbegin(), windowed_buffer.begin());
        fft(windowed_buffer, output);
    }

    void
    compute_magnitudes() noexcept
    {
        std::transform(
            std::cbegin(output),
            std::cend(output) - 1,
            std::begin(magnitudes),
            [](auto const value) {
                return abs(value * norm_factor());
            }
        );
    }

    T
    estimate_power_rise() noexcept
    {
        auto result = 0.000001f; // avoid zero values
        for (auto i = 0; i < magnitudes_size; ++i) {
            if (magnitudes[i] > previous_magnitudes[i] * 2) {
                ++result;
            }
            previous_magnitudes[i] = magnitudes[i];
        }
        return result / magnitudes_size;
    }

    static constexpr T
    norm_factor()
    {
        return 1.f
               / (N * fft::hann_window<T, window_size>::norm_correction());
    }

    fft::real_fft<T, window_size, fft::direction::forward> const fft;
    fft::hann_window <T, window_size> const window;
    ring_array<T, window_size> input_buffer;

    std::array<std::complex<T>, fft_output_size> output;
    std::array<T, magnitudes_size> previous_magnitudes;
    std::array<T, magnitudes_size> magnitudes;
};

}
}
