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

#include <array>
#include <complex>

#include <beatnik/common/math.hpp>
#include <beatnik/decoder/Decoder.hpp>
#include <beatnik/fft/FFT.hpp>

#include <benchmark/benchmark.h>
#include <kiss_fft/kiss_fftr.h>
#include <kiss_fft/kiss_fft.h>

using namespace reBass;

static void BM_FFT_float_512real(benchmark::State& state) {
    std::array<float, 512> input;
    input.fill(0);
    input[1] = 1;

    std::array<std::complex<float>, 257> output;
    Real_FFT<float, 512> fft;

    while (state.KeepRunning()) {
        fft.transform_forward({input}, {output});
    }
}
BENCHMARK(BM_FFT_float_512real);

static void BM_FFT_float_512cpx(benchmark::State& state) {
    std::array<std::complex<float>, 512> input;
    input.fill(0);
    input[1] = 1;

    std::array<std::complex<float>, 512> output;
    FFT<float, 512> fft;

    while (state.KeepRunning()) {
        fft.transform_forward(input, output);
        fft.transform_backward(output, input);
        input[1] = 1;
    }
}
BENCHMARK(BM_FFT_float_512cpx);

static void BM_Kiss_FFT_float_512cpx(benchmark::State& state) {
    std::array<std::complex<float>, 512> input;
    input.fill(0);
    input[1] = 1;

    std::array<std::complex<float>, 512> output;
    auto plan = kiss_fft_alloc(512, false, nullptr, nullptr);
    auto inv_plan = kiss_fft_alloc(512, true, nullptr, nullptr);

    while (state.KeepRunning()) {
        kiss_fft(
            plan,
            reinterpret_cast<kiss_fft_cpx const*>(input.data()),
            reinterpret_cast<kiss_fft_cpx*>(output.data())
        );
        kiss_fft(
            inv_plan,
            reinterpret_cast<kiss_fft_cpx const*>(output.data()),
            reinterpret_cast<kiss_fft_cpx*>(input.data())
        );
        input[1] = 1;
    }
}
BENCHMARK(BM_Kiss_FFT_float_512cpx);

static void BM_Adaptive_Threshold(benchmark::State& state) {
    std::array<float, 512> input;

    while(state.KeepRunning()) {
        math::adaptive_threshold<7>(
            gsl::span<float const, 512>(input),
            gsl::span<float, 512>(input)
        );
    }
}
BENCHMARK(BM_Adaptive_Threshold);

static void BM_Threshold_Reference(benchmark::State& state) {
    std::array<float, 512> input;
    std::array<float, 512> thresh;

    while(state.KeepRunning()) {
        math::threshold_reference(
            gsl::span<float const, 512>(input),
            gsl::span<float, 512>(input),
            gsl::span<float, 512>(thresh),
            7
        );
    }
}
BENCHMARK(BM_Threshold_Reference);

/*
static void BM_Decoder(benchmark::State& state) {
    Decoder<float> tracker;
    for (auto i = 0u; i < 512; i++) {
        tracker.process_sample(sin(static_cast<float>(i) / 256 * 3.1415f));
    }
    unsigned period = 0;
    while (state.KeepRunning()) {
        period = tracker.calculate_period();
        tracker.process_sample(static_cast<float>(period) / 256.f);
    }
}
BENCHMARK(BM_Decoder);
 */

static void BM_Mean(benchmark::State& state) {
    std::array<float, 512> input;
    input.fill(1);
    while (state.KeepRunning()) {
        input[0] = math::mean(gsl::span<float const>{input});
    }
}
BENCHMARK(BM_Mean);

static void BM_Normalize(benchmark::State& state) {
    std::array<float, 256> input;
    input.fill(1);
    while (state.KeepRunning()) {
        math::normalize(gsl::span<float const>{input}, gsl::span<float>{input});
    }
}
BENCHMARK(BM_Normalize);

static void BM_Comb_filter(benchmark::State& state) {
    std::array<float, 512> input;
    std::array<float, 128> output;
    input.fill(1);
    while (state.KeepRunning()) {
        math::comb_filter(
            gsl::span<float const, 512>{input},
            gsl::span<float, 128>{output}
        );
    }
}
BENCHMARK(BM_Comb_filter);

BENCHMARK_MAIN();
