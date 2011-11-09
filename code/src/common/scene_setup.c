#include "common/scene_setup.h"

#include "common/test.h"
#include "platform/gl.h"
#include "common/gl.h"
#include <stdlib.h>

// from http://code.google.com/p/androidshaders/source/browse/res/raw/gouraud_vs.txt
// Vertex shader Gouraud Shading - Per-vertex lighting
static const GLfloat IDENTITY_MAT4 [] = 
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };

//These don't really matter
static const GLfloat LIGHT_POS [] =
    { 0.0f, 0.0f, -1000.0f, 1.0f };

static const GLfloat MAT_DIFFUSE [] =
    { 1.0f, 0.0f, 0.0f, 1.0f };

static GLchar vtx [] =
"precision mediump float;\n"
"uniform mat4 uMVPMatrix;\n"
"uniform vec4 lightPos;\n"
"uniform vec4 matDiffuse;\n"
"\n"
"attribute vec4 aPosition;\n"
"attribute vec3 aNormal; \n"
"attribute vec2 textureCoord;\n"
"\n"
"varying vec2 tCoord;\n"
"varying vec4 color;\n"
"\n"
"void main() {\n"
"    // pass on texture variables\n"
"    tCoord = textureCoord;\n"
"    // normal\n"
"\n"
"    // the vertex position\n"
"    vec4 posit = uMVPMatrix * aPosition; \n"
"\n"
"    // light dir\n"
"    vec3 lightDir = lightPos.xyz - posit.xyz;\n"
//"    vec3 EyespaceNormal = vec3(uMVPMatrix * vec4(aNormal*0.000000001+vec3(1,0,0), 1.0));\n"
"    vec3 EyespaceNormal = vec3(uMVPMatrix * vec4(aNormal, 1.0));\n"
"    vec3 N = normalize(EyespaceNormal);\n"
"    vec3 L = normalize(lightDir);\n"
"    \n"
"    vec4 diffuseTerm = matDiffuse * max(dot(N, L), 0.0);\n"
"    \n"
"    color = diffuseTerm;\n"
"	\n"
"    gl_Position = posit; \n"
"}\n"
;

//TODO go back to normal color
static GLchar frg [] =
"precision mediump float;\n"
"\n"
"uniform sampler2D texture1; // color texture\n"
"\n"
"varying vec2 tCoord;\n"
"varying vec4 color;\n"
"\n"
"void main() {\n"
//"    gl_FragColor = vec4(vec3(0.0,0.0,0.5) + 0.5*color*texture2D(texture1, tCoord).xyz, 1.0);\n" //Offset to make shader effect obvious
"    gl_FragColor = vec4(vec3(0.0,0.0,0.5) + 0.500001*color.xyz + 0.000001*texture2D(texture1, tCoord).xyz, 1.0);\n" 
"}\n"
;

TestError SetupDepthAndColorFbo(SceneFramebuffer* fb)
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
    glGenRenderbuffers(1, &fb->depth);
    glBindRenderbuffer(GL_RENDERBUFFER, fb->depth);
    glRenderbufferStorage(GL_RENDERBUFFER,
            GL_DEPTH_COMPONENT16, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            fb->depth);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
    TestError err = checkAndReportFramebufferStatus();
    if (err != SUCCESS)
        return err;
    return SUCCESS;
}

TestError SetupDepthRBOAndColorTex(SceneFramebuffer* fb)
{
    glGenFramebuffers(1, &fb->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
    glGenTextures(1, &fb->color);
    glBindTexture(GL_TEXTURE_2D, fb->color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,
            0, GL_RGBA,
            WIDTH, HEIGHT, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            fb->color,
            0);
    glGenRenderbuffers(1, &fb->depth);
    glBindRenderbuffer(GL_RENDERBUFFER, fb->depth);
    glRenderbufferStorage(GL_RENDERBUFFER,
            GL_DEPTH_COMPONENT16, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            fb->depth);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
    TestError err = checkAndReportFramebufferStatus();
    if (err != SUCCESS)
        return err;
    return SUCCESS;
}

TestError bufferDataSceneVertexGrid(int width, int height) {

    SceneVertex* verts = malloc(width*height*sizeof(SceneVertex));
    if (!verts)
        return OUT_OF_MEMORY;

    //Setup grid of vertices
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            verts[i*height + j] = (SceneVertex)
            {
                .pos = {(((((float)i)/width)*2)-1),
                    (((((float)j)/height)*2)-1),
                    0.5f},
                .nrm = {0.0f, 0.0f, 1.0f},
                .tex = {((float)i)/width, 
                    ((float)j)/height},
            };
        }
    }

    glBufferData(GL_ARRAY_BUFFER,
            width*height*sizeof(SceneVertex),
            verts,
            GL_STATIC_DRAW);

    free(verts);
    verts = NULL;

    uint32_t* indices = malloc((width-1)*(height-1)*sizeof(uint32_t)*6);

    //Setup indexed triangles
    for (int i = 0; i < (width-1); i++) {
        for (int j = 0; j < (height-1); j++) {
           indices[6*(i*(height-1) + j) + 0] = i*height + j;
           indices[6*(i*(height-1) + j) + 1] = i*height + j + 1;
           indices[6*(i*(height-1) + j) + 2] = (i+1)*height + j;

           indices[6*(i*(height-1) + j) + 3] = i*height + j + 1;
           indices[6*(i*(height-1) + j) + 4] = (i+1)*height + j + 1;
           indices[6*(i*(height-1) + j) + 5] = (i+1)*height + j;
        }
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            (width-1)*(height-1)*sizeof(uint32_t)*6,
            indices,
            GL_STATIC_DRAW);
    
    free(indices);
    indices = NULL;

    return SUCCESS;
}

TestError CompileSceneShaders(ShaderPair* priv)
{
    priv->vs = glCreateShader(GL_VERTEX_SHADER);
    int len = sizeof(vtx);
    const GLchar* vtxSrc = vtx;
    glShaderSource(priv->vs, 1, &vtxSrc, &len);
    glCompileShader(priv->vs);
    TestError err = checkAndReportCompilationStatus(priv->vs);
    if (err != SUCCESS)
        return err;

    priv->fs = glCreateShader(GL_FRAGMENT_SHADER);
    len = sizeof(frg);
    const GLchar* frgSrc = frg;
    glShaderSource(priv->fs, 1, &frgSrc, &len);
    glCompileShader(priv->fs);
    err = checkAndReportCompilationStatus(priv->fs);
    if (err != SUCCESS)
        return err;

    priv->prg = glCreateProgram();
    glAttachShader(priv->prg, priv->vs);
    glAttachShader(priv->prg, priv->fs);
    glLinkProgram(priv->prg);
    err = checkAndReportLinkStatus(priv->prg);
    if (err != SUCCESS)
        return err;

    return SUCCESS;
}

void DebugDumpAttribsAndUniforms(GLuint prg)
{
    int total = -1;
    glGetProgramiv( prg, GL_ACTIVE_ATTRIBUTES, &total ); 
    for(int i=0; i<total; ++i)  {
        int name_len=-1, num=-1;
        GLenum type = GL_ZERO;
        char name[100];
        glGetActiveAttrib( prg, (GLuint)i, sizeof(name)-6,
                &name_len, &num, &type, name );
        name[name_len] = 0;
        GLuint location = glGetAttribLocation( prg, name );
        fprintf(stderr, "%d %s %u\n", i, name, location);
    }

    glGetProgramiv( prg, GL_ACTIVE_UNIFORMS, &total ); 
    for(int i=0; i<total; ++i)  {
        int name_len=-1, num=-1;
        GLenum type = GL_ZERO;
        char name[100];
        glGetActiveUniform( prg, (GLuint)i, sizeof(name)-6,
                &name_len, &num, &type, name );
        name[name_len] = 0;
        GLuint location = glGetUniformLocation( prg, name );
        fprintf(stderr, "%d %s %u\n", i, name, location);
    }

}

#define ATTRIB(PRG, X) .X = glGetAttribLocation(PRG, #X)
#define UNIFOR(PRG, X) .X = glGetUniformLocation(PRG, #X)

TestError GetSceneUniformAndAttribLocs(GLuint prg,
        SceneAttribLocs* attribs,
        SceneUniformLocs* uniforms)
{
    *attribs = (SceneAttribLocs){
        ATTRIB(prg, aPosition),
        ATTRIB(prg, aNormal),
        ATTRIB(prg, textureCoord),
    };

    *uniforms = (SceneUniformLocs){
        UNIFOR(prg, uMVPMatrix),
        UNIFOR(prg, lightPos),
        UNIFOR(prg, matDiffuse),
        UNIFOR(prg, texture1),
    };

    if (attribs->aPosition == -1 ||
            attribs->aNormal == -1 ||
            attribs->textureCoord == -1) {
        return INVALID_ATTRIBUTE;
    }

    if (uniforms->uMVPMatrix == -1 ||
            uniforms->lightPos == -1 ||
            uniforms->matDiffuse == -1 ||
            uniforms->texture1 == -1) {
        return INVALID_UNIFORM;
    }

    return SUCCESS;
}

#undef ATTRIB
#undef UNIFOR

#define SIZEOF_FIELD(TYPE, FIELD) sizeof(((TYPE*)0)->FIELD)

