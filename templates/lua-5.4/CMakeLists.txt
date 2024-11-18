cmake_minimum_required(VERSION 3.10)

project(__PROJECT_NAME__
        VERSION __VERSION__
        DESCRIPTION "__DESCRIPTION__")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB LUA_SOURCES "include/lua/*.c")
file(GLOB CXX_SOURCES "src/*.c*")

add_executable(__NAME__ ${CXX_SOURCES} ${LUA_SOURCES})