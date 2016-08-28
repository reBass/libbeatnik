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

#include <gsl/span>

namespace reBass {
    template <typename T, unsigned N, bool Inverse = false>
    class FFT
    {
        using real_t = T;
        using cpx_t = std::complex<T>;

        // only radix-2 and radix-4 butterflies implemented (for sake of simplicity)
        static_assert((N & (N - 1)) == 0, "N must be a power of 2.");
    public:
        static constexpr auto pi = real_t{3.141592653589793238462643L};

        FFT ()
        noexcept {
            auto const step = real_t{ (Inverse ? 2 : -2) * pi / N };
            for (auto i = 0u; i < N; ++i) {
                twiddles[i] = std::polar(real_t{1}, i * step);
            }
            for (auto i = 0u; 2*i < N; ++i) {
                super_twiddles[i] = std::polar(real_t{1}, (i + N/2) * step/2);
            }
        }

        void transform (
                gsl::span<cpx_t const, N> input,
                gsl::span<cpx_t, N> output
        ) const noexcept {
            step_into(
                    gsl::span<cpx_t const>(input.data(), N),
                    gsl::span<cpx_t>(output.data(), N),
                    1
            );
        }

        void transform_real (
                std::conditional_t<
                        Inverse,
                        gsl::span<cpx_t, N + 1>,
                        gsl::span<real_t const, 2*N>
                > input,
                std::conditional_t<
                        Inverse,
                        gsl::span<real_t, 2*N>,
                        gsl::span<cpx_t, N + 1>
                > output
        ) const noexcept {
            real_transform(input, output);
        }

    private:
        void step_into (
                gsl::span<cpx_t const> input,
                gsl::span<cpx_t> output,
                unsigned stride
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
                            input.subspan(i * stride),
                            output.subspan(i * remainder, remainder),
                            stride * radix
                    );
                }
            }

            if (radix == 4) {
                butterfly_radix4(output, stride);
            } else if (radix == 2) {
                butterfly_radix2(output, stride);
            }
        }

        void butterfly_radix2 (gsl::span<cpx_t> output, unsigned stride)
        const noexcept {
            auto const n = output.size() >> 1;

            for (auto i = 0; i < n; ++i) {
                const auto t = output[i + n] * twiddles[i * stride];
                output[i + n] = output[i] - t;
                output[i] += t;
            }
        }

        void butterfly_radix4 (gsl::span<cpx_t> output, unsigned stride)
        const noexcept {
            auto const n = output.size() >> 2;

            std::array<std::complex<T>, 6> scratch;
            auto const negative_if_fwd = cpx_t{0, (Inverse ? 1 : -1)};

            for (auto i = 0; i < n; ++i) {
                scratch[0] = output[i + 1*n] * twiddles[1*i * stride];
                scratch[1] = output[i + 2*n] * twiddles[2*i * stride];
                scratch[2] = output[i + 3*n] * twiddles[3*i * stride];
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

        void real_transform(
                gsl::span<real_t const, 2*N> in,
                gsl::span<cpx_t, N + 1> out
        ) const noexcept {
            auto cpx_in = gsl::span<cpx_t const, N>(
                    reinterpret_cast<cpx_t const*>(in.data()), N
            );
            transform(cpx_in, gsl::span<cpx_t, N>(out.data(), N));
            real_to_cpx(out);
        }

        void real_transform(gsl::span<cpx_t, N + 1> in, gsl::span<real_t, 2*N> out)
        const noexcept {
            real_to_cpx(in);
            auto cpx_out = gsl::span<cpx_t, N>(
                    reinterpret_cast<cpx_t *>(out.data()), N
            );
            auto const_in = gsl::span<cpx_t const, N>(in.data(), N);
            transform(const_in, cpx_out);
        }

        void real_to_cpx(gsl::span<cpx_t, N + 1> data)
        const noexcept {
            if (Inverse) {
                data[0] = {
                        re(data[0]) + re(data[N]),
                        re(data[0]) - re(data[N])
                };
            } else {
                data[N] = re(data[0]) - im(data[0]);
                data[0] = re(data[0]) + im(data[0]);
            }

            for (auto i = 1u; 2*i <= N; ++i) {
                auto w =  data[i] + std::conj(data[N - i]);
                auto z = (data[i] - std::conj(data[N - i])) * super_twiddles[i];

                data[i] = (Inverse ? 1 : 0.5f) * (w + z);
                data[N - i] = (Inverse ? 1 : 0.5f) * (std::conj(w - z));
            }
        }

        static constexpr real_t re(cpx_t c) {
            return std::real(c);
        }
        static constexpr real_t im(cpx_t c) {
            return std::imag(c);
        }

        std::array<cpx_t, N> twiddles;
        std::array<cpx_t, N/2> super_twiddles;
    };
}
