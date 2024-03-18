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
// A range adaptor closure object is a unary function object that accepts a range argument. For a range adaptor closure
// object C and an expression R such that decltype((R)) models range, the following expressions are equivalent:
//   C(R)
//   R | C

#include <ranges>

#include <algorithm>
#include <array>
#include <cassert>
#include <type_traits>
#include <vector>

#include "types.h"

constexpr bool test() {
  { // Test range adaptor closure on class that models `viewable_range`.
    int vals[4] = {0, 1, 2, 3};
    ViewableRange r(vals);

    { // Test standard range adaptor closure object.
      auto expected = std::views::single(0);

      auto v1 = std::views::take(r, 1);
      assert(std::ranges::equal(v1, expected));

      auto v2 = r | std::views::take(1);
      assert(std::ranges::equal(v2, expected));
    }

    { // Test user-defined range adaptor closure object.
      std::array expected{0, 1, 2};

      auto v1 = take_three(r);
      assert(std::ranges::equal(v1, expected));

      auto v2 = r | take_three;
      assert(std::ranges::equal(v2, expected));
    }

    { // Test standard range adaptor closure object that does not return a view.
      std::same_as<std::vector<int>> decltype(auto) c1 = std::ranges::to<std::vector>(r);
      assert(std::ranges::equal(c1, vals));

      std::same_as<std::vector<int>> decltype(auto) c2 = r | std::ranges::to<std::vector>();
      assert(std::ranges::equal(c2, vals));
    }

    { // Test user-defined range adaptor closure object that does not return a view.
      std::same_as<int> decltype(auto) i1 = substitute_zero(r);
      assert(i1 == 0);

      std::same_as<int> decltype(auto) i2 = r | substitute_zero;
      assert(i2 == 0);
    }
  }

  { // Test range adaptor closure on class that does not model `viewable_range` but models `range`.
    int vals[4] = {0, 1, 2, 3};

    // Test standard range adaptor closure object - should fail because `views::all` requires `viewable_range`.
    static_assert(!std::invocable<decltype(std::views::take), NotAViewableRange, int>);
    static_assert(!CanPipe<NotAViewableRange, std::invoke_result_t<decltype(std::views::take), int>>);

    { // Test user-defined range adaptor closure object.
      std::array expected{0, 1, 2};

      std::same_as<TakeThreeView<NotAViewableRange>> decltype(auto) v1 = take_three(NotAViewableRange(vals));
      assert(std::ranges::equal(v1, expected));

      std::same_as<TakeThreeView<NotAViewableRange>> decltype(auto) v2 = NotAViewableRange(vals) | take_three;
      assert(std::ranges::equal(v2, expected));
    }

    { // Test standard range adaptor closure object that does not return a view.
      std::same_as<std::vector<int>> decltype(auto) c1 = std::ranges::to<std::vector>(NotAViewableRange(vals));
      assert(std::ranges::equal(c1, vals));

      std::same_as<std::vector<int>> decltype(auto) c2 = NotAViewableRange(vals) | std::ranges::to<std::vector>();
      assert(std::ranges::equal(c2, vals));
    }

    { // Test user-defined range adaptor closure object that does not return a view.
      std::same_as<int> decltype(auto) i1 = substitute_zero(NotAViewableRange(vals));
      assert(i1 == 0);

      std::same_as<int> decltype(auto) i2 = NotAViewableRange(vals) | substitute_zero;
      assert(i2 == 0);
    }
  }

  { // Test range adaptor closure on class that does not model `range`.
    // Test standard range adaptor closure object.
    static_assert(!std::invocable<decltype(std::views::take), NotARange&, int>);
    static_assert(!CanPipe<NotARange&, std::invoke_result_t<decltype(std::views::take), int>>);

    // Test user-defined range adaptor closure object.
    static_assert(!std::invocable<TakeThree, NotARange&>);
    static_assert(!CanPipe<NotARange&, TakeThree>);

    // Test standard range adaptor closure object that does not return a view.
    static_assert(!std::invocable<decltype(std::ranges::to<std::vector>()), NotARange&>);
    static_assert(!CanPipe<NotARange&, decltype(std::ranges::to<std::vector>())>);

    // Test user-defined range adaptor closure object that does not return a view.
    static_assert(!std::invocable<SubstituteZero, NotARange&>);
    static_assert(!CanPipe<NotARange&, SubstituteZero>);
  }

  return true;
}

int main(int, char**) {
  test();
  static_assert(test());

  return 0;
}
