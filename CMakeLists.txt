cmake_minimum_required(VERSION 3.0)

project(Luapp VERSION 0.1)

add_subdirectory(external/cool)

add_library(luapp
  src/function.cpp
  src/reference.cpp
  src/state.cpp
  src/table.cpp
  src/tuple.cpp
  src/type.cpp
  src/userdata.cpp
  src/value.cpp
  include/luapp/function.hpp
  include/luapp/reference.hpp
  include/luapp/state.hpp
  include/luapp/table.hpp
  include/luapp/tuple.hpp
  include/luapp/type.hpp
  include/luapp/userdata.hpp
  include/luapp/value.hpp)

target_compile_features(luapp PUBLIC cxx_std_17)
target_include_directories(luapp PUBLIC include)

find_package(Lua 5.3 REQUIRED)
target_link_libraries(luapp PRIVATE cool ${LUA_LIBRARIES})
target_include_directories(luapp PUBLIC ${LUA_INCLUDE_DIR})

option(LUAPP_BUILD_TEST "whether or not to build the test" OFF)
if(LUAPP_BUILD_TEST)
  enable_testing()
  add_subdirectory(test)
endif()
