cmake_minimum_required(VERSION 2.6)
project(all)

# NOTE: Build flags by default are set to the debug flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra")
# message("Default flags: ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra")
# message("Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra")
# message("Release flags: ${CMAKE_CXX_FLAGS_RELEASE}")

message("Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
 
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
 
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

include_directories(${PROJECT_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR})

if (WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

add_subdirectory(leakage)
add_subdirectory(fluxuate)

add_subdirectory(extra)
add_subdirectory(flux)
add_subdirectory(fuse)
add_subdirectory(imgui)
add_subdirectory(flare)

add_subdirectory(game)

add_custom_target(archive
    COMMAND tar -zcvf ../release.tar.gz game base.flx
    DEPENDS assets
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_target(assets
    COMMAND ./fluxuate ../game/assets
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_target(run
    COMMAND ./game
    DEPENDS assets
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_target(debug
    COMMAND gdb ./game -ex run -ex bt -ex quit --silent
    DEPENDS assets
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

if(UNIX)
    add_custom_target(distclean @echo cleaning for source distribution)
    set(DISTCLEANED
	build
	release.tar.gz
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
