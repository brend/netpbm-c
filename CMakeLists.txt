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