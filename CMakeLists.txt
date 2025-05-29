cmake_minimum_required(VERSION 3.29)
project(proj1)

set(CMAKE_CXX_STANDARD 20)

# 1. Define the executable first
add_executable(proj1 src/main.cpp) # <--- This line defines the target "proj1"

# 2. THEN specify properties for that target
target_include_directories(proj1 PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)