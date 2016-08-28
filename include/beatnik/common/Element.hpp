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

#include <algorithm>
#include <numeric>
#include <utility>
#include <type_traits>

namespace reBass
{
template <typename T>
struct Element
{
    T value;
    std::size_t index;

    Element ()
    {
    }

    template <typename InputIt>
    Element (InputIt first, InputIt element) :
        value(*element),
        index(static_cast<std::size_t>(std::distance(first, element)))
    {
    }

    Element (T& value, std::size_t index) :
        value(value),
        index(index)
    {
    }

    Element (T&& value, std::size_t index) :
        value(value),
        index(index)
    {
    }

    template <typename InputIt>
    static Element<T>
    max (InputIt first, InputIt last)
    {
        auto m = std::max_element(
            first,
            last
        );
        return Element<T>(first, m);
    };
};

template <
    typename InputIt1,
    typename InputIt2,
    typename BinaryOp,
    typename ElementType = typename BinaryOp::result_type
>
auto max_result (
    InputIt1 first1,
    InputIt1 last1,
    InputIt2 first2,
    BinaryOp op
)
{
    if (first1 == last1) {
        return Element<ElementType>(static_cast<ElementType>(0), size_t{0});
    }

    if (std::distance(first1, last1) == 0) {
        return Element<ElementType>(op(*first1, *first2), std::size_t{0});
    }

    auto it1 = first1;
    auto it2 = first2;
    auto largest1 = first1;
    auto largest2 = first2;

    while (it1 != last1) {
        if (op(*largest1, *largest2) < op(*(++it1), *(++it2))) {
            largest1 = it1;
            largest2 = it2;
        }
    }

    return Element<ElementType>(
        op(*largest1, *largest2),
        std::distance(first1, largest1)
    );

    /*
    auto first = boost::make_zip_iterator(
        boost::make_tuple(
            first1,
            first2
        )
    );
    auto last = boost::make_zip_iterator(
        boost::make_tuple(
            last1,
            first2 + std::distance(first1, last1)
        )
    );
    auto m = std::max_element(
        first,
        last,
        [&op] (auto const& a, auto const& b) {
            return op(boost::get<0>(a), boost::get<1>(a))
                   < op(boost::get<0>(b), boost::get<1>(b));
        }
    );
    return Element<ElementType>(
        op(boost::get<0>(*m), boost::get<1>(*m)),
        std::distance(first, m)
    );
     */
};
}
