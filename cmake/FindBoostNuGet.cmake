# FindBoostNuGet.cmake
# Custom module to find Boost from NuGet packages

set(BOOST_NUGET_ROOT "${CMAKE_SOURCE_DIR}/packages/boost.1.87.0")
set(BOOST_NUGET_INCLUDE "${BOOST_NUGET_ROOT}/lib/native/include")

if(EXISTS "${BOOST_NUGET_INCLUDE}")
    # Set Boost variables
    set(Boost_FOUND TRUE)
    set(Boost_INCLUDE_DIRS "${BOOST_NUGET_INCLUDE}")
    set(Boost_VERSION "1.87.0")
    set(Boost_VERSION_STRING "1.87.0")
    set(Boost_MAJOR_VERSION 1)
    set(Boost_MINOR_VERSION 87)
    set(Boost_SUBMINOR_VERSION 0)

    # Create interface library for Boost headers
    if(NOT TARGET Boost::headers)
        add_library(Boost::headers INTERFACE IMPORTED)
        set_target_properties(Boost::headers PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${BOOST_NUGET_INCLUDE}"
        )
    endif()

    if(NOT TARGET Boost::boost)
        add_library(Boost::boost INTERFACE IMPORTED)
        set_target_properties(Boost::boost PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${BOOST_NUGET_INCLUDE}"
        )
    endif()

    # Helper function to create Boost component targets
    function(add_boost_component COMPONENT)
        set(COMPONENT_PKG_DIR "${CMAKE_SOURCE_DIR}/packages/boost_${COMPONENT}-vc142.1.87.0/lib/native")

        if(EXISTS "${COMPONENT_PKG_DIR}")
            # Determine library names based on configuration
            if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                set(ARCH_SUFFIX "x64")
            else()
                set(ARCH_SUFFIX "x32")
            endif()

            # Create imported library target
            if(NOT TARGET Boost::${COMPONENT})
                add_library(Boost::${COMPONENT} UNKNOWN IMPORTED)
                set_target_properties(Boost::${COMPONENT} PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${BOOST_NUGET_INCLUDE}"
                )

                # Set library locations for different configurations
                # Debug configuration
                set(DEBUG_LIB "${COMPONENT_PKG_DIR}/libboost_${COMPONENT}-vc142-mt-gd-${ARCH_SUFFIX}-1_87.lib")
                if(EXISTS "${DEBUG_LIB}")
                    set_target_properties(Boost::${COMPONENT} PROPERTIES
                        IMPORTED_LOCATION_DEBUG "${DEBUG_LIB}"
                    )
                endif()

                # Release configuration
                set(RELEASE_LIB "${COMPONENT_PKG_DIR}/libboost_${COMPONENT}-vc142-mt-s-${ARCH_SUFFIX}-1_87.lib")
                if(EXISTS "${RELEASE_LIB}")
                    set_target_properties(Boost::${COMPONENT} PROPERTIES
                        IMPORTED_LOCATION_RELEASE "${RELEASE_LIB}"
                        IMPORTED_LOCATION_RELWITHDEBINFO "${RELEASE_LIB}"
                        IMPORTED_LOCATION_MINSIZEREL "${RELEASE_LIB}"
                    )
                endif()
            endif()

            set(Boost_${COMPONENT}_FOUND TRUE PARENT_SCOPE)
        else()
            message(WARNING "Boost component ${COMPONENT} not found in NuGet packages")
            set(Boost_${COMPONENT}_FOUND FALSE PARENT_SCOPE)
        endif()
    endfunction()

    # Header-only components (no separate library needed)
    set(HEADER_ONLY_COMPONENTS system)

    # Add requested components
    foreach(component ${BoostNuGet_FIND_COMPONENTS})
        if(component IN_LIST HEADER_ONLY_COMPONENTS)
            # Create alias for header-only component
            if(NOT TARGET Boost::${component})
                add_library(Boost::${component} INTERFACE IMPORTED)
                set_target_properties(Boost::${component} PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${BOOST_NUGET_INCLUDE}"
                )
            endif()
            set(Boost_${component}_FOUND TRUE)
            message(STATUS "  Found Boost component: ${component} (header-only)")
        else()
            add_boost_component(${component})
            if(Boost_${component}_FOUND)
                message(STATUS "  Found Boost component: ${component}")
            endif()
        endif()
    endforeach()

    message(STATUS "Found Boost (NuGet): ${Boost_VERSION} (${BOOST_NUGET_INCLUDE})")
else()
    set(Boost_FOUND FALSE)
    message(FATAL_ERROR "Boost NuGet package not found at ${BOOST_NUGET_ROOT}")
endif()
