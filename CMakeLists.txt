cmake_minimum_required(VERSION 3.29)
project(CircuitSimulator)

set(CMAKE_CXX_STANDARD 20)

add_library(CircuitSimulator SHARED
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

target_include_directories(CircuitSimulator PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

# Add this line to define the export symbol when building the DLL
target_compile_definitions(CircuitSimulator PRIVATE CIRCUITSIMULATOR_EXPORTS)
