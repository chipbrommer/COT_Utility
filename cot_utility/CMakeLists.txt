
# Define the cot_utility library
add_library(cot_utility
    cot_utility.cpp
    cot_info.h
    cot_utility.h
)

# Link cot_utility to pugi_xml
target_link_libraries(cot_utility 
    PRIVATE
    pugixml
)

# Include the directory with the header files for cot_utility
target_include_directories(cot_utility
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}  # Adds the current directory to the include path
)
