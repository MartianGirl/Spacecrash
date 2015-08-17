cmake_minimum_required(VERSION 2.6)
project(Spacecrash)

if(APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED libglfw)
    pkg_search_module(GL REQUIRED gl)
    pkg_search_module(AL REQUIRED openal)
    add_executable(Spacecrash game0.cc sys_osx.cc stdafx.cc sys.h stdafx.h)
    target_link_libraries(Spacecrash 
            ${GLFW_LIBRARIES}
            ${GL_LIBRARIES}
            ${AL_LIBRARIES})
elseif(WIN32)
    find_path(AL_INCLUDE_DIRS al.h
            PATH_SUFFIXES include/AL include/OpenAL include
            PATHS
            "$ENV{PROGRAMFILES(X86)}/OpenAL 1.1 SDK"
            )

    find_library(AL_LIBRARIES
            NAMES OpenAL al openal OpenAL32
            PATH_SUFFIXES lib64 lib libs64 libs libs/Win32 libs/Win64
            PATHS
            "$ENV{PROGRAMFILES(X86)}/OpenAL 1.1 SDK"
            )

    set (OPENGL_gl_LIBRARY opengl32 CACHE STRING "OpenGL library for win32")
    set (OPENGL_glu_LIBRARY glu32 CACHE STRING "GLU library for win32")
    set (GL_LIBRARIES ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
    add_executable(Spacecrash game0.cc sys_win.cc stdafx.cc sys.h stdafx.h)
    target_link_libraries(Spacecrash
            ${GL_LIBRARIES}
            ${AL_LIBRARIES})
    set_target_properties(Spacecrash PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
    set_target_properties(Spacecrash PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
    set_target_properties(Spacecrash PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    set_target_properties(Spacecrash PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
endif()

# This is for all platforms
install(TARGETS Spacecrash
        RUNTIME
        DESTINATION ${PROJECT_INSTALL_DIR})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${GL_INCLUDE_DIRS})
include_directories(${AL_INCLUDE_DIRS})

