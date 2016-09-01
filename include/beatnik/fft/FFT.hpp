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
#include <cmath>

#include <array>
#include <complex>
#include <utility>
#include <type_traits>

#include <gsl/span>

namespace reBass {
template <typename T, int N>
class FFT
{
    using real_t = T;
    using cpx_t = std::complex<T>;

    // only radix-2 and radix-4 butterflies implemented
    // (for sake of simplicity)
    static_assert((N & (N - 1)) == 0, "N must be a power of 2.");
public:
    static constexpr auto const pi = real_t{3.141592653589793238462643L};

    FFT()
    noexcept {
        auto const step = real_t{ -2 * pi / N };
        for (auto i = 0u; i <= N; ++i) {
            twiddles[i] = std::polar(real_t{1}, i * step);
        }
    }

    void
    transform_forward(
        gsl::span<cpx_t const, N> input,
        gsl::span<cpx_t, N> output
    ) const noexcept {
        step_into<false>(
            gsl::span<cpx_t const>(input.data(), N),
            gsl::span<cpx_t, N>(output.data(), N)
        );
    }

    void
    transform_backward(
        gsl::span<cpx_t const, N> input,
        gsl::span<cpx_t, N> output
    ) const noexcept {
        step_into<true>(
            gsl::span<cpx_t const>(input.data(), N),
            gsl::span<cpx_t, N>(output.data(), N)
        );
    }

private:
    template <
        bool Inverse,
        std::ptrdiff_t N_out,
        std::ptrdiff_t Radix = N_out % 4 == 0 ? 4 : 2,
        std::ptrdiff_t Remainder = N_out / Radix,
        std::ptrdiff_t Stride = (N_out > 0) ? N / N_out : 0
    >
    void
    step_into(
        gsl::span<cpx_t const> input,
        gsl::span<cpx_t, N_out> output
    ) const noexcept {

        if (Remainder == 1) {
            for (auto i = 0; i < N_out; ++i) {
                output[i] = input[i * Stride];
            }
        } else {
            for (auto i = 0u; i < Radix; ++i) {
                step_into<Inverse>(
                    input.subspan(i * Stride, gsl::dynamic_extent),
                    gsl::span<cpx_t, Remainder>(
                        output.data() + i * Remainder,
                        Remainder
                    )
                );
            }
        }

        if (Radix == 4) {
            butterfly_radix4<Inverse>(output);
        } else if (Radix == 2) {
            butterfly_radix2<Inverse>(output);
        }
    }

    template <
        bool Inverse,
        std::ptrdiff_t N_out,
        std::ptrdiff_t N_2 = N_out/2,
        std::ptrdiff_t Stride = (N_out > 0) ? N / N_out : 0
    >
    void
    butterfly_radix2(
        gsl::span<cpx_t, N_out> output
    ) const noexcept {
        //auto const n = Remainder;

        for (auto i = 0; i < N_2; ++i) {
            const auto t = output[i + N_2] * get_twiddle<Inverse, Stride>(i);
            output[i + N_2] = output[i] - t;
            output[i] += t;
        }
    }

    template <
        bool Inverse,
        std::ptrdiff_t N_out,
        std::ptrdiff_t N_4 = N_out/4,
        std::ptrdiff_t Stride = (N_out > 0) ? N / N_out : 0
    >
    void
    butterfly_radix4(
        gsl::span<cpx_t, N_out> output
    ) const noexcept {
        //auto const n = output.size() >> 2;

        std::array<std::complex<T>, 6> scratch;
        auto const negative_if_fwd = cpx_t{0, (Inverse ? 1.f : -1.f)};

        for (auto i = 0; i < N_4; ++i) {
            scratch[0] = output[i + 1*N_4]
                         * get_twiddle<Inverse, Stride>(1*i);
            scratch[1] = output[i + 2*N_4]
                         * get_twiddle<Inverse, Stride>(2*i);
            scratch[2] = output[i + 3*N_4]
                         * get_twiddle<Inverse, Stride>(3*i);
            scratch[5] = output[i      ] - scratch[1];

            output[i] += scratch[1];
            scratch[3] = scratch[0] + scratch[2];
            scratch[4] = scratch[0] - scratch[2];
            scratch[4] = negative_if_fwd * scratch[4];

            output[i + 2*N_4] =  output[i] - scratch[3];
            output[i      ] += scratch[3];
            output[i + 1*N_4] =  scratch[5] + scratch[4];
            output[i + 3*N_4] =  scratch[5] - scratch[4];
        }
    }

    template <bool Inverse, std::ptrdiff_t Stride = 1>
    constexpr cpx_t const&
    get_twiddle(int index)
    const noexcept {
        return twiddles[Inverse ? N - index : index * Stride];
    }

    std::array<cpx_t, N + 1> twiddles;
};
}
