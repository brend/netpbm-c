# Collect all .c files in this directory
file(GLOB SOURCES *.c)

# Add the include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Create the executable
add_executable(pbm ${SOURCES})