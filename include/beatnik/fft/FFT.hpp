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
            step_into(
                gsl::span<cpx_t const>(input.data(), N),
                gsl::span<cpx_t>(output.data(), N),
                false,
                1
            );
        }

        void
        transform_backward(
            gsl::span<cpx_t const, N> input,
            gsl::span<cpx_t, N> output
        ) const noexcept {
            step_into(
                gsl::span<cpx_t const>(input.data(), N),
                gsl::span<cpx_t>(output.data(), N),
                true,
                1
            );
        }

    private:
        void
        step_into (
            gsl::span<cpx_t const> input,
            gsl::span<cpx_t> output,
            bool inverse,
            int stride
        ) const noexcept {
            auto const output_size = output.size();
            auto const radix = (output_size % 4 == 0) ? 4u : 2u;
            auto const remainder = output_size / radix;

            if (remainder == 1) {
                for (auto i = 0; i < output_size; ++i) {
                    output[i] = input[i * stride];
                }
            } else {
                for (auto i = 0u; i < radix; ++i) {
                    step_into(
                            input.subspan(i * stride, gsl::dynamic_extent),
                            output.subspan(i * remainder, remainder),
                            inverse,
                            stride * radix
                    );
                }
            }

            if (radix == 4) {
                butterfly_radix4(output, inverse, stride);
            } else if (radix == 2) {
                butterfly_radix2(output, inverse, stride);
            }
        }

        void
        butterfly_radix2(gsl::span<cpx_t> output, bool inverse, int stride)
        const noexcept {
            auto const n = output.size() >> 1;

            for (auto i = 0; i < n; ++i) {
                const auto t = output[i + n] * get_twiddle(i * stride, inverse);
                output[i + n] = output[i] - t;
                output[i] += t;
            }
        }

        void
        butterfly_radix4(gsl::span<cpx_t> output, bool inverse, int stride)
        const noexcept {
            auto const n = output.size() >> 2;

            std::array<std::complex<T>, 6> scratch;
            auto const negative_if_fwd = cpx_t{0, (inverse ? 1.f : -1.f)};

            for (auto i = 0; i < n; ++i) {
                scratch[0] = output[i + 1*n]
                             * get_twiddle(1*i * stride, inverse);
                scratch[1] = output[i + 2*n]
                             * get_twiddle(2*i * stride, inverse);
                scratch[2] = output[i + 3*n]
                             * get_twiddle(3*i * stride, inverse);
                scratch[5] = output[i      ] - scratch[1];

                output[i] += scratch[1];
                scratch[3] = scratch[0] + scratch[2];
                scratch[4] = scratch[0] - scratch[2];
                scratch[4] = negative_if_fwd * scratch[4];

                output[i + 2*n] =  output[i] - scratch[3];
                output[i      ] += scratch[3];
                output[i + 1*n] =  scratch[5] + scratch[4];
                output[i + 3*n] =  scratch[5] - scratch[4];
            }
        }

        constexpr cpx_t const&
        get_twiddle(int index, bool inverse)
        const noexcept {
            return inverse ? twiddles[N - index] : twiddles[index];
        }

        std::array<cpx_t, N + 1> twiddles;
    };
}
