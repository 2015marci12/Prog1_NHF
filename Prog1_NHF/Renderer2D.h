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

#define MAX_QUADS 65536
#define MAX_TEXTURES 32

typedef struct Renderer2D 
{
	//Rendering objects.
	GLBuffer* quadVBO;
	GLBuffer* quadIBO;
	GLVertexArray* quadVAO;
	GLShader* quadShader;
	
	//Texturing.
	GLTexture* WhiteTex;
	uint32_t lastTex;
	GLTexture* textures[MAX_TEXTURES];

	//Quad vertex data.
	QuadVertex* Base;
	QuadVertex* Head;
	uint32_t quadCount;

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

void Renderer2D_DrawQuad(Renderer2D* inst, mat4 transform, vec4 color, GLTexture* texture, Rect texrect);
void Renderer2D_DrawSprite(Renderer2D* inst, mat4 transform, vec4 tint, SubTexture subtex);

//TODO drawing functions, gbuffer for potential normal mapping, rendertarget switching, text, lights.
