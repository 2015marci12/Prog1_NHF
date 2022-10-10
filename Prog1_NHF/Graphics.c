#include "Graphics.h"

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		TRACE("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
		break;
	case GL_DEBUG_SEVERITY_LOW:
		INFO("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		WARN("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
		break;
	case GL_DEBUG_SEVERITY_HIGH:
	default:
		ERROR("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
		break;
	}
}

void GLEnableDebugOutput()
{
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
}

GLBuffer* GLBuffer_Create(size_t size, GLBufferFlags flags, const void* Init_Data)
{
	//Create gl object.
	uint32_t buffer;
	glCreateBuffers(1, &buffer);

	void* dataptr = NULL;

	//Init storage.
	if (flags & GLBufferFlags_STREAM) 
	{
		//Create storage and init data.
		glNamedBufferStorage(buffer, size, Init_Data,
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
		);

		//Map buffer.
		dataptr = glMapNamedBufferRange(buffer, 0, size,
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
		);

	} else
	{
		//Create storage and init data.
		glNamedBufferStorage(buffer, size, Init_Data,
			GL_MAP_WRITE_BIT
		);
	}

	GLBuffer temp = {
		size, flags,
		{ buffer, dataptr }
	}; //Init const members.

	GLBuffer* ret = malloc(sizeof(GLBuffer));
	memcpy(ret, &temp, sizeof(GLBuffer));

	return ret;
}

void GLBuffer_Destroy(GLBuffer* ptr) 
{
	if (!ptr) return;
	
	if (ptr->flags & GLBufferFlags_STREAM) glUnmapNamedBuffer(ptr->_Priv.NativeHandle);
	glDeleteBuffers(1, &ptr->_Priv.NativeHandle);
	free(ptr);
}

void GLBuffer_Upload(GLBuffer* ptr, ptrdiff_t offset, size_t size, const void* data) 
{
	if (!ptr) return;

	if (ptr->flags & GLBufferFlags_STREAM) memcpy((char*)ptr->_Priv.Data_ptr + offset, data, size);
	else 
	{
		void* mem_ptr = glMapNamedBufferRange(ptr->_Priv.NativeHandle, offset, size, GL_MAP_WRITE_BIT);
		memcpy(mem_ptr, data, size);
		glUnmapNamedBuffer(ptr->_Priv.NativeHandle);
	}
}

void* GLBuffer_BeginWriteRange(GLBuffer* ptr, ptrdiff_t offset, size_t size) 
{
	if (!ptr) return NULL;

	if (ptr->flags & GLBufferFlags_STREAM) return (char*)ptr->_Priv.Data_ptr + offset;
	else return glMapNamedBufferRange(ptr->_Priv.NativeHandle, offset, size, GL_MAP_WRITE_BIT);
}
void GLBuffer_EndWriteRange(GLBuffer* ptr) 
{
	if (!ptr) return;

	glUnmapNamedBuffer(ptr->_Priv.NativeHandle);
}

void GLBuffer_BindTarget(GLBuffer* ptr, GLBufferTarget target) 
{
	if (!ptr) { glBindBuffer(target, 0); return; }

	glBindBuffer(target, ptr->_Priv.NativeHandle);
}

void GLBuffer_BindTargetBase(GLBuffer* ptr, GLBufferTarget target, uint32_t index)
{
	if (!ptr) return;

	glBindBufferBase(target, index, ptr->_Priv.NativeHandle);
}

void GLBuffer_BindTargetRange(GLBuffer* ptr, GLBufferTarget target, uint32_t index, ptrdiff_t offset, size_t size) {
	if (!ptr) return;

	glBindBufferRange(target, index, ptr->_Priv.NativeHandle, offset, size);
}

GLShader* GLShader_Create(const shaderSource_t* sources, size_t source_count)
{
	uint32_t program;
	uint32_t shaders[6];
	ASSERT((source_count < 7) && (source_count > 0), "Invalid number of shaders!");
	bool noErrors = true;

	//Create program and compile shaders.
	program = glCreateProgram();
	for (int i = 0; i < source_count; i++) 
	{
		ASSERT(sources[i].source && sources[i].type, "Shader is invalid!");

		//Create shader.
		shaders[i] = glCreateShader(sources[i].type);

		//Upload source.
		uint32_t length = (uint32_t)strlen(sources[i].source);
		glShaderSource(shaders[i], 1, &sources[i].source, &length);

		//Compile.
		glCompileShader(shaders[i]);

		//Check for errors.
		int success = GL_TRUE;
		glGetShaderiv(shaders[i], GL_COMPILE_STATUS, &success);
		if (!success) 
		{
			noErrors = false;

			char buff[4096] = { '\0' };
			uint32_t len = 0;
			glGetShaderInfoLog(shaders[i], 4096, &len, buff);
			ERROR("Shader compilation error: %s\n", buff);
		}

		glAttachShader(program, shaders[i]);
	}

	//Link shader.
	if (noErrors) 
	{
		glLinkProgram(program);

		//Check for errors.
		int success = GL_TRUE;
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) 
		{
			noErrors = false;

			char buff[4096] = { '\0' };
			uint32_t len = 0;
			glGetProgramInfoLog(program, 4096, &len, buff);
			ERROR("Shader Link error: %s\n", buff);
		}
	}

	//Detach and destroy shaders.
	for (int i = 0; i < source_count; i++)
	{
		glDetachShader(program, shaders[i]);
		glDeleteShader(shaders[i]);
	}

	//Delete on error.
	if (!noErrors) { glDeleteProgram(program); return NULL; }

	//Allocate and initialize the shader.
	GLShader shader = { { program } };

	GLShader* ret = malloc(sizeof(GLShader));
	memcpy(ret, &shader, sizeof(GLShader));
	return ret;
}

