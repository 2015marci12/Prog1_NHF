#include "LeaderBoardScene.h"

int compareScores(void* a, void* b) 
{
	return (int64_t)((Score*)b)->Score - (int64_t)((Score*)a)->Score;
}

void LoadScores(LeaderBoard* l) 
{
	FILE* file = NULL;
	fopen_s(&file, "LeaderBoard.csv", "r");

	if (!file) return;

	//Read file.
	Score read;
	while (fscanf_s(file, "%[^,], %llu, %u, %llu,", &read.name, 64, &read.Score, &read.Wave, &read.timePoint) != EOF)
	{
		if (l->scoreCount >= l->scoreCapacity) 
		{
			size_t newCap = max(64, l->scoreCount * 2);
			l->scores = realloc(l->scores, newCap * sizeof(Score));
			l->scoreCapacity = newCap;
		}
		l->scores[l->scoreCount++] = read;

		//The file must have a trailing comma!
	}

	fclose(file);

	//Sort scores.
	qsort(l->scores, l->scoreCount, sizeof(Score), compareScores);
}

LeaderBoard* InitLeaderBoard(LeaderBoard* l, SDL_Window* window)
{
	if (l)
	{
		l->GoBack = false;
		l->scroll = 0.f;
		l->scoreCount = 0;
		l->scoreCapacity = 0;
		l->scores = NULL;
		l->window = window;
		l->font = LoadBitmapFont("Resources\\@Malgun Gothic.bff", true);

		l->BG = TextureAtlas_create(LoadTex2D("Resources\\BG.png"), new_uvec2(550, 367));
		l->HUD = TextureAtlas_create(LoadTex2D("Resources\\HUDIcons.png"), new_uvec2(32, 32));

		//Load scores.
		LoadScores(l);
	}
	return l;
}

void CleanupLeaderBoard(LeaderBoard* l)
{
	if (l)
	{
		DeleteBitmapFont(l->font);
		GLTexture_Destroy(l->BG.texture);
		GLTexture_Destroy(l->HUD.texture);

		free(l->scores);
	}
}

bool LeaderBoardScroll(SDL_Event* e, void* data)
{
	LeaderBoard* l = (LeaderBoard*)data;
	SDL_MouseWheelEvent* ev = e;

	float height = l->scoreCount * 50.f + 50.f;
	height = max(height - 900.f, 0);

	l->scroll += -ev->preciseY * 40.f;
	l->scroll = clamp(0.f, height, l->scroll);

	return false;
}

bool LeaderBoardKeyDown(SDL_Event* e, void* data)
{
	LeaderBoard* l = (LeaderBoard*)data;
	SDL_KeyboardEvent* ev = e;

	float height = l->scoreCount * 50.f + 50.f;
	height = max(height - 900.f, 0);

	if (ev->keysym.scancode == SDL_SCANCODE_ESCAPE) 
	{
		l->GoBack = true;
	}
	else if (ev->keysym.scancode == SDL_SCANCODE_DOWN)
	{
		l->scroll += 50.f;
	}
	else if (ev->keysym.scancode == SDL_SCANCODE_UP)
	{
		l->scroll -= 50.f;
	}

	l->scroll = clamp(0.f, height, l->scroll);

	return false;
}

bool LeaderBoardMouseDown(SDL_Event* e, void* data)
{
	LeaderBoard* l = (LeaderBoard*)data;
	SDL_MouseButtonEvent* ev = e;

	const float margin = 50.f;

	int w, h;
	SDL_GetWindowSize(l->window, &w, &h);
	float scale = 1080.f / (float)h;
	float aspect = (float)w / (float)h;
	w = aspect * 1080.f;
	h = 1080.f;

	vec2 MousePos = new_vec2(ev->x, ev->y);
	MousePos.x *= scale;
	MousePos.y *= scale;

	if (Rect_Contains(new_Rect(margin, margin, 100.f, 100.f), MousePos)) 
	{
		l->GoBack = true;
	}

	return false;
}

void DispatchLeaderBoardEvents(EventDispatcher_t* ev, LeaderBoard* l)
{
	DispatchEvent(ev, SDL_MOUSEWHEEL, LeaderBoardScroll, l);
	DispatchEvent(ev, SDL_KEYDOWN, LeaderBoardKeyDown, l);
	DispatchEvent(ev, SDL_MOUSEBUTTONDOWN, LeaderBoardMouseDown, l);
}

