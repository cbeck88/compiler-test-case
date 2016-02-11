#pragma once

#include "mock_lua.hpp"

template <typename T>
T*& extra_space_ptr_ref(mock_lua_State * L) {
  return *static_cast<T**>(mock_lua_getextraspace(L));
}

template <typename T>
class owned_state {
  mock_lua_State * L_;

  T*& my_owner_ptr() const {
    return extra_space_ptr_ref<T>(L_);
  }

public:
  owned_state(T * owner)
    : L_(mock_lua_newstate())
  {
    my_owner_ptr() = owner;
  }

  ~owned_state() {
    mock_lua_close(L_);
  }

  mock_lua_State * get() const { return L_; }

  T * get_owner_ptr() const {
    return my_owner_ptr();
  }  
};
