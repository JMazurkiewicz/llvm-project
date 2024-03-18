//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef TEST_STD_RANGES_RANGE_ADAPTORS_RANGE_ADAPTOR_OBJECT_H
#define TEST_STD_RANGES_RANGE_ADAPTORS_RANGE_ADAPTOR_OBJECT_H

#include <concepts>
#include <initializer_list>
#include <ranges>
#include <type_traits>
#include <utility>

#include "test_macros.h"

template <class D>
concept CanInstantiateRangeAdaptorClosure = requires { typename std::ranges::range_adaptor_closure<D>; };

template <class T, class U>
concept CanPipe = requires(T&& t, U&& u) { std::forward<T>(t) | std::forward<U>(u); };

struct NotARange {};

static_assert(!std::ranges::range<NotARange>);

class NotAViewableRange {
  int* b_;
  int* e_;

public:
  template <size_t N>
  constexpr explicit NotAViewableRange(int (&arr)[N]) : b_(arr), e_(arr + N) {}

  NotAViewableRange(const NotAViewableRange&)            = default;
  NotAViewableRange& operator=(const NotAViewableRange&) = default;

  NotAViewableRange(NotAViewableRange&&)            = delete;
  NotAViewableRange& operator=(NotAViewableRange&&) = delete;

  constexpr int* begin() const { return b_; }
  constexpr int* end() const { return e_; }
};

static_assert(std::ranges::range<NotAViewableRange>);
static_assert(!std::ranges::viewable_range<NotAViewableRange>);

class ViewableRange {
  int* b_;
  int* e_;

public:
  template <size_t N>
  constexpr explicit ViewableRange(int (&arr)[N]) : b_(arr), e_(arr + N) {}

  constexpr int* begin() const { return b_; }
  constexpr int* end() const { return e_; }
};

static_assert(std::ranges::viewable_range<ViewableRange>);

template <std::ranges::forward_range V>
  requires std::is_copy_constructible_v<V>
class TakeThreeView : public std::ranges::view_base {
  V base_;

public:
  constexpr explicit TakeThreeView(V& base) : base_(base) {}
  constexpr explicit TakeThreeView(V&& base) : base_(base) {} // intentional copy

  constexpr auto begin() const { return std::ranges::begin(base_); }
  constexpr auto end() const { return std::ranges::next(begin(), 3); }
};

static_assert(std::ranges::view<TakeThreeView<std::ranges::single_view<int>>>);

struct Tag {};

struct TakeThree : std::ranges::range_adaptor_closure<TakeThree> {
  constexpr TakeThree(Tag) {}

  template <class T>
  TakeThree(std::initializer_list<T>) = delete; // disable list initialization

  template <std::ranges::range R>
  constexpr auto operator()(R&& r) const noexcept
    requires requires { TakeThreeView<std::remove_reference_t<R>>{std::forward<R>(r)}; }
  {
    return TakeThreeView<std::remove_reference_t<R>>{std::forward<R>(r)};
  }
};

LIBCPP_STATIC_ASSERT(std::_RangeAdaptorClosure<TakeThree>);
constexpr TakeThree take_three(Tag{});

struct SubstituteZero : std::ranges::range_adaptor_closure<SubstituteZero> {
  constexpr SubstituteZero(Tag) {}

  template <class T>
  SubstituteZero(std::initializer_list<T>) = delete; // disable list initialization

  // Range adaptor closure objects don't have to return a view since C++23.
  constexpr auto operator()(std::ranges::range auto&&) const noexcept { return 0; }
};

LIBCPP_STATIC_ASSERT(std::_RangeAdaptorClosure<SubstituteZero>);
constexpr SubstituteZero substitute_zero(Tag{});

#endif // TEST_STD_RANGES_RANGE_ADAPTORS_RANGE_ADAPTOR_OBJECT_H