void SaveScoreToLeaderBoard(Score score)
{
	FILE* file;
	fopen_s(&file, "LeaderBoard.csv", "a");
	ASSERT(file, "File could not be opened/created!");

	fprintf(file, "%s, %llu, %u, %llu,", score.name, score.Score, score.Wave, score.timePoint);

	fclose(file);
}

void DeleteLeaderBoard()
{
	FILE* file;
	fopen_s(&file, "LeaderBoard.csv", "w");
	ASSERT(file, "File could not be opened/created!");
	fclose(file);
}

void RenderLeaderBoard(LeaderBoard* l, Renderer2D* renderer)
{
	int w, h;
	SDL_GetWindowSize(l->window, &w, &h);
	float aspect = (float)w / (float)h;
	w = aspect * 1080.f;
	h = 1080.f;


	mat4 view = mat4_Ortho(0, aspect * 1080.f, 0, 1080.f, 30, -30);
	mat4 transform = mat4_Translate(mat4x4_Identity(), new_vec3(0.f, -l->scroll, 0.f));

	Renderer2D_BeginScene(renderer, mat4x4x4_Mul(view, transform));
	Renderer2D_Clear(renderer, new_vec4_v(0.f));

	const float center = (float)w / 2;
	char buff[256]; //Buffer to format the displayed text with.

	const float margin = 50.f;
	const float fontSize = 50.f;

	Renderer2D_DrawFilledRect_t(renderer, new_Rect(0, h + l->scroll, w, -h), 2.f, new_vec4_v(0.5f),
		TextureAtlas_SubTexture(&l->BG, new_uvec2(0, 0), new_uvec2(1, 1)));

	//Back button.
	Renderer2D_DrawFilledRect_t(renderer, new_Rect(margin, margin + l->scroll + 100.f, 100.f, -100.f), 0.f, new_vec4_v(1.f),
		TextureAtlas_SubTexture(&l->HUD, new_uvec2(1, 3), new_uvec2(1, 1)));

	//Scores.
	float y = margin + 75.f;

	float columnX[5] =
	{
		margin + 150.f,
		margin + 300.f,
		margin + 700.f,
		margin + 850.f,
		margin + 1000.f,
	};
	
	const char* columnHeaders[5] =
	{
		"Place",
		"Name",
		"Score",
		"Wave",
		"Date"
	};

	for (int i = 0; i < 5; i++) 
	{
		Renderer2D_DrawText(renderer, new_vec3(columnX[i], y, 0.f), l->font, fontSize, new_vec4_v(1.f), columnHeaders[i], true);
	}

	y += 50.f;

	for (int i = 0; i < l->scoreCount; i++) 
	{
		Score* current = &l->scores[i];

		//Place.
		snprintf(buff, 256, "%d.", i + 1);
		Renderer2D_DrawText(renderer, new_vec3(columnX[0], y, 0.f), l->font, fontSize, new_vec4_v(1.f), buff, true);
			
		//Name.
		Renderer2D_DrawText(renderer, new_vec3(columnX[1], y, 0.f), l->font, fontSize, new_vec4_v(1.f), current->name, true);

		//Score.
		snprintf(buff, 256, "%llu", current->Score);
		Renderer2D_DrawText(renderer, new_vec3(columnX[2], y, 0.f), l->font, fontSize, new_vec4_v(1.f), buff, true);

		//Wave.
		snprintf(buff, 256, "%u", current->Wave);
		Renderer2D_DrawText(renderer, new_vec3(columnX[3], y, 0.f), l->font, fontSize, new_vec4_v(1.f), buff, true);

		//Date.
		time_t rawTime = current->timePoint;
		struct tm* time = localtime(&rawTime);
		snprintf(buff, 256, "%04d-%02d-%02d %02d:%02d", time->tm_year + 1900, time->tm_mon, time->tm_mday, time->tm_hour, time->tm_min);
		Renderer2D_DrawText(renderer, new_vec3(columnX[4], y, 0.f), l->font, fontSize, new_vec4_v(1.f), buff, true);

		y += 50.f;
	}

	Renderer2D_EndScene(renderer);
}
