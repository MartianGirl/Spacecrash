cmake_minimum_required(VERSION 2.6)
project(Spacecrash)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED libglfw)
pkg_search_module(GL REQUIRED gl)
pkg_search_module(AL REQUIRED openal)
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${GL_INCLUDE_DIRS})
include_directories(${AL_INCLUDE_DIRS})

add_executable(Spacecrash game0.cc sys_osx.cc stdafx.cc sys.h stdafx.h)
target_link_libraries(Spacecrash 
    ${GLFW_LIBRARIES}
    ${GL_LIBRARIES}
    ${AL_LIBRARIES})
install(
    TARGETS Spacecrash
    RUNTIME
    DESTINATION ${PROJECT_INSTALL_DIR})
