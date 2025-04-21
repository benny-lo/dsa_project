cmake_minimum_required(VERSION 3.10)
project(dsa_project C)

# Set C standard to C11 and enable VLA
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)  # Use strict C11

# Add compiler flags for VLA support and sanitizers
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-std=c11 -g -O0 -fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

# Add source files
set(SOURCES
    src/main.c
    src/rax.c
    src/utils.c
    src/constants.c
    src/memory_pool.c
)

# Add header files
set(HEADERS
    src/rax.h
    src/utils.h
    src/constants.h
    src/memory_pool.h
)

# Create main executable
add_executable(dsa_project ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(dsa_project PRIVATE src)

# Set output directory
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin) 