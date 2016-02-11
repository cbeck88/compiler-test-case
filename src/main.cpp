#include "mock_lua.hpp"

template <typename T, T t> struct dispatch_helper;

template <typename T, typename... Args>
using member_callback_t = int (T::*)(mock_lua_State *, Args...);

//

template <typename T, typename ... Args, member_callback_t<T, Args...> target_func>
struct dispatch_helper<member_callback_t<T, Args...>, target_func> {
  static int dispatch(mock_lua_State * L) {
    return 0;
  }
};

#define DISPATCH(F) &dispatch_helper<decltype(F), (F)>::dispatch

//

struct test {
  int callback_one(mock_lua_State *, const char *) {
    return 0;
  }
};

int main() {
  mock_lua_State * L = 0;

  mock_lua_pushcfunction(L, DISPATCH(&test::callback_one));
}
