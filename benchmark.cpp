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

#include <beatnik/common/Ring_array.hpp>
#include <beatnik/decoder/Decoder.hpp>
#include <beatnik/fft/FFT.hpp>
#include <beatnik/odf/Onset_detector.hpp>

#include <benchmark/benchmark.h>

using namespace reBass;

static void BM_FFT_float_512real(benchmark::State& state) {
    std::array<float, 512> input;
    input.fill(0);
    input[1] = 1;

    std::array<std::complex<float>, 257> output;
    FFT<float, 256> fft;

    while (state.KeepRunning()) {
        fft.transform_real({input}, {output});
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
        fft.transform({input}, {output});
    }
}
BENCHMARK(BM_FFT_float_512cpx);

static void BM_ODF_float_256(benchmark::State& state) {
    std::array<float, 256> input;
    input.fill(1);

    Onset_detector<float, 256> odf;

    while (state.KeepRunning()) {
        input[0] = odf.process({input});
    }
}
BENCHMARK(BM_ODF_float_256);

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
            gsl::span<float const>{input},
            gsl::span<float>{output}
        );
    }
}
BENCHMARK(BM_Comb_filter);

BENCHMARK_MAIN();
