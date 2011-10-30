#ifndef PLATFORM_GL_H
#define PLATFORM_GL_H

#include "common/test.h"

#ifdef LINUX
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

TestError GlPlatformSetup();

#endif
