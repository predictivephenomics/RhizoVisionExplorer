cmake_minimum_required(VERSION 3.28)

# Define the project
project(RhizoVisionExplorer VERSION 2.0.3 DESCRIPTION "RhizoVision Explorer" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the preferred compilers in order of priority
if(NOT CMAKE_CXX_COMPILER)
    find_program(CLANG_COMPILER NAMES clang++ clang)
    find_program(GCC_COMPILER NAMES g++ gcc)
    find_program(MSVC_COMPILER NAMES cl)

    if(CLANG_COMPILER)
        set(CMAKE_CXX_COMPILER ${CLANG_COMPILER})
    elseif(GCC_COMPILER)
        set(CMAKE_CXX_COMPILER ${GCC_COMPILER})
    elseif(MSVC_COMPILER)
        set(CMAKE_CXX_COMPILER ${MSVC_COMPILER})
    else()
        message(FATAL_ERROR "No suitable compiler found")
    endif()
endif()

# Options to enable or disable AVX-512 and AVX2/FMA support
option(ENABLE_AVX512 "Enable AVX-512 support" OFF) # To be enabled on x86_64 server processors
option(ENABLE_AVX2_FMA "Enable AVX2 and FMA support" ON) # Default support for x86_64 desktop processors

# Detect the OS
if(WIN32)
    message(STATUS "Configuring for Windows")
elseif(UNIX)
    message(STATUS "Configuring for Unix/Linux")
else()
    message(FATAL_ERROR "Unsupported OS")
endif()

message(STATUS "Using ${CMAKE_CXX_COMPILER_ID} as the C++ compiler")
# Set global CMake build options
if(MSVC)
    # MSVC-specific flags
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd /Zi /Ob0 /Od /RTC1 /DDEBUG /D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG /INCREMENTAL")
    
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /Ox /DNDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF")
    
    # Enable AVX-512 and AVX2/FMA support
    if(ENABLE_AVX512)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX512")
    endif()
    if(ENABLE_AVX2_FMA)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2 /arch:AVX")
    endif()
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:precise /fp:strict")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG -D_DEBUG -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
    
    # Enable AVX-512 and AVX2/FMA support
    if(ENABLE_AVX512)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512")
    endif()
    if(ENABLE_AVX2_FMA)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma")
    endif()
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-fast-math -frounding-math")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG -D_DEBUG -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
    
    # Enable AVX-512 and AVX2/FMA support
    if(ENABLE_AVX512)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512")
    endif()
    if(ENABLE_AVX2_FMA)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma")
    endif()
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-fast-math -frounding-math")
else()
    message(FATAL_ERROR "Unsupported compiler")
endif()

# Find OpenCV
find_package(OpenCV REQUIRED)

# Find Qt6
find_package(Qt6 COMPONENTS Core Widgets Charts Gui OpenGL)

if(Qt6_found)
    qt_standard_project_setup()
    add_compile_definitions(BUILD_GUI)
else()
    message(WARNING "qt6 not found, building without GUI support.")
endif()

# Find cvutil
find_package(cvutil REQUIRED)

if(Qt6_FOUND)
    if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        find_package(X11 REQUIRED)
    endif()
endif()

get_cmake_property(_variableNames VARIABLES)
foreach (_variableName ${_variableNames})
    if (_variableName MATCHES "^cvutil_")
        message(STATUS "${_variableName}=${${_variableName}}")
    endif()
endforeach()

# Set the Debug postfix
set(CMAKE_DEBUG_POSTFIX "d")

# Enable automatic MOC and RCC for all projects
set(CMAKE_AUTOMOC ON)    # Enable automatic MOC
set(CMAKE_AUTORCC ON)    # Enable automatic RCC
set(CMAKE_AUTOUIC ON)    # Enable automatic UIC

# Define the source files for RoiManager
set(SOURCES
    RhizoVisionExplorer/feature_extraction.cpp
    RhizoVisionExplorer/rootsegmentprop.cpp
    RhizoVisionExplorer/roottopology.cpp
    RhizoVisionExplorer/resources.qrc
)

# Define the header files
set(HEADERS
    RhizoVisionExplorer/feature_extraction.h
    RhizoVisionExplorer/feature_config.h
    RhizoVisionExplorer/common_types.h
    RhizoVisionExplorer/resource.h
    RhizoVisionExplorer/rootsegmentprop.h
    RhizoVisionExplorer/roottopology.h
)

# Include the resource file only if it's WIN32 and MSVC
if(WIN32)
    list(APPEND SOURCES RhizoVisionExplorer/RhizoVisionExplorer.rc)
endif()

if(Qt6_found)
    list(APPEND SOURCES RhizoVisionExplorer/MainUI.cpp)
    list(APPEND HEADERS RhizoVisionExplorer/demo.h RhizoVisionExplorer/MainUI.h)
endif()

if(Qt6_found)
    # Define the target as an executable.
    add_executable(RhizoVisionExplorer WIN32
        RhizoVisionExplorer/main.cpp
        ${SOURCES}
        ${HEADERS})
    set_target_properties(RhizoVisionExplorer PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    # Add the generated resource .cpp file to the target
    target_sources(RhizoVisionExplorer PRIVATE ${RESOURCES})

    # Include directories (Qt and OpenCV paths are already handled at the solution level)
    target_include_directories(RhizoVisionExplorer
        PRIVATE
            #${CMAKE_BINARY_DIR}/library/include
            ${OpenCV_INCLUDE_DIRS}
    )
    # Link libraries (cvutil depends on PluginManager and RoiManager)
    target_link_libraries(RhizoVisionExplorer
        PRIVATE
            ${OpenCV_LIBS}
            Qt6::Core
            Qt6::Widgets
            Qt6::Charts
            Qt6::Gui
            Qt6::OpenGL
            cvutil::cvutil
            cvutil::PluginManager
            cvutil::RoiManager
    )
endif()

add_executable(rv
    RhizoVisionExplorer/rv.cpp
    RhizoVisionExplorer/indicators/indicators.hpp
    ${SOURCES}
    ${HEADERS})


set_target_properties(rv PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

target_include_directories(rv
    PRIVATE
        #${CMAKE_BINARY_DIR}/library/include
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(rv
    PRIVATE
        ${OpenCV_LIBS}
        cvutil::cvutil
)

set(CPACK_PROPERTIES_FILE "${CMAKE_BINARY_DIR}/RhizoVisionCPackProperties-$<CONFIG>.cmake")
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/RhizoVisionCPackProperties-$<CONFIG>.cmake" #allows use of generator expression to retrieve CONFIG
        CONTENT "set(CPACK_BUILD_TYPE \"$<CONFIG>\")\n")

# Include code for installing the targets
include(CMake/cvutilInstallTargets.cmake)

# Include the CPack module
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    include(CMake/PackageWindows.cmake)
else(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Check if we are using a conda env or not
    if (DEFINED ENV{CONDA_PREFIX})
        message(STATUS "Using conda environment: $ENV{CONDA_PREFIX} not generating a Debian package.")
        # Do not package if inside a conda environment
    else()
        message(STATUS "Not using conda environment, use CPack to create a Debian package.")
        include(CMake/PackageLinux.cmake)
    endif()
endif()
