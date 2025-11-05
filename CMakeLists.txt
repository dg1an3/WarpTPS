cmake_minimum_required(VERSION 3.15)

project(WarpTPS VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add cmake modules directory to module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Platform-specific settings
if(WIN32)
    # Use dynamic runtime for MFC applications
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # Enable MFC support
    option(USE_MFC "Use MFC for GUI applications" ON)

    # Add Windows-specific definitions
    add_compile_definitions(
        _WINDOWS
        _CRT_SECURE_NO_WARNINGS
        _UNICODE
        UNICODE
    )
endif()

# Find Boost - try standard CMake find first, then NuGet
find_package(Boost 1.70 QUIET COMPONENTS system date_time regex)

if(NOT Boost_FOUND)
    message(STATUS "Standard Boost not found, trying NuGet packages...")
    find_package(BoostNuGet REQUIRED COMPONENTS system date_time regex)
endif()

# Find OpenCV (optional, for FeatureExtractionConsole)
find_package(OpenCV QUIET)

# Add subdirectories
add_subdirectory(WarpTpsLib)
add_subdirectory(WarpWebServer)
add_subdirectory(FeatureExtractionConsole)
add_subdirectory(src)  # WarpTPS MFC application
add_subdirectory(UnitTest1)  # Unit tests

# Print configuration summary
message(STATUS "========================================")
message(STATUS "WarpTPS Configuration Summary")
message(STATUS "========================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Boost version: ${Boost_VERSION}")
if(OpenCV_FOUND)
    message(STATUS "OpenCV version: ${OpenCV_VERSION}")
else()
    message(STATUS "OpenCV: Not found (FeatureExtractionConsole will be disabled)")
endif()
message(STATUS "========================================")
