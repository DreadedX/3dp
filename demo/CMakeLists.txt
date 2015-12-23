cmake_minimum_required(VERSION 2.6)
project(demo)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
    ${flare_INCLUDE_DIRS}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

file(GLOB CPP_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

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
    target_link_libraries (${PROJECT_NAME} ${GLFW_LIBRARIES})
endif (GLFW_FOUND)
find_package (ZLIB REQUIRED)
if (ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
    target_link_libraries (${PROJECT_NAME} ${ZLIB_LIBRARIES})
endif (ZLIB_FOUND)

target_link_libraries(${PROJECT_NAME} flare fuse flux extra)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_range_for)
