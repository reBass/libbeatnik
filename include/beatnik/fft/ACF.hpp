#pragma once

#include <algorithm>
#include <array>
#include <complex>
#include <gsl/span>

#include "Real_FFT.hpp"

namespace reBass {
template <typename T, int N>
/// Computes autocorrelation function of a given input
class ACF
{
public:
    void
    compute(gsl::span<T, N> data)
    noexcept {
        compute(data, data);
    }

    void
    compute(gsl::span<T const, N> input, gsl::span<T, N> output)
    noexcept {
        std::copy(
            std::cbegin(input),
            std::cend(input),
            std::begin(time_domain)
        );
        std::fill(
            std::end(time_domain) - N,
            std::end(time_domain),
            0
        );

        fft.transform_forward(time_domain, frequency_domain);

        std::transform(
            std::cbegin(frequency_domain),
            std::cend(frequency_domain),
            std::begin(frequency_domain),
            [] (auto const& value) {
                return std::norm(value);
            }
        );

        fft.transform_backward(frequency_domain, time_domain);

        auto lag = N;
        std::transform(
            std::cbegin(time_domain),
            std::cbegin(time_domain) + N,
            std::begin(output),
            [this, &lag] (auto value) {
                return std::abs(value) / (N * lag--);
            }
        );
    }

private:
    Real_FFT<T, 2*N> const fft;
    std::array<std::complex<T>, N + 1> frequency_domain;
    std::array<T, 2*N> time_domain;
};
}
