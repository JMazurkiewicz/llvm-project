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
// Given an additional range adaptor closure object D, the expression C | D produces another range
// adaptor closure object E. E is a perfect forwarding call wrapper ([func.require]) with the following properties:
// - Its target object is an object d of type decay_t<decltype((D))> direct-non-list-initialized with D.
// - It has one bound argument entity, an object c of type decay_t<decltype((C))> direct-non-list-initialized with C.
// - Its call pattern is d(c(arg)), where arg is the argument used in a function call expression of E.
// The expression C | D is well-formed if and only if the initializations of the state
// entities of E are all well-formed.

#include <ranges>

#include <algorithm>
#include <array>
#include <cassert>
#include <vector>

#include "test_macros.h"
#include "types.h"

constexpr void basic_tests() {
  { // Test composed range adaptor closure on class that models `viewable_range`.
    int vals[4] = {0, 1, 2, 3};
    ViewableRange r(vals);

    { // Compose two standard range adaptor closure objects.
      auto expected = std::views::single(0);
      auto composed = std::views::take(1) | std::ranges::to<std::vector>();

      std::same_as<std::vector<int>> decltype(auto) c1 = composed(r);
      assert(std::ranges::equal(c1, expected));

      std::same_as<std::vector<int>> decltype(auto) c2 = r | composed;
      assert(std::ranges::equal(c2, expected));
    }

    { // Compose two user-defined range adaptor closure objects.
      auto composed = take_three | substitute_zero;

      auto i1 = composed(r);
      assert(i1 == 0);

      auto i2 = r | composed;
      assert(i2 == 0);
    }

    { // Compose one standard and one user defined range adaptor closure objects.
      auto composed = std::views::take(2) | substitute_zero;

      auto i1 = composed(r);
      assert(i1 == 0);

      auto i2 = r | composed;
      assert(i2 == 0);
    }

    { // Compose three range adaptor closure objects.
      std::array expected{1, 2, 3};
      auto composed = std::views::drop(1) | take_three | std::ranges::to<std::vector<int>>();

      std::same_as<std::vector<int>> decltype(auto) v1 = composed(r);
      assert(std::ranges::equal(v1, expected));

      std::same_as<std::vector<int>> decltype(auto) v2 = r | composed;
      assert(std::ranges::equal(v2, expected));
    }
  }

  { // Test range adaptor closure on class that does not model `viewable_range` but models `range`.
    int vals[4] = {0, 1, 2, 3};

    { // Compose two standard range adaptor closure objects (should not work, std::views::take requires viewable_range).
      using Composed = decltype(std::views::take(1) | std::ranges::to<std::vector>());
      static_assert(!std::invocable<Composed, NotAViewableRange>);
      static_assert(!CanPipe<NotAViewableRange, Composed>);
    }

    { // Compose two user-defined range adaptor closure objects.
      auto composed = take_three | substitute_zero;

      auto i1 = composed(NotAViewableRange(vals));
      assert(i1 == 0);

      auto i2 = NotAViewableRange(vals) | composed;
      assert(i2 == 0);
    }

    { // Compose one standard and one user defined range adaptor closure
      // objects (should not work, std::views::take requires viewable_range).
      using Composed = decltype(std::views::take(2) | substitute_zero);
      static_assert(!std::invocable<Composed, NotAViewableRange>);
      static_assert(!CanPipe<NotAViewableRange, Composed>);
    }

    { // Compose three range adaptor closure objects (should not work, std::views::drop requires viewable_range).
      using Composed = decltype(std::views::drop(1) | take_three | std::ranges::to<std::vector<int>>());
      static_assert(!std::invocable<Composed, NotAViewableRange>);
      static_assert(!CanPipe<NotAViewableRange, Composed>);
    }
  }

  {   // Test range adaptor closure on class that does not model `range`.
    { // Compose two standard range adaptor closure objects.
      using Composed = decltype(std::views::take(1) | std::ranges::to<std::vector>());
      static_assert(!std::invocable<Composed, NotARange>);
      static_assert(!CanPipe<NotARange, Composed>);
    }

    { // Compose two user-defined range adaptor closure objects.
      using Composed = decltype(take_three | substitute_zero);
      static_assert(!std::invocable<Composed, NotARange>);
      static_assert(!CanPipe<NotARange, Composed>);
    }

    { // Compose one standard and one user defined range adaptor closure.
      using Composed = decltype(std::views::take(2) | substitute_zero);
      static_assert(!std::invocable<Composed, NotARange>);
      static_assert(!CanPipe<NotARange, Composed>);
    }

    { // Compose three range adaptor closure objects.
      using Composed = decltype(std::views::drop(1) | take_three | std::ranges::to<std::vector<int>>());
      static_assert(!std::invocable<Composed, NotARange>);
      static_assert(!CanPipe<NotARange, Composed>);
    }
  }
}

