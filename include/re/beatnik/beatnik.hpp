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

#include <gsl/span>

#include <re/lib/container/ring_array.hpp>

#include <re/beatnik/odf/onset_detector.hpp>
#include <re/beatnik/decoder/decoder.hpp>
#include <re/beatnik/tracker/tracker.hpp>

namespace re {
namespace beatnik {

template <
    typename T = float,
    int_t FFTSize = 1024,
    int_t FFTStep = 128,
    int_t ODFSize = 2048,
    int_t ODFStep = 128
>
class beatnik
{
public:
    using float_t = T;
    static constexpr int_t fft_window_size = FFTSize;
    static constexpr int_t fft_magnitudes_size = fft_window_size / 2;
    static constexpr int_t fft_step = FFTStep;
    static constexpr int_t odf_size = ODFSize;
    static constexpr int_t odf_step = ODFStep;
    static constexpr int_t decimate_by = 4;
    static constexpr int_t max_period = odf_size / decimate_by;
    static constexpr int_t min_period = max_period / 2;
    static constexpr int_t max_beats = 16 * 512 / ODFSize;
    static constexpr float_t min_tempo = 90.f;
    static constexpr float_t max_tempo = 2 * min_tempo;

    beatnik(float_t sample_rate) noexcept:
        frames_per_minute(60.f * sample_rate / fft_step)
    {
    }

    bool
    process(gsl::span<float_t const, fft_step> audio) noexcept
    {
        float_t sample = onset_detector_.process(audio);
        odf_buffer.push_back(sample);
        tracker_.update_score(sample);

        if (++counter >= odf_step) {
            counter = 0;
            tracker_.set_period_guess(
                decoder_.calculate_period(odf_buffer.linearize())
            );
        }

        return tracker_.new_estimate_expected();
    }

    float_t
    estimate_tempo() noexcept
    {
        float_t period = tracker_.estimate_period();

        auto bpm = frames_per_minute / period;
        while (bpm > max_tempo) {
            bpm /= 2;
        }
        while (bpm < min_tempo) {
            bpm *= 2;
        }

        return bpm;
    }

    gsl::span<float_t const, odf_size>
    get_odf_buffer() noexcept
    {
        return odf_buffer.linearize();
    }

    gsl::span<float_t const, fft_magnitudes_size>
    get_fft_magnitudes() const noexcept
    {
        return onset_detector_.get_magnitudes();
    }

    void
    clear() noexcept
    {
        counter = 0;
        std::fill(
            std::begin(odf_buffer),
            std::end(odf_buffer),
            static_cast<T>(0.1)
        );
        tracker_.clear();
    }

private:
    onset_detector<float_t, fft_window_size> onset_detector_;
    decoder<float_t, odf_size, decimate_by> decoder_;
    tracker<float_t, min_period, max_beats> tracker_;

    ring_array<float_t, odf_size> odf_buffer;

    float_t const frames_per_minute;
    int_t counter = 0;
};

}
}
