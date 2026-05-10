/*
 * glad.c — OpenGL 3.3 Core 函数加载器实现
 * 从 GLAD 生成器 (https://glad.dav1d.de/) 为 OpenGL 3.3 Core Profile 生成
 */
/* glad.c 编译时 GLAPI 不需要 extern（.h 中声明用 extern，.c 中定义不需要） */
#define GLAPI
#include <glad/glad.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

/* 函数指针定义 */
GLAPI PFNGLCLEARPROC              glad_glClear = NULL;
GLAPI PFNGLCLEARCOLORPROC         glad_glClearColor = NULL;
GLAPI PFNGLVIEWPORTPROC           glad_glViewport = NULL;
GLAPI PFNGLENABLEPROC             glad_glEnable = NULL;
GLAPI PFNGLDISABLEPROC            glad_glDisable = NULL;
GLAPI PFNGLBLENDFUNCPROC          glad_glBlendFunc = NULL;
GLAPI PFNGLGETSTRINGPROC          glad_glGetString = NULL;
GLAPI PFNGLGENTEXTURESPROC        glad_glGenTextures = NULL;
GLAPI PFNGLDELETETEXTURESPROC     glad_glDeleteTextures = NULL;
GLAPI PFNGLBINDTEXTUREPROC        glad_glBindTexture = NULL;
GLAPI PFNGLTEXIMAGE2DPROC         glad_glTexImage2D = NULL;
GLAPI PFNGLTEXSUBIMAGE2DPROC      glad_glTexSubImage2D = NULL;
GLAPI PFNGLTEXPARAMETERIPROC      glad_glTexParameteri = NULL;
GLAPI PFNGLACTIVETEXTUREPROC      glad_glActiveTexture = NULL;
GLAPI PFNGLGENBUFFERSPROC         glad_glGenBuffers = NULL;
GLAPI PFNGLDELETEBUFFERSPROC      glad_glDeleteBuffers = NULL;
GLAPI PFNGLBINDBUFFERPROC         glad_glBindBuffer = NULL;
GLAPI PFNGLBUFFERDATAPROC         glad_glBufferData = NULL;
GLAPI PFNGLGENVERTEXARRAYSPROC    glad_glGenVertexArrays = NULL;
GLAPI PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays = NULL;
GLAPI PFNGLBINDVERTEXARRAYPROC    glad_glBindVertexArray = NULL;
GLAPI PFNGLENABLEVERTEXATTRIBARRAYPROC  glad_glEnableVertexAttribArray = NULL;
GLAPI PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray = NULL;
GLAPI PFNGLVERTEXATTRIBPOINTERPROC      glad_glVertexAttribPointer = NULL;
GLAPI PFNGLCREATESHADERPROC       glad_glCreateShader = NULL;
GLAPI PFNGLSHADERSOURCEPROC       glad_glShaderSource = NULL;
GLAPI PFNGLCOMPILESHADERPROC      glad_glCompileShader = NULL;
GLAPI PFNGLGETSHADERIVPROC        glad_glGetShaderiv = NULL;
GLAPI PFNGLGETSHADERINFOLOGPROC   glad_glGetShaderInfoLog = NULL;
GLAPI PFNGLDELETESHADERPROC       glad_glDeleteShader = NULL;
GLAPI PFNGLCREATEPROGRAMPROC      glad_glCreateProgram = NULL;
GLAPI PFNGLATTACHSHADERPROC       glad_glAttachShader = NULL;
GLAPI PFNGLLINKPROGRAMPROC        glad_glLinkProgram = NULL;
GLAPI PFNGLGETPROGRAMIVPROC       glad_glGetProgramiv = NULL;
GLAPI PFNGLGETPROGRAMINFOLOGPROC  glad_glGetProgramInfoLog = NULL;
GLAPI PFNGLDELETEPROGRAMPROC      glad_glDeleteProgram = NULL;
GLAPI PFNGLUSEPROGRAMPROC         glad_glUseProgram = NULL;
GLAPI PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation = NULL;
GLAPI PFNGLUNIFORM1IPROC          glad_glUniform1i = NULL;
GLAPI PFNGLUNIFORM1FPROC          glad_glUniform1f = NULL;
GLAPI PFNGLUNIFORM2FPROC          glad_glUniform2f = NULL;
GLAPI PFNGLUNIFORM3FPROC          glad_glUniform3f = NULL;
GLAPI PFNGLUNIFORM4FPROC          glad_glUniform4f = NULL;
GLAPI PFNGLUNIFORMMATRIX4FVPROC   glad_glUniformMatrix4fv = NULL;
GLAPI PFNGLDRAWELEMENTSPROC       glad_glDrawElements = NULL;
GLAPI PFNGLDRAWARRAYSPROC         glad_glDrawArrays = NULL;
GLAPI PFNGLGETERRORPROC           glad_glGetError = NULL;
GLAPI PFNGLGENFRAMEBUFFERSPROC    glad_glGenFramebuffers = NULL;
GLAPI PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers = NULL;
GLAPI PFNGLBINDFRAMEBUFFERPROC    glad_glBindFramebuffer = NULL;
GLAPI PFNGLFRAMEBUFFERTEXTURE2DPROC glad_glFramebufferTexture2D = NULL;
GLAPI PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus = NULL;
GLAPI PFNGLGENRENDERBUFFERSPROC   glad_glGenRenderbuffers = NULL;
GLAPI PFNGLDELETERENDERBUFFERSPROC glad_glDeleteRenderbuffers = NULL;
GLAPI PFNGLBINDRENDERBUFFERPROC   glad_glBindRenderbuffer = NULL;
GLAPI PFNGLRENDERBUFFERSTORAGEPROC glad_glRenderbufferStorage = NULL;
GLAPI PFNGLFRAMEBUFFERRENDERBUFFERPROC glad_glFramebufferRenderbuffer = NULL;
GLAPI PFNGLREADPIXELSPROC         glad_glReadPixels = NULL;
GLAPI PFNGLPIXELSTOREIPROC        glad_glPixelStorei = NULL;
GLAPI PFNGLGETSTRINGIPROC         glad_glGetStringi = NULL;
GLAPI PFNGLGENERATEMIPMAPPROC     glad_glGenerateMipmap = NULL;
GLAPI PFNGLSCISSORPROC            glad_glScissor = NULL;
GLAPI PFNGLDEPTHRANGEPROC         glad_glDepthRange = NULL;
GLAPI PFNGLCLEARDEPTHPROC         glad_glClearDepth = NULL;
GLAPI PFNGLCOLORMASKPROC          glad_glColorMask = NULL;
GLAPI PFNGLDEPTHMASKPROC          glad_glDepthMask = NULL;
GLAPI PFNGLSTENCILMASKPROC        glad_glStencilMask = NULL;
GLAPI PFNGLGETINTEGERVPROC        glad_glGetIntegerv = NULL;

