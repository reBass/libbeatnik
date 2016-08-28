#pragma once

#include <cmath>
#include <cstdlib>

#include <array>
#include <iterator>
#include <utility>

#include "../common/Element.hpp"

namespace reBass {
template <
    typename T,
    unsigned N
>
class Transition_window
{
public:
    Transition_window ()
    noexcept {
        encache();
    }

    template <typename I1, typename I2, typename O1, typename O2>
    void make_transition (I1 input, I2 delta_in, O1 delta_out, O2 indices_out)
    noexcept {
        for (unsigned i = 0; i < 3 * radius; i++) {
            auto m = i % radius;
            if (i >= radius) {
                *(delta_out++) = *(input++) * temp[m];
            }
            if (i < N) {
                auto max_el = max_product(i, delta_in);
                *(indices_out++) = max_el.first; // index
                temp[m] = max_el.second; // value
            }
        }
    }
private:
    template <typename I>
    std::pair<std::size_t, T> max_product (unsigned permutation, I first)
    const noexcept {
        T max_value = *first * at(permutation, 0);
        size_t max_index = 0;
        for (unsigned i = 0; i < N; i++) {
            auto product = *(first + i) * at(i, permutation);
            if (product > max_value) {
                max_index = i;
                max_value = product;
            }
        }
        return std::make_pair(max_index, max_value);
    }

    T at (unsigned i, unsigned j)
    const noexcept {
        auto distance = math::abs_difference(i, j);
        if (distance >= cache.size()) {
            return 0.f;
        }
        return cache[distance];
    }

    void encache ()
    {
        auto sigma_squared = powf(radius / 4.f, 2.f);

        for (auto i = 0u; i < radius; i++) {
            T mu_squared = i * i;
            cache[i] = std::exp(-1 * mu_squared / (2 * sigma_squared));
        }
    }

    static constexpr unsigned radius = N / 2;
    std::array<T, radius> cache;
    std::array<T, radius> temp;
};
}

