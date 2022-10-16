#include "Renderer2D.h"

Renderer2D* Renderer2D_Init(Renderer2D* inst)
{
	if (inst)
	{
		/*
		* Quads.
		*/

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
		inst->quadBase = GLBuffer_BeginWriteRange(inst->quadVBO, 0, inst->quadVBO->Size);

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
			"	gl_Position = MVP * vec4(Pos, 1);\n"
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

		inst->quadCount = 0;

		/*
		* Lines.
		*/

		inst->lineVBO = GLBuffer_Create(MAX_LINES * 2 * sizeof(LineVertex), GLBufferFlags_STREAM, NULL);
		inst->lineBase = GLBuffer_BeginWriteRange(inst->lineVBO, 0, inst->lineVBO->Size);
		inst->lineHead = inst->lineBase;

		vertexAttribute_t lineAttribs[] =
		{
			vertexAttribute(0, 0, GLDataType_FLOAT, 3, false, offsetof(LineVertex, Pos)),
			vertexAttribute(0, 1, GLDataType_FLOAT, 4, false, offsetof(LineVertex, Col)),
		};
		inst->lineVAO = GLVertexArray_Create(lineAttribs, 2);
		GLVertexArray_BindBuffer(inst->lineVAO, 0, inst->lineVBO, 0, sizeof(LineVertex), GLAttribDiv_PERVERTEX);

		const char linevertexsource[] =
			"#version 450 core\n"
			"\n"
			"layout(location = 0) in vec3 Pos;\n"
			"layout(location = 1) in vec4 Col;\n"
			"\n"
			"layout(location = 0) uniform mat4 MVP;\n"
			"\n"
			"layout(location = 0) out vec4 vCol;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	gl_Position = MVP * vec4(Pos, 1);\n"
			"	vCol = Col;\n"
			"}\n";
		const char linefragmentsource[] =
			"#version 450 core\n"
			"\n"
			"layout(location = 0) in vec4 vCol;\n"
			"\n"
			"layout(location = 0) out vec4 oCol;\n"
			"\n"
			"void main()\n"
			"{\n"
			"	oCol = vCol;\n"
			"}\n";
		shaderSource_t linesources[] =
		{
			{ GlShaderType_VERT, linevertexsource },
			{ GlShaderType_FRAG, linefragmentsource },
		};
		inst->lineShader = GLShader_Create(linesources, 2);

		inst->lineWidth = 1.f;

		inst->lineCount = 0;

		inst->Camera = mat4x4_Identity();
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

		GLBuffer_Destroy(inst->lineVBO);
		GLVertexArray_Destroy(inst->lineVAO);
		GLShader_Destroy(inst->lineShader);
	}
}

void Renderer2D_BeginBatch(Renderer2D* inst)
{
	if (inst)
	{
		//Reset textures.
		inst->lastTex = 0;
		for (int i = 0; i < MAX_TEXTURES; i++)
		{
			inst->textures[i] = inst->WhiteTex;
		}

		//Reset vertex data.
		inst->quadCount = 0;
		inst->quadHead = inst->quadBase;

		//Reset lines.
		inst->lineCount = 0;
		inst->lineHead = inst->lineBase;
	}
}

