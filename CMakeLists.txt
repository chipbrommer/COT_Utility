cmake_minimum_required(VERSION 3.8)

set(
  CMAKE_CXX_STANDARD 20
  CMAKE_CXX_STANDARD_REQUIRED true
  CMAKE_CXX_EXTENSIONS off
)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("cot_utility_example")

# Include the pugi_xml.cmake to fetch the pugixml library
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(pugixml)

# Add the cot_utility subdirectory
add_subdirectory("cot_utility")

# Define the target
set(target examples)

# Include source file for the executable
add_executable(${target} "Examples.cpp")

# Link the libraries to your target
target_link_libraries(${target} 
    PRIVATE
    cot_utility
    pugixml
)

# Optionally, if you have additional setup like include directories:
target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/cot_utility)
