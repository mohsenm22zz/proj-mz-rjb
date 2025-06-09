cmake_minimum_required(VERSION 3.29)
project(proj1)

set(CMAKE_CXX_STANDARD 20)

# The add_executable command should only list the .cpp source files that
# need to be compiled into the final program.
add_executable(proj1
        src/main.cpp
        src/Node.cpp
        src/Component.cpp
        src/Resistor.cpp
        src/Capacitor.cpp
        src/Inductor.cpp
        src/Diode.cpp
        src/VoltageSource.cpp
        src/CurrentSource.cpp
        src/Circuit.cpp
        src/LinearSolver.cpp
        src/Analysis.cpp
        src/CircuitIO.cpp
)

# This part correctly tells the compiler where to find the header files (#include <...>)
target_include_directories(proj1 PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)