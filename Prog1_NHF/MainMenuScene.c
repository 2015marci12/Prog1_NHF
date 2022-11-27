#include "MainMenuScene.h"

MainMenu* InitMenu(MainMenu* menu, SDL_Window* window)
{
	if (menu)
	{
		menu->clicked = false;
		menu->font = LoadBitmapFont("Resources\\@Malgun Gothic.bff", true);
		menu->selected = STARTGAME;
		menu->window = window;

		menu->BG = TextureAtlas_create(LoadTex2D("Resources\\BG.png"), new_uvec2(550, 367));
	}
	return menu;
}

void CleanupMenu(MainMenu* menu)
{
	if (menu)
	{
		DeleteBitmapFont(menu->font);
		GLTexture_Destroy(menu->BG.texture);
	}
}

bool MenuKeyDown(SDL_Event* e, const void* data)
{
	MainMenu* menu = data;
	SDL_KeyboardEvent* ev = e;

	if (ev->keysym.scancode == SDL_SCANCODE_DOWN)
	{
		menu->selected = (menu->selected + 1) % MenuOptions_MAX;
	}
	else if (ev->keysym.scancode == SDL_SCANCODE_UP)
	{
		menu->selected = (menu->selected == STARTGAME) ? QUIT : menu->selected - 1;
	}
	else if (ev->keysym.scancode == SDL_SCANCODE_RETURN)
	{
		menu->clicked = true;
	}

	return false;
}

bool MenuMouseDown(SDL_Event* e, const void* data)
{
	MainMenu* menu = data;
	SDL_MouseButtonEvent* ev = e;

	int w, h;
	SDL_GetWindowSize(menu->window, &w, &h);
	float scale = 1080.f / (float)h;
	float aspect = (float)w / (float)h;
	w = aspect * 1080.f;
	h = 1080.f;

	vec2 MousePos = new_vec2(ev->x, ev->y);
	MousePos.x *= scale;
	MousePos.y *= scale;

	const float center = (float)w / 2;
	const float margin = 300.f;

	if (Rect_Contains(new_Rect(center - 200.f, margin - 25.f, 400.f, 50.f), MousePos))
	{
		menu->selected = STARTGAME;
		menu->clicked = true;
	}
	else if (Rect_Contains(new_Rect(center - 200.f, margin + 50.f - 25.f, 400.f, 50.f), MousePos)) 
	{
		menu->selected = LEADERBOARD;
		menu->clicked = true;
	}
	else if (Rect_Contains(new_Rect(center - 200.f, margin + 100.f - 25.f, 400.f, 50.f), MousePos))
	{
		menu->selected = SETTINGS;
		menu->clicked = true;
	}
	else if (Rect_Contains(new_Rect(center - 200.f, margin + 150.f - 25.f, 400.f, 50.f), MousePos))
	{
		menu->selected = CREDITS;
		menu->clicked = true;
	}
	else if (Rect_Contains(new_Rect(center - 200.f, margin + 200.f - 25.f, 400.f, 50.f), MousePos))
	{
		menu->selected = QUIT;
		menu->clicked = true;
	}

	return false;
}

void MenuDispatchEvents(EventDispatcher_t* ev, MainMenu* menu)
{
	DispatchEvent(ev, SDL_KEYDOWN, MenuKeyDown, menu);
	DispatchEvent(ev, SDL_MOUSEBUTTONDOWN, MenuMouseDown, menu);
}

void RenderMenu(MainMenu* menu, Renderer2D* renderer)
{
	int w, h;
	SDL_GetWindowSize(menu->window, &w, &h);
	float aspect = (float)w / (float)h;
	w = aspect * 1080.f;
	h = 1080.f;

	mat4 view = mat4_Ortho(0, aspect * 1080.f, 0, 1080.f, 30, -30);

	Renderer2D_BeginScene(renderer, view);
	Renderer2D_Clear(renderer, new_vec4_v(0.f));

	const float center = (float)w / 2;
	char buff[256]; //Buffer to format the displayed text with.

	const float margin = 300.f;
	const float fontSize = 50.f;

	Renderer2D_DrawFilledRect_t(renderer, new_Rect(0, h, w, -h), 2.f, new_vec4_v(1.f),
		TextureAtlas_SubTexture(&menu->BG, new_uvec2(0, 0), new_uvec2(1, 1)));

	//TODO title screen I have the time.

	snprintf(buff, 256, "New Game");
	vec2 size = Renderer2D_CalcTextSize(renderer, menu->font, fontSize, buff);
	float x = center - size.x / 2;
	Renderer2D_DrawText(renderer, new_vec3(x, margin, 0.f), menu->font, fontSize, new_vec4_v(1.f), buff, true);
	if (menu->selected == STARTGAME)
	{
		Renderer2D_DrawFilledRect(renderer, new_Rect(center - 200.f, margin - 25.f, 400.f, 50.f), 1.f, new_vec4(0.5f, 0.5f, 0.6f, 1.f));
	}

	snprintf(buff, 256, "Leaderboard");
	size = Renderer2D_CalcTextSize(renderer, menu->font, fontSize, buff);
	x = center - size.x / 2;
	Renderer2D_DrawText(renderer, new_vec3(x, margin + 50.f, 0.f), menu->font, fontSize, new_vec4_v(1.f), buff, true);
	if (menu->selected == LEADERBOARD)
	{
		Renderer2D_DrawFilledRect(renderer, new_Rect(center - 200.f, margin + 50.f - 25.f, 400.f, 50.f), 1.f, new_vec4(0.5f, 0.5f, 0.6f, 1.f));
	}

	snprintf(buff, 256, "Settings");
	size = Renderer2D_CalcTextSize(renderer, menu->font, fontSize, buff);
	x = center - size.x / 2;
	Renderer2D_DrawText(renderer, new_vec3(x, margin + 100.f, 0.f), menu->font, fontSize, new_vec4_v(1.f), buff, true);
	if (menu->selected == SETTINGS)
	{
		Renderer2D_DrawFilledRect(renderer, new_Rect(center - 200.f, margin + 100.f - 25.f, 400.f, 50.f), 1.f, new_vec4(0.5f, 0.5f, 0.6f, 1.f));
	}

	snprintf(buff, 256, "Credits");
	size = Renderer2D_CalcTextSize(renderer, menu->font, fontSize, buff);
	x = center - size.x / 2;
	Renderer2D_DrawText(renderer, new_vec3(x, margin + 150.f, 0.f), menu->font, fontSize, new_vec4_v(1.f), buff, true);
	if (menu->selected == CREDITS)
	{
		Renderer2D_DrawFilledRect(renderer, new_Rect(center - 200.f, margin + 150.f - 25.f, 400.f, 50.f), 1.f, new_vec4(0.5f, 0.5f, 0.6f, 1.f));
	}

	snprintf(buff, 256, "Quit");
	size = Renderer2D_CalcTextSize(renderer, menu->font, fontSize, buff);
	x = center - size.x / 2;
	Renderer2D_DrawText(renderer, new_vec3(x, margin + 200.f, 0.f), menu->font, fontSize, new_vec4_v(1.f), buff, true);
	if (menu->selected == QUIT)
	{
		Renderer2D_DrawFilledRect(renderer, new_Rect(center - 200.f, margin + 200.f - 25.f, 400.f, 50.f), 1.f, new_vec4(0.5f, 0.5f, 0.6f, 1.f));
	}

	Renderer2D_EndScene(renderer);
}