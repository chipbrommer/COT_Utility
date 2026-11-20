cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable Hot Reload for MSVC compilers if supported.
if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
    "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(cot_utility LANGUAGES CXX)

# Only toggle defaults when cot_utility is the top-level project
set(COT_IS_TOP_LEVEL OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(COT_IS_TOP_LEVEL ON)
endif()

option(COT_BUILD_EXAMPLES "Build example executables" ${COT_IS_TOP_LEVEL})
option(COT_BUILD_TESTS    "Build unit and integration tests" ${COT_IS_TOP_LEVEL})

# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(pugixml)

# Header-only interface library exposing src/
add_library(cot_utility INTERFACE)
target_include_directories(cot_utility INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>
)

# Link pugixml target
if(TARGET pugixml::pugixml)
  target_link_libraries(cot_utility INTERFACE pugixml::pugixml)
elseif(TARGET pugixml)
  target_link_libraries(cot_utility INTERFACE pugixml)
endif()

if(COT_BUILD_EXAMPLES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt")
  add_subdirectory(examples)
endif()

if(COT_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
  enable_testing()
  add_subdirectory(tests)
endif()