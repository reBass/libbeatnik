#pragma once

#include <algorithm>
#include <array>
#include <complex>
#include <gsl/span>

#include "../fft/FFT.hpp"

namespace reBass {
template <typename T, unsigned N>
class Balanced_ACF final {
public:
    void perform_ACF(gsl::span<T const, N> input, gsl::span<T, N> output)
    noexcept {
        std::fill(
            std::end(time_domain) - N,
            std::end(time_domain),
            0
        );
        std::copy(
            std::cbegin(input),
            std::cend(input),
            std::begin(time_domain)
        );

        forward_fft.transform_real({time_domain}, {frequency_domain});

        std::transform(
            std::cbegin(frequency_domain),
            std::cend(frequency_domain),
            std::begin(frequency_domain),
            [] (const auto& value) {
                return std::norm(value);
            }
        );

        inverse_fft.transform_real({frequency_domain}, {time_domain});

        auto lag = N;
        std::transform(
            std::cbegin(time_domain),
            std::cbegin(time_domain) + N,
            std::begin(output),
            [this, &lag] (auto value) {
                return std::abs(value) / (N * lag--);
            }
        );


    };

private:
    FFT<T, N, false> forward_fft;
    FFT<T, N, true> inverse_fft;
    std::array<std::complex<T>, N + 1> frequency_domain;
    std::array<T, 2*N> time_domain;
};
}
