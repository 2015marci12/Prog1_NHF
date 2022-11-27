#include "SettingsScene.h"

bool globalSettings_Init = false;
GlobalSettings globalSettings;

GlobalSettings* GetGlobalSettings()
{
	//Load or init settings if necessary.
	if (!globalSettings_Init)
	{
		FILE* file = NULL;
		fopen_s(&file, "Resources\\GlobalSettings.set", "r");
		if (file)
			fread(&globalSettings, sizeof(GlobalSettings), 1, file);
		else
		{
			globalSettings.masterVolume = 1.f;
			globalSettings.musicVolume = 1.f;
			globalSettings.effectsVolume = 1.f;
			globalSettings.ResolutionVariation = 0;
			globalSettings.FullScreen = false;

			SaveGlobalSettings();
		}

		if (file) fclose(file);

		globalSettings_Init = true;
	}

	return &globalSettings;
}

void SaveGlobalSettings()
{
	FILE* file = NULL;
	fopen_s(&file, "Resources\\GlobalSettings.set", "w");

	ASSERT(file, "Failed to create or open file!");

	fwrite(&globalSettings, sizeof(GlobalSettings), 1, file);

	fclose(file);
}

//Basic resolutions.
//800 x 600
//1280 x 720
//1920 x 1080

uint32_t resolutionsX[] =
{
	800, 1280, 1920
};
uint32_t resolutionsY[] =
{
	600, 720, 1080
};

uvec2 GetResolutionVariation(uint32_t variation)
{
	return new_uvec2(resolutionsX[variation], resolutionsY[variation]);
}

uint32_t GetResolutionVariationCount()
{
	return sizeof(resolutionsX) / sizeof(uint32_t);
}

SettingsScene* InitSettings(SettingsScene* s, SDL_Window* window)
{
	if (s)
	{
		s->selected = VOLUME_MASTER;
		s->GoBack = false;

		s->window = window;
		s->font = LoadBitmapFont("Resources\\@Malgun Gothic.bff", true);

		s->BG = TextureAtlas_create(LoadTex2D("Resources\\BG.png"), new_uvec2(550, 367));
		s->HUD = TextureAtlas_create(LoadTex2D("Resources\\HUDIcons.png"), new_uvec2(32, 32));
	}
	return s;
}

void CleanupSettings(SettingsScene* s)
{
	if (s)
	{
		DeleteBitmapFont(s->font);
		GLTexture_Destroy(s->BG.texture);
		GLTexture_Destroy(s->HUD.texture);
	}
}

