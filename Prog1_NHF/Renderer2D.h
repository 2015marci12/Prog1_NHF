#pragma once
#include "Core.h"
#include "Log.h"
#include "Graphics.h"
#include "Animation.h"

typedef struct QuadVertex 
{
	vec3 Pos;
	float Tex;
	vec4 Col;
	vec2 tUV;
} QuadVertex;

typedef struct LineVertex 
{
	vec3 Pos;
	vec4 Col;
} LineVertex;

#define MAX_QUADS 65536
#define MAX_LINES 65536
#define MAX_TEXTURES 32

typedef struct Renderer2D 
{
	//Rendering objects.
	GLBuffer* quadVBO;
	GLBuffer* quadIBO;
	GLVertexArray* quadVAO;
	GLShader* quadShader;

	GLBuffer* lineVBO;
	GLVertexArray* lineVAO;
	GLShader* lineShader;
	
	//Texturing.
	GLTexture* WhiteTex;
	uint32_t lastTex;
	GLTexture* textures[MAX_TEXTURES];

	//Quad vertex data.
	QuadVertex* quadBase;
	QuadVertex* quadHead;
	uint32_t quadCount;

	//Lines.
	LineVertex* lineBase;
	LineVertex* lineHead;
	uint32_t lineCount;
	float lineWidth;

	//Camera.
	mat4 Camera;
} Renderer2D;

Renderer2D* Renderer2D_Init(Renderer2D* inst);
void Renderer2D_Destroy(Renderer2D* inst);

void Renderer2D_BeginBatch(Renderer2D* inst);
void Renderer2D_EndBatch(Renderer2D* inst);
void Renderer2D_NextBatch(Renderer2D* inst);

int Renderer2D_AddTexture(Renderer2D* inst, GLTexture* tex);

void Renderer2D_BeginScene(Renderer2D* inst, mat4 camera);
void Renderer2D_EndScene(Renderer2D* inst);

void Renderer2D_Clear(Renderer2D* inst, vec4 color);
void Renderer2D_SetLineWidth(Renderer2D* inst, float width);

void Renderer2D_DrawQuad(Renderer2D* inst, mat4 transform, vec2 size, vec4 color, GLTexture* texture, Rect texrect);
void Renderer2D_DrawSprite(Renderer2D* inst, mat4 transform, vec2 size, vec4 tint, SubTexture subtex);

void Renderer2D_DrawLine(Renderer2D* inst, vec3 a, vec3 b, vec4 color);
void Renderer2D_DrawRect(Renderer2D* inst, Rect rect, float z, vec4 color);
void Renderer2D_DrawRect_t(Renderer2D* inst, mat4 transform, Rect rect, float z, vec4 color);

//TODO drawing functions, gbuffer for potential normal mapping, rendertarget switching, text, lights.
