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

#include <array>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "Index_iterator.hpp"

namespace reBass {

    template <typename T, std::size_t Size>
    class Ring_array {
    public:
        using value_type = T;
        using reference = T&;
        using const_reference = T const&;
        using container_type = std::array<value_type, Size>;
        using size_type = typename container_type::size_type;
        using difference_type = typename container_type::difference_type;
        using container_iterator = typename container_type::iterator;
        using index_type = difference_type;
        using iterator = Index_iterator<Ring_array, false>;
        using const_iterator = Index_iterator<Ring_array, true>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;


        constexpr Ring_array()
        noexcept(std::is_nothrow_default_constructible<T>::value) :
            next(std::begin(c))
        {
        }

        constexpr Ring_array(Ring_array const& rhs)
        noexcept(std::is_nothrow_copy_constructible<T>::value) :
            c(rhs.c),
            next(std::begin(c) + std::distance(std::begin(rhs.c), rhs.next))
        {
        }

        constexpr Ring_array(Ring_array&& rhs)
        noexcept(std::is_nothrow_move_constructible<T>::value) :
            c(std::move(rhs.c)),
            next(std::begin(c) + std::distance(std::begin(rhs.c), rhs.next))
        {
        }

        constexpr Ring_array(container_type const& rhs)
        noexcept(std::is_nothrow_copy_constructible<T>::value) :
            c(rhs),
            next(std::begin(c))
        {
        }

        constexpr Ring_array(container_type&& rhs)
        noexcept(std::is_nothrow_move_constructible<T>::value) :
            c(std::move(rhs)),
            next(std::begin(c))
        {
        }

        constexpr Ring_array& operator=(Ring_array const& rhs)
        noexcept(std::is_nothrow_copy_assignable<T>::value) {
            c = rhs.c;
            next = std::begin(c) + std::distance(std::begin(rhs.c), rhs.next);

            return *this;
        }

        constexpr Ring_array& operator=(Ring_array&& rhs)
        noexcept(std::is_nothrow_move_assignable<T>::value) {
            auto position = static_cast<size_type>(
                std::distance(std::begin(rhs.c), rhs.next)
            );
            c = std::move(rhs.c);
            next = std::begin(c) + position;

            return *this;
        }

        constexpr container_type& linearize()
        noexcept {
            std::rotate(std::begin(c), next, std::end(c));
            next = std::begin(c);
            return c;
        }

        constexpr size_type size() noexcept {
            return Size;
        }

        constexpr bool empty() noexcept {
            return Size == 0;
        }

        constexpr reference operator[](size_type position) noexcept {
            return c[position_to_index(position)];
        }
        constexpr const_reference operator[](size_type position)
        const noexcept {
            return c[position_to_index(position)];
        }

        constexpr reference at(size_type position) {
            if (position >= Size || position < 0) {
                throw_out_of_range();
            }
            return c[position_to_index(position)];
        }
        constexpr const_reference at(size_type position) const {
            if (position >= Size || position < 0) {
                throw_out_of_range();
            }
            return c[position_to_index(position)];
        }

        constexpr reference back()
        noexcept {
            return (next != std::begin(c)) ? *(next - 1) : c.front();
        }
        constexpr const_reference back()
        const noexcept {
            return (next != std::begin(c)) ? *(next - 1) : c.front();
        }

        constexpr reference front()
        noexcept {
            return *next;
        }
        constexpr const_reference front()
        const noexcept {
            return *next;
        }

        constexpr iterator begin()
        noexcept {
            return iterator(this, 0u);
        }
        constexpr iterator end()
        noexcept {
            return iterator(this, Size);
        }

        constexpr const_iterator begin()
        const noexcept {
            return const_iterator(this, 0u);
        }
        constexpr const_iterator end()
        const noexcept {
            return const_iterator(this, Size);
        }
        constexpr const_iterator cbegin()
        const noexcept {
            return const_iterator(this, 0u);
        }
        constexpr const_iterator cend()
        const noexcept {
            return const_iterator(this, Size);
        }

        constexpr reverse_iterator rbegin()
        noexcept {
            return reverse_iterator(end());
        }
        constexpr reverse_iterator rend()
        noexcept {
            return reverse_iterator(begin());
        }

        constexpr const_reverse_iterator rbegin()
        const noexcept {
            return const_reverse_iterator(cend());
        }
        constexpr reverse_iterator rend()
        const noexcept {
            return const_reverse_iterator(cbegin());
        }
        constexpr const_reverse_iterator crbegin()
        const noexcept {
            return const_reverse_iterator(cend());
        }
        constexpr const_reverse_iterator crend()
        const noexcept {
            return const_reverse_iterator(cbegin());
        }

        constexpr void push_back(value_type const& value)
        noexcept(std::is_nothrow_copy_constructible<T>::value) {
            auto value_copy{value};
            push_back(std::move(value_copy));
        }

        constexpr void push_back(value_type&& value)
        noexcept(std::is_nothrow_move_assignable<T>::value) {
            *(next++) = std::move(value);
            if (next == std::end(c)) {
                next = std::begin(c);
            }
        }

        template <typename... From>
        constexpr void emplace_back(From&&... from)
        noexcept(std::is_nothrow_constructible<T, From...>::value
                 && std::is_nothrow_move_assignable<T>::value) {
            *(next++) = T(std::forward<From>(from)...);
            if (next == std::end(c)) {
                next = std::begin(c);
            }
        }

        template <typename InputIterator>
        constexpr void insert_at_end(InputIterator first, InputIterator last) {
            assert(std::distance(first, last) >= 0);
            while (first != last) {
                push_back(*(first++));
            }
        }

    private:
        constexpr void throw_out_of_range()
        const {
            throw std::out_of_range("Ring_array");
        }

        constexpr size_type position_to_index(size_type position)
        const noexcept {
            difference_type d = next - std::cbegin(c);
            assert(d < static_cast<difference_type>(Size) && d >= 0);
            size_type index = static_cast<size_type>(d) + position;
            return (index < Size) ? index : (index - Size);
        }

        container_type c;
        container_iterator next;
    };
}
