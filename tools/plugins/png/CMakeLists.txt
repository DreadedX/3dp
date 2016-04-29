cmake_minimum_required(VERSION 2.6)
project(plugin_png)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}
    ${leakage_INCLUDE_DIRS}
    ${extra_INCLUDE_DIRS}
    ${flux_INCLUDE_DIRS}
    ${flare_INCLUDE_DIRS}
    ${fluxuate_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

file(GLOB_RECURSE CPP_FILES ${PROJECT_SOURCE_DIR}/*.cpp)

add_library(${PROJECT_NAME} SHARED ${CPP_FILES})

set(EXTRA_FLAGS "${EXTRA_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${EXTRA_FLAGS}")

# NOTE: zlib is included in libpng
find_package (PNG REQUIRED)
if (PNG_FOUND)
    include_directories(${PNG_INCLUDE_DIRS})
    target_link_libraries (${PROJECT_NAME} ${PNG_LIBRARIES})
endif (PNG_FOUND)

target_link_libraries(${PROJECT_NAME} flux extra leakage)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_range_for)
