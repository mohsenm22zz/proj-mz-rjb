cmake_minimum_required(VERSION 3.29)
project(proj1)

set(CMAKE_CXX_STANDARD 20)

add_executable(proj1
        src/main.cpp
        src/Node.cpp
        include/Component.h
        src/Component.cpp
        include/Resistor.h
        src/Resistor.cpp
        include/Capacitor.h
        src/Capacitor.cpp
        include/Inductor.h
        src/Inductor.cpp
        include/Diode.h
        src/Diode.cpp
        include/VoltageSource.h
        src/VoltageSource.cpp
        include/CurrentSource.h
        src/CurrentSource.cpp
        include/Circuit.h
        src/Circuit.cpp
        include/LinearSolver.h
        src/LinearSolver.cpp
        include/Analysis.h
        src/Analysis.cpp
        include/CircuitIO.h
        src/CircuitIO.cpp
        # ... other .cpp files will go here later
)

# 2. THEN specify properties for that target
target_include_directories(proj1 PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)