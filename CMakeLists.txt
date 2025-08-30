cmake_minimum_required(VERSION 3.10)
project(CircuitSimulator)

set(CMAKE_CXX_STANDARD 17)

# Create a shared library (DLL on Windows)
add_library(CircuitSimulator SHARED
    src/ACVoltageSource.cpp
    src/Analysis.cpp
    src/Capacitor.cpp
    src/Circuit.cpp
    src/CircuitIO.cpp
    src/Component.cpp
    src/CurrentSource.cpp
    src/Diode.cpp
    src/Inductor.cpp
    src/LinearSolver.cpp
    src/Node.cpp
    src/Resistor.cpp
    src/VoltageSource.cpp
    src/CircuitSimulatorInterface.cpp
)

# Define the export symbol for the DLL
target_compile_definitions(CircuitSimulator PRIVATE CIRCUITSIMULATOR_EXPORTS)

# Include directories
target_include_directories(CircuitSimulator PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# For Windows, link required libraries
if(WIN32)
    target_link_libraries(CircuitSimulator)
endif()