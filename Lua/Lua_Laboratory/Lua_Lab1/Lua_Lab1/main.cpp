#include <iostream>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#pragma comment(lib, "lua53.lib")

using namespace std;

int add_two_c(lua_State* L) {
	int a{ static_cast<int>(lua_tonumber(L, -2)) };
	int b{ static_cast<int>(lua_tonumber(L, -1)) };
	lua_pop(L, 3);
	lua_pushnumber(L, a + b);
	return 1;
}

int main() {
	
	lua_State* L{ luaL_newstate() };		// Create Lua Virtual Machine
	luaL_openlibs(L);						// load Lua Standard Library
	
	luaL_loadfile(L, "test.LUA");

	if (int error{ lua_pcall(L, 0, 0, 0) }; error) {// executation, Param 0, return 0, error handling 0
		cout << "Error : " << lua_tostring(L, -1);
		lua_pop(L, 1);
	}
	lua_register(L, "c_add_two", add_two_c);
	lua_getglobal(L, "add_two");
	lua_pushnumber(L, 100);
	lua_pushnumber(L, 200);


	if (int error{ lua_pcall(L, 2, 1, 0) }; error) {
		cout << "Error : " << lua_tostring(L, -1);
		lua_pop(L, 1);
	}
	else {
		int result{ static_cast<int>(lua_tonumber(L, -1)) };
		lua_pop(L, 1);
		std::cout << result << std::endl;
	}


	lua_close(L);	// delete Lua Virtual Machine
}