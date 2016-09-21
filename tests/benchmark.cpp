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

#include <gsl/span>

#include <re/lib/math/adaptive_threshold.hpp>
#include <re/beatnik/decoder/decoder.hpp>

#include <benchmark/benchmark.h>


using namespace re;
using namespace re::beatnik;
using namespace re::math;

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
