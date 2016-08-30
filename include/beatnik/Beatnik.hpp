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

#include "odf/Onset_detector.hpp"
#include "decoder/Decoder.hpp"
#include "tracker/Tracker.hpp"

namespace reBass {
template <
    typename T = float,
    unsigned FFTSize = 1024,
    unsigned FFTStep = 128,
    unsigned ODFSize = 2048,
    unsigned ODFStep = 128
>
class Beatnik {
public:
    using float_t = T;
    static constexpr unsigned fft_window_size = FFTSize;
    static constexpr unsigned fft_magnitudes_size = fft_window_size / 2;
    static constexpr unsigned fft_step = FFTStep;
    static constexpr unsigned odf_size = ODFSize;
    static constexpr unsigned odf_step = ODFStep;
    static constexpr unsigned decimate_by = 4;
    static constexpr unsigned max_period = odf_size / decimate_by;
    static constexpr unsigned max_beats = 16 * 512 / ODFSize;
    static constexpr float_t min_tempo = 90.f;
    static constexpr float_t max_tempo = 2*min_tempo;

    Beatnik(float_t sample_rate)
    noexcept:
        frames_per_minute(60.f * sample_rate / fft_step)
    {
    }

    bool process(gsl::span<float_t const, fft_step> audio)
    noexcept {
        float_t sample = onset_detector.process(audio.subspan(0u));
        odf_buffer.push_back(sample);
        tracker.update_score(sample);

        if (++counter >= odf_step) {
            counter = 0;
            auto guess = decoder.calculate_period({odf_buffer.linearize()});
            tracker.set_period_guess(guess);
        }

        return tracker.new_estimate_expected();
    }

    float_t estimate_tempo()
    noexcept {
        float_t period = tracker.estimate_period();

        auto bpm = frames_per_minute / period;
        while (bpm > max_tempo) {
            bpm /= 2;
        } while (bpm < min_tempo) {
            bpm *= 2;
        }

        return bpm;
    }

    gsl::span<float_t const, odf_size> get_odf_buffer()
    noexcept {
        return odf_buffer.linearize();
    }

    gsl::span<float_t const, fft_magnitudes_size> get_fft_magnitudes()
    const noexcept {
        return onset_detector.get_magnitudes();
    }

    void clear()
    noexcept {
        counter = 0;
        std::fill(
                std::begin(odf_buffer),
                std::end(odf_buffer),
                static_cast<T>(0.1)
        );
        tracker.clear();
    }

private:
    Onset_detector<float_t, fft_window_size> onset_detector;
    Decoder<float_t, odf_size, decimate_by> decoder;
    Tracker<float_t, max_period, max_beats> tracker;
    Ring_array<float_t, odf_size> odf_buffer;
    unsigned counter = 0;
    float_t const frames_per_minute;
};
}
