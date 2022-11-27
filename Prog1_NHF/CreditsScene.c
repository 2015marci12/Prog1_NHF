#include "CreditsScene.h"

CreditsScene* InitCredits(CreditsScene* c, SDL_Window* window)
{
	if (c)
	{
		c->GoBack = false;
		c->window = window;
		c->font = LoadBitmapFont("Resources\\@Malgun Gothic.bff", true);
		c->BG = TextureAtlas_create(LoadTex2D("Resources\\BG.png"), new_uvec2(550, 367));
		c->HUD = TextureAtlas_create(LoadTex2D("Resources\\HUDIcons.png"), new_uvec2(32, 32));
	}
	return c;
}

void CleanupCredits(CreditsScene* c)
{
	if (c)
	{
		DeleteBitmapFont(c->font);
		GLTexture_Destroy(c->BG.texture);
		GLTexture_Destroy(c->HUD.texture);
	}
}

bool CreditsMouseDown(SDL_Event* e, void* data)
{
	CreditsScene* c = (CreditsScene*)data;
	SDL_MouseButtonEvent* ev = e;

	const float margin = 50.f;

	int w, h;
	SDL_GetWindowSize(c->window, &w, &h);
	float scale = 1080.f / (float)h;
	float aspect = (float)w / (float)h;
	w = aspect * 1080.f;
	h = 1080.f;

	vec2 MousePos = new_vec2(ev->x, ev->y);
	MousePos.x *= scale;
	MousePos.y *= scale;

	//Back button.
	if (Rect_Contains(new_Rect(margin, margin, 100.f, 100.f), MousePos))
	{
		c->GoBack = true;
	}

	return false;
}

bool CreditsKeyDown(SDL_Event* e, void* data)
{
	CreditsScene* c = (CreditsScene*)data;
	SDL_KeyboardEvent* ev = e;

	//Go back.
	if (ev->keysym.scancode == SDL_SCANCODE_ESCAPE)
	{
		c->GoBack = true;
	}
	return false;
}

void DispatchCreditsEvents(EventDispatcher_t* ev, CreditsScene* c)
{
	DispatchEvent(ev, SDL_KEYDOWN, CreditsKeyDown, c);
	DispatchEvent(ev, SDL_MOUSEBUTTONDOWN, CreditsMouseDown, c);
}

void RenderCredits(CreditsScene* c, Renderer2D* renderer)
{
	int w, h;
	SDL_GetWindowSize(c->window, &w, &h);
	float aspect = (float)w / (float)h;
	w = aspect * 1080.f;
	h = 1080.f;

	mat4 view = mat4_Ortho(0, aspect * 1080.f, 0, 1080.f, 30, -30);

	Renderer2D_BeginScene(renderer, view);
	Renderer2D_Clear(renderer, new_vec4_v(0.f));

	const float center = (float)w / 2;
	char buff[256]; //Buffer to format the displayed text with.

	const float margin = 50.f;
	const float fontSize = 50.f;

	Renderer2D_DrawFilledRect_t(renderer, new_Rect(0, h, w, -h), 2.f, new_vec4_v(0.5f),
		TextureAtlas_SubTexture(&c->BG, new_uvec2(0, 0), new_uvec2(1, 1)));

	//Back button.
	Renderer2D_DrawFilledRect_t(renderer, new_Rect(margin, margin + 100.f, 100.f, -100.f), 0.f, new_vec4_v(1.f),
		TextureAtlas_SubTexture(&c->HUD, new_uvec2(1, 3), new_uvec2(1, 1)));

	//Credits text.
	const char credits[] =
		"Textures:\n"
		"	Pixel explosion - JROB774 https://opengameart.org/content/pixel-explosion-12-frames\n"
		"	Smoke and fire animated particle - KnoblePersona\n\t\thttps://opengameart.org/content/smoke-fire-animated-particle-16x16\n"
		"	Pixel night city - fridaruiz https://opengameart.org/content/pixel-night-city\n"
		"	All other art is custom.\n"
		"\n"
		"Sounds:\n"
		"	\n"
		"\n"
		"Libraries:\n"
		"	SDL2 - https://www.libsdl.org/\n"
		"	MT19937-64 - Takuji Nishimura and Makoto Matsumoto\n"
		"\n"
		"Game Author:\n"
		"	Papp Marcell Miklos\n"
		;

	Renderer2D_DrawText(renderer, new_vec3(margin + 150.f, margin, 0.f), c->font, fontSize, new_vec4_v(1.f),
		credits, true);

	Renderer2D_EndScene(renderer);
}
