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

#include <cassert>
#include <iterator>

namespace reBass {
template <typename Container, bool IsConst>
class Index_iterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::conditional_t<
        IsConst,
        std::add_const_t<typename Container::value_type>,
        typename Container::value_type
    >;
    using difference_type = typename Container::difference_type;
    using pointer = std::add_pointer_t<value_type>;
    using reference = std::add_lvalue_reference_t<value_type>;
    using container_t = std::conditional_t<
        IsConst,
        std::add_const_t<Container>,
        Container
    >;

    constexpr Index_iterator(
        container_t* c,
        difference_type index
    ) :
        container(c),
        index(index)
    {
        assert(index >= 0);
    }

    friend class Index_iterator<Container, true>;
    constexpr Index_iterator(Index_iterator<Container, false> const& other)
    noexcept :
        Index_iterator(other.container, other.index)
    {
    }

    constexpr reference operator*()
    const {
        return (*container)[index];
    }

    constexpr pointer operator->()
    const {
        return &((*container)[index]);
    }

    constexpr Index_iterator& operator++()
    noexcept {
        ++index;
        return *this;
    }

    constexpr Index_iterator operator++(int)
    noexcept {
        auto ret = *this;
        ++index;
        return ret;
    }

    constexpr Index_iterator& operator--()
    noexcept {
        --index;
        return *this;
    }

    constexpr Index_iterator operator--(int)
    noexcept {
        auto ret = *this;
        --index;
        return ret;
    }

    constexpr Index_iterator operator+(difference_type n)
    const noexcept {
        auto ret = *this;
        return ret += n;
    }

    constexpr Index_iterator& operator+=(difference_type n)
    noexcept {
        index += n;
        return *this;
    }

    constexpr Index_iterator operator-(difference_type n)
    const noexcept {
        auto ret = *this;
        return ret -= n;
    }

    constexpr Index_iterator& operator-=(difference_type n)
    noexcept {
        index += -n;
        return *this;
    }

    constexpr difference_type operator-(const Index_iterator& rhs)
    const noexcept {
        assert(container == rhs.container);
        return index - rhs.index;
    }

    constexpr Index_iterator operator[](difference_type n)
    const noexcept {
        return Index_iterator(container, index + n);
    }

    constexpr friend bool operator==(
        Index_iterator const& lhs,
        Index_iterator const& rhs
    ) noexcept {
        return lhs.container == rhs.container && lhs.index == rhs.index;
    }

    constexpr friend bool operator!=(
        Index_iterator const& lhs,
        Index_iterator const& rhs
    ) noexcept {
        return !(lhs == rhs);
    }

    constexpr friend bool operator<(
        Index_iterator const& lhs,
        Index_iterator const& rhs
    ) noexcept {
        assert(lhs.container == rhs.container);
        return lhs.index < rhs.index;
    }

    constexpr friend bool operator<=(
        Index_iterator const& lhs,
        Index_iterator const& rhs
    ) noexcept {
        return !(rhs < lhs);
    }

    constexpr friend bool operator>(
        Index_iterator const& lhs,
        Index_iterator const& rhs
    ) noexcept {
        return rhs < lhs;
    }

    constexpr friend bool operator>=(
        Index_iterator const& lhs,
        Index_iterator const& rhs
    ) noexcept {
        return !(rhs > lhs);
    }

protected:
    container_t* container;
    difference_type index;
};

template <typename Container, bool IsConst>
constexpr Index_iterator<Container, IsConst> operator+(
    typename Index_iterator<Container, IsConst>::difference_type n,
    Index_iterator<Container, IsConst> const& rhs
) noexcept {
    return rhs + n;
}

template <typename Container, bool IsConst>
constexpr Index_iterator<Container, IsConst> operator-(
    typename Index_iterator<Container, IsConst>::difference_type n,
    const Index_iterator<Container, IsConst>& rhs
) noexcept {
    return rhs - n;
}
}

/*
template <bool IsConst = false>
class iterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::conditional_t<
        IsConst,
        std::add_const_t<value_type>,
        value_type
    >;
    using pointer = std::add_pointer_t<value_type>;
    using reference = std::add_lvalue_reference_t<value_type>;

    constexpr iterator(
        std::conditional_t<
            IsConst,
            Ring_array const&,
            Ring_array&
        > ring,
        size_type position
    ) noexcept :
        ring(ring),
        position(position) {
    }

    constexpr iterator(
        iterator const& it,
        difference_type diff
    ) noexcept :
        ring(it.ring),
        position(it.position + diff) {
    }

    TODO
    iterator<true>(iterator<false> const& non_const_iterator) {
        return iterator<true>(
            non_const_iterator.ring,
            non_const_iterator.position
        );
    }


    constexpr iterator& operator++() noexcept {
        ++position;
        return *this;
    }
    constexpr iterator& operator++(int) noexcept {
        ++position;
        return *this;
    }
    constexpr iterator& operator--() noexcept {
        --position;
        return *this;
    }
    constexpr iterator& operator--(int) noexcept {
        --position;
        return *this;
    }
    constexpr iterator operator+(iterator const& rhs) const noexcept {
        assert(&ring == &rhs.ring);
        return iterator(ring, position + rhs.position);
    }
    constexpr iterator operator-(iterator const& rhs) const noexcept {
        assert(&ring == &rhs.ring);
        return iterator(ring, position - rhs.position);
    }
    constexpr iterator operator+(difference_type rhs) const noexcept {
        return iterator(ring, position + rhs);
    }
    constexpr iterator& operator+=(difference_type rhs) noexcept {
        position += rhs;
        return *this;
    }
    constexpr iterator operator-(difference_type rhs) const noexcept {
        return iterator(ring, position - rhs);
    }
    constexpr iterator& operator-=(difference_type rhs) noexcept {
        position -= rhs;
        return *this;
    }
    constexpr bool operator==(iterator const& rhs) const noexcept {
        return (&ring == &rhs.ring) && (position == rhs.position);
    }
    constexpr bool operator!=(iterator const& rhs) const noexcept {
        return (&ring != &rhs.ring) || (position != rhs.position);
    }
    constexpr bool operator<(iterator const& rhs) const noexcept {
        assert(&ring == &rhs.ring);
        return position < rhs.position;
    }
    constexpr bool operator<=(iterator const& rhs) const noexcept {
        assert(&ring == &rhs.ring);
        return position <= rhs.position;
    }
    constexpr bool operator>(iterator const& rhs) const noexcept {
        assert(&ring == &rhs.ring);
        return position > rhs.position;
    }
    constexpr bool operator>=(iterator const& rhs) const noexcept {
        assert(&ring == &rhs.ring);
        return position >= rhs.position;
    }
    constexpr reference
    operator[](difference_type d) const noexcept {
        return ring[static_cast<size_type>(position + d)];
    }
    constexpr reference
    operator*() const noexcept {
        return ring[position];
    }
    constexpr pointer
    operator->() const noexcept {
        return &ring[position];
    }
private:
    std::conditional_t<IsConst, Ring_array const&, Ring_array&> ring;
    size_type position;
};
*/
