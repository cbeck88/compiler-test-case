#pragma once

#include "mock_lua.hpp"
#include <string>

namespace traits {

template <typename T> struct read;

template <>
struct read<std::string> {
  static bool check_read(mock_lua_State * L, int index) {
    return mock_lua_isstring(L, index);
  }

  static std::string do_read(mock_lua_State * L, int index) {
    return mock_lua_tostring(L, index);
  }
};

template <>
struct read<int> {
  static bool check_read(mock_lua_State * L, int index) {
    return mock_lua_isinteger(L, index);
  }

  static int do_read(mock_lua_State * L, int index) {
    return mock_lua_tointeger(L, index);
  }
};

} // end namespace traits
