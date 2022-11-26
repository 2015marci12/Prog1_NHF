#include "LeaderBoardScene.h"

bool compareScores(void* a, void* b) 
{
	return ((Score*)a)->Score > ((Score*)b)->Score;
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
			size_t newCap = max(64, l->scoreCapacity * 2);
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

LeaderBoard* InitLeaderBoard(LeaderBoard* l)
{
	if (l)
	{
		l->GoBack = false;
		l->scroll = 0.f;
		l->scoreCount = 0;
		l->scoreCapacity = 0;
		l->scores = NULL;
		l->font = LoadBitmapFont("Resources\\@Malgun Gothic.bff", true);

		l->BG = TextureAtlas_create(LoadTex2D("Resources\\BG.png"), new_uvec2(550, 367));

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

		free(l->scores);
	}
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
