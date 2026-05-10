/*
 * glad.h — OpenGL 3.3 Core 加载器头文件
 * 由 GLAD 生成器 (https://glad.dav1d.de/) 为 OpenGL 3.3 Core Profile 生成
 * Language: C/C++, Specification: OpenGL, API: gl=3.3, Profile: Core
 *
 * 使用方法:
 *   #include <glad/glad.h>
 *   // 在创建 OpenGL 上下文后调用:
 *   gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
 */
#ifndef __glad_h_
#define __glad_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <KHR/khrplatform.h>

/* GLAD API 宏 */
#ifndef GLAD_API_PTR
 #ifdef _WIN32
  #define GLAD_API_PTR __stdcall
 #else
  #define GLAD_API_PTR
 #endif
#endif

#ifndef GLAPI
 #define GLAPI extern
#endif

typedef void* (*GLADloadproc)(const char *name);
int gladLoadGLLoader(GLADloadproc);

/* ---------- OpenGL 类型 ---------- */
typedef void GLvoid;
typedef unsigned int GLenum;
typedef khronos_float_t GLfloat;
typedef int GLint;
typedef int GLsizei;
typedef unsigned int GLbitfield;
typedef double GLdouble;
typedef unsigned int GLuint;
typedef unsigned char GLboolean;
typedef khronos_uint8_t GLubyte;
typedef khronos_int8_t GLbyte;
typedef short GLshort;
typedef unsigned short GLushort;
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_intptr_t GLintptr;
typedef char GLchar;
typedef khronos_int16_t GLhalf;

/* ---------- OpenGL 常量 ---------- */
#define GL_FALSE                          0
#define GL_TRUE                           1
#define GL_NONE                           0
#define GL_ZERO                           0
#define GL_ONE                            1

/* 数据类型 */
#define GL_BYTE                           0x1400
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_SHORT                          0x1402
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_INT                            0x1404
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406
#define GL_DOUBLE                         0x140A

/* 错误 */
#define GL_NO_ERROR                       0
#define GL_INVALID_ENUM                   0x0500
#define GL_INVALID_VALUE                  0x0501
#define GL_INVALID_OPERATION              0x0502
#define GL_OUT_OF_MEMORY                  0x0505

/* Enable/Disable */
#define GL_DEPTH_TEST                     0x0B71
#define GL_BLEND                          0x0BE2
#define GL_SCISSOR_TEST                   0x0C11
#define GL_CULL_FACE                      0x0B44
#define GL_STENCIL_TEST                   0x0B90
#define GL_MULTISAMPLE                    0x809D

/* Blend 因子 */
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_DST_ALPHA                      0x0304
#define GL_ONE_MINUS_DST_ALPHA            0x0305
#define GL_SRC_COLOR                      0x0300
#define GL_ONE_MINUS_SRC_COLOR            0x0301

/* Clear 位 */
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_STENCIL_BUFFER_BIT             0x00000400

/* Draw 模式 */
#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006

/* 纹理 */
#define GL_TEXTURE_2D                     0x0DE1
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601
#define GL_LINEAR_MIPMAP_LINEAR           0x2703
#define GL_REPEAT                         0x2901
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_RGBA                           0x1908
#define GL_RGB                            0x1907
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_DEPTH_COMPONENT                0x1902

/* Buffer */
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_STATIC_DRAW                    0x88E4
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_STREAM_DRAW                    0x88E0

/* Shader */
#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_INFO_LOG_LENGTH                0x8B84

/* Framebuffer */
#define GL_FRAMEBUFFER                    0x8D40
#define GL_RENDERBUFFER                   0x8D41
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5

/* 杂项 */
#define GL_VERSION                        0x1F02
#define GL_RENDERER                       0x1F01
#define GL_VENDOR                         0x1F00
#define GL_EXTENSIONS                     0x1F03
#define GL_NUM_EXTENSIONS                 0x821D

/* ============================================================ */
/* OpenGL 3.3 Core 函数指针声明                                    */
/* ============================================================ */

