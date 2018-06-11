// Copyright (c) 2016-2018 Roman Beránek. All rights reserved.
//
// Released under MIT license. See LICENSE file.

#include <array>
#include <complex>

#include <re/beatnik/decoder/decoder.hpp>

#include <benchmark/benchmark.h>

static void BM_Adaptive_Threshold(benchmark::State& state) {
    std::array<float, 512> input;

    while(state.KeepRunning()) {
        re::math::adaptive_threshold<7>(
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
        re::math::threshold_reference(
            gsl::span<float const, 512>(input),
            gsl::span<float, 512>(input),
            gsl::span<float, 512>(thresh),
            7
        );
    }
}
BENCHMARK(BM_Threshold_Reference);


static void BM_Mean(benchmark::State& state) {
    std::array<float, 512> input;
    input.fill(1);
    while (state.KeepRunning()) {
        input[0] = re::math::mean(gsl::span<float const>{input});
    }
}
BENCHMARK(BM_Mean);

static void BM_Normalize(benchmark::State& state) {
    std::array<float, 256> input;
    input.fill(1);
    while (state.KeepRunning()) {
        re::math::normalize(gsl::span<float const>{input}, gsl::span<float>{input});
    }
}
BENCHMARK(BM_Normalize);

static void BM_Comb_filter(benchmark::State& state) {
    std::array<float, 512> input;
    std::array<float, 128> output;
    input.fill(1);
    while (state.KeepRunning()) {
        re::math::comb_filter(
            gsl::span<float const, 512>{input},
            gsl::span<float, 128>{output}
        );
    }
}
BENCHMARK(BM_Comb_filter);

BENCHMARK_MAIN();