TestError BindSceneShader(
        GLuint prg,
        GLuint tex,
        SceneAttribLocs* attribs,
        SceneUniformLocs* uniforms)
{
    glUseProgram(prg);
    glEnableVertexAttribArray(attribs->aPosition);
    glVertexAttribPointer(attribs->aPosition,
            SIZEOF_FIELD(SceneVertex, pos)/sizeof(GL_FLOAT),
            GL_FLOAT,
            GL_FALSE,
            sizeof(SceneVertex),
            0);
    glEnableVertexAttribArray(attribs->aNormal);
    glVertexAttribPointer(attribs->aNormal,
            SIZEOF_FIELD(SceneVertex, nrm)/sizeof(GL_FLOAT),
            GL_FLOAT,
            GL_FALSE,
            sizeof(SceneVertex),
            (void*)SIZEOF_FIELD(SceneVertex, pos));
    glEnableVertexAttribArray(attribs->textureCoord);
    glVertexAttribPointer(attribs->textureCoord,
            SIZEOF_FIELD(SceneVertex, tex)/sizeof(GL_FLOAT),
            GL_FLOAT,
            GL_FALSE,
            sizeof(SceneVertex),
            (void*)(SIZEOF_FIELD(SceneVertex, pos) +
                SIZEOF_FIELD(SceneVertex, nrm)));

    glUniformMatrix4fv(uniforms->uMVPMatrix,
            1,
            GL_FALSE,
            IDENTITY_MAT4);
    glUniform4fv(uniforms->lightPos, 1, LIGHT_POS);
    glUniform4fv(uniforms->matDiffuse, 1, MAT_DIFFUSE);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(uniforms->texture1, 0);
    return SUCCESS;
}

#undef SIZEOF_FIELD


TestError CreateTexture(GLuint* out) 
{
    glGenTextures(1, out);
    glBindTexture(GL_TEXTURE_2D, *out);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GLuint* texdata = 
        malloc(WIDTH*HEIGHT*sizeof(GLuint));
    if (!texdata)
        return OUT_OF_MEMORY;
    for (int i = 0; i < WIDTH*HEIGHT; i++) {
        GLuint val = 0xFFFFFFFF; //White?
        texdata[i] = val;
    }
    glTexImage2D(GL_TEXTURE_2D,
            0, GL_RGBA,
            WIDTH, HEIGHT, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, texdata);
    glGenerateMipmap(GL_TEXTURE_2D);
    free(texdata);
    return SUCCESS;
}

TestError DeleteSceneFramebuffer(SceneFramebuffer* fb) 
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteRenderbuffers(1, &fb->color);
    glDeleteRenderbuffers(1, &fb->depth);
    glDeleteFramebuffers(1, &fb->fbo);
    return SUCCESS;
}

TestError DeleteSceneFramebufferWithTex(SceneFramebuffer* fb) 
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteTextures(1, &fb->color);
    glDeleteRenderbuffers(1, &fb->depth);
    glDeleteFramebuffers(1, &fb->fbo);
    return SUCCESS;
}

void DebugDumpPixels()
{
    GLfloat* pixs = malloc(sizeof(GLfloat)*4*WIDTH*HEIGHT);
    glReadPixels(0, 0,
            WIDTH,
            HEIGHT,
            GL_RGB,
            GL_FLOAT,
            pixs);
    for (int i = 0; i < WIDTH; i+=1) {
        for (int j = 0; j < HEIGHT; j+=1) {
            int ind = 3*(i*HEIGHT+j);
            fprintf(stderr, "%d %f %f %f\n", ind/3, 
                    pixs[ind+0], pixs[ind+1],pixs[ind+2]);
        }
    }
    free(pixs);
}

//glGetTexImage not defined in base OES
void DebugDumpTexture(GLuint tex, int width, int height)
{
//    GLfloat* pixs = malloc(sizeof(GLfloat)*4*WIDTH*HEIGHT);
//    glBindTexture(GL_TEXTURE_2D, tex);
//    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB,
//            GL_FLOAT, pixs);
//    for (int i = 0; i < width; i+=1) {
//        for (int j = 0; j < height; j+=1) {
//            int ind = 3*(i*height+j);
//            fprintf(stderr, "%d %f %f %f\n", ind/3, 
//                    pixs[ind+0], pixs[ind+1],pixs[ind+2]);
//        }
//    }
//    free(pixs);
}

//Not defined in base OES
void DebugDumpVertices()
{
//    SceneVertex* verts = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
//    for (int i = 0; i < SCENE_VERTEX_HEIGHT*SCENE_VERTEX_WIDTH; i++) {
//        SceneVertex* vtx = verts + i;
//        fprintf(stderr, "idx: %d pos: %f %f %f nrm: %f %f %f tex %f %f\n",
//                i,
//                vtx->pos[0], vtx->pos[1], vtx->pos[2],
//                vtx->nrm[0], vtx->nrm[1], vtx->nrm[2],
//                vtx->tex[0], vtx->tex[1]);
//    }
//    glUnmapBuffer(GL_ARRAY_BUFFER);
}

//MapBuffer undefined in OES
void DebugDumpIndices()
{
//    uint32_t* verts = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
//    if (!verts)
//        fprintf(stderr, "Could not map index buffer\n");
//
//    for (int i = 0; i < 6*(SCENE_VERTEX_HEIGHT-1)*(SCENE_VERTEX_WIDTH-1); i++) {
//        fprintf(stderr, "idx: %d index: %u\n", i, verts[i]);
//    }
//    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}
