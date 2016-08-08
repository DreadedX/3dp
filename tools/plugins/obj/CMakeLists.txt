cmake_minimum_required(VERSION 2.6)
project(plugin_obj)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
    ${flare_INCLUDE_DIRS}
    ${fluxuate_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

# set(EXTRA_FLAGS "${EXTRA_FLAGS} -O3")
# set(CMAKE_CXX_FLAGS "${EXTRA_FLAGS}")
# set(CMAKE_CXX_FLAGS_DEBUG "${EXTRA_FLAGS}")

file(GLOB_RECURSE CPP_FILES ${PROJECT_SOURCE_DIR}/*.cpp)

add_definitions(-DNO_LEAKAGE)

add_library(${PROJECT_NAME} SHARED ${CPP_FILES})

target_link_libraries(${PROJECT_NAME} extra tinyobjloader)
