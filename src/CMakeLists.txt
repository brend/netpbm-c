# Collect all .c files in this directory
file(GLOB SOURCES *.c)

# Create the executable
add_executable(pbm ${SOURCES})