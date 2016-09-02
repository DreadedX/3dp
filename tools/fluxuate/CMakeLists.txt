cmake_minimum_required(VERSION 2.6)
project(fluxuate)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
    ${extra_INCLUDE_DIRS}
    ${flux_INCLUDE_DIRS}
    ${flare_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

# set(EXTRA_FLAGS "${EXTRA_FLAGS} -O3")
# set(CMAKE_CXX_FLAGS "${EXTRA_FLAGS}")
# set(CMAKE_CXX_FLAGS_DEBUG "${EXTRA_FLAGS}")

file(GLOB_RECURSE CPP_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${CPP_FILES})

target_link_libraries(${PROJECT_NAME} flux extra)
target_link_libraries(${PROJECT_NAME} zlibstatic)
target_link_libraries(${PROJECT_NAME} xxhashstatic)
target_link_libraries(${PROJECT_NAME} stdc++fs)
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
