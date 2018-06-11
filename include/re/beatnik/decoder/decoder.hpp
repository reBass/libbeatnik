#pragma once

#include <cassert>

#include <array>
#include <functional>
#include <gsl/span>

#include <re/lib/container/ring_array.hpp>
#include <re/lib/fft/acf.hpp>
#include <re/lib/math/adaptive_threshold.hpp>
#include <re/lib/math/comb_filter.hpp>
#include <re/lib/math/normalize.hpp>

#include <re/beatnik/decoder/viterbi.hpp>

namespace re {
namespace beatnik {

template <typename T, int_t WindowSize, int_t DecimationFactor = 4>
class decoder
{
public:
    static constexpr int_t window_size = WindowSize;

    int_t
    calculate_period(gsl::span<T const, window_size> input) noexcept
    {
        std::array<T, window_size> odf_frame;

        // subtract the baseline
        math::adaptive_threshold<threshold_range>(
            input,
            gsl::span<T, window_size>(odf_frame)
        );
        // calculate autocorrelation function -
        // - find the likelihood of periodicity
        acf(odf_frame);

        std::array<T, combed_size> combed_frame;
        // "echo" the acf at 1/4, 2/4 and 3/4 of the frame size
        // - effective downsampling retaining the resolution
        math::comb_filter(
            gsl::span<T const, window_size>(odf_frame),
            gsl::span<T, combed_size>(combed_frame)
        );
        // look for peaks
        math::adaptive_threshold<threshold_range>(
            gsl::span<T, combed_size>(combed_frame)
        );

        // a periodicity at T implies a periodicity at 2*T,
        // therefore the lower half of the output is redundant
        auto const viterbi_input = gsl::span<T, viterbi_size>(
            combed_frame.data() + viterbi_offset,
            viterbi_size
        );
        // make a transition from a previous frame to the current one
        // and find the maximum
        auto const period_offset = viterbi_.decode(viterbi_input);

        return period_offset + min_period;
    }

private:
    static constexpr int_t combed_size = WindowSize / DecimationFactor;
    static constexpr int_t viterbi_size = combed_size / 2;
    static constexpr int_t viterbi_offset = combed_size - viterbi_size;
    static constexpr int_t max_period = combed_size;
    static constexpr int_t min_period = max_period - viterbi_size;
    static constexpr int_t threshold_range = 7;

    fft::acf<T, window_size> acf;
    viterbi<T, viterbi_size> viterbi_;
};

}
}
