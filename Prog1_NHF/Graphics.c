#include "Graphics.h"

GLBuffer* GLBuffer_Create(size_t size, GLBufferFlags flags, const void* Init_Data) 
{
	GLBuffer temp = {
		size, flags //Init const members.
	};

	GLBuffer* ret = malloc(sizeof(GLBuffer));
	memcpy(ret, &temp, sizeof(GLBuffer));

	//Create gl object.
	glGenBuffers(1, &ret->_Priv.NativeHandle);

	//Init storage.
	if (flags & GLBufferFlags_STREAM) 
	{
		//Create storage and init data.
		glNamedBufferStorage(ret->_Priv.NativeHandle, ret->Size, Init_Data,
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
		);

		//Map buffer.
		ret->_Priv.Data_ptr = glMapNamedBufferRange(ret->_Priv.NativeHandle, 0, ret->Size,
			GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT
		);

	} else
	{
		//Create storage and init data.
		glNamedBufferStorage(ret->_Priv.NativeHandle, ret->Size, Init_Data,
			GL_MAP_WRITE_BIT
		);
	}
}

void GLBuffer_Destroy(GLBuffer* ptr) 
{
	if (!ptr) return;
	
	if (ptr->flags & GLBufferFlags_STREAM) glUnmapNamedBuffer(ptr->_Priv.NativeHandle);
	glDeleteBuffers(1, ptr->_Priv.NativeHandle);
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
	if (!ptr) return;

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
