#include "mock_lua.hpp"
#include "dispatch.hpp"

struct test {
  int callback_one(mock_lua_State *, std::string) {
    return 0;
  }
};

int main() {
  mock_lua_State * L = mock_lua_newstate();

  mock_lua_pushcfunction(L, DISPATCH(&test::callback_one));
}
