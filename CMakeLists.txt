cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Prevent dereference variable in if statement
cmake_policy(SET CMP0054 NEW)
set(ProjectName BoostLogWrap)

# Force C++14 in eclise CDT
if(${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")
    set(CMAKE_CXX_COMPILER_ARG1 "-std=c++17" CACHE STRING "C++ version for eclipse" FORCE)
    set(CMAKE_ECLIPSE_VERSION "4.6" CACHE STRING "Eclipse version" FORCE)
endif()
# Force cmake generate C++17 compiler flag instead of -std=gnu++11 in Linux.
set(CMAKE_CXX_STANDARD 17)

# Colorful output for ninja
string(TOLOWER ${CMAKE_GENERATOR} CMakeGenLower)
if (CMakeGenLower STREQUAL "ninja")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=always")
endif()

project(${ProjectName})

# Set binary output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

set(Boost_USE_STATIC_LIBS ON CACHE BOOL "Use boost static library")
set(Boost_USE_STATIC_RUNTIME ON CACHE BOOL "Use boost static runtime")
find_package(Boost REQUIRED COMPONENTS filesystem timer log log_setup system locale)
if (Boost_FOUND)
    message(STATUS "Found Boost, version: ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
    if (Boost_USE_STATIC_LIBS)
        add_definitions(-DBOOST_ALL_NO_LIB)
    endif()
endif()

## Set dependencies
find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()   


# Setup sources
set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-*)
include_directories(src)
# Source files
file(GLOB_RECURSE Sources RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" src/*.h src/*.hpp src/*.cpp)
foreach(src IN LISTS Sources)
    get_filename_component(sourcePath "${src}" PATH)
    string(REPLACE "/" "\\" sourcePathMsvc "${sourcePath}")
    source_group("Sources\\${sourcePathMsvc}" FILES "${src}")
endforeach()

add_executable(${ProjectName} ${Sources} ${Resources})
target_link_libraries(${ProjectName} ${Boost_LIBRARIES})

