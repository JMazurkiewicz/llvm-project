//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++03, c++11, c++14, c++17

// template<input_range V>
//   requires view<V> && input_range<range_reference_t<V>>
// template<bool Const>
// struct join_view<V>::iterator { // non-final

#include <ranges>

#include <type_traits>
#include <vector>

struct Range : std::ranges::view_base {
  std::vector<int>* begin();
  std::vector<int>* end();
};

using JoinView = std::ranges::join_view<Range>;
using JoinIter = std::ranges::iterator_t<JoinView>;
static_assert(!std::is_final_v<JoinIter>);

struct PublicChild : public JoinIter {};
static_assert(!std::input_iterator<PublicChild>);

struct ProtectedChild : protected JoinIter {};
static_assert(!std::input_iterator<ProtectedChild>);

struct PrivateChild : private JoinIter {};
static_assert(!std::input_iterator<PrivateChild>);
