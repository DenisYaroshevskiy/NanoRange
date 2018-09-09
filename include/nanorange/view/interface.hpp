// nanorange/view/interface.hpp
//
// Copyright (c) 2018 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef NANORANGE_VIEW_INTERFACE_HPP_INCLUDED
#define NANORANGE_VIEW_INTERFACE_HPP_INCLUDED

#include <nanorange/detail/ranges/concepts.hpp>
#include <nanorange/iterator/common_iterator.hpp>
#include <nanorange/iterator/operations.hpp>

NANO_BEGIN_NAMESPACE

// [ranges.view_interface]

namespace detail {

template <typename, typename = void>
struct range_common_iterator_impl;

template <typename R>
struct range_common_iterator_impl<
    R, std::enable_if_t<Range<R> && !CommonRange<R>>> {
    using type = common_iterator<iterator_t<R>, sentinel_t<R>>;
};

template <typename R>
struct range_common_iterator_impl<R, std::enable_if_t<CommonRange<R>>> {
    using type = iterator_t<R>;
};

template <typename R>
using range_common_iterator_t = typename range_common_iterator_impl<R>::type;

} // namespace detail

template <typename D>
class view_interface {

    static_assert(std::is_class<D>::value, "");

private:
    constexpr D& derived() noexcept { return static_cast<D&>(*this); }

    constexpr const D& derived() const noexcept
    {
        return static_cast<const D&>(*this);
    }

public:
    template <typename R = D>
    NANO_NODISCARD constexpr auto empty() const
        -> std::enable_if_t<ForwardRange<const R>, bool>
    {
        return ranges::begin(derived()) == ranges::end(derived());
    }

    template <typename R = D, typename = decltype(ranges::empty(std::declval<const R&>()))>
    constexpr explicit operator bool() const
    {
        return !ranges::empty(derived());
    }

    // FIXME: This is to spec (P0896R2) but seems wrong when begin() does not return a pointer type
    template <typename R = D, typename = std::enable_if_t<ContiguousIterator<iterator_t<R>>>>
    constexpr auto data()
    {
        return ranges::begin(derived());
    }

    template <typename R = const D, typename = std::enable_if_t<
            Range<R> &&
            ContiguousIterator<iterator_t<R>>>>
    constexpr auto data() const
    {
        return ranges::begin(derived());
    }

    template <typename R = D, typename = std::enable_if_t<
            ForwardRange<const R> &&
            SizedSentinel<sentinel_t<const R>, iterator_t<const R>>
    >>
    constexpr auto size() const
    {
        return ranges::end(derived()) - ranges::begin(derived());
    }

    template <typename R = D, typename = std::enable_if_t<ForwardRange<R>>>
    constexpr decltype(auto) front()
    {
        return *ranges::begin(derived());
    }

    template <typename R = D, typename = std::enable_if_t<ForwardRange<const R>>>
    constexpr decltype(auto) front() const
    {
        return *ranges::begin(derived());
    }

    template <typename R = D, typename = std::enable_if_t<
            BidirectionalRange<R> && CommonRange<R>>>
    constexpr decltype(auto) back()
    {
        return *ranges::prev(ranges::end(derived()));
    }

    template <typename R = D, typename = std::enable_if_t<
            BidirectionalRange<const R> && CommonRange<const R>>>
    constexpr decltype(auto) back() const
    {
        return *ranges::prev(ranges::end(derived()));
    }

    template <typename R = D, typename = std::enable_if_t<RandomAccessRange<R>>>
    constexpr decltype(auto) operator[](iter_difference_t<iterator_t<R>> n)
    {
        return ranges::begin(derived())[n];
    }

    template <typename R = const D,  typename = std::enable_if_t<RandomAccessRange<R>>>
    constexpr decltype(auto) operator[](iter_difference_t<iterator_t<R>> n) const
    {
        return ranges::begin(derived())[n];
    }

    template <typename C, typename R = D,
              typename = std::enable_if_t<
                  ForwardRange<C> && !View<C> &&
                  ConvertibleTo<iter_reference_t<iterator_t<const R>>,
                                iter_value_t<iterator_t<C>>> &&
                  Constructible<C, detail::range_common_iterator_t<const R>,
                                detail::range_common_iterator_t<const R>>>>
    operator C() const
    {
        using I = detail::range_common_iterator_t<D>;
        return C(I{ranges::begin(derived())}, I{ranges::end(derived())});
    }
};

NANO_END_NAMESPACE

#endif
