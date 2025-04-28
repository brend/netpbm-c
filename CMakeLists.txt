cmake_minimum_required(VERSION 3.10)
project(mytool C)

# Set C standard (optional but good practice)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add the "src" directory
add_subdirectory(src)

# Optionally set build types
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Option to enable or disable building documentation
option(BUILD_DOC "Build documentation with Doxygen" ON)

find_package(Doxygen)

if (BUILD_DOC AND DOXYGEN_FOUND)
    # Set the Doxyfile path
    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)

    # Set output directory
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doc)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_IN}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
else()
    message(STATUS "Doxygen not found, 'doc' target will not be available.")
endif()