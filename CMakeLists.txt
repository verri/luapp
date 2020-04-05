cmake_minimum_required(VERSION 3.0)

project(Luapp VERSION 0.1)

add_library(luapp
  src/state.cpp
  src/value.cpp
  src/type.cpp
  include/luapp/state.hpp
  include/luapp/value.hpp
  include/luapp/type.hpp)

target_compile_features(luapp PUBLIC cxx_std_17)
target_include_directories(luapp PUBLIC include)

find_package(Lua 5.3 REQUIRED)
target_link_libraries(luapp PRIVATE ${LUA_LIBRARIES})
target_include_directories(luapp PUBLIC ${LUA_INCLUDE_DIR})
