#include "BitmapFont.h"


BitmapFont* LoadBitmapFont(const char* filepath, bool invertY)
{
	BitmapFont* ret = malloc(sizeof(BitmapFont)); //Allocate space for the font.
	ret->invertY = invertY;

	FILE* file;
	fopen_s(&file, filepath, "r");
	if (!file) 
	{
		ERROR("Could not open bitmap font file: %s\n", filepath);
		free(ret); return NULL;
	};

	//Check file format.
	char check[2];
	char valid[2] = { 0xBF, 0xF2 };
	fread(check, 1, 2, file);
	if (0 != memcmp(check, valid, 2))
	{
		ERROR("Invalid bitmap font file: %s\n", filepath);
		free(ret); return NULL;
	}

	//Cell width/height in UV coords.
	uint32_t w, h, cellW, cellH;
	fread(&w, sizeof(uint32_t), 1, file);
	fread(&h, sizeof(uint32_t), 1, file);
	fread(&cellW, sizeof(uint32_t), 1, file);
	fread(&cellH, sizeof(uint32_t), 1, file);
	ret->CellFactors = new_vec2((float)cellW / (float)w, (float)cellH / (float)h);
	ret->rowPitch = w / cellW;
	ret->advanceFactor = 1.1f / (float)cellW;

	//Check bits per pixel. due to the nature of the renderer, only fonts
	//with an alpha channel, AKA 32bit images are supported.
	int8_t BPP;
	fread(&BPP, 1, 1, file);
	if (BPP != 32)
	{
		ERROR("Invalid bitmap font image format: %s\n", filepath);
		free(ret); return NULL;
	};

	//Char offset.
	fread(&ret->baseCharOffset, 1, 1, file);

	//Read character widths.
	fread(&ret->charWidths, 1, 256, file);

	//Read image data and create the OpenGL texture.
	ret->FontTexture = GLTexture_Create(GLTextureType_2D, GLFormat_RGBA, new_uvec3(w, h, 1), 1);
	char* imgData = malloc(w * h * 4/*4 bytes per pixel*/);
	fread(imgData, 4, w * h, file);
	GLTexture_Upload(ret->FontTexture, 0, GLFormat_RGBA, new_uvec3_v(0), ret->FontTexture->Size, imgData);
	free(imgData);

	return ret;
}

void DeleteBitmapFont(BitmapFont* font)
{
	if (font) 
	{
		GLTexture_Destroy(font->FontTexture);
		free(font);
	}
}

SubTexture FontGetChar(BitmapFont* font, float fontSize, char ch, vec2* size, float* advance)
{
	SubTexture ret;
	ret.texture = font->FontTexture;

	uint32_t index = ch - font->baseCharOffset;
	uint32_t row = index / font->rowPitch;
	uint32_t col = index - row * font->rowPitch;

	ret.texRect.Size = font->CellFactors;
	ret.texRect.Pos = vec2_Mul(font->CellFactors, new_vec2(col, row));

	if (font->invertY) { ret.texRect.Size.y *= -1.f; ret.texRect.Pos.y += font->CellFactors.y; };

	float aspect = font->CellFactors.y / font->CellFactors.x;
	if (size) *size = new_vec2(fontSize, aspect * fontSize);
	if (advance) *advance = fontSize * (float)font->charWidths[ch] * font->advanceFactor;

	return ret;
}
