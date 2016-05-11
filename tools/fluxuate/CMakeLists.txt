cmake_minimum_required(VERSION 2.6)
project(fluxuate)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
    ${leakage_INCLUDE_DIRS}
    ${extra_INCLUDE_DIRS}
    ${flux_INCLUDE_DIRS}
    ${flare_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

file(GLOB_RECURSE CPP_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${CPP_FILES})

# set (EXTRA_FLAGS "${EXTRA_FLAGS} -O3")
set (EXTRA_FLAGS "${EXTRA_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${EXTRA_FLAGS}")

# NOTE: zlib is included in libpng
find_package (PNG REQUIRED)
if (PNG_FOUND)
    include_directories(${PNG_INCLUDE_DIRS})
    target_link_libraries (${PROJECT_NAME} ${PNG_LIBRARIES})
endif (PNG_FOUND)

target_link_libraries(${PROJECT_NAME} flux extra ${CMAKE_DL_LIBS})
if (NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	target_link_libraries(${PROJECT_NAME} leakage)
endif()
target_compile_features(${PROJECT_NAME} PRIVATE cxx_range_for)

cotire(${PROJECT_NAME})
