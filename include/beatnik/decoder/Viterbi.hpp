#pragma once

#include <cmath>

#include <algorithm>
#include <array>
#include <iterator>
#include <numeric>
#include <gsl/span>

#include "../common/math.hpp"
#include "Transition_window.hpp"

namespace reBass {
template <typename T, unsigned N>
class Viterbi
{
public:
    Viterbi()
    noexcept {
        delta.fill(INITIAL_DELTA);
    }

    unsigned decode(gsl::span<T const, N> input)
    noexcept {
        window.make_transition(
            std::cbegin(input),
            std::cbegin(delta),
            std::begin(delta),
            std::begin(psi)
        );
        math::normalize(
            gsl::span<T const>{delta},
            gsl::span<T>{delta}
        );

        return psi[
            Element<T>::max(
                std::cbegin(delta), 
                std::cend(delta)
            ).index
        ];
    }
private:
    static constexpr unsigned INITIAL_DELTA = 1u;

    std::array<T, N> delta;
    std::array<unsigned, N> psi;
    Transition_window<T, N> window;
};
}


