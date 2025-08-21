#pragma once

// This file defines the CIRCUITSIMULATOR_API macro.
// This macro is used to control the export and import of symbols (functions, classes, etc.)
// from a Dynamic Link Library (DLL) on Windows systems.

// Check if the compiler is targeting a Windows platform.
#ifdef _WIN32

// Check if we are building the DLL itself.
// This preprocessor symbol (CIRCUITSIMULATOR_EXPORTS) should be defined in the
// build settings of the DLL project (e.g., in CMakeLists.txt or project properties).
#ifdef CIRCUITSIMULATOR_EXPORTS
// If we are building the DLL, mark the symbols for export.
        // __declspec(dllexport) tells the compiler to export the function or class
        // so it can be used by other applications that link against this DLL.
        #define CIRCUITSIMULATOR_API __declspec(dllexport)
#else
// If we are not building the DLL (i.e., a client application is including this header),
// mark the symbols for import.
// __declspec(dllimport) tells the compiler that the function or class
// is defined in an external DLL.
#define CIRCUITSIMULATOR_API __declspec(dllimport)
#endif

#else

// If the platform is not Windows (e.g., Linux, macOS), DLL export/import
    // is handled differently or not needed in this way. Define the macro as empty
    // so that the code can compile without changes on these platforms.
    #define CIRCUITSIMULATOR_API

#endif
