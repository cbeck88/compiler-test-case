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
 * get values from stack
 */
inline bool mock_lua_isstring(mock_lua_State *, int) { return true; }

inline const char * mock_lua_tostring(mock_lua_State *, int) {
  return "dummy";
}

inline bool mock_lua_isinteger(mock_lua_State *, int) { return true; }

inline int mock_lua_tointeger(mock_lua_State *, int) {
  return 42;
}

/***
 * Push c function
 */

inline void mock_lua_pushcfunction(mock_lua_State *, mock_lua_CFunction) {
  std::cout << "Pushed a C function" << std::endl; 
}

/***
 * Signal an error
 */

inline int mock_lua_error(mock_lua_State *) {
  std::cout << "Error reported" << std::endl;
  return 0;
}
