#include "Renderer2D.h"

Renderer2D* Renderer2D_Init(Renderer2D* inst)
{
	if (inst)
	{
		//Create buffers.
		inst->quadVBO = GLBuffer_Create(MAX_QUADS * 4 * sizeof(QuadVertex), GLBufferFlags_STREAM, NULL);
		inst->quadIBO = GLBuffer_Create(MAX_QUADS * 6 * sizeof(uint32_t), GLBufferFlags_NONE, NULL);

		//Fill index buffer with the predictable indices.
		uint32_t* iboPtr = GLBuffer_BeginWriteRange(inst->quadIBO, 0, inst->quadIBO->Size);
		for (uint32_t i = 0; i < MAX_QUADS * 6; i += 6)
		{
			iboPtr[i + 0] = i + 0;
			iboPtr[i + 1] = i + 1;
			iboPtr[i + 2] = i + 2;
			iboPtr[i + 3] = i + 2;
			iboPtr[i + 4] = i + 3;
			iboPtr[i + 5] = i + 0;
		};
		GLBuffer_EndWriteRange(inst->quadIBO);
		//Get the pointer to the persistently mapped vbo.
		inst->Base = GLBuffer_BeginWriteRange(inst->quadVBO, 0, inst->quadVBO->Size);

		//Setup vertex attributes.
		vertexAttribute_t attribs[] =
		{
			vertexAttribute(0, 0, GLDataType_FLOAT, 3, false, offsetof(QuadVertex, Pos)),
			vertexAttribute(0, 1, GLDataType_FLOAT, 1, false, offsetof(QuadVertex, Tex)),
			vertexAttribute(0, 2, GLDataType_FLOAT, 4, false, offsetof(QuadVertex, Col)),
			vertexAttribute(0, 3, GLDataType_FLOAT, 2, false, offsetof(QuadVertex, tUV)),
		};
		inst->quadVAO = GLVertexArray_Create(attribs, 4);
		GLVertexArray_BindBuffer(inst->quadVAO, 0, inst->quadVBO, 0, sizeof(QuadVertex), GLAttribDiv_PERVERTEX);

		//Load and compile shader.
		const char vertexsource[] =
			"#version 450 core\n"
			"\n"
			"layout(location = 0) in vec3 Pos;\n"
			"layout(location = 1) in float Tex;\n"
			"layout(location = 2) in vec4 Col;\n"
			"layout(location = 3) in vec2 tUV;\n"
			"\n"
			"layout(location = 0) uniform mat4 MVP;\n"
			"\n"
			"layout(location = 0) out vec4 vCol;\n"
			"layout(location = 1) out vec2 vtUV;\n"
			"layout(location = 2) out float vTex;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	gl_Position = MVP * vec4(Pos, 0);\n"
			"	vCol = Col;\n"
			"	vtUV = tUV;\n"
			"	vTex = Tex;\n"
			"}\n";
		const char fragmentsource[] =
			"#version 450 core\n"
			"\n"
			"layout(location = 0) in vec4 vCol;\n"
			"layout(location = 1) in vec2 vtUV;\n"
			"layout(location = 2) in float vTex;\n"
			"\n"
			"layout(binding = 0) uniform sampler2D textures[32];\n"
			"\n"
			"layout(location = 0) out vec4 oCol;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	oCol = vCol * texture(textures[int(vTex)], vtUV);\n"
			"}\n";
		shaderSource_t sources[] =
		{
			{ GlShaderType_VERT, vertexsource },
			{ GlShaderType_FRAG, fragmentsource },
		};
		inst->quadShader = GLShader_Create(sources, 2);

		//Upload default white texture.
		unsigned char whitetexdata[] = { 0xFF, 0xFF, 0xFF, 0xFF };
		inst->WhiteTex = GLTexture_Create(GLTextureType_2D, GLFormat_RGBA, new_uvec3(1, 1, 1), 1);
		GLTexture_Upload(inst->WhiteTex, 0, GLFormat_RGBA, new_uvec3(0, 0, 0), inst->WhiteTex->Size, whitetexdata);

	}
	return inst;
}

void Renderer2D_Destroy(Renderer2D* inst)
{
	if (inst)
	{
		GLBuffer_Destroy(inst->quadVBO);
		GLBuffer_Destroy(inst->quadIBO);
		GLVertexArray_Destroy(inst->quadVAO);
		GLShader_Destroy(inst->quadShader);
		GLTexture_Destroy(inst->WhiteTex);
	}
}

void Renderer2D_BeginBatch(Renderer2D* inst)
{
	if (inst) 
	{
		//Reset textures.
		inst->lastTex = 1;
		for (int i = 0; i < MAX_TEXTURES; i++) 
		{
			inst->textures[i] = inst->WhiteTex;
		}

		//Reset vertex data.
		inst->quadCount = 0;
		inst->Head = inst->Base;
	}
}

void Renderer2D_EndBatch(Renderer2D* inst)
{
	if(inst) 
	{
		//Setup render state.
		GLShader_Bind(inst->quadShader);
		GLVertexArray_Bind(inst->quadVAO);
		GLBuffer_BindTarget(inst->quadIBO, GLBufferTarget_INDEXBUFFER);

		//Bind textures.
		for (int i = 0; i < MAX_TEXTURES; i++) 
		{
			GLTexture_BindUnit(inst->textures[i], i);
		}

		//Set MVP matrix.
		GLShader_UniformMat4fv(inst->quadShader, 0, 1, &inst->Camera);

		glDrawElements(GL_TRIANGLES, inst->quadCount * 6, GL_UNSIGNED_INT, NULL);
	}
}

void Renderer2D_NextBatch(Renderer2D* inst)
{
	Renderer2D_EndBatch(inst);
	Renderer2D_BeginBatch(inst);
}