void Renderer2D_EndBatch(Renderer2D* inst)
{
	if (inst)
	{
		//Setup render state.

		//Blending.
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

		//Depth.
		glEnable(GL_DEPTH_TEST);

		//Line width.
		glLineWidth(inst->lineWidth);

		//Quads.
		if (inst->quadCount)
		{
			//Bind objects.
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

		//Lines.
		if (inst->lineCount) 
		{
			//Bind objects.
			GLShader_Bind(inst->lineShader);
			GLVertexArray_Bind(inst->lineVAO);

			//Set MVP matrix.
			GLShader_UniformMat4fv(inst->lineShader, 0, 1, &inst->Camera);

			glDrawArrays(GL_LINES, 0, inst->lineCount * 2);
		}
	}
}

void Renderer2D_NextBatch(Renderer2D* inst)
{
	Renderer2D_EndBatch(inst);
	Renderer2D_BeginBatch(inst);
}

int Renderer2D_AddTexture(Renderer2D* inst, GLTexture* tex)
{
	ASSERT(inst, "Renderer functions cannot work without an instance!");
	if (!tex) return 0;
	if (++(inst->lastTex) > MAX_TEXTURES - 1)
	{
		Renderer2D_NextBatch(inst);
		return Renderer2D_AddTexture(inst, tex);
	}
	else
	{
		inst->textures[inst->lastTex] = tex;
		return inst->lastTex;
	}
}

void Renderer2D_BeginScene(Renderer2D* inst, mat4 camera)
{
	ASSERT(inst, "Renderer functions cannot work without an instance!");

	if (inst->quadCount > 0) Renderer2D_NextBatch(inst);
	else Renderer2D_BeginBatch(inst);

	inst->Camera = camera;
}

void Renderer2D_EndScene(Renderer2D* inst)
{
	ASSERT(inst, "Renderer functions cannot work without an instance!");
	Renderer2D_EndBatch(inst);
}

void Renderer2D_Clear(Renderer2D* inst, vec4 color)
{
	ASSERT(inst, "Renderer functions cannot work without an instance!");
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer2D_SetLineWidth(Renderer2D* inst, float width)
{
	ASSERT(inst, "Renderer functions cannot work without an instance!");
	inst->lineWidth = width;
}

void Renderer2D_DrawQuad(Renderer2D* inst, mat4 transform, vec4 color, GLTexture* texture, Rect texrect)
{
	ASSERT(inst, "Renderer functions cannot work without an instance!");

	const vec4 quadVertexPos[] =
	{
		new_vec4(-0.5f, -0.5f, 0, 0),
		new_vec4(-0.5f, 0.5f, 0, 0),
		new_vec4(0.5f, 0.5f, 0, 0),
		new_vec4(0.5f, -0.5f, 0, 0)
	};

	const vec2 texCoords[] =
	{
		texrect.Pos,
		vec2_Add(texrect.Pos, new_vec2(texrect.Size.x, 0)),
		vec2_Add(texrect.Pos, texrect.Size),
		vec2_Add(texrect.Pos, new_vec2(0, texrect.Size.y)),
	};

	int texInd = Renderer2D_AddTexture(inst, texture);
	if (inst->quadCount + 1 >= MAX_QUADS) Renderer2D_BeginBatch(inst);

	for (int i = 0; i < 4; i++)
	{
		QuadVertex* v = inst->quadHead;
		vec4 temp = mat4x4_Mul_v(transform, quadVertexPos[i]);
		v->Pos = new_vec3(temp.x, temp.y, temp.z);
		v->Tex = (float)texInd;
		v->Col = color;
		v->tUV = texCoords[i];
		inst->quadHead++;
	}
	inst->quadCount++;
}

void Renderer2D_DrawSprite(Renderer2D* inst, mat4 transform, vec4 tint, SubTexture subtex)
{
	Renderer2D_DrawQuad(inst, transform, tint, subtex.texture, subtex.texRect);
}

void Renderer2D_DrawLine(Renderer2D* inst, vec3 a, vec3 b, vec4 color)
{
	ASSERT(inst, "Renderer functions cannot work without an instance!");
	if (inst->lineCount + 1 >= MAX_LINES) Renderer2D_BeginBatch(inst);

	LineVertex* v = inst->lineHead;
	v->Pos = a;
	v->Col = color;
	inst->lineHead++;

	v = inst->lineHead;
	v->Pos = b;
	v->Col = color;
	inst->lineHead++;

	inst->lineCount++;
}

void Renderer2D_DrawRect(Renderer2D* inst, Rect rect, float z, vec4 color)
{
	const vec2 points[4] =
	{
		rect.Pos,
		new_vec2(rect.x + rect.w, rect.y),
		new_vec2(rect.x + rect.w, rect.y + rect.h),
		new_vec2(rect.x, rect.y + rect.h),
	};

	for(int i = 0; i < 4; i++) 
	{
		Renderer2D_DrawLine(inst, new_vec3_v2(points[i], z), new_vec3_v2(points[(i + 1) % 4], z), color);
	}
}
