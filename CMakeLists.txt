cmake_minimum_required(VERSION 3.0)
project(Polynomials C)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif ()


# We do want to see make calling things
#set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Compilation flags
set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra -pedantic")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-g -O0 ")

# Inclusion directories
include_directories(./src/)
include_directories(./test_utils/)

# Sources
file(GLOB SRC_FILES ./src/*.c)
file(GLOB TEST_UTILS_SRC_FILES ./test_utils/*.c)

# Specify source files
add_executable(calc_poly ${SRC_FILES})


# Try to find Cmocka
set(_CMOCKA_ROOT_HINTS)
set(_CMOCKA_ROOT_PATHS "$ENV{PROGRAMFILES}/cmocka")
find_path(CMOCKA_ROOT_DIR NAMES include/cmocka.h HINTS ${_CMOCKA_ROOT_HINTS} PATHS ${_CMOCKA_ROOT_PATHS})
mark_as_advanced(CMOCKA_ROOT_DIR)
find_path(CMOCKA_INCLUDE_DIR NAMES cmocka.h PATHS ${CMOCKA_ROOT_DIR}/include)
find_library(CMOCKA_LIBRARY NAMES cmocka PATHS ${CMOCKA_ROOT_DIR}/lib)


set(TEST_FILES_COUNT 0)
if (CMOCKA_LIBRARY)
  # Compile test suites
  message(STATUS "Cmocka support found")

  enable_testing()
  file(GLOB files ./tests/*.c)
  message(STATUS "Discovering tests...")
  foreach(file ${files})
    get_filename_component(file_name ${file} NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" file_loc ${file_name})
    add_executable(${file_loc} ${SRC_FILES} ${TEST_UTILS_SRC_FILES} ${file})
    target_link_libraries(${file_loc} cmocka)

    #target_compile_definitions(${file_loc} PRIVATE UNIT_TESTING)
    set_target_properties(
        ${file_loc}
        PROPERTIES
        COMPILE_DEFINITIONS UNIT_TESTING=1
    )

    add_test(NAME ${file_loc} COMMAND ${file_loc})
    MATH(EXPR TEST_FILES_COUNT "${TEST_FILES_COUNT}+1")
  endforeach()
  message(STATUS "Found ${TEST_FILES_COUNT} test/-s.")

else()
  message(WARNING "Cmocka not found. The tests won't be generated.")
  message(WARNING "Please install cmocka and provide needed headers and .a library to be linked.")
endif (CMOCKA_LIBRARY)



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
