include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

file(GLOB COT_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp")

if(NOT COT_TEST_SOURCES)
  message(FATAL_ERROR "No test_*.cpp files found in ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

set(COT_TEST_TARGETS "")

foreach(test_src IN LISTS COT_TEST_SOURCES)
  get_filename_component(test_target "${test_src}" NAME_WE)
  add_executable("${test_target}" "${test_src}")
  target_link_libraries("${test_target}" PRIVATE
    cot_utility
    GTest::gtest_main
  )
  gtest_discover_tests("${test_target}")
  list(APPEND COT_TEST_TARGETS "${test_target}")
endforeach()

add_custom_target(run_tests
  COMMAND ${CMAKE_CTEST_COMMAND} --test-dir "${CMAKE_BINARY_DIR}" -C $<CONFIG> --output-on-failure
  DEPENDS ${COT_TEST_TARGETS}
  USES_TERMINAL
)