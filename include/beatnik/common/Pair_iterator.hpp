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

#include <algorithm>
#include <iterator>
#include <tuple>
#include <cassert>
#include <utility>

namespace reBass {
template <typename I1, typename I2>
class Pair_iterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::pair<
        typename I1::value_type,
        typename I2::value_type
    >;
    using reference = std::pair<
        std::add_lvalue_reference_t<typename I1::value_type>,
        std::add_lvalue_reference_t<typename I2::value_type>
    >;
    using difference_type = typename I1::difference_type;

    constexpr Pair_iterator(std::pair<I1, I2>&& iterator_pair)
    noexcept :
        iterator_pair(iterator_pair)
    {
    }

    constexpr reference operator*()
    const {
        return reference(*iterator_pair.first, *iterator_pair.second);
    }

    constexpr Pair_iterator& operator++()
    noexcept {
        ++iterator_pair.first;
        ++iterator_pair.second;
        return *this;
    }

    constexpr Pair_iterator operator++(int)
    const noexcept {
        auto ret = *this;
        ++iterator_pair.first;
        ++iterator_pair.second;
        return *this;
    }

    constexpr Pair_iterator& operator--()
    noexcept {
        --iterator_pair.first;
        --iterator_pair.second;
        return *this;
    }

    constexpr Pair_iterator operator--(int)
    const noexcept {
        auto ret = *this;
        --iterator_pair.first;
        --iterator_pair.second;
        return *this;
    }

    constexpr Pair_iterator& operator+=(difference_type n)
    noexcept {
        iterator_pair.first += n;
        iterator_pair.second += n;
        return *this;
    }

    constexpr Pair_iterator operator+(difference_type n)
    const noexcept {
        auto ret = *this;
        return ret += n;
    }

    constexpr Pair_iterator& operator-=(difference_type n)
    noexcept {
        iterator_pair.first -= n;
        iterator_pair.second -= n;
        return *this;
    }

    constexpr Pair_iterator operator-(difference_type n)
    const noexcept {
        auto ret = *this;
        return ret -= n;
    }

    constexpr Pair_iterator operator[](difference_type n)
    const noexcept {
        return Pair_iterator(
            std::make_pair(
                iterator_pair.first[n],
                iterator_pair.second[n]
            )
        );
    }

    constexpr friend bool operator==(
        Pair_iterator const& lhs,
        Pair_iterator const& rhs
    ) noexcept {
        return
            lhs.iterator_pair.first == rhs.iterator_pair.first
            && lhs.iterator_pair.second == rhs.iterator_pair.second;
    }

    constexpr friend bool operator!=(
        Pair_iterator const& lhs,
        Pair_iterator const& rhs
    ) noexcept {
        return !(lhs == rhs);
    }

    constexpr friend bool operator<(
        Pair_iterator const& lhs,
        Pair_iterator const& rhs
    ) noexcept {
        assert(
            lhs.iterator_pair.first < rhs.iterator_pair.first
            == lhs.iterator_pair.second < rhs.iterator_pair.second
        );
        return lhs.iterator_pair.first < rhs.iterator_pair.first;
    }

    constexpr friend bool operator<=(
        Pair_iterator const& lhs,
        Pair_iterator const& rhs
    ) noexcept {
        return !(rhs < lhs);
    }

    constexpr friend bool operator>(
        Pair_iterator const& lhs,
        Pair_iterator const& rhs
    ) noexcept {
        return rhs < lhs;
    }

    constexpr friend bool operator>=(
        Pair_iterator const& lhs,
        Pair_iterator const& rhs
    ) noexcept {
        return !(rhs > lhs);
    }

private:
    std::pair<I1, I2> iterator_pair;
};

template <typename I1, typename I2>
constexpr Pair_iterator<I1, I2> operator+(
    typename Pair_iterator<I1, I2>::difference_type n,
    const Pair_iterator<I1, I2> const& rhs
) noexcept {
    return rhs + n;
}

template <typename I1, typename I2>
constexpr Pair_iterator<I1, I2> operator-(
    typename Pair_iterator<I1, I2>::difference_type n,
    const Pair_iterator<I1, I2> const& rhs
) noexcept {
    return rhs - n;
}

template <typename I1, typename I2>
constexpr Pair_iterator<I1, I2> make_pair_iterator(I1&& i1, I2&& i2)
noexcept {
    return Pair_iterator<I1, I2>(std::make_pair(i1, i2));
}
}