typedef void   (GLAD_API_PTR *PFNGLCLEARPROC)(GLbitfield);
typedef void   (GLAD_API_PTR *PFNGLCLEARCOLORPROC)(GLfloat, GLfloat, GLfloat, GLfloat);
typedef void   (GLAD_API_PTR *PFNGLVIEWPORTPROC)(GLint, GLint, GLsizei, GLsizei);
typedef void   (GLAD_API_PTR *PFNGLENABLEPROC)(GLenum);
typedef void   (GLAD_API_PTR *PFNGLDISABLEPROC)(GLenum);
typedef void   (GLAD_API_PTR *PFNGLBLENDFUNCPROC)(GLenum, GLenum);
typedef const GLubyte* (GLAD_API_PTR *PFNGLGETSTRINGPROC)(GLenum);
typedef void   (GLAD_API_PTR *PFNGLGENTEXTURESPROC)(GLsizei, GLuint*);
typedef void   (GLAD_API_PTR *PFNGLDELETETEXTURESPROC)(GLsizei, const GLuint*);
typedef void   (GLAD_API_PTR *PFNGLBINDTEXTUREPROC)(GLenum, GLuint);
typedef void   (GLAD_API_PTR *PFNGLTEXIMAGE2DPROC)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*);
typedef void   (GLAD_API_PTR *PFNGLTEXSUBIMAGE2DPROC)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void*);
typedef void   (GLAD_API_PTR *PFNGLTEXPARAMETERIPROC)(GLenum, GLenum, GLint);
typedef void   (GLAD_API_PTR *PFNGLACTIVETEXTUREPROC)(GLenum);
typedef void   (GLAD_API_PTR *PFNGLGENBUFFERSPROC)(GLsizei, GLuint*);
typedef void   (GLAD_API_PTR *PFNGLDELETEBUFFERSPROC)(GLsizei, const GLuint*);
typedef void   (GLAD_API_PTR *PFNGLBINDBUFFERPROC)(GLenum, GLuint);
typedef void   (GLAD_API_PTR *PFNGLBUFFERDATAPROC)(GLenum, GLsizeiptr, const void*, GLenum);
typedef void   (GLAD_API_PTR *PFNGLGENVERTEXARRAYSPROC)(GLsizei, GLuint*);
typedef void   (GLAD_API_PTR *PFNGLDELETEVERTEXARRAYSPROC)(GLsizei, const GLuint*);
typedef void   (GLAD_API_PTR *PFNGLBINDVERTEXARRAYPROC)(GLuint);
typedef void   (GLAD_API_PTR *PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint);
typedef void   (GLAD_API_PTR *PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint);
typedef void   (GLAD_API_PTR *PFNGLVERTEXATTRIBPOINTERPROC)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*);
typedef GLuint (GLAD_API_PTR *PFNGLCREATESHADERPROC)(GLenum);
typedef void   (GLAD_API_PTR *PFNGLSHADERSOURCEPROC)(GLuint, GLsizei, const GLchar* const*, const GLint*);
typedef void   (GLAD_API_PTR *PFNGLCOMPILESHADERPROC)(GLuint);
typedef void   (GLAD_API_PTR *PFNGLGETSHADERIVPROC)(GLuint, GLenum, GLint*);
typedef void   (GLAD_API_PTR *PFNGLGETSHADERINFOLOGPROC)(GLuint, GLsizei, GLsizei*, GLchar*);
typedef void   (GLAD_API_PTR *PFNGLDELETESHADERPROC)(GLuint);
typedef GLuint (GLAD_API_PTR *PFNGLCREATEPROGRAMPROC)(void);
typedef void   (GLAD_API_PTR *PFNGLATTACHSHADERPROC)(GLuint, GLuint);
typedef void   (GLAD_API_PTR *PFNGLLINKPROGRAMPROC)(GLuint);
typedef void   (GLAD_API_PTR *PFNGLGETPROGRAMIVPROC)(GLuint, GLenum, GLint*);
typedef void   (GLAD_API_PTR *PFNGLGETPROGRAMINFOLOGPROC)(GLuint, GLsizei, GLsizei*, GLchar*);
typedef void   (GLAD_API_PTR *PFNGLDELETEPROGRAMPROC)(GLuint);
typedef void   (GLAD_API_PTR *PFNGLUSEPROGRAMPROC)(GLuint);
typedef GLint  (GLAD_API_PTR *PFNGLGETUNIFORMLOCATIONPROC)(GLuint, const GLchar*);
typedef void   (GLAD_API_PTR *PFNGLUNIFORM1IPROC)(GLint, GLint);
typedef void   (GLAD_API_PTR *PFNGLUNIFORM1FPROC)(GLint, GLfloat);
typedef void   (GLAD_API_PTR *PFNGLUNIFORM2FPROC)(GLint, GLfloat, GLfloat);
typedef void   (GLAD_API_PTR *PFNGLUNIFORM3FPROC)(GLint, GLfloat, GLfloat, GLfloat);
typedef void   (GLAD_API_PTR *PFNGLUNIFORM4FPROC)(GLint, GLfloat, GLfloat, GLfloat, GLfloat);
typedef void   (GLAD_API_PTR *PFNGLUNIFORMMATRIX4FVPROC)(GLint, GLsizei, GLboolean, const GLfloat*);
typedef void   (GLAD_API_PTR *PFNGLDRAWELEMENTSPROC)(GLenum, GLsizei, GLenum, const void*);
typedef void   (GLAD_API_PTR *PFNGLDRAWARRAYSPROC)(GLenum, GLint, GLsizei);
typedef GLenum (GLAD_API_PTR *PFNGLGETERRORPROC)(void);
typedef void   (GLAD_API_PTR *PFNGLGENFRAMEBUFFERSPROC)(GLsizei, GLuint*);
typedef void   (GLAD_API_PTR *PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei, const GLuint*);
typedef void   (GLAD_API_PTR *PFNGLBINDFRAMEBUFFERPROC)(GLenum, GLuint);
typedef void   (GLAD_API_PTR *PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum, GLenum, GLenum, GLuint, GLint);
typedef GLenum (GLAD_API_PTR *PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum);
typedef void   (GLAD_API_PTR *PFNGLGENRENDERBUFFERSPROC)(GLsizei, GLuint*);
typedef void   (GLAD_API_PTR *PFNGLDELETERENDERBUFFERSPROC)(GLsizei, const GLuint*);
typedef void   (GLAD_API_PTR *PFNGLBINDRENDERBUFFERPROC)(GLenum, GLuint);
typedef void   (GLAD_API_PTR *PFNGLRENDERBUFFERSTORAGEPROC)(GLenum, GLenum, GLsizei, GLsizei);
typedef void   (GLAD_API_PTR *PFNGLFRAMEBUFFERRENDERBUFFERPROC)(GLenum, GLenum, GLenum, GLuint);
typedef void   (GLAD_API_PTR *PFNGLREADPIXELSPROC)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void*);
typedef void   (GLAD_API_PTR *PFNGLPIXELSTOREIPROC)(GLenum, GLint);
typedef const GLubyte* (GLAD_API_PTR *PFNGLGETSTRINGIPROC)(GLenum, GLuint);
typedef void   (GLAD_API_PTR *PFNGLGENERATEMIPMAPPROC)(GLenum);
typedef void   (GLAD_API_PTR *PFNGLSCISSORPROC)(GLint, GLint, GLsizei, GLsizei);
typedef void   (GLAD_API_PTR *PFNGLDEPTHRANGEPROC)(GLdouble, GLdouble);
typedef void   (GLAD_API_PTR *PFNGLCLEARDEPTHPROC)(GLdouble);
typedef void   (GLAD_API_PTR *PFNGLCOLORMASKPROC)(GLboolean, GLboolean, GLboolean, GLboolean);
typedef void   (GLAD_API_PTR *PFNGLDEPTHMASKPROC)(GLboolean);
typedef void   (GLAD_API_PTR *PFNGLSTENCILMASKPROC)(GLuint);
typedef void   (GLAD_API_PTR *PFNGLGETINTEGERVPROC)(GLenum, GLint*);