bool SettingsKeyDown(SDL_Event* e, void* data)
{
	SettingsScene* s = (SettingsScene*)data;
	SDL_KeyboardEvent* ev = e;

	GlobalSettings* settings = GetGlobalSettings();

	//Selection.
	if (ev->keysym.scancode == SDL_SCANCODE_DOWN)
	{
		s->selected = (s->selected + 1) % SETTINGS_MAX;
	}
	else if (ev->keysym.scancode == SDL_SCANCODE_UP)
	{
		s->selected = (s->selected == RESOLUTION) ? VOLUME_MASTER : s->selected - 1;
	}

	//Go back.
	else if (ev->keysym.scancode == SDL_SCANCODE_ESCAPE)
	{
		s->GoBack = true;
		SaveGlobalSettings();
	}

	//Delete leaderboard.
	else if (ev->keysym.scancode == SDL_SCANCODE_RETURN && s->selected == RESETLEADERBOARD)
	{
		DeleteLeaderBoard();
	}

	//Fullscreen toggle.
	else if (ev->keysym.scancode == SDL_SCANCODE_RETURN && s->selected == FULLSCREEN)
	{
		SDL_SetWindowFullscreen(s->window, settings->FullScreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
		settings->FullScreen = !settings->FullScreen;
	}

	//Volume adjustments.
	else if (ev->keysym.scancode == SDL_SCANCODE_LEFT && s->selected == VOLUME_MASTER)
	{
		settings->masterVolume -= 0.05f;
		settings->masterVolume = clamp(0.f, 1.f, settings->masterVolume);
		//TODO adjust volume on the actual mixer.
	}
	else if (ev->keysym.scancode == SDL_SCANCODE_LEFT && s->selected == VOLUME_MUSIC)
	{
		settings->musicVolume -= 0.05f;
		settings->musicVolume = clamp(0.f, 1.f, settings->musicVolume);
		//TODO adjust volume on the actual mixer.
	}
	else if (ev->keysym.scancode == SDL_SCANCODE_LEFT && s->selected == VOLUME_EFFECTS)
	{
		settings->effectsVolume -= 0.05f;
		settings->effectsVolume = clamp(0.f, 1.f, settings->effectsVolume);
		//TODO adjust volume on the actual mixer.
	}
	else if (ev->keysym.scancode == SDL_SCANCODE_RIGHT && s->selected == VOLUME_MASTER)
	{
		settings->masterVolume += 0.05f;
		settings->masterVolume = clamp(0.f, 1.f, settings->masterVolume);
		//TODO adjust volume on the actual mixer.
	}
	else if (ev->keysym.scancode == SDL_SCANCODE_RIGHT && s->selected == VOLUME_MUSIC)
	{
		settings->musicVolume += 0.05f;
		settings->musicVolume = clamp(0.f, 1.f, settings->musicVolume);
		//TODO adjust volume on the actual mixer.
	}
	else if (ev->keysym.scancode == SDL_SCANCODE_RIGHT && s->selected == VOLUME_EFFECTS)
	{
		settings->effectsVolume += 0.05f;
		settings->effectsVolume = clamp(0.f, 1.f, settings->effectsVolume);
		//TODO adjust volume on the actual mixer.
	}

	//Resolution adjustment.
	else if (ev->keysym.scancode == SDL_SCANCODE_RIGHT && s->selected == RESOLUTION)
	{
		settings->ResolutionVariation = (settings->ResolutionVariation + 1) % GetResolutionVariationCount();
		uvec2 size = GetResolutionVariation(settings->ResolutionVariation);
		SDL_SetWindowSize(s->window, size.x, size.y);
		glViewport(0, 0, size.x, size.y);
	}
	else if (ev->keysym.scancode == SDL_SCANCODE_LEFT && s->selected == RESOLUTION)
	{
		if (settings->ResolutionVariation == 0) settings->ResolutionVariation = GetResolutionVariationCount() - 1;
		else settings->ResolutionVariation -= 1;
		uvec2 size = GetResolutionVariation(settings->ResolutionVariation);
		SDL_SetWindowSize(s->window, size.x, size.y);
		glViewport(0, 0, size.x, size.y);
	}

	return false;
}

bool SettingsMouseDown(SDL_Event* e, void* data)
{
	SettingsScene* s = (SettingsScene*)data;
	SDL_MouseButtonEvent* ev = e;

	const float margin = 300.f;
	const float margin2 = 50.f;

	int w, h;
	SDL_GetWindowSize(s->window, &w, &h);
	float scale = 1080.f / (float)h;
	float aspect = (float)w / (float)h;
	w = aspect * 1080.f;
	h = 1080.f;

	vec2 MousePos = new_vec2(ev->x, ev->y);
	MousePos.x *= scale;
	MousePos.y *= scale;

	const float center = (float)w / 2;

	//Back button.
	if (Rect_Contains(new_Rect(margin2, margin2, 100.f, 100.f), MousePos))
	{
		s->GoBack = true;
		SaveGlobalSettings();
	}

	//Selection.
	for (int i = 0; i < SETTINGS_MAX; i++) 
	{
		if (Rect_Contains(new_Rect(center - 250.f, margin + i * 50.f - 25.f, 500.f, 50.f), MousePos))
			s->selected = i;
	}

	return false;
}

void DispatchSettingsEvents(EventDispatcher_t* ev, SettingsScene* s)
{
	DispatchEvent(ev, SDL_KEYDOWN, SettingsKeyDown, s);
	DispatchEvent(ev, SDL_MOUSEBUTTONDOWN, SettingsMouseDown, s);
}

void RenderSettings(SettingsScene* s, Renderer2D* renderer)
{
	GlobalSettings* settings = GetGlobalSettings();

	int w, h;
	SDL_GetWindowSize(s->window, &w, &h);
	float aspect = (float)w / (float)h;
	w = aspect * 1080.f;
	h = 1080.f;

	mat4 view = mat4_Ortho(0, aspect * 1080.f, 0, 1080.f, 30, -30);

	Renderer2D_BeginScene(renderer, view);
	Renderer2D_Clear(renderer, new_vec4_v(0.f));

	const float center = (float)w / 2;
	char buff[256]; //Buffer to format the displayed text with.

	const float margin = 300.f;
	const float margin2 = 50.f;
	const float fontSize = 50.f;

	Renderer2D_DrawFilledRect_t(renderer, new_Rect(0, h, w, -h), 2.f, new_vec4_v(1.f),
		TextureAtlas_SubTexture(&s->BG, new_uvec2(0, 0), new_uvec2(1, 1)));

	//Back button.
	Renderer2D_DrawFilledRect_t(renderer, new_Rect(margin2, margin2 + 100.f, 100.f, -100.f), 0.f, new_vec4_v(1.f),
		TextureAtlas_SubTexture(&s->HUD, new_uvec2(1, 3), new_uvec2(1, 1)));

	int mastervol = settings->masterVolume * 100;
	snprintf(buff, 256, "Master Volume: %d", mastervol);
	vec2 size = Renderer2D_CalcTextSize(renderer, s->font, fontSize, buff);
	float x = center - size.x / 2;
	Renderer2D_DrawText(renderer, new_vec3(x, margin, 0.f), s->font, fontSize, new_vec4_v(1.f), buff, true);
	if (s->selected == VOLUME_MASTER)
	{
		Renderer2D_DrawFilledRect(renderer, new_Rect(center - 250.f, margin - 25.f, 500.f, 50.f), 1.f, new_vec4(0.5f, 0.5f, 0.6f, 1.f));
	}

	int musicvol = settings->musicVolume * 100;
	snprintf(buff, 256, "Music Volume: %d", musicvol);
	size = Renderer2D_CalcTextSize(renderer, s->font, fontSize, buff);
	x = center - size.x / 2;
	Renderer2D_DrawText(renderer, new_vec3(x, margin + 50.f, 0.f), s->font, fontSize, new_vec4_v(1.f), buff, true);
	if (s->selected == VOLUME_MUSIC)
	{
		Renderer2D_DrawFilledRect(renderer, new_Rect(center - 250.f, margin + 50.f - 25.f, 500.f, 50.f), 1.f, new_vec4(0.5f, 0.5f, 0.6f, 1.f));
	}

	int effectsvol = settings->effectsVolume * 100;
	snprintf(buff, 256, "Effects Volume: %d", effectsvol);
	size = Renderer2D_CalcTextSize(renderer, s->font, fontSize, buff);
	x = center - size.x / 2;
	Renderer2D_DrawText(renderer, new_vec3(x, margin + 100, 0.f), s->font, fontSize, new_vec4_v(1.f), buff, true);
	if (s->selected == VOLUME_EFFECTS)
	{
		Renderer2D_DrawFilledRect(renderer, new_Rect(center - 250.f, margin + 100 - 25.f, 500.f, 50.f), 1.f, new_vec4(0.5f, 0.5f, 0.6f, 1.f));
	}

	snprintf(buff, 256, "Reset Leaderboard");
	size = Renderer2D_CalcTextSize(renderer, s->font, fontSize, buff);
	x = center - size.x / 2;
	Renderer2D_DrawText(renderer, new_vec3(x, margin + 150, 0.f), s->font, fontSize, new_vec4_v(1.f), buff, true);
	if (s->selected == RESETLEADERBOARD)
	{
		Renderer2D_DrawFilledRect(renderer, new_Rect(center - 250.f, margin + 150 - 25.f, 500.f, 50.f), 1.f, new_vec4(0.5f, 0.5f, 0.6f, 1.f));
	}

	snprintf(buff, 256, settings->FullScreen ? "Set Windowed" : "Set Fullscreen");
	size = Renderer2D_CalcTextSize(renderer, s->font, fontSize, buff);
	x = center - size.x / 2;
	Renderer2D_DrawText(renderer, new_vec3(x, margin + 200, 0.f), s->font, fontSize, new_vec4_v(1.f), buff, true);
	if (s->selected == FULLSCREEN)
	{
		Renderer2D_DrawFilledRect(renderer, new_Rect(center - 250.f, margin + 200 - 25.f, 500.f, 50.f), 1.f, new_vec4(0.5f, 0.5f, 0.6f, 1.f));
	}

	uvec2 Resolution = GetResolutionVariation(settings->ResolutionVariation);
	snprintf(buff, 256, "Resolution: %d x %d", Resolution.x, Resolution.y);
	size = Renderer2D_CalcTextSize(renderer, s->font, fontSize, buff);
	x = center - size.x / 2;
	Renderer2D_DrawText(renderer, new_vec3(x, margin + 250, 0.f), s->font, fontSize, new_vec4_v(1.f), buff, true);
	if (s->selected == RESOLUTION)
	{
		Renderer2D_DrawFilledRect(renderer, new_Rect(center - 250.f, margin + 250 - 25.f, 500.f, 50.f), 1.f, new_vec4(0.5f, 0.5f, 0.6f, 1.f));
	}

	Renderer2D_EndScene(renderer);
}