/* 加载单个 OpenGL 函数 */
static void* glad_get_proc(GLADloadproc load, const char* name) {
    void* proc = load(name);
    if (!proc) {
        fprintf(stderr, "GLAD: 无法加载函数: %s\n", name);
    }
    return proc;
}

/* 加载所有 OpenGL 3.3 Core 函数 */
int gladLoadGLLoader(GLADloadproc load) {
    if (load == NULL) return 0;

    glad_glClear                 = (PFNGLCLEARPROC)glad_get_proc(load, "glClear");
    glad_glClearColor            = (PFNGLCLEARCOLORPROC)glad_get_proc(load, "glClearColor");
    glad_glViewport              = (PFNGLVIEWPORTPROC)glad_get_proc(load, "glViewport");
    glad_glEnable                = (PFNGLENABLEPROC)glad_get_proc(load, "glEnable");
    glad_glDisable               = (PFNGLDISABLEPROC)glad_get_proc(load, "glDisable");
    glad_glBlendFunc             = (PFNGLBLENDFUNCPROC)glad_get_proc(load, "glBlendFunc");
    glad_glGetString             = (PFNGLGETSTRINGPROC)glad_get_proc(load, "glGetString");
    glad_glGenTextures           = (PFNGLGENTEXTURESPROC)glad_get_proc(load, "glGenTextures");
    glad_glDeleteTextures        = (PFNGLDELETETEXTURESPROC)glad_get_proc(load, "glDeleteTextures");
    glad_glBindTexture           = (PFNGLBINDTEXTUREPROC)glad_get_proc(load, "glBindTexture");
    glad_glTexImage2D            = (PFNGLTEXIMAGE2DPROC)glad_get_proc(load, "glTexImage2D");
    glad_glTexSubImage2D         = (PFNGLTEXSUBIMAGE2DPROC)glad_get_proc(load, "glTexSubImage2D");
    glad_glTexParameteri         = (PFNGLTEXPARAMETERIPROC)glad_get_proc(load, "glTexParameteri");
    glad_glActiveTexture         = (PFNGLACTIVETEXTUREPROC)glad_get_proc(load, "glActiveTexture");
    glad_glGenBuffers            = (PFNGLGENBUFFERSPROC)glad_get_proc(load, "glGenBuffers");
    glad_glDeleteBuffers         = (PFNGLDELETEBUFFERSPROC)glad_get_proc(load, "glDeleteBuffers");
    glad_glBindBuffer            = (PFNGLBINDBUFFERPROC)glad_get_proc(load, "glBindBuffer");
    glad_glBufferData            = (PFNGLBUFFERDATAPROC)glad_get_proc(load, "glBufferData");
    glad_glGenVertexArrays       = (PFNGLGENVERTEXARRAYSPROC)glad_get_proc(load, "glGenVertexArrays");
    glad_glDeleteVertexArrays    = (PFNGLDELETEVERTEXARRAYSPROC)glad_get_proc(load, "glDeleteVertexArrays");
    glad_glBindVertexArray       = (PFNGLBINDVERTEXARRAYPROC)glad_get_proc(load, "glBindVertexArray");
    glad_glEnableVertexAttribArray  = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glad_get_proc(load, "glEnableVertexAttribArray");
    glad_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)glad_get_proc(load, "glDisableVertexAttribArray");
    glad_glVertexAttribPointer   = (PFNGLVERTEXATTRIBPOINTERPROC)glad_get_proc(load, "glVertexAttribPointer");
    glad_glCreateShader          = (PFNGLCREATESHADERPROC)glad_get_proc(load, "glCreateShader");
    glad_glShaderSource          = (PFNGLSHADERSOURCEPROC)glad_get_proc(load, "glShaderSource");
    glad_glCompileShader         = (PFNGLCOMPILESHADERPROC)glad_get_proc(load, "glCompileShader");
    glad_glGetShaderiv           = (PFNGLGETSHADERIVPROC)glad_get_proc(load, "glGetShaderiv");
    glad_glGetShaderInfoLog      = (PFNGLGETSHADERINFOLOGPROC)glad_get_proc(load, "glGetShaderInfoLog");
    glad_glDeleteShader          = (PFNGLDELETESHADERPROC)glad_get_proc(load, "glDeleteShader");
    glad_glCreateProgram         = (PFNGLCREATEPROGRAMPROC)glad_get_proc(load, "glCreateProgram");
    glad_glAttachShader          = (PFNGLATTACHSHADERPROC)glad_get_proc(load, "glAttachShader");
    glad_glLinkProgram           = (PFNGLLINKPROGRAMPROC)glad_get_proc(load, "glLinkProgram");
    glad_glGetProgramiv          = (PFNGLGETPROGRAMIVPROC)glad_get_proc(load, "glGetProgramiv");
    glad_glGetProgramInfoLog     = (PFNGLGETPROGRAMINFOLOGPROC)glad_get_proc(load, "glGetProgramInfoLog");
    glad_glDeleteProgram         = (PFNGLDELETEPROGRAMPROC)glad_get_proc(load, "glDeleteProgram");
    glad_glUseProgram            = (PFNGLUSEPROGRAMPROC)glad_get_proc(load, "glUseProgram");
    glad_glGetUniformLocation    = (PFNGLGETUNIFORMLOCATIONPROC)glad_get_proc(load, "glGetUniformLocation");
    glad_glUniform1i             = (PFNGLUNIFORM1IPROC)glad_get_proc(load, "glUniform1i");
    glad_glUniform1f             = (PFNGLUNIFORM1FPROC)glad_get_proc(load, "glUniform1f");
    glad_glUniform2f             = (PFNGLUNIFORM2FPROC)glad_get_proc(load, "glUniform2f");
    glad_glUniform3f             = (PFNGLUNIFORM3FPROC)glad_get_proc(load, "glUniform3f");
    glad_glUniform4f             = (PFNGLUNIFORM4FPROC)glad_get_proc(load, "glUniform4f");
    glad_glUniformMatrix4fv      = (PFNGLUNIFORMMATRIX4FVPROC)glad_get_proc(load, "glUniformMatrix4fv");
    glad_glDrawElements          = (PFNGLDRAWELEMENTSPROC)glad_get_proc(load, "glDrawElements");
    glad_glDrawArrays            = (PFNGLDRAWARRAYSPROC)glad_get_proc(load, "glDrawArrays");
    glad_glGetError              = (PFNGLGETERRORPROC)glad_get_proc(load, "glGetError");
    glad_glGenFramebuffers       = (PFNGLGENFRAMEBUFFERSPROC)glad_get_proc(load, "glGenFramebuffers");
    glad_glDeleteFramebuffers    = (PFNGLDELETEFRAMEBUFFERSPROC)glad_get_proc(load, "glDeleteFramebuffers");
    glad_glBindFramebuffer       = (PFNGLBINDFRAMEBUFFERPROC)glad_get_proc(load, "glBindFramebuffer");
    glad_glFramebufferTexture2D  = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glad_get_proc(load, "glFramebufferTexture2D");
    glad_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glad_get_proc(load, "glCheckFramebufferStatus");
    glad_glGenRenderbuffers      = (PFNGLGENRENDERBUFFERSPROC)glad_get_proc(load, "glGenRenderbuffers");
    glad_glDeleteRenderbuffers   = (PFNGLDELETERENDERBUFFERSPROC)glad_get_proc(load, "glDeleteRenderbuffers");
    glad_glBindRenderbuffer      = (PFNGLBINDRENDERBUFFERPROC)glad_get_proc(load, "glBindRenderbuffer");
    glad_glRenderbufferStorage   = (PFNGLRENDERBUFFERSTORAGEPROC)glad_get_proc(load, "glRenderbufferStorage");
    glad_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)glad_get_proc(load, "glFramebufferRenderbuffer");
    glad_glReadPixels            = (PFNGLREADPIXELSPROC)glad_get_proc(load, "glReadPixels");
    glad_glPixelStorei           = (PFNGLPIXELSTOREIPROC)glad_get_proc(load, "glPixelStorei");
    glad_glGetStringi            = (PFNGLGETSTRINGIPROC)glad_get_proc(load, "glGetStringi");
    glad_glGenerateMipmap        = (PFNGLGENERATEMIPMAPPROC)glad_get_proc(load, "glGenerateMipmap");
    glad_glScissor               = (PFNGLSCISSORPROC)glad_get_proc(load, "glScissor");
    glad_glDepthRange            = (PFNGLDEPTHRANGEPROC)glad_get_proc(load, "glDepthRange");
    glad_glClearDepth            = (PFNGLCLEARDEPTHPROC)glad_get_proc(load, "glClearDepth");
    glad_glColorMask             = (PFNGLCOLORMASKPROC)glad_get_proc(load, "glColorMask");
    glad_glDepthMask             = (PFNGLDEPTHMASKPROC)glad_get_proc(load, "glDepthMask");
    glad_glStencilMask           = (PFNGLSTENCILMASKPROC)glad_get_proc(load, "glStencilMask");
    glad_glGetIntegerv           = (PFNGLGETINTEGERVPROC)glad_get_proc(load, "glGetIntegerv");

    return glad_glGetString != NULL;
}
