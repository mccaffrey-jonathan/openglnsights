#ifndef PLATFORM_GL_H
#define PLATFORM_GL_H

#include "common/test.h"

#ifdef LINUX
#include <GL/glew.h>
#include <GL/freeglut.h>
#define LOGI(...)  \
do { \
    fprintf(stdout, __VA_ARGS__);\
    fprintf(stdout, "\n"); \
} while(0)

#define LOGW(...) \
do { \
    fprintf(stderr, __VA_ARGS__);\
    fprintf(stderr, "\n"); \
} while(0)
#endif

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
typedef char GLchar;

#include <android/log.h>
#define  LOG_TAG    "platform_gl"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)

#endif

TestError GlPlatformSetup();

#endif
