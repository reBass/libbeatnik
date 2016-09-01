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

#include <array>
#include <complex>
#include <gsl/span>

#include "FFT.hpp"

namespace reBass {
template <typename T, int N>
class Real_FFT {
    using real_t = T;
    using cpx_t = std::complex<T>;
    static_assert(N % 4 == 0, "N must be a divisible by 4.");
public:
    Real_FFT()
    noexcept {
        auto const step = real_t{ -2 * pi / N };
        for (auto i = 0u; i < twiddles.size(); ++i) {
            twiddles[i] = std::polar(real_t{1}, (i + N/4) * step);
        }
    }

    void
    transform_forward(
        gsl::span<real_t const, N> input,
        gsl::span<cpx_t, N/2 + 1> output
    ) const noexcept {
        auto cpx_in = gsl::span<cpx_t const, N/2>(
            reinterpret_cast<cpx_t const*>(input.data()), N/2
        );
        auto out = gsl::span<cpx_t, N/2>(output.data(), N/2);

        fft.transform_forward(cpx_in, out);
        real_to_cpx(output, false);
    }

    void
    transform_backward(
        gsl::span<cpx_t, N/2 + 1> input,
        gsl::span<real_t, N> output
    ) const noexcept {
        real_to_cpx(input, true);
        auto const_in = gsl::span<cpx_t const, N/2>(input.data(), N/2);
        auto cpx_out = gsl::span<cpx_t, N/2>(
            reinterpret_cast<cpx_t *>(output.data()), N/2
        );

        fft.transform_backward(const_in, cpx_out);
    }

private:
    void
    real_to_cpx(gsl::span<cpx_t, N/2 + 1> data, bool inverse)
    const noexcept {
        auto const n = N >> 1;

        if (inverse) {
            data[0] = {
                re(data[0]) + re(data[n]),
                re(data[0]) - re(data[n])
            };
        } else {
            data[n] = re(data[0]) - im(data[0]);
            data[0] = re(data[0]) + im(data[0]);
        }

        for (auto i = 1u; 2*i <= n; ++i) {
            auto const w =  data[i] + std::conj(data[n - i]);
            auto const z = (data[i] - std::conj(data[n - i]))
                           * twiddles[inverse ? N/2 - i : i];

            data[i] = (inverse ? 1 : 0.5f) * (w + z);
            data[n - i] = (inverse ? 1 : 0.5f) * (std::conj(w - z));
        }
    }

    static constexpr real_t
    re(cpx_t c) {
        return std::real(c);
    }

    static constexpr real_t
    im(cpx_t c) {
        return std::imag(c);
    }

    static constexpr auto const pi = real_t{3.141592653589793238462643L};

    FFT<real_t, N/2> fft;
    std::array<cpx_t, N/2> twiddles;
};
}
