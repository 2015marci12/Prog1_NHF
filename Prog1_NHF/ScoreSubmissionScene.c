#include "ScoreSubmissionScene.h"

ScoreSubmissionScene* InitScoreSubmission(ScoreSubmissionScene* s, uint64_t score, uint32_t Wave, SDL_Window* window)
{
	if (s)
	{
		SDL_StartTextInput();
		s->score.Score = score;
		s->score.Wave = Wave;
		memset(s->score.name, 0, 64);
		time_t rawTime;
		time(&rawTime);
		s->score.timePoint = rawTime;
		s->next = false;
		s->window = window;

		s->font = LoadBitmapFont("Resources\\@Malgun Gothic.bff", true);
		s->BG = TextureAtlas_create(LoadTex2D("Resources\\BG.png"), new_uvec2(550, 367));
	}
	return s;
}

void CleanupScoreSubmission(ScoreSubmissionScene* s)
{
	if (s) 
	{
		SDL_StopTextInput();
		DeleteBitmapFont(s->font);
		GLTexture_Destroy(s->BG.texture);
	}
}

bool ScoreSubmissionKeyDown(SDL_Event* e, void* data)
{
	ScoreSubmissionScene* s = (ScoreSubmissionScene*)data;
	SDL_KeyboardEvent* ev = e;

	if (ev->keysym.scancode == SDL_SCANCODE_BACKSPACE) 
	{
		int len = strlen(s->score.name);
		s->score.name[max(0, len - 1)] = '\0';
	}
	else if(ev->keysym.scancode == SDL_SCANCODE_RETURN)
	{
		s->next = true;
	}

	return false;
}

bool ScoreSubmissionTextInput(SDL_Event* e, void* data)
{
	ScoreSubmissionScene* s = (ScoreSubmissionScene*)data;
	SDL_TextInputEvent* ev = e;

	int len = strlen(ev->text);
	int nameLen = strlen(s->score.name);
	if (nameLen == 0 && ev->text[0] == ' ') return false;
	if (len == 1 && ev->text[0] != ',' && ev->text[0] != '\n' && nameLen < 64)
	{
		s->score.name[nameLen] = ev->text[0];
	}
	else if (len == 1 && ev->text[0] != '\n')
	{
		SDL_StopTextInput();
		s->next = true;
	}
	return false;
}

void DispatchEventsScoreSubmission(EventDispatcher_t* ev, ScoreSubmissionScene* s)
{
	DispatchEvent(ev, SDL_TEXTINPUT, ScoreSubmissionTextInput, s);
	DispatchEvent(ev, SDL_KEYDOWN, ScoreSubmissionKeyDown, s);
}

void RenderScoreSubmission(ScoreSubmissionScene* s, Renderer2D* renderer)
{
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
	const float fontSize = 50.f;

	Renderer2D_DrawFilledRect_t(renderer, new_Rect(0, h, w, -h), 2.f, new_vec4_v(1.f),
		TextureAtlas_SubTexture(&s->BG, new_uvec2(0, 0), new_uvec2(1, 1)));

	//Game over text
	snprintf(buff, 256, "Game Over!");
	vec2 size = Renderer2D_CalcTextSize(renderer, s->font, fontSize * 2, buff);
	float x = center - size.x / 2;
	Renderer2D_DrawText(renderer, new_vec3(x, margin, 0.f), s->font, fontSize * 2, new_vec4(1.f, 0.f, 0.f, 1.f), buff, true);

	//Name.
	snprintf(buff, 256, "Enter name:");
	size = Renderer2D_CalcTextSize(renderer, s->font, fontSize, buff);
	x = center - size.x / 2;
	Renderer2D_DrawText(renderer, new_vec3(x, margin + 100.f, 0.f), s->font, fontSize, new_vec4_v(1.f), buff, true);

	size = Renderer2D_CalcTextSize(renderer, s->font, fontSize, s->score.name);
	x = center - size.x / 2;
	Renderer2D_DrawText(renderer, new_vec3(x, margin + 150.f, 0.f), s->font, fontSize, new_vec4_v(1.f), s->score.name, true);

	Renderer2D_EndScene(renderer);
}
