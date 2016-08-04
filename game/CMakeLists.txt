cmake_minimum_required(VERSION 2.6)
project(game)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
    ${flare_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

file(GLOB_RECURSE CPP_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${CPP_FILES})

find_package (OpenGL REQUIRED)
if (OPENGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIRS})
    target_link_libraries (${PROJECT_NAME} ${OPENGL_LIBRARIES})
endif (OPENGL_FOUND)
find_package (GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    target_link_libraries (${PROJECT_NAME} ${GLEW_LIBRARIES})
endif (GLEW_FOUND)
find_package (PkgConfig REQUIRED)
pkg_search_module (GLFW REQUIRED glfw3)
if (GLFW_FOUND)
    include_directories(${GLFW_INCLUDE_DIRS})
    if(UNIX)
	target_link_libraries (${PROJECT_NAME} ${GLFW_LIBRARIES})
    endif(UNIX)
endif (GLFW_FOUND)

# find_package (Lua REQUIRED)
# if (LUA_FOUND)
#     include_directories(${LUA_INCLUDE_DIR})
#     target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES})
# endif (LUA_FOUND)

target_link_libraries(${PROJECT_NAME} flare fuse flux extra)
target_link_libraries(${PROJECT_NAME} zlibstatic)

if (NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	# target_link_libraries(${PROJECT_NAME} leakage)
	target_link_libraries(${PROJECT_NAME} imgui)
endif()
if(WIN32)
    target_link_libraries(${PROJECT_NAME} glew32 glfw3 opengl32 imm32)
endif(WIN32)

cotire(${PROJECT_NAME})
