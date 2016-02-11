#pragma once

#include "mock_lua.hpp"

/***
 * Helper template which allows to shoe-horn various C++ functions into the required 
 * int (mock_lua_State *) signature so that they can be registered
 */

template <typename T, T t> struct dispatch_helper;

/***
 * Member callback with arguments
 *
 * (reduces to free function case, using "get_extraspace" and forwarding the arguments)
 */
template <typename T, typename... Args>
using member_callback_t = int (T::*)(mock_lua_State *, Args...);

template <typename T, typename ... Args, member_callback_t<T, Args...> target_func>
struct dispatch_helper<member_callback_t<T, Args...>, target_func> {
  static int dispatch(mock_lua_State * L) {
    return 0;
  }
};

/***
 * Macro to work around inability to deduce non-type template parameters
 */
#define DISPATCH(F) &dispatch_helper<decltype(F), (F)>::dispatch
