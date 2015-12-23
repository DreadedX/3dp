cmake_minimum_required(VERSION 2.6)
project(all)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
 
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
 
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

include_directories(${PROJECT_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR})

if (WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

add_subdirectory(fluxuate)
add_subdirectory(extra)
add_subdirectory(flux)
add_subdirectory(fuse)
add_subdirectory(flare)
add_subdirectory(demo)

add_custom_target(assets
    COMMAND ./fluxuate ../demo/assets
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_target(run
    COMMAND ./demo
    DEPENDS assets
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_target(debug
    COMMAND gdb ./demo -ex run -ex bt -ex quit --silent
    DEPENDS assets
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

if(UNIX)
    add_custom_target(distclean @echo cleaning for source distribution)
    set(DISTCLEANED
	build
	cmake.depends
	cmake.check_depends
	CMakeCache.txt
	cmake.check_cache
	*.cmake
	Makefile
	core core.*
	gmon.out
	*~
	)

    add_custom_command(
	DEPENDS clean
	COMMENT "distribution clean"
	COMMAND rm
	ARGS    -Rf CMakeTmp ${DISTCLEANED}
	TARGET  distclean
	)
endif(UNIX)
