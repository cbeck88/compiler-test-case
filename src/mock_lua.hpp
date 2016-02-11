#pragma once

#include <iostream>

struct mock_lua_State {
  void * extraspace;
};

typedef int (*mock_lua_CFunction)(mock_lua_State *);

/***
 * Push c function
 */

inline void mock_lua_pushcfunction(mock_lua_State *, mock_lua_CFunction) {
  std::cout << "Pushed a C function" << std::endl; 
}
