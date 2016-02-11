#pragma once

#include <cstddef>

// Metafunction Concat: Concatenate two lists
template <typename L, typename R>
struct Concat;

template <typename L, typename R>
using Concat_t = typename Concat<L, R>::type;

/***
 * Utility for manipulating lists of integers
 */

template <std::size_t... Ss>
struct SizeList {
  static constexpr std::size_t size = sizeof...(Ss);
};

template <std::size_t... TL, std::size_t... TR>
struct Concat<SizeList<TL...>, SizeList<TR...>> {
  typedef SizeList<TL..., TR...> type;
};

/***
 * Count_t<n> produces a sizelist containing numbers 0 to n-1.
 */
template <std::size_t n>
struct Count {
  typedef Concat_t<typename Count<n - 1>::type, SizeList<n - 1>> type;
};

template <>
struct Count<0> {
  typedef SizeList<> type;
};

template <std::size_t n>
using Count_t = typename Count<n>::type;
