# WarpWebServer - HTTP Server using Boost.Asio
project(WarpWebServer VERSION 1.0.0)

# Source files
set(SOURCES
    pch.cpp
    warp_web_server.cpp
    connection.cpp
    connection_manager.cpp
    mime_types.cpp
    reply.cpp
    request_handler.cpp
    request_parser.cpp
    server.cpp
)

# Header files
set(HEADERS
    connection.hpp
    connection_manager.hpp
    header.hpp
    mime_types.hpp
    pch.h
    reply.hpp
    request.hpp
    request_handler.hpp
    request_parser.hpp
    server.hpp
)

# Create executable
add_executable(WarpWebServer ${SOURCES} ${HEADERS})

# Set precompiled header
if(MSVC)
    target_precompile_headers(WarpWebServer PRIVATE pch.h)
endif()

# Include directories
target_include_directories(WarpWebServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/WarpTpsLib
    ${CMAKE_SOURCE_DIR}/packages/libpng.1.6.28.1/build/native/include
    ${CMAKE_SOURCE_DIR}/packages/zlib.v120.windesktop.msvcstl.dyn.rt-dyn.1.2.8.8/build/native/include
)

# Link dependencies
target_link_libraries(WarpWebServer PRIVATE
    WarpTpsLib
    Boost::system
    Boost::date_time
    Boost::regex
)

# Find and link PNG and ZLIB if available
find_package(PNG QUIET)
find_package(ZLIB QUIET)

if(PNG_FOUND)
    target_link_libraries(WarpWebServer PRIVATE PNG::PNG)
endif()

if(ZLIB_FOUND)
    target_link_libraries(WarpWebServer PRIVATE ZLIB::ZLIB)
endif()

# Compiler-specific settings
if(MSVC)
    target_compile_definitions(WarpWebServer PRIVATE
        WIN32
        _CONSOLE
        _CRT_SECURE_NO_WARNINGS
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
    )

    # Set conformance mode settings to match vcxproj
    target_compile_options(WarpWebServer PRIVATE
        /W3
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Debug>:/permissive>  # Disable conformance for Debug
        $<$<CONFIG:Release>:/permissive>  # Disable conformance for Release
    )

    # Set character set to MultiByte for Debug Win32
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)  # 32-bit
        target_compile_definitions(WarpWebServer PRIVATE
            $<$<CONFIG:Debug>:_MBCS>
        )
    endif()
endif()

# Set target properties
set_target_properties(WarpWebServer PROPERTIES
    OUTPUT_NAME "WarpWebServer"
)

# Install rules
install(TARGETS WarpWebServer
    RUNTIME DESTINATION bin
)
