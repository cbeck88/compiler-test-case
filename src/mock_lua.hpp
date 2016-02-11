#pragma once

#include <iostream>

struct mock_lua_State {
  void * extraspace;
};

typedef int (*mock_lua_CFunction)(mock_lua_State *);

/***
 * Create, destroy
 */

inline mock_lua_State * mock_lua_newstate() {
  return new mock_lua_State();
}

inline void mock_lua_close(mock_lua_State * L) {
  delete L;
}

/***
 * extraspace
 */

inline void * mock_lua_getextraspace(mock_lua_State * L) {
  return &(L->extraspace);
}

/***
 * Push c function
 */

inline void mock_lua_pushcfunction(mock_lua_State *, mock_lua_CFunction) {
  std::cout << "Pushed a C function" << std::endl; 
}
