#if !defined(KNAVE_OPENGL_H)
#define KNAVE_OPENGL_H

#include <GL/gl.h>

/* #define EXPAND(RETURN, NAME, ARGUMENTS) */
#define KNAVE_OPENGL_FUNCTIONS(EXPAND)                                  \
	EXPAND(void, glActiveShaderProgram, (GLuint pipeline, GLuint program)) \
	EXPAND(void, glAttachShader, (GLuint program, GLuint shader)) \
	EXPAND(void, glBeginConditionalRender, (GLuint id, GLenum mode)) \
	EXPAND(void, glBeginQuery, (GLenum target, GLuint id)) \
	EXPAND(void, glBeginQueryIndexed, (GLenum target, GLuint index, GLuint id)) \
	EXPAND(void, glBeginTransformFeedback, (GLenum primitiveMode)) \
	EXPAND(void, glBindAttribLocation, (GLuint program, GLuint index, const GLchar *name)) \
	EXPAND(void, glBindBuffer, (GLenum target, GLuint buffer)) \
	EXPAND(void, glBindBufferBase, (GLenum target, GLuint index, GLuint buffer)) \
	EXPAND(void, glBindBufferRange, (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)) \
	EXPAND(void, glBindBuffersBase, (GLenum target, GLuint first, GLsizei count, const GLuint *buffers)) \
	EXPAND(void, glBindBuffersRange, (GLenum target, GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLintptr *sizes)) \
	EXPAND(void, glBindFragDataLocation, (GLuint program, GLuint colorNumber, const char * name)) \
	EXPAND(void, glBindFragDataLocationIndexed, (GLuint program, GLuint colorNumber, GLuint index, const char *name)) \
	EXPAND(void, glBindFramebuffer, (GLenum target, GLuint framebuffer)) \
	EXPAND(void, glBindImageTexture, (GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format)) \
	EXPAND(void, glBindImageTextures, (GLuint first, GLsizei count, const GLuint *textures)) \
	EXPAND(void, glBindProgramPipeline, (GLuint pipeline)) \
	EXPAND(void, glBindRenderbuffer, (GLenum target, GLuint renderbuffer)) \
	EXPAND(void, glBindSampler, (GLuint unit, GLuint sampler)) \
	EXPAND(void, glBindSamplers, (GLuint first, GLsizei count, const GLuint *samplers)) \
	EXPAND(void, glBindTextureUnit, (GLuint unit, GLuint texture)) \
	EXPAND(void, glBindTextures, (GLuint first, GLsizei count, const GLuint *textures)) \
	EXPAND(void, glBindTransformFeedback, (GLenum target, GLuint id)) \
	EXPAND(void, glBindVertexArray, (GLuint array)) \
	EXPAND(void, glBindVertexBuffer, (GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride)) \
	EXPAND(void, glBindVertexBuffers, (GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizei *strides)) \
	EXPAND(void, glBlendEquationi, (GLuint buf, GLenum mode)) \
	EXPAND(void, glBlendEquationSeparate, (GLenum modeRGB, GLenum modeAlpha)) \
	EXPAND(void, glBlendEquationSeparatei, (GLuint buf, GLenum modeRGB, GLenum modeAlpha)) \
	EXPAND(void, glBlendFunci, (GLuint buf, GLenum sfactor, GLenum dfactor)) \
	EXPAND(void, glBlendFuncSeparate, (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)) \
	EXPAND(void, glBlendFuncSeparatei, (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)) \
	EXPAND(void, glBlitFramebuffer, (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)) \
	EXPAND(void, glBlitNamedFramebuffer, (GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)) \
	EXPAND(void, glBufferData, (GLenum target, GLsizeiptr size, const void * data, GLenum usage)) \
	EXPAND(void, glBufferStorage, (GLenum target, GLsizeiptr size, const void * data, GLbitfield flags)) \
	EXPAND(void, glBufferSubData, (GLenum target, GLintptr offset, GLsizeiptr size, const void * data)) \
	EXPAND(void, glEndQuery, (GLenum target)) \
	EXPAND(void, glEndQueryIndexed, (GLenum target, GLuint index)) \
	EXPAND(void, glEndTransformFeedback, (void)) \
	EXPAND(void, glNamedBufferData, (GLuint buffer, GLsizeiptr size, const void *data, GLenum usage)) \
	EXPAND(void, glNamedBufferStorage, (GLuint buffer, GLsizeiptr size, const void *data, GLbitfield flags)) \
	EXPAND(void, glNamedBufferSubData, (GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data)) \
	EXPAND(void, glVertexArrayVertexBuffer, (GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride)) \
	EXPAND(void, glVertexArrayVertexBuffers, (GLuint vaobj, GLuint first, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLsizei *strides))

#define EXPAND(RETURN, NAME, ARGUMENTS) \
	extern RETURN (*NAME)ARGUMENTS;
KNAVE_OPENGL_FUNCTIONS(EXPAND)
#undef EXPAND

int knave_opengl_library_load(void);

#endif /* KNAVE_OPENGL_H */

#if defined(KNAVE_OPENGL_IMPLEMENTATION)
#undef KNAVE_OPENGL_IMPLEMENTATION

#define EXPAND(RETURN, NAME, ARGUMENTS) \
	RETURN (*NAME)ARGUMENTS;
KNAVE_OPENGL_FUNCTIONS(EXPAND)
#undef EXPAND

void knave_error(const char *format, ...);

#if defined(__linux__)

int knave_opengl_library_load(void)
{
#	define EXPAND(RETURN, NAME, ARGUMENTS) \
		NAME = (RETURN (*)ARGUMENTS)glXGetProcAddressARB((const GLubyte *)#NAME); \
		if (NAME == NULL) { \
			knave_error("Failed to load OpenGL function: " #NAME); \
			return -1; \
		}
	KNAVE_OPENGL_FUNCTIONS(EXPAND)
#	undef EXPAND
	return 0;
}

#endif /* __linux__ */

#endif /* KNAVE_OPENGL_IMPLEMENTATION */
