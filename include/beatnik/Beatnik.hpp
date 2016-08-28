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

#include "fft/Rolling_FFT.hpp"
#include "odf/Onset_detector.hpp"
#include "decoder/Decoder.hpp"
#include "tracker/Tracker.hpp"

namespace reBass {
template <
    typename T = float,
    unsigned FFTSize = 512,
    unsigned FFTStep = 128,
    unsigned ODFSize = 2048,
    unsigned ODFStep = 128
>
class Beatnik {
public:
    using float_t = T;
    static constexpr unsigned fft_size = FFTSize;
    static constexpr unsigned fft_step = FFTStep;
    static constexpr unsigned odf_size = ODFSize;
    static constexpr unsigned odf_step = ODFStep;
    static constexpr unsigned decimate_by = 4;
    static constexpr unsigned max_period = odf_size / decimate_by;
    static constexpr unsigned max_beats = 16 * 512 / ODFSize;
    static constexpr float_t min_tempo = 90.f;
    static constexpr float_t max_tempo = 2*min_tempo;

    Beatnik(float_t sample_rate) noexcept:
        frames_per_minute(60.f * sample_rate / FFTStep)
    {
    }

    bool process (gsl::span<short const, fft_step> audio)
    {
        fft.append_short(audio.subspan(0u));
        auto magnitudes = fft.get_magnitudes();
        auto sample = onset_detector.process(magnitudes);

        odf_buffer.push_back(sample);
        tracker.update_score(sample);

        if (++counter < odf_step) {
            return false;
        }
        counter = 0;

        auto guess = decoder.calculate_period({odf_buffer.linearize()});
        tracker.set_period_guess(guess);

        if (!tracker.new_estimate_expected()) {
            return false;
        }
        current_period = tracker.estimate_period();

        return true;
    }

    float_t get_current_tempo()
    const noexcept {
        auto bpm = frames_per_minute / current_period;
        while (bpm > max_tempo) {
            bpm /= 2;
        } while (bpm < min_tempo) {
            bpm *= 2;
        }

        return bpm;
    }

    gsl::span<float_t const, odf_size> get_odf_buffer()
    noexcept {
        return {odf_buffer.linearize()};
    }

    gsl::span<float_t const, fft_size> get_magnitudes()
    const noexcept {
        return fft.get_magnitudes();
    }

    void clear()
    noexcept {
        counter = 0;
        current_period = 0;
        std::fill(
                std::begin(odf_buffer),
                std::end(odf_buffer),
                static_cast<T>(0.1)
        );
        tracker.clear();
    }

private:
    Rolling_FFT<float_t, FFTSize> fft;
    Onset_detector<float_t, FFTSize> onset_detector;
    Ring_array<float_t, ODFSize> odf_buffer;
    Decoder<float_t, ODFSize, decimate_by> decoder;
    Tracker<float_t, max_period, max_beats> tracker;

    unsigned counter = 0;
    float_t const frames_per_minute;
    float_t current_period = 0;
};
}
