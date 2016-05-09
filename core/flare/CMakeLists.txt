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

add_library(${PROJECT_NAME} STATIC ${CPP_FILES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_range_for)

cotire(${PROJECT_NAME})
