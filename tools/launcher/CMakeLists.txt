cmake_minimum_required(VERSION 2.6)
project(launcher)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
    ${leakage_INCLUDE_DIRS}
    ${extra_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

file(GLOB_RECURSE CPP_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${CPP_FILES})

set(CURSES_NEEDS_NCURSES TRUE)
find_package (Curses REQUIRED)
if (CURSES_FOUND)
	include_directories(${CURSES_INCLUDE_DIRS})
	target_link_libraries (${PROJECT_NAME} ${CURSES_LIBRARIES})
endif (CURSES_FOUND)

target_link_libraries(${PROJECT_NAME} flux extra leakage)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_range_for)
