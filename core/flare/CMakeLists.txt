cmake_minimum_required(VERSION 2.6)
project(flare)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
    ${leakage_INCLUDE_DIRS}
    ${imgui_INCLUDE_DIRS}
    ${glm_INCLUDE_DIRS}
    ${extra_INCLUDE_DIRS}
    ${flux_INCLUDE_DIRS}
    ${fuse_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

file(GLOB_RECURSE CPP_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# NOTE: This only work when compiling on linux
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)
add_custom_command(
    PRE_BUILD
    COMMAND ./tools/scripts/generate_info.sh ${CMAKE_CURRENT_BINARY_DIR}/generated/info.cpp
    COMMAND echo "Generating info"
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/generated/info.cpp
    DEPENDS ${CPP_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

set(CPP_FILES ${CPP_FILES} ${CMAKE_CURRENT_BINARY_DIR}/generated/info.cpp)
add_library(${PROJECT_NAME} STATIC ${CPP_FILES})

cotire(${PROJECT_NAME})
