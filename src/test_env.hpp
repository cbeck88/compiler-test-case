#pragma once

#include "mock_lua.hpp"
#include "owned_state.hpp"
#include "dispatch.hpp"

#include <iostream>
#include <set>
#include <string>

struct test_env {
  std::set<std::string> foo;
  std::set<int> bar;

  owned_state<test_env> state;

  int callback_one(mock_lua_State *, std::string str) {
    if (foo.count(str)) {
      return 0;
    }
    std::cerr << "foo '" << str << "' is not available\n";
    return -1;
  }

  int callback_two(mock_lua_State *, int i) {
    if (bar.count(i)) {
      return 0;
    }
    std::cerr << "bar '" << i << "' is not available\n";
    return -1;
  }

  test_env()
    : foo{"x", "y", "z"}
    , bar{999, 666}
    , state(this)
  {
    mock_lua_State * L = state.get();

    mock_lua_pushcfunction(L, DISPATCH(&test_env::callback_one));
    mock_lua_pushcfunction(L, DISPATCH(&test_env::callback_two));
  }
};
