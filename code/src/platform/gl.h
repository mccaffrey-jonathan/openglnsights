//TODO fork off platform specific libs and init funcs
#include <GL/glew.h>
#include <GL/freeglut.h>

TestError GlPlatformSetup() {
    int argc = 0;
    char** argv = NULL;
    glutInit(&argc, argv);
    glutCreateWindow("OpenGL Insights");
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return FEATURE_UNSUPPORTED;
    }
    return SUCCESS;
}
