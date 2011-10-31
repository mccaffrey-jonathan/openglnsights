#ifndef COMMON_GL_H
#define COMMON_GL_H

#include "common/test.h"
#include "platform/gl.h"

TestError checkAndReportFramebufferStatus();
TestError checkAndReportCompilationStatus(GLuint name);
TestError checkAndReportLinkStatus(GLuint name);

#endif
