cmake_minimum_required(VERSION 2.6)
project(all)

# Make sure there is no in source build
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()

set(EXTA_FLAGS "-Wall -Wextra")

# If compiling for windows statically link everything
if (${CMAKE_SYSTEM_NAME} MATCHES Window)
    set(EXTA_FLAGS "${EXTA_FLAGS} -static")
endif()
message("System name: ${CMAKE_SYSTEM_NAME}")

# NOTE: Build flags by default are set to the debug flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} ${EXTA_FLAGS}")
# message("Default flags: ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${EXTRA_FLAGS}")
# message("Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${EXTRA_FLAGS}")
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

add_subdirectory(libs/leakage)
add_subdirectory(libs/imgui)

add_subdirectory(core/extra)
add_subdirectory(core/flux)
add_subdirectory(core/fuse)
add_subdirectory(core/flare)

add_subdirectory(fluxuate)
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
