cmake_minimum_required(VERSION 2.6)
project(fluxuate)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

file(GLOB CPP_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${CPP_FILES})

# NOTE: zlib is included in libpng
find_package (PNG REQUIRED)
if (PNG_FOUND)
    include_directories(${PNG_INCLUDE_DIRS})
    target_link_libraries (${PROJECT_NAME} ${PNG_LIBRARIES})
endif (PNG_FOUND)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_range_for)
