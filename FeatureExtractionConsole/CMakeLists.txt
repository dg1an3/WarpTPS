# FeatureExtractionConsole - OpenCV-based feature extraction tool
project(FeatureExtractionConsole VERSION 1.0.0)

# Only build if OpenCV is found
if(NOT OpenCV_FOUND)
    message(STATUS "OpenCV not found - FeatureExtractionConsole will be skipped")
    return()
endif()

# Source files
set(SOURCES
    FeatureExtractionConsole.cpp
)

# Create executable
add_executable(FeatureExtractionConsole ${SOURCES})

# Include directories
target_include_directories(FeatureExtractionConsole PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

# Link OpenCV
target_link_libraries(FeatureExtractionConsole PRIVATE
    ${OpenCV_LIBS}
)

# Compiler-specific settings
if(MSVC)
    target_compile_definitions(FeatureExtractionConsole PRIVATE
        WIN32
        _CONSOLE
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
    )

    target_compile_options(FeatureExtractionConsole PRIVATE
        /W3
        $<$<CONFIG:Release>:/O2>
    )
endif()

# Set target properties
set_target_properties(FeatureExtractionConsole PROPERTIES
    OUTPUT_NAME "FeatureExtractionConsole"
)

# Install rules
install(TARGETS FeatureExtractionConsole
    RUNTIME DESTINATION bin
)

message(STATUS "FeatureExtractionConsole will be built with OpenCV ${OpenCV_VERSION}")
