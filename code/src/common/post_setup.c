#include "common/test.h"
#include "platform/gl.h"
#include "common/gl.h"
#include "common/scene_setup.h"
#include "common/post_setup.h"

static GLchar vtx [] =
"precision mediump float;\n"
"\n"
"attribute vec4 aPosition;\n"
"attribute vec2 textureCoord;\n"
"\n"
"varying vec2 tCoord;\n"
"\n"
"void main() {\n"
"    tCoord = textureCoord;\n"
"    gl_Position = aPosition; \n"
"}\n"
;

static GLchar frg [] =
"precision mediump float;\n"
"\n"
"uniform sampler2D texture1; // color texture\n"
"\n"
"varying vec2 tCoord;\n"
"\n"
"void main() {\n"
"    gl_FragColor = vec4(vec3(0.0,0.5,0.0) + "//So we can see that post happened
"       0.2*(texture2D(texture1, tCoord).xyz + \n"
"       texture2D(texture1, tCoord + vec2(0.0, 0.01)).xyz +\n"
"       texture2D(texture1, tCoord + vec2(0.0, -0.01)).xyz +\n"
"       texture2D(texture1, tCoord + vec2(0.01, 0.0)).xyz +\n"
"       texture2D(texture1, tCoord + vec2(-0.01, 0.0)).xyz),1.0) ;\n"
"}\n" ;

TestError SetupColorFbo(PostFramebuffer* fb)
{
    glGenFramebuffers(1, &fb->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
    glGenRenderbuffers(1, &fb->color);
    glBindRenderbuffer(GL_RENDERBUFFER, fb->color);
    glRenderbufferStorage(GL_RENDERBUFFER,
            GL_RGB5_A1, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_RENDERBUFFER,
            fb->color);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    TestError err = checkAndReportFramebufferStatus();
    if (err != SUCCESS)
        return err;
    return SUCCESS;
}

TestError DeletePostFramebuffer(PostFramebuffer* fb)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteRenderbuffers(1, &fb->color);
    glDeleteFramebuffers(1, &fb->fbo);
    return SUCCESS;
}

TestError BufferPostVertices() {

    PostVertex* verts = malloc(4*sizeof(PostVertex));
    if (!verts)
        return OUT_OF_MEMORY;

    //Setup grid of vertices
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            verts[i*2 + j] = (PostVertex)
            {
                .pos = {((((float)i)*2)-1),
                    ((((float)j)*2)-1),
                    0.5f},
                .tex = {(float)i, 
                    (float)j},
            };
        }
    }

    glBufferData(GL_ARRAY_BUFFER,
            4*sizeof(PostVertex),
            verts,
            GL_STATIC_DRAW);

    free(verts);
    verts = NULL;

    uint32_t indices [] = 
    {0,1,2,2,1,3};

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            sizeof(indices),
            indices,
            GL_STATIC_DRAW);
    
    return SUCCESS;
}


TestError CompilePostShaders(ShaderPair* priv)
{
    return CompileShaders(priv,
            vtx,
            sizeof(vtx),
            frg,
            sizeof(frg));
}

#define ATTRIB(PRG, X) .X = glGetAttribLocation(PRG, #X)
#define UNIFOR(PRG, X) .X = glGetUniformLocation(PRG, #X)

TestError GetPostUniformAndAttribLocs(GLuint prg,
        PostAttribLocs* attribs,
        PostUniformLocs* uniforms)
{
    *attribs = (PostAttribLocs){
        ATTRIB(prg, aPosition),
        ATTRIB(prg, textureCoord),
    };

    *uniforms = (PostUniformLocs){
        UNIFOR(prg, texture1),
    };

    if (attribs->aPosition == -1 ||
            attribs->textureCoord == -1) {
        return INVALID_ATTRIBUTE;
    }

    if (uniforms->texture1 == -1) {
        return INVALID_UNIFORM;
    }

    return SUCCESS;
}

#undef ATTRIB
#undef UNIFOR

#define SIZEOF_FIELD(TYPE, FIELD) sizeof(((TYPE*)0)->FIELD)

TestError BindPostShader(
        GLuint prg,
        GLuint tex,
        PostAttribLocs* attribs,
        PostUniformLocs* uniforms)
{
    glUseProgram(prg);
    glEnableVertexAttribArray(attribs->aPosition);
    glVertexAttribPointer(attribs->aPosition,
            SIZEOF_FIELD(PostVertex, pos)/sizeof(GL_FLOAT),
            GL_FLOAT,
            GL_FALSE,
            sizeof(PostVertex),
            0);
    glEnableVertexAttribArray(attribs->textureCoord);
    glVertexAttribPointer(attribs->textureCoord,
            SIZEOF_FIELD(PostVertex, tex)/sizeof(GL_FLOAT),
            GL_FLOAT,
            GL_FALSE,
            sizeof(PostVertex),
                (void*)SIZEOF_FIELD(PostVertex, pos));

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(uniforms->texture1, 0);
    return SUCCESS;
}

#undef SIZEOF_FIELD