constexpr void test_perfect_forwarding_call_wrapper() {
  int vals[4] = {0, 5, 10, 15};
  ViewableRange r(vals);

  { // Make sure we call the correctly cv-ref qualified operator()
    // based on the value category of the composition's result type.
    struct X : std::ranges::range_adaptor_closure<X> {
      constexpr auto operator()(ViewableRange r) & { return std::forward<decltype(r)>(r) | std::views::take(1); }

      constexpr auto operator()(ViewableRange r) const& {
        return std::forward<decltype(r)>(r) | std::views::drop(1) | std::views::take(1);
      }

      constexpr auto operator()(ViewableRange r) && {
        return std::forward<decltype(r)>(r) | std::views::drop(2) | std::views::take(1);
      }

      constexpr auto operator()(ViewableRange r) const&& {
        return std::forward<decltype(r)>(r) | std::views::drop(3) | std::views::take(1);
      }
    };
    LIBCPP_STATIC_ASSERT(std::_RangeAdaptorClosure<X>);

    auto composed  = X{} | std::ranges::to<std::vector<int>>();
    using Composed = decltype(composed);

    assert(static_cast<Composed&>(composed)(r).front() == 0);
    assert(static_cast<const Composed&>(composed)(r).front() == 5);
    assert(static_cast<Composed&&>(composed)(r).front() == 10);
    assert(static_cast<const Composed&&>(composed)(r).front() == 15);
  }

  // Make sure the composition's unspecified-type does not model invocable
  // when the call would select a differently-qualified operator().
  //
  // For example, if the call to `operator()() &` is ill-formed, the call to the unspecified-type
  // should be ill-formed and not fall back to the `operator()() const&` overload.
  { // Make sure we delete the & overload when the underlying call isn't valid.
    {
      struct X : std::ranges::range_adaptor_closure<X> {
        ViewableRange operator()(ViewableRange) & = delete;
        ViewableRange operator()(ViewableRange) const&;
        ViewableRange operator()(ViewableRange) &&;
        ViewableRange operator()(ViewableRange) const&&;
      };
      LIBCPP_STATIC_ASSERT(std::_RangeAdaptorClosure<X>);

      using F = decltype(X{} | std::views::take(1));
      static_assert(!std::invocable<F&, ViewableRange>);
      static_assert(std::invocable<const F&, ViewableRange>);
      static_assert(std::invocable<F, ViewableRange>);
      static_assert(std::invocable<const F, ViewableRange>);
    }

    // There's no way to make sure we delete the const& overload when the underlying call isn't valid,
    // so we can't check this one.

    { // Make sure we delete the && overload when the underlying call isn't valid.
      struct X : std::ranges::range_adaptor_closure<X> {
        ViewableRange operator()(ViewableRange) &;
        ViewableRange operator()(ViewableRange) const&;
        ViewableRange operator()(ViewableRange) && = delete;
        ViewableRange operator()(ViewableRange) const&&;
      };
      LIBCPP_STATIC_ASSERT(std::_RangeAdaptorClosure<X>);

      using F = decltype(X{} | std::views::take(1));
      static_assert(std::invocable<F&, ViewableRange>);
      static_assert(std::invocable<const F&, ViewableRange>);
      static_assert(!std::invocable<F, ViewableRange>);
      static_assert(std::invocable<const F, ViewableRange>);
    }

    { // Make sure we delete the const&& overload when the underlying call isn't valid.
      struct X : std::ranges::range_adaptor_closure<X> {
        ViewableRange operator()(ViewableRange) &;
        ViewableRange operator()(ViewableRange) const&;
        ViewableRange operator()(ViewableRange) &&;
        ViewableRange operator()(ViewableRange) const&& = delete;
      };
      LIBCPP_STATIC_ASSERT(std::_RangeAdaptorClosure<X>);

      using F = decltype(X{} | std::views::take(1));
      static_assert(std::invocable<F&, ViewableRange>);
      static_assert(std::invocable<const F&, ViewableRange>);
      static_assert(std::invocable<F, ViewableRange>);
      static_assert(!std::invocable<const F, ViewableRange>);
    }
  }
}

