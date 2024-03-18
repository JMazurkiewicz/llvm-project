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
// class range_adaptor_closure;

#include <ranges>

#include "types.h"

struct Adaptor : std::ranges::range_adaptor_closure<Adaptor> {
  constexpr int operator()(std::ranges::range auto&&); // not defined
};

// Constraint: is_class_v<D>
static_assert(!CanInstantiateRangeAdaptorClosure<int>);
static_assert(!CanInstantiateRangeAdaptorClosure<Adaptor&>);
static_assert(!CanInstantiateRangeAdaptorClosure<const Adaptor&>);
static_assert(!CanInstantiateRangeAdaptorClosure<Adaptor&&>);
static_assert(!CanInstantiateRangeAdaptorClosure<const Adaptor&&>);

// Constraint: same_as<D, remove_cv_t<D>>
static_assert(CanInstantiateRangeAdaptorClosure<Adaptor>);
static_assert(!CanInstantiateRangeAdaptorClosure<const Adaptor>);
static_assert(!CanInstantiateRangeAdaptorClosure<volatile Adaptor>);
static_assert(!CanInstantiateRangeAdaptorClosure<const volatile Adaptor>);
