#pragma once

#include "mock_lua.hpp"

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

/***
 * Utility for manipulating lists of integers
 */

template <std::size_t... Ss>
struct SizeList {
  static constexpr std::size_t size = sizeof...(Ss);
};

// Metafunction Concat: Concatenate two lists
template <typename L, typename R>
struct Concat;

template <std::size_t... TL, std::size_t... TR>
struct Concat<SizeList<TL...>, SizeList<TR...>> {
  typedef SizeList<TL..., TR...> type;
};

template <typename L, typename R>
using Concat_t = typename Concat<L, R>::type;

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

/***
 * Utility to cast extraspace pointer to owner type
 */
template <typename T>
T*& extra_space_ptr_ref(mock_lua_State * L) {
  return *static_cast<T**>(mock_lua_getextraspace(L));
}

/***
 * Traits to read an argument type
 */

namespace traits {

template <typename T> struct read;

template <>
struct read<const char *> {
  static bool check_read(mock_lua_State *, int) {
    return true;
  }

  static const char * do_read(mock_lua_State *, int) {
    return "asdf";
  }
};

template <>
struct read<int> {
  static bool check_read(mock_lua_State *, int) {
    return true;
  }

  static int do_read(mock_lua_State *, int) {
    return 42;
  }
};

} // end namespace traits

/***
 * Helper template which allows to shoe-horn various C++ functions into the required 
 * int (mock_lua_State *) signature so that they can be registered
 */

template <typename T, T t> struct dispatch_helper;

/***
 * Free function with auto-parsed arguments
 */

template <typename ... Args>
using free_callback_t = int (*)(mock_lua_State *, Args...);

template <typename ... Args, free_callback_t<Args...> target_func>
struct dispatch_helper<free_callback_t<Args...>, target_func> {

  static int dispatch(mock_lua_State * L) {
    return invoker<Count_t<sizeof...(Args)>>::dispatch(L);
  }
  
private:
  typedef std::tuple<Args...> tuple_t;

  template <typename T> struct invoker;

  template <std::size_t... indices>
  struct invoker<SizeList<indices...>> {

    static_assert(sizeof...(indices) == sizeof...(Args), "Something is wrong with template parameters");

    static int dispatch(mock_lua_State * L) noexcept {

      tuple_t tup;

      {
        bool results[sizeof...(indices) + 1] = { try_read_from_stack<indices>(L, std::get<indices>(tup))..., true };

        bool ok = true;
        for (int i = 0; i < sizeof...(indices); ++i) { ok &= results[i]; }
        if (!ok) { return -1; }
      }

      return target_func(L, std::forward<Args>(std::get<indices>(tup))...);
    }

  private:
    template <std::size_t index, typename T>
    static bool try_read_from_stack(mock_lua_State * L, T & t) {
      if (traits::read<T>::check_read(L, index)) {
        t = traits::read<T>::do_read(L, index);
        return true;
      }
      return false;
    }
  };
};

/***
 * Member callback with arguments
 *
 * (reduces to free function case, using "get_extraspace" and forwarding the arguments)
 */
template <typename T, typename... Args>
using member_callback_t = int (T::*)(mock_lua_State *, Args...);

template <typename T, typename ... Args, member_callback_t<T, Args...> target_func>
struct dispatch_helper<member_callback_t<T, Args...>, target_func> {
private:
  static int dispatch_from_extraspace(mock_lua_State * L, Args... args) {
    T & owner = *extra_space_ptr_ref<T>(L);
    return (owner.*target_func)(L, std::forward<Args>(args)...);
  }

public:
  static int dispatch(mock_lua_State * L) {
    typedef dispatch_helper<free_callback_t<Args...>, dispatch_from_extraspace> helper_t;
    return helper_t::dispatch(L);
  }
};

/***
 * Macro to work around inability to deduce non-type template parameters
 */
#define DISPATCH(F) &dispatch_helper<decltype(F), (F)>::dispatch
