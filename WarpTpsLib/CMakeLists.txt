# WarpTpsLib - Core TPS Transform Library
project(WarpTpsLib VERSION 1.0.0)

# Source files
set(SOURCES
    ModelObject.cpp
    pch.cpp
    TPSTransform.cpp
)

# Header files
set(HEADERS
    framework.h
    MathUtil.h
    ModelObject.h
    pch.h
    Resource.h
    targetver.h
    TPSTransform.h
    UtilMacros.h
    VectorBase.h
    VectorD.h
)

# Resource files
set(RESOURCES
    WarpTpsLib.rc
    res/WarpTpsLib.rc2
)

# Create library (static by default, shared if BUILD_SHARED_LIBS is ON)
if(BUILD_SHARED_LIBS)
    add_library(WarpTpsLib SHARED ${SOURCES} ${HEADERS} ${RESOURCES})

    # Add DLL export definitions
    target_compile_definitions(WarpTpsLib PRIVATE
        WARPTPSLIB_EXPORTS
        _USRDLL
    )

    # Use module definition file for exports
    if(MSVC)
        set_target_properties(WarpTpsLib PROPERTIES
            LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/WarpTpsLib.def"
        )
    endif()
else()
    add_library(WarpTpsLib STATIC ${SOURCES} ${HEADERS})
endif()

# Set precompiled header
if(MSVC)
    target_precompile_headers(WarpTpsLib PRIVATE pch.h)
endif()

# Include directories
target_include_directories(WarpTpsLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)

# Link Boost
target_link_libraries(WarpTpsLib
    PUBLIC
        Boost::headers
)

# Compiler-specific settings
if(MSVC)
    target_compile_definitions(WarpTpsLib PRIVATE
        WIN32
        _WINDOWS
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
    )

    # Set conformance mode to false to match vcxproj
    target_compile_options(WarpTpsLib PRIVATE
        /W3
        $<$<CONFIG:Release>:/O2>
        /permissive  # Disable conformance mode
    )
endif()

# Set target properties
set_target_properties(WarpTpsLib PROPERTIES
    OUTPUT_NAME "WarpTpsLib"
    VERSION ${PROJECT_VERSION}
)

# Install rules
install(TARGETS WarpTpsLib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${HEADERS}
    DESTINATION include/WarpTpsLib
)

# ============================================================================
# Python bindings with pybind11
# ============================================================================
option(BUILD_PYTHON_BINDINGS "Build Python bindings with pybind11" OFF)

if(BUILD_PYTHON_BINDINGS)
    message(STATUS "Building Python bindings")

    # Find Python
    find_package(Python COMPONENTS Interpreter Development REQUIRED)
    message(STATUS "Python version: ${Python_VERSION}")
    message(STATUS "Python include dirs: ${Python_INCLUDE_DIRS}")

    # Find pybind11
    find_package(pybind11 CONFIG REQUIRED)
    message(STATUS "pybind11 version: ${pybind11_VERSION}")

    # Create Python bindings module
    pybind11_add_module(_warptps_core
        python/bindings.cpp
    )

    # Link against WarpTpsLib
    target_link_libraries(_warptps_core PRIVATE WarpTpsLib)

    # Set output name and properties
    set_target_properties(_warptps_core PROPERTIES
        OUTPUT_NAME "_warptps_core"
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN ON
    )

    # Install the Python module
    install(TARGETS _warptps_core
        LIBRARY DESTINATION warptps
        RUNTIME DESTINATION warptps
    )
endif()
