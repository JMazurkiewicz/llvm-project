//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17, c++20

// <ranges>

// template<class D>
//   requires is_class_v<D> && same_as<D, remove_cv_t<D>>
// class range_adaptor_closure { };
//
// Given an object t of type T, where
// - t is a unary function object that accepts a range argument,
// - T models derived_from<range_adaptor_closure<T>>,
// - T has no other base classes of type range_adaptor_closure<U> for any other type U, and
// - T does not model range
// then the implementation ensures that t is a range adaptor closure object.
//
// The template parameter D for range_adaptor_closure may be an incomplete type. (...)

#include <ranges>

#include <utility>

#include "test_macros.h"
#include "types.h"

struct Base {};

// T is a unary function object that does not accept a range argument.
struct NotARangeAdaptor : std::ranges::range_adaptor_closure<NotARangeAdaptor> {
  template <class R>
    requires(!std::ranges::range<R>)
  constexpr R&& operator()(R&& r); // not defined
};

// LIBCPP_STATIC_ASSERT(!std::_RangeAdaptorClosure<NotARangeAdaptor>); // Impossible to verify.

// T does not model derived_from<range_adaptor_closure<T>>.
struct RangeAdaptorWithoutCorrectBase {
  template <std::ranges::range R>
  R&& operator()(R&& r) const; // not defined
};

LIBCPP_STATIC_ASSERT(!std::_RangeAdaptorClosure<RangeAdaptorWithoutCorrectBase>);

// T has base class of type range_adaptor_closure<U> for other type U.
struct RangeAdaptorWithIncorrectBase : std::ranges::range_adaptor_closure<Base> {
  template <std::ranges::range R>
  R&& operator()(R&&) const; // not defined
};

LIBCPP_STATIC_ASSERT(!std::_RangeAdaptorClosure<RangeAdaptorWithIncorrectBase>);

struct RangeAdaptorWithMultipleBases
    : std::ranges::range_adaptor_closure<RangeAdaptorWithMultipleBases>,
      std::ranges::range_adaptor_closure<Base> {
  template <std::ranges::range R>
  R&& operator()(R&& r) const; // not defined
};

LIBCPP_STATIC_ASSERT(!std::_RangeAdaptorClosure<RangeAdaptorWithMultipleBases>);

// T models a range
struct RangeAdaptorAndARange : std::ranges::range_adaptor_closure<RangeAdaptorAndARange> {
  template <std::ranges::range R>
  R&& operator()(R&& r) const; // not defined

  int* begin() const; // not defined
  int* end() const;   // not defined
};

static_assert(std::ranges::range<RangeAdaptorAndARange>);
LIBCPP_STATIC_ASSERT(!std::_RangeAdaptorClosure<RangeAdaptorAndARange>);

namespace test_viewable_range {
using R = ViewableRange&;

static_assert(!std::invocable<NotARangeAdaptor, R>);
static_assert(!CanPipe<R, NotARangeAdaptor>);

static_assert(std::invocable<RangeAdaptorWithoutCorrectBase, R>);
static_assert(!CanPipe<R, RangeAdaptorWithoutCorrectBase>);

static_assert(std::invocable<RangeAdaptorWithIncorrectBase, R>);
static_assert(!CanPipe<R, RangeAdaptorWithIncorrectBase>);

static_assert(std::invocable<RangeAdaptorWithMultipleBases, R>);
static_assert(!CanPipe<R, RangeAdaptorWithMultipleBases>);

static_assert(std::invocable<RangeAdaptorAndARange, R>);
static_assert(!CanPipe<R, RangeAdaptorAndARange>);
} // namespace test_viewable_range

namespace test_not_a_viewable_range {
using R = NotAViewableRange;

static_assert(!std::invocable<NotARangeAdaptor, R>);
static_assert(!CanPipe<R, NotARangeAdaptor>);

static_assert(std::invocable<RangeAdaptorWithoutCorrectBase, R>);
static_assert(!CanPipe<R, RangeAdaptorWithoutCorrectBase>);

static_assert(std::invocable<RangeAdaptorWithIncorrectBase, R>);
static_assert(!CanPipe<R, RangeAdaptorWithIncorrectBase>);

static_assert(std::invocable<RangeAdaptorWithMultipleBases, R>);
static_assert(!CanPipe<R, RangeAdaptorWithMultipleBases>);

static_assert(std::invocable<RangeAdaptorAndARange, R>);
static_assert(!CanPipe<R, RangeAdaptorAndARange>);
} // namespace test_not_a_viewable_range

namespace test_not_a_range {
using R = NotARange&;

static_assert(std::invocable<NotARangeAdaptor, R>);
static_assert(!CanPipe<R, NotARangeAdaptor>);

static_assert(!std::invocable<RangeAdaptorWithoutCorrectBase, R>);
static_assert(!CanPipe<R, RangeAdaptorWithoutCorrectBase>);

static_assert(!std::invocable<RangeAdaptorWithIncorrectBase, R>);
static_assert(!CanPipe<R, RangeAdaptorWithIncorrectBase>);

static_assert(!std::invocable<RangeAdaptorWithMultipleBases, R>);
static_assert(!CanPipe<R, RangeAdaptorWithMultipleBases>);

static_assert(!std::invocable<RangeAdaptorAndARange, R>);
static_assert(!CanPipe<R, RangeAdaptorAndARange>);
} // namespace test_not_a_range

// The template parameter D for range_adaptor_closure may be an incomplete type.
struct Incomplete;
static_assert(CanInstantiateRangeAdaptorClosure<Incomplete>);
