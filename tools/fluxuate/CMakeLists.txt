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

target_link_libraries(${PROJECT_NAME} flux extra)
target_link_libraries(${PROJECT_NAME} zlibstatic)
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})

if (NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	target_link_libraries(${PROJECT_NAME} leakage)
endif()

cotire(${PROJECT_NAME})
