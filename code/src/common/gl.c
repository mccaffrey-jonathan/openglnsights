#include "common/gl.h"

#include "platform/gl.h"

TestError checkAndReportFramebufferStatus() {
//from http://stackoverflow.com/questions/5993634/using-gles-2-0-framebuffer-fbo-and-stencil-in-android-native-code-ndk
 GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status)
    {
        case GL_FRAMEBUFFER_COMPLETE:
            return SUCCESS;

        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: 
            fprintf(stderr, "FBO GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT  %x\n", status); 
            return INIT_FAILED;

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            fprintf(stderr, "FBO FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT  %x\n", status); 
            return INIT_FAILED;

        case GL_FRAMEBUFFER_UNSUPPORTED: 
            fprintf(stderr, "FBO GL_FRAMEBUFFER_UNSUPPORTED  %x\n", status); 
            return INIT_FAILED;

        default : 
            fprintf(stderr, "failed to make complete framebuffer object %x\n", status);
            return INIT_FAILED;
    }
}


//http://db-in.com/blog/2011/02/all-about-opengl-es-2-x-part-23/#shader_program
TestError checkAndReportCompilationStatus(GLuint name) 
{
    GLint compiled;
    glGetShaderiv(name, GL_COMPILE_STATUS, &compiled);
    if (compiled)
        return SUCCESS;

    GLint logLength;
    glGetShaderiv(name, GL_INFO_LOG_LENGTH, &logLength);
 
    GLchar *log = (GLchar *)malloc(logLength);
    glGetShaderInfoLog(name, logLength, &logLength, log);
    fprintf(stderr, "%s",log);
    free(log);

    return SHADER_COMPILATION_FAILED;
}

//http://joshbeam.com/articles/getting_started_with_glsl/
TestError checkAndReportLinkStatus(GLuint name)
{
    GLint linked;
    glGetProgramiv(name, GL_LINK_STATUS, &linked);
    if(linked)
        return SUCCESS;
    GLint length;
    char *log;

    /* get the program info log */
    glGetProgramiv(name, GL_INFO_LOG_LENGTH, &length);
    log = malloc(length);
    GLint result;
    glGetProgramInfoLog(name, length, &result, log);

    /* print an error message and the info log */
    fprintf(stderr, "sceneInit(): Program linking failed: %s\n", log);
    free(log);
    return SHADER_LINK_FAILED;
}

