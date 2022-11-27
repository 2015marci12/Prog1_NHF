#pragma once
#include "Core.h"
#include "Log.h"
#include "Graphics.h"
#include "Animation.h"
#include "BitmapFont.h"

//A graphical vertex that is part of a textured quad.
typedef struct QuadVertex 
{
	vec3 Pos;
	float Tex;
	vec4 Col;
	vec2 tUV;
} QuadVertex;

//A graphical vertex that is part of a colored line.
typedef struct LineVertex 
{
	vec3 Pos;
	vec4 Col;
} LineVertex;

//Rendering limits.
#define MAX_QUADS 65536
#define MAX_LINES 65536
#define MAX_TEXTURES 32

//A 2D batch renderer.
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
	uint32_t texCount;
	GLTexture* textures[MAX_TEXTURES];

	//Quad vertex data.
	QuadVertex* quadHead;
	uint32_t quadCount;

	//Lines.
	LineVertex* lineHead;
	uint32_t lineCount;
	float lineWidth;

	//Camera.
	mat4 Camera;
} Renderer2D;

//Initialize the renderer.
Renderer2D* Renderer2D_Init(Renderer2D* inst);
//Release the resources held by the renderer.
void Renderer2D_Destroy(Renderer2D* inst);

//Begin a new batch. for internal use only.
void Renderer2D_BeginBatch(Renderer2D* inst);
//End and draw the current batch. for internal use only.
void Renderer2D_EndBatch(Renderer2D* inst);
//End the current batch and begin a new one. for internal use only.
void Renderer2D_NextBatch(Renderer2D* inst);

//Add a texture to the texture stack. for internal use only.
int Renderer2D_AddTexture(Renderer2D* inst, GLTexture* tex);

//Begin a scene with the given camera.
void Renderer2D_BeginScene(Renderer2D* inst, mat4 camera);
//End the current scene and render everything submitted.
void Renderer2D_EndScene(Renderer2D* inst);

//Clear the screen and the depht buffer with the given color.
void Renderer2D_Clear(Renderer2D* inst, vec4 color);
//Clear the depth buffer.
void Renderer2D_ClearDepth(Renderer2D* inst);
//Set the line width.
void Renderer2D_SetLineWidth(Renderer2D* inst, float width);

//Draw a textured quad.
void Renderer2D_DrawQuad(Renderer2D* inst, vec3 pos, vec2 size, vec4 color, GLTexture* texture, Rect texrect);
//Draw a textured quad with the subtexture API.
void Renderer2D_DrawQuad_s(Renderer2D* inst, vec3 pos, vec2 size, vec4 color, SubTexture subtex);
//Draw a rotated textured quad.
void Renderer2D_DrawRotatedQuad(Renderer2D* inst, vec3 pos, vec2 size, float rotation, vec4 color, GLTexture* texture, Rect texrect);
//Draw a rotated textured quad with the subtexture API.
void Renderer2D_DrawRotatedQuad_s(Renderer2D* inst, vec3 pos, vec2 size, float rotation, vec4 color, SubTexture subtex);
//Draw a textured quad with the given transform.
void Renderer2D_DrawQuad_t(Renderer2D* inst, mat4 transform, vec2 size, vec4 color, GLTexture* texture, Rect texrect);
//Draw a sprite.
void Renderer2D_DrawSprite(Renderer2D* inst, mat4 transform, vec2 size, vec4 tint, SubTexture subtex);
//Draw a filled rectangle on the screen.
void Renderer2D_DrawFilledRect(Renderer2D* inst, Rect rect, float z, vec4 color);
//Draw a textured rectangle on the screen.
void Renderer2D_DrawFilledRect_t(Renderer2D* inst, Rect rect, float z, vec4 color, SubTexture subtex);

//Draw a line from one point to another.
void Renderer2D_DrawLine(Renderer2D* inst, vec3 a, vec3 b, vec4 color);
//Draw a rectangle outline.
void Renderer2D_DrawRect(Renderer2D* inst, Rect rect, float z, vec4 color);
//Draw a rectangle outline with a transform.
void Renderer2D_DrawRect_t(Renderer2D* inst, mat4 transform, Rect rect, float z, vec4 color);

//Draw text on the screen with the given font, size and color and the option to flip it vertically.
void Renderer2D_DrawText(Renderer2D* inst, vec3 pos, BitmapFont* font, float fontSize, vec4 color, const char* text, bool flipVertical);
//Calculate the size of text when drawn.
vec2 Renderer2D_CalcTextSize(Renderer2D* inst, BitmapFont* font, float fontSize, const char* text);
