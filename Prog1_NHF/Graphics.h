#pragma once

#include "Core.h"

#include "Mathcell.h"

#include <glad/glad.h>
#include <stdint.h>
#include <string.h>

/*
*	Buffer
*/

typedef enum GLBufferFlags
{
	GLBufferFlags_NONE = 0, //Nothing
	GLBufferFlags_STREAM = 1, //Persistently mapped coherent.
} GLBufferFlags;

typedef enum GLBufferTarget
{
	GLBufferTarget_NONE = 0,
	GLBufferTarget_VERTEXBUFFER = GL_ARRAY_BUFFER,
	GLBufferTarget_INDEXBUFFER = GL_ELEMENT_ARRAY_BUFFER,
	GLBufferTarget_UNIFORMBUFFER = GL_UNIFORM_BUFFER,
	GLBufferTarget_STORAGEBUFFER = GL_SHADER_STORAGE_BUFFER,
} GLBufferTarget;

typedef struct GLBuffer
{
	const size_t Size;
	const GLBufferFlags flags;

	struct
	{
		const uint32_t NativeHandle;
		void* Data_ptr;
	} _Priv;
} GLBuffer;

GLBuffer* GLBuffer_Create(size_t size, GLBufferFlags flags, const void* Init_Data);
void GLBuffer_Destroy(GLBuffer* ptr);

void GLBuffer_Upload(GLBuffer* ptr, ptrdiff_t offset, size_t size, const void* data);
void* GLBuffer_BeginWriteRange(GLBuffer* ptr, ptrdiff_t offset, size_t size);
void GLBuffer_EndWriteRange(GLBuffer* ptr);

void GLBuffer_BindTarget(GLBuffer* ptr, GLBufferTarget target);
void GLBuffer_BindTargetBase(GLBuffer* ptr, GLBufferTarget target, uint32_t index);
void GLBuffer_BindTargetRange(GLBuffer* ptr, GLBufferTarget target, uint32_t index, ptrdiff_t offset, size_t size);

/*
*	Shader
*/

typedef enum GlShaderType
{
	GlShaderType_NONE = 0,
	GlShaderType_VERT = GL_VERTEX_SHADER,
	GlShaderType_TESC = GL_TESS_CONTROL_SHADER,
	GlShaderType_TESE = GL_TESS_EVALUATION_SHADER,
	GlShaderType_GEOM = GL_GEOMETRY_SHADER,
	GlShaderType_FRAG = GL_FRAGMENT_SHADER,
	GlShaderType_COMP = GL_COMPUTE_SHADER,
} GlShaderType;

typedef struct shaderSource_t 
{
	GlShaderType type;
	const char* source;
} shaderSource_t;

typedef struct GLShader 
{
	struct 
	{
		const uint32_t NativeHandle;
	} _Priv;
} GLShader;

GLShader* GLShader_Create(size_t count, const shaderSource_t* sources);
void GLShader_Destroy(GLShader* ptr);

void GLShader_Uniform1fv	(GLShader* ptr, int loc, size_t count, const float* val);
void GLShader_Uniform2fv	(GLShader* ptr, int loc, size_t count, const vec2* val);
void GLShader_Uniform3fv	(GLShader* ptr, int loc, size_t count, const vec3* val);
void GLShader_Uniform4fv	(GLShader* ptr, int loc, size_t count, const vec4* val);
void GLShader_UniformMat3fv	(GLShader* ptr, int loc, size_t count, const mat3* val);
void GLShader_UniformMat4fv	(GLShader* ptr, int loc, size_t count, const mat4* val);