/* 全局函数指针 */
GLAPI PFNGLCLEARPROC              glad_glClear;
GLAPI PFNGLCLEARCOLORPROC         glad_glClearColor;
GLAPI PFNGLVIEWPORTPROC           glad_glViewport;
GLAPI PFNGLENABLEPROC             glad_glEnable;
GLAPI PFNGLDISABLEPROC            glad_glDisable;
GLAPI PFNGLBLENDFUNCPROC          glad_glBlendFunc;
GLAPI PFNGLGETSTRINGPROC          glad_glGetString;
GLAPI PFNGLGENTEXTURESPROC        glad_glGenTextures;
GLAPI PFNGLDELETETEXTURESPROC     glad_glDeleteTextures;
GLAPI PFNGLBINDTEXTUREPROC        glad_glBindTexture;
GLAPI PFNGLTEXIMAGE2DPROC         glad_glTexImage2D;
GLAPI PFNGLTEXSUBIMAGE2DPROC      glad_glTexSubImage2D;
GLAPI PFNGLTEXPARAMETERIPROC      glad_glTexParameteri;
GLAPI PFNGLACTIVETEXTUREPROC      glad_glActiveTexture;
GLAPI PFNGLGENBUFFERSPROC         glad_glGenBuffers;
GLAPI PFNGLDELETEBUFFERSPROC      glad_glDeleteBuffers;
GLAPI PFNGLBINDBUFFERPROC         glad_glBindBuffer;
GLAPI PFNGLBUFFERDATAPROC         glad_glBufferData;
GLAPI PFNGLGENVERTEXARRAYSPROC    glad_glGenVertexArrays;
GLAPI PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays;
GLAPI PFNGLBINDVERTEXARRAYPROC    glad_glBindVertexArray;
GLAPI PFNGLENABLEVERTEXATTRIBARRAYPROC  glad_glEnableVertexAttribArray;
GLAPI PFNGLDISABLEVERTEXATTRIBARRAYPROC glad_glDisableVertexAttribArray;
GLAPI PFNGLVERTEXATTRIBPOINTERPROC      glad_glVertexAttribPointer;
GLAPI PFNGLCREATESHADERPROC       glad_glCreateShader;
GLAPI PFNGLSHADERSOURCEPROC       glad_glShaderSource;
GLAPI PFNGLCOMPILESHADERPROC      glad_glCompileShader;
GLAPI PFNGLGETSHADERIVPROC        glad_glGetShaderiv;
GLAPI PFNGLGETSHADERINFOLOGPROC   glad_glGetShaderInfoLog;
GLAPI PFNGLDELETESHADERPROC       glad_glDeleteShader;
GLAPI PFNGLCREATEPROGRAMPROC      glad_glCreateProgram;
GLAPI PFNGLATTACHSHADERPROC       glad_glAttachShader;
GLAPI PFNGLLINKPROGRAMPROC        glad_glLinkProgram;
GLAPI PFNGLGETPROGRAMIVPROC       glad_glGetProgramiv;
GLAPI PFNGLGETPROGRAMINFOLOGPROC  glad_glGetProgramInfoLog;
GLAPI PFNGLDELETEPROGRAMPROC      glad_glDeleteProgram;
GLAPI PFNGLUSEPROGRAMPROC         glad_glUseProgram;
GLAPI PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation;
GLAPI PFNGLUNIFORM1IPROC          glad_glUniform1i;
GLAPI PFNGLUNIFORM1FPROC          glad_glUniform1f;
GLAPI PFNGLUNIFORM2FPROC          glad_glUniform2f;
GLAPI PFNGLUNIFORM3FPROC          glad_glUniform3f;
GLAPI PFNGLUNIFORM4FPROC          glad_glUniform4f;
GLAPI PFNGLUNIFORMMATRIX4FVPROC   glad_glUniformMatrix4fv;
GLAPI PFNGLDRAWELEMENTSPROC       glad_glDrawElements;
GLAPI PFNGLDRAWARRAYSPROC         glad_glDrawArrays;
GLAPI PFNGLGETERRORPROC           glad_glGetError;
GLAPI PFNGLGENFRAMEBUFFERSPROC    glad_glGenFramebuffers;
GLAPI PFNGLDELETEFRAMEBUFFERSPROC glad_glDeleteFramebuffers;
GLAPI PFNGLBINDFRAMEBUFFERPROC    glad_glBindFramebuffer;
GLAPI PFNGLFRAMEBUFFERTEXTURE2DPROC glad_glFramebufferTexture2D;
GLAPI PFNGLCHECKFRAMEBUFFERSTATUSPROC glad_glCheckFramebufferStatus;
GLAPI PFNGLGENRENDERBUFFERSPROC   glad_glGenRenderbuffers;
GLAPI PFNGLDELETERENDERBUFFERSPROC glad_glDeleteRenderbuffers;
GLAPI PFNGLBINDRENDERBUFFERPROC   glad_glBindRenderbuffer;
GLAPI PFNGLRENDERBUFFERSTORAGEPROC glad_glRenderbufferStorage;
GLAPI PFNGLFRAMEBUFFERRENDERBUFFERPROC glad_glFramebufferRenderbuffer;
GLAPI PFNGLREADPIXELSPROC         glad_glReadPixels;
GLAPI PFNGLPIXELSTOREIPROC        glad_glPixelStorei;
GLAPI PFNGLGETSTRINGIPROC         glad_glGetStringi;
GLAPI PFNGLGENERATEMIPMAPPROC     glad_glGenerateMipmap;
GLAPI PFNGLSCISSORPROC            glad_glScissor;
GLAPI PFNGLDEPTHRANGEPROC         glad_glDepthRange;
GLAPI PFNGLCLEARDEPTHPROC         glad_glClearDepth;
GLAPI PFNGLCOLORMASKPROC          glad_glColorMask;
GLAPI PFNGLDEPTHMASKPROC          glad_glDepthMask;
GLAPI PFNGLSTENCILMASKPROC        glad_glStencilMask;
GLAPI PFNGLGETINTEGERVPROC        glad_glGetIntegerv;

