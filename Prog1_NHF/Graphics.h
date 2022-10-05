#pragma once

#include "Core.h"

#include "Mathcell.h"

#include <glad/glad.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void GLEnableDebugOutput();

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
	GLBufferTarget_VERTEXBUFFER		= GL_ARRAY_BUFFER,
	GLBufferTarget_INDEXBUFFER		= GL_ELEMENT_ARRAY_BUFFER,
	GLBufferTarget_UNIFORMBUFFER	= GL_UNIFORM_BUFFER,
	GLBufferTarget_STORAGEBUFFER	= GL_SHADER_STORAGE_BUFFER,
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

GLShader* GLShader_Create(const shaderSource_t* sources, size_t source_count);
void GLShader_Destroy(GLShader* ptr);

void GLShader_Uniform1fv	(GLShader* ptr, int loc, size_t count, const float* val);
void GLShader_Uniform2fv	(GLShader* ptr, int loc, size_t count, const vec2* val);
void GLShader_Uniform3fv	(GLShader* ptr, int loc, size_t count, const vec3* val);
void GLShader_Uniform4fv	(GLShader* ptr, int loc, size_t count, const vec4* val);
void GLShader_UniformMat3fv	(GLShader* ptr, int loc, size_t count, const mat3* val);
void GLShader_UniformMat4fv	(GLShader* ptr, int loc, size_t count, const mat4* val);

void GLShader_Bind(GLShader* ptr);

/*
*	Vertex Array Object
*/

typedef enum GLDataType
{
	GLDataType_NONE = 0,
	GLDataType_BYTE			= GL_BYTE,
	GLDataType_UBYTE		= GL_UNSIGNED_BYTE,
	GLDataType_SHORT		= GL_SHORT,

	GLDataType_USHORT		= GL_UNSIGNED_SHORT,
	GLDataType_INT			= GL_INT,
	GLDataType_UINT			= GL_UNSIGNED_INT,

	GLDataType_HALF_FLOAT	= GL_HALF_FLOAT,
	GLDataType_FLOAT		= GL_FLOAT,
	GLDataType_DOUBLE		= GL_DOUBLE,
} GLDataType;

typedef enum GLAttribDiv 
{
	GLAttribDiv_PERVERTEX = 0,
	GLAttribDiv_PERINSTANCE = 1,
} GLAttribDiv;

typedef struct vertexAttribute_t 
{
	uint32_t bindingIndex;
	uint32_t attribute;
	GLDataType type;
	uint32_t size;
	bool normalize;
	ptrdiff_t offset;
} vertexAttribute_t;

typedef struct GLVertexArray 
{
	struct
	{
		const uint32_t NativeHandle;
	} _Priv;
} GLVertexArray;

GLVertexArray* GLVertexArray_Create(const vertexAttribute_t* attribs, size_t attrib_count);
void GLVertexArray_Destroy(GLVertexArray* ptr);

void GLVertexArray_Bind(GLVertexArray* ptr);
void GLVertexArray_BindBuffer(GLVertexArray* ptr, uint32_t binding, GLBuffer* buffer, ptrdiff_t offset, size_t stride, GLAttribDiv divisor);