constexpr void test_return_type() {
  { // Test properties of the constructor of the composition's return type.
    int vals[3] = {2, 3, 5};
    ViewableRange r(vals);

    { // Test composition's return type that is move only.
      struct MoveOnlyAdaptor : std::ranges::range_adaptor_closure<MoveOnlyAdaptor> {
        MoveOnlyAdaptor()                  = default;
        MoveOnlyAdaptor(MoveOnlyAdaptor&&) = default;

        constexpr ViewableRange operator()(ViewableRange r) { return r; }
      };
      LIBCPP_STATIC_ASSERT(std::_RangeAdaptorClosure<MoveOnlyAdaptor>);

      auto f = MoveOnlyAdaptor{} | std::views::take(1);
      assert(f(r).front() == 2);
      assert((r | f).front() == 2);

      auto f1 = std::move(f);
      assert(f1(r).front() == 2);
      assert((r | f1).front() == 2);

      using F = decltype(f);
      static_assert(std::is_move_constructible_v<F>);
      static_assert(!std::is_copy_constructible_v<F>);
      static_assert(!std::is_move_assignable_v<F>);
      static_assert(!std::is_copy_assignable_v<F>);
    }

    { // Test test composition's return type that is copyable but not assignable.
      struct CopyableAdaptor : std::ranges::range_adaptor_closure<CopyableAdaptor> {
        CopyableAdaptor() = default;
        constexpr CopyableAdaptor(const CopyableAdaptor&) {}
        constexpr CopyableAdaptor(CopyableAdaptor&&) {}

        constexpr ViewableRange operator()(ViewableRange r) { return r; }
      };
      LIBCPP_STATIC_ASSERT(std::_RangeAdaptorClosure<CopyableAdaptor>);

      auto f = CopyableAdaptor{} | std::views::drop(1);
      assert(f(r).front() == 3);
      assert((r | f).front() == 3);

      auto f1 = f;
      assert(f1(r).front() == 3);
      assert((r | f1).front() == 3);

      using F = decltype(f);
      static_assert(std::is_move_constructible<F>::value);
      static_assert(std::is_copy_constructible<F>::value);
      static_assert(!std::is_move_assignable<F>::value);
      static_assert(!std::is_copy_assignable<F>::value);
    }

    { // Test constructors when function is copy assignable.
      struct CopyAssignableAdaptor : std::ranges::range_adaptor_closure<CopyAssignableAdaptor> {
        CopyAssignableAdaptor()                                        = default;
        CopyAssignableAdaptor(const CopyAssignableAdaptor&)            = default;
        CopyAssignableAdaptor(CopyAssignableAdaptor&&)                 = default;
        CopyAssignableAdaptor& operator=(const CopyAssignableAdaptor&) = default;
        CopyAssignableAdaptor& operator=(CopyAssignableAdaptor&&)      = default;

        constexpr ViewableRange operator()(ViewableRange r);
      };
      LIBCPP_STATIC_ASSERT(std::_RangeAdaptorClosure<CopyAssignableAdaptor>);

      using F = decltype(CopyAssignableAdaptor{} | std::views::take(1));
      static_assert(std::is_move_constructible<F>::value);
      static_assert(std::is_copy_constructible<F>::value);
      static_assert(std::is_move_assignable<F>::value);
      static_assert(std::is_copy_assignable<F>::value);
    }

    { // Test constructors when function is move assignable only.
      struct MoveAssignableAdaptor : std::ranges::range_adaptor_closure<MoveAssignableAdaptor> {
        MoveAssignableAdaptor()                                        = default;
        MoveAssignableAdaptor(const MoveAssignableAdaptor&)            = delete;
        MoveAssignableAdaptor(MoveAssignableAdaptor&&)                 = default;
        MoveAssignableAdaptor& operator=(const MoveAssignableAdaptor&) = delete;
        MoveAssignableAdaptor& operator=(MoveAssignableAdaptor&&)      = default;

        ViewableRange operator()(ViewableRange);
      };
      LIBCPP_STATIC_ASSERT(std::_RangeAdaptorClosure<MoveAssignableAdaptor>);

      using F = decltype(MoveAssignableAdaptor{} | std::views::drop(1));
      static_assert(std::is_move_constructible<F>::value);
      static_assert(!std::is_copy_constructible<F>::value);
      static_assert(std::is_move_assignable<F>::value);
      static_assert(!std::is_copy_assignable<F>::value);
    }
  }

  { // Make sure compositions's return type's operator() is SFINAE-friendly.
    struct Adaptor : std::ranges::range_adaptor_closure<Adaptor> {
      ViewableRange operator()(ViewableRange);
    };
    LIBCPP_STATIC_ASSERT(std::_RangeAdaptorClosure<Adaptor>);

    using F = decltype(Adaptor{} | std::views::take(1));
    static_assert(!std::is_invocable<F>::value);
    static_assert(std::is_invocable<F, ViewableRange>::value);
    static_assert(!std::is_invocable<F, NotAViewableRange>::value);
  }
}

constexpr bool test() {
  basic_tests();
  test_perfect_forwarding_call_wrapper();
  test_return_type();

  return true;
}

int main(int, char**) {
  test();
  static_assert(test());

  return 0;
}
