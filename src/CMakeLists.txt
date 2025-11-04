# WarpTPS - Main MFC Application
project(WarpTPS VERSION 1.0.0)

# Check if MFC is available (Windows only)
if(NOT WIN32)
    message(WARNING "WarpTPS MFC application requires Windows - skipping")
    return()
endif()

if(NOT USE_MFC)
    message(WARNING "WarpTPS MFC application requires USE_MFC=ON - skipping")
    return()
endif()

# Source files
set(SOURCES
    Dib.cpp
    DibView.cpp
    MainFrm.cpp
    MorphSlider.cpp
    StdAfx.cpp
    WarpTPS.cpp
    WarpTPSDoc.cpp
    WarpTPSView.cpp
)

# Header files
set(HEADERS
    Dib.h
    DibView.h
    MainFrm.h
    MorphSlider.h
    Resource.h
    StdAfx.h
    WarpTPS.h
    WarpTPSDoc.h
    WarpTPSView.h
)

# Resource files
set(RESOURCES
    WarpTPS.rc
    res/Toolbar.bmp
    res/WarpTPS.ico
    res/WarpTPS.rc2
    res/WarpTPSDoc.ico
)

# Create executable
add_executable(WarpTPS WIN32 ${SOURCES} ${HEADERS} ${RESOURCES})

# Enable MFC
set_target_properties(WarpTPS PROPERTIES
    MFC_FLAG 2  # 1 = static MFC, 2 = shared MFC
)

# Set precompiled header
if(MSVC)
    target_precompile_headers(WarpTPS PRIVATE StdAfx.h)
endif()

# Include directories
target_include_directories(WarpTPS PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/WarpTpsLib
)

# Link dependencies
target_link_libraries(WarpTPS PRIVATE
    WarpTpsLib
    Boost::headers
)

# Compiler-specific settings
if(MSVC)
    target_compile_definitions(WarpTPS PRIVATE
        WIN32
        _WINDOWS
        _AFXDLL
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
    )

    target_compile_options(WarpTPS PRIVATE
        /W3
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/Ob1>  # Inline expansion
    )

    # Set subsystem to Windows with MFC entry point
    set_target_properties(WarpTPS PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup"
    )
endif()

# Install rules
install(TARGETS WarpTPS
    RUNTIME DESTINATION bin
)

message(STATUS "WarpTPS MFC application will be built")