void GLShader_Destroy(GLShader* ptr)
{
	if (!ptr) return;

	glDeleteProgram(ptr->_Priv.NativeHandle);
	free(ptr);
}

void GLShader_Uniform1fv(GLShader* ptr, int loc, size_t count, const float* val)
{
	if (!ptr) return;

	glProgramUniform1fv(ptr->_Priv.NativeHandle, loc, (GLsizei)count, val);
}

void GLShader_Uniform2fv(GLShader* ptr, int loc, size_t count, const vec2* val)
{
	if (!ptr) return;

	glProgramUniform2fv(ptr->_Priv.NativeHandle, loc, (GLsizei)count, (float*)val);
}

void GLShader_Uniform3fv(GLShader* ptr, int loc, size_t count, const vec3* val)
{
	if (!ptr) return;

	glProgramUniform3fv(ptr->_Priv.NativeHandle, loc, (GLsizei)count, (float*)val);
}

void GLShader_Uniform4fv(GLShader* ptr, int loc, size_t count, const vec4* val)
{
	if (!ptr) return;

	glProgramUniform4fv(ptr->_Priv.NativeHandle, loc, (GLsizei)count, (float*)val);
}

void GLShader_UniformMat3fv(GLShader* ptr, int loc, size_t count, const mat3* val)
{
	if (!ptr) return;

	glProgramUniformMatrix3fv(ptr->_Priv.NativeHandle, loc, (GLsizei)count, GL_FALSE, (float*)val);
}

void GLShader_UniformMat4fv(GLShader* ptr, int loc, size_t count, const mat4* val)
{
	if (!ptr) return;

	glProgramUniformMatrix4fv(ptr->_Priv.NativeHandle, loc, (GLsizei)count, GL_FALSE, (float*)val);
}

void GLShader_Bind(GLShader* ptr)
{
	if (ptr) glUseProgram(ptr->_Priv.NativeHandle);
	else glUseProgram(0);
}

GLVertexArray* GLVertexArray_Create(const vertexAttribute_t* attribs, size_t attrib_count)
{
	uint32_t vao;

	//Create vertex array.
	glCreateVertexArrays(1, &vao);

	//Set attribute formats.
	for (int i = 0; i < attrib_count; i++) 
	{
		ASSERT(attribs[i].type, "Invalid attribute type: NONE!");

		glEnableVertexArrayAttrib(vao, attribs[i].attribute);

		switch (attribs[i].type)
		{
			case GLDataType_BYTE	  :
			case GLDataType_UBYTE	  :
			case GLDataType_SHORT	  :
			case GLDataType_USHORT	  :
			case GLDataType_INT		  :
			case GLDataType_UINT	  :
				if (attribs[i].normalize) glVertexArrayAttribFormat(vao, attribs[i].attribute, attribs[i].size, attribs[i].type, GL_TRUE, (GLuint)attribs[i].offset);
				else glVertexArrayAttribIFormat(vao, attribs[i].attribute, attribs[i].size, attribs[i].type, (GLuint)attribs[i].offset);
				glVertexArrayAttribBinding(vao, attribs[i].attribute, attribs[i].bindingIndex);
				break;
			case GLDataType_HALF_FLOAT:
			case GLDataType_FLOAT	  :
				glVertexArrayAttribFormat(vao, attribs[i].attribute, attribs[i].size, attribs[i].type, attribs[i].normalize, (GLuint)attribs[i].offset);
				break;
			case GLDataType_DOUBLE	  :
				glVertexArrayAttribLFormat(vao, attribs[i].attribute, attribs[i].size, attribs[i].type, (GLuint)attribs[i].offset);
				break;
		default:
			break;
		}

		glVertexArrayAttribBinding(vao, attribs[i].attribute, attribs[i].bindingIndex);
	}

	GLVertexArray temp = { { vao } };
	GLVertexArray* ret = malloc(sizeof(GLVertexArray));
	memcpy(ret, &temp, sizeof(GLVertexArray));
	return ret;
}