/* 宏别名（与标准 OpenGL 头文件兼容） */
#define glClear                 glad_glClear
#define glClearColor            glad_glClearColor
#define glViewport              glad_glViewport
#define glEnable                glad_glEnable
#define glDisable               glad_glDisable
#define glBlendFunc             glad_glBlendFunc
#define glGetString             glad_glGetString
#define glGenTextures           glad_glGenTextures
#define glDeleteTextures        glad_glDeleteTextures
#define glBindTexture           glad_glBindTexture
#define glTexImage2D            glad_glTexImage2D
#define glTexSubImage2D         glad_glTexSubImage2D
#define glTexParameteri         glad_glTexParameteri
#define glActiveTexture         glad_glActiveTexture
#define glGenBuffers            glad_glGenBuffers
#define glDeleteBuffers         glad_glDeleteBuffers
#define glBindBuffer            glad_glBindBuffer
#define glBufferData            glad_glBufferData
#define glGenVertexArrays       glad_glGenVertexArrays
#define glDeleteVertexArrays    glad_glDeleteVertexArrays
#define glBindVertexArray       glad_glBindVertexArray
#define glEnableVertexAttribArray  glad_glEnableVertexAttribArray
#define glDisableVertexAttribArray glad_glDisableVertexAttribArray
#define glVertexAttribPointer   glad_glVertexAttribPointer
#define glCreateShader          glad_glCreateShader
#define glShaderSource          glad_glShaderSource
#define glCompileShader         glad_glCompileShader
#define glGetShaderiv           glad_glGetShaderiv
#define glGetShaderInfoLog      glad_glGetShaderInfoLog
#define glDeleteShader          glad_glDeleteShader
#define glCreateProgram         glad_glCreateProgram
#define glAttachShader          glad_glAttachShader
#define glLinkProgram           glad_glLinkProgram
#define glGetProgramiv          glad_glGetProgramiv
#define glGetProgramInfoLog     glad_glGetProgramInfoLog
#define glDeleteProgram         glad_glDeleteProgram
#define glUseProgram            glad_glUseProgram
#define glGetUniformLocation    glad_glGetUniformLocation
#define glUniform1i             glad_glUniform1i
#define glUniform1f             glad_glUniform1f
#define glUniform2f             glad_glUniform2f
#define glUniform3f             glad_glUniform3f
#define glUniform4f             glad_glUniform4f
#define glUniformMatrix4fv      glad_glUniformMatrix4fv
#define glDrawElements          glad_glDrawElements
#define glDrawArrays            glad_glDrawArrays
#define glGetError              glad_glGetError
#define glGenFramebuffers       glad_glGenFramebuffers
#define glDeleteFramebuffers    glad_glDeleteFramebuffers
#define glBindFramebuffer       glad_glBindFramebuffer
#define glFramebufferTexture2D  glad_glFramebufferTexture2D
#define glCheckFramebufferStatus glad_glCheckFramebufferStatus
#define glGenRenderbuffers      glad_glGenRenderbuffers
#define glDeleteRenderbuffers   glad_glDeleteRenderbuffers
#define glBindRenderbuffer      glad_glBindRenderbuffer
#define glRenderbufferStorage   glad_glRenderbufferStorage
#define glFramebufferRenderbuffer glad_glFramebufferRenderbuffer
#define glReadPixels            glad_glReadPixels
#define glPixelStorei           glad_glPixelStorei
#define glGetStringi            glad_glGetStringi
#define glGenerateMipmap        glad_glGenerateMipmap
#define glScissor               glad_glScissor
#define glDepthRange            glad_glDepthRange
#define glClearDepth            glad_glClearDepth
#define glColorMask             glad_glColorMask
#define glDepthMask             glad_glDepthMask
#define glStencilMask           glad_glStencilMask
#define glGetIntegerv           glad_glGetIntegerv

#ifdef __cplusplus
}
#endif

#endif /* __glad_h_ */
