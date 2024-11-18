#include <cstdlib>

extern "C" {
    #include <lauxlib.h>
    #include <lualib.h>
}

int main(int argc, char** argv) {
    lua_State* L = luaL_newstate();
    if(L == nullptr) {
        fprintf(stderr, "Failed to allocate state.");
        return EXIT_FAILURE;
    }

    luaL_openlibs(L);

    lua_getglobal(L, "print");
    lua_pushstring(L, "Hello, World!");

    if(lua_pcall(L, 1, 1, 0) != LUA_OK) {
        const char *error_msg = lua_tostring(L, -1);
        fprintf(stderr, "Error: %s\n", error_msg);
        lua_pop(L, 1);
        lua_close(L);
        return EXIT_FAILURE;
    }

    lua_close(L);
    return EXIT_SUCCESS;
}