#pragma once

#include <cmath>

#include <algorithm>
#include <array>
#include <iterator>

#include "../common/math.hpp"

namespace reBass
{
template<typename T, std::size_t N>
class Hann_window {
public:
    Hann_window() {
        encache();
    };

    template <class InputIt, class OutputIt>
    void cut(InputIt in, OutputIt out)
    const noexcept {
        std::transform(
            std::cbegin(cache),
            std::cend(cache),
            in,
            out,
            std::multiplies<>()
        );
    }

    static constexpr T norm_correction() {
        return static_cast<T>(0.5);
    }
private:
    std::array<T, N> cache;

    void encache()
    noexcept {
        for (auto i = 0u; i < cache.size(); ++i) {
            cache[i] = window_function(i, cache.size());
        }
    }

    static constexpr T window_function(
        std::size_t position,
        std::size_t window_size
    ) noexcept {
        auto relative_position = static_cast<T>(position) / window_size;
        return (1 - std::cos(relative_position * 2 * math::PI<T>)) / 2;
    }
};

}
