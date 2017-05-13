cmake_minimum_required(VERSION 3.0)
project(Polynomials C)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

# We dont want to watch compilation progress info
set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)

# We do want to see make calling things
set(CMAKE_VERBOSE_MAKEFILE ON)

# Compilation flags
set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra")
set(CMAKE_C_FLAGS_RELEASE "-std=c11 -Wall -Wextra -O3 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-std=c11 -Wall -Wextra -g -O0 ")

# Inclusion directories
include_directories(./)

# Specify source files
add_executable(test_poly ./src/memalloc.c ./src/list.c ./src/poly.c ./src/utils.c ./tests/test_poly.c)

# Check doxygen installation
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # Point doxyfile
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    # Add "doc" target for generating docuemntation with Doxygen
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
endif (DOXYGEN_FOUND)
