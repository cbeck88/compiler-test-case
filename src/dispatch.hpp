#pragma once

#include "count.hpp"
#include "mock_lua.hpp"
#include "read.hpp"

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

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