void GLVertexArray_Destroy(GLVertexArray* ptr)
{
	if (!ptr) return;

	glDeleteVertexArrays(1, &ptr->_Priv.NativeHandle);
	free(ptr);
}

void GLVertexArray_Bind(GLVertexArray* ptr)
{
	if (ptr) glBindVertexArray(ptr->_Priv.NativeHandle);
	else glBindVertexArray(0);
}

void GLVertexArray_BindBuffer(GLVertexArray* ptr, uint32_t binding, GLBuffer* buffer, ptrdiff_t offset, size_t stride, GLAttribDiv divisor)
{
	if (!ptr) return;

	glVertexArrayBindingDivisor(ptr->_Priv.NativeHandle, binding, divisor);
	glVertexArrayVertexBuffer(ptr->_Priv.NativeHandle, binding, (buffer) ? buffer->_Priv.NativeHandle : 0, offset, (GLsizei)stride);
}

GLTexture* GLTexture_Create(GLTextureType type, GLFormat format, uvec3 size, uint32_t levels)
{
	uint32_t tex = 0;
	glCreateTextures(type, 1, &tex);
	switch (type)
	{
	case GLTextureType_1D: glTextureStorage1D(tex, levels, format, size.x); break;
	case GLTextureType_2D: glTextureStorage2D(tex, levels, format, size.x, size.y); break;
	case GLTextureType_3D: glTextureStorage3D(tex, levels, format, size.x, size.y, size.z); break;
	case GLTextureType_Cube: glTextureStorage2D(tex, levels, format, size.x, size.y); break;
	default:
		break;
	}
	GLTexture temp = { type, format, size,{ tex } };
	GLTexture* ret = malloc(sizeof(GLTexture));
	memcpy(ret, &temp, sizeof(GLTexture));
	return ret;
}

void GLTexture_Destroy(GLTexture* ptr)
{
	if (!ptr) return;
	glDeleteTextures(1, &ptr->_Priv.NativeHandle);
	free(ptr);
}

void GLTexture_Upload(GLTexture* ptr, uint32_t level, GLFormat pixelFormat, uvec3 offset, uvec3 size, const void* data)
{
	if (!ptr) return;
	GLenum type = 0;
	GLenum format = 0;
	switch (pixelFormat)
	{
	case GLFormat_RGBA:		type = GL_UNSIGNED_BYTE; format = GL_RGBA; break;
	case GLFormat_BGRA: 	type = GL_UNSIGNED_BYTE; format = GL_BGRA; break;
	case GLFormat_RGB:		type = GL_UNSIGNED_BYTE; format = GL_RGB; break;
	case GLFormat_R32UI:	type = GL_UNSIGNED_INT; format = GL_R; break;
	case GLFormat_R32F:		type = GL_FLOAT; format = GL_R; break;
	case GLFormat_RGBA32F:	type = GL_FLOAT; format = GL_RGBA; break;
	case GLFormat_RGB32F:	type = GL_FLOAT; format = GL_RGB; break;
	case GLFormat_NONE:
	default:
		break;
	}

	switch (ptr->Type)
	{
	case GLTextureType_1D:
		glTextureSubImage1D(ptr->_Priv.NativeHandle, level, offset.x, size.x, format, type, data); break;
	case GLTextureType_2D:
		glTextureSubImage2D(ptr->_Priv.NativeHandle, level, offset.x, offset.y, size.x, size.y, format, type, data); break;
	case GLTextureType_3D:
		glTextureSubImage3D(ptr->_Priv.NativeHandle, level, offset.x, offset.y, offset.z, size.x, size.y, size.z, format, type, data); break;
	case GLTextureType_Cube:
		glTextureSubImage2D(ptr->_Priv.NativeHandle, level, offset.x, offset.y, size.x, size.y, format, type, data); break;
	default:
		break;
	}
}

void GLTexture_BindUnit(GLTexture* ptr, uint32_t unit)
{
	if (!ptr) return;
	glBindTextures(unit, 1, &ptr->_Priv.NativeHandle);
}
