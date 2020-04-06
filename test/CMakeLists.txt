if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/catch.hpp)
  file(DOWNLOAD
    https://github.com/catchorg/Catch2/releases/download/v2.4.0/catch.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/catch.hpp
    STATUS status
    LOG log)

  list(GET status 0 status_code)
  list(GET status 1 status_string)

  if(NOT status_code EQUAL 0)
    message(FATAL_ERROR "error downloading catch: ${status_string}" "${log}")
  endif()
endif()

set(source_files
  test_suite.cpp
  value.cpp)

add_executable(luapp_test_suite ${source_files})

set_target_properties(luapp_test_suite PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)

option(COOL_TEST_COVERAGE "whether or not add coverage instrumentation" OFF)
if (COOL_TEST_COVERAGE)
  target_compile_options(luapp_test_suite PRIVATE "$<$<CONFIG:DEBUG>:-O0>")
  target_compile_options(luapp_test_suite PRIVATE "$<$<CONFIG:DEBUG>:--coverage>")
  set_target_properties(luapp_test_suite PROPERTIES LINK_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
else()
  target_compile_options(luapp_test_suite PRIVATE "$<$<CONFIG:DEBUG>:-O0>")
  target_compile_options(luapp_test_suite PRIVATE "$<$<CONFIG:DEBUG>:-fno-omit-frame-pointer>")
  target_compile_options(luapp_test_suite PRIVATE "$<$<CONFIG:DEBUG>:-fsanitize=address,leak,undefined>")
  set_target_properties(luapp_test_suite PROPERTIES LINK_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address,leak,undefined -fuse-ld=gold")
endif()

target_include_directories(luapp_test_suite PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(luapp_test_suite PUBLIC luapp)

if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    target_compile_options(luapp_test_suite PRIVATE "/W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(luapp_test_suite PRIVATE -Wall -Wextra -Werror -pedantic)
endif()

add_test(NAME test.luapp COMMAND luapp_test_suite)
