// stdafx.h
#ifndef _STDAFX_H_
#define _STDAFX_H_

#ifdef _WIN32
#pragma pack(1)
#pragma warning(disable:4996) // Using open/close/read... for file access
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include <GL/glu.h>
#include <al.h>
#include <alc.h>
#include <io.h>

#elif __APPLE__
#include "TargetConditionals.h"
#ifdef __MACH__ && TARGET_OS_MAC && !TARGET_OS_IPHONE
#include <unistd.h>
#include <GL/glfw.h>
#define GL_BGRA_EXT GL_BGRA
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

#elif __linux__
#include <unistd.h>
#include <GL/glfw.h>
//#define GL_BGRA_EXT GL_BGRA
#include <AL/al.h>
#include <AL/alc.h>
#endif

// Common includes

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#endif
// vim: set et sw=2 ts=2:
