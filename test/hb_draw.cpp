#include <vector>
#include <algorithm>
#include <cassert>
#include "util.h"

namespace MTC{	namespace Util{
char bw  ;

float cal_table[] =
{ 
	0.00, 0.01, 0.01, 0.02, 0.02, 0.02, 0.03, 0.03, 0.04, 0.04, 0.04, 0.05, 0.05, 0.05, 0.06, 0.06,
	0.07, 0.07, 0.07, 0.08, 0.08, 0.09, 0.09, 0.09, 0.10, 0.10, 0.11, 0.11, 0.11, 0.12, 0.12, 0.13,
	0.13, 0.13, 0.14, 0.14, 0.14, 0.15, 0.15, 0.16, 0.16, 0.16, 0.17, 0.17, 0.18, 0.18, 0.18, 0.19,
	0.19, 0.20, 0.20, 0.20, 0.21, 0.21, 0.21, 0.22, 0.22, 0.23, 0.23, 0.23, 0.24, 0.24, 0.25, 0.25,
	0.25, 0.26, 0.26, 0.27, 0.27, 0.27, 0.28, 0.28, 0.29, 0.29, 0.29, 0.30, 0.30, 0.30, 0.31, 0.31,
	0.32, 0.32, 0.32, 0.33, 0.33, 0.34, 0.34, 0.34, 0.35, 0.35, 0.36, 0.36, 0.36, 0.37, 0.37, 0.38,
	0.38, 0.38, 0.39, 0.39, 0.39, 0.40, 0.40, 0.41, 0.41, 0.41, 0.42, 0.42, 0.43, 0.43, 0.43, 0.44,
	0.44, 0.45, 0.45, 0.45, 0.46, 0.46, 0.46, 0.47, 0.47, 0.48, 0.48, 0.48, 0.49, 0.49, 0.50, 0.50,
	0.50, 0.51, 0.51, 0.52, 0.52, 0.52, 0.53, 0.53, 0.54, 0.54, 0.54, 0.55, 0.55, 0.55, 0.56, 0.56,
	0.57, 0.57, 0.57, 0.58, 0.58, 0.59, 0.59, 0.59, 0.60, 0.60, 0.61, 0.61, 0.61, 0.62, 0.62, 0.63,
	0.63, 0.63, 0.64, 0.64, 0.64, 0.65, 0.65, 0.66, 0.66, 0.66, 0.67, 0.67, 0.68, 0.68, 0.68, 0.69,
	0.69, 0.70, 0.70, 0.70, 0.71, 0.71, 0.71, 0.72, 0.72, 0.73, 0.73, 0.73, 0.74, 0.74, 0.75, 0.75,
	0.75, 0.76, 0.76, 0.77, 0.77, 0.77, 0.78, 0.78, 0.79, 0.79, 0.79, 0.80, 0.80, 0.80, 0.81, 0.81,
	0.82, 0.82, 0.82, 0.83, 0.83, 0.84, 0.84, 0.84, 0.85, 0.85, 0.86, 0.86, 0.86, 0.87, 0.87, 0.88,
	0.88, 0.88, 0.89, 0.89, 0.89, 0.90, 0.90, 0.91, 0.91, 0.91, 0.92, 0.92, 0.93, 0.93, 0.93, 0.94,
	0.94, 0.95, 0.95, 0.95, 0.96, 0.96, 0.96, 0.97, 0.97, 0.98, 0.98, 0.98, 0.99, 0.99, 1.00, 1.00 
};

bool Rotate90Degree(const FT_Bitmap *src, FT_Bitmap *dest)
{
	//if (src->pixel_mode=)
	int size = src->width * src->rows;
	
	dest->buffer = new unsigned char[size];
	int height = src->width;
	int width = src->rows;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			assert(i * width + j < size);
			int index = (src->rows - 1 - j) * src->width + i;
			assert(index < size && index >= 0);
			dest->buffer[i * width + j] = src->buffer[index];
		}
	}
	dest->rows = src->width;
	dest->width = src->rows;
	return true;
}
void FreeTypeDrawBitmap256(unsigned int * buffer, int width, int height, DRAW_MODE mode, FT_Bitmap *bitmap, int x, int y, unsigned fore, unsigned back)
{
	int x_max = bitmap->width;
	int y_max = bitmap->rows;
	//int x, y;
	unsigned int fontColor = fore;
	unsigned int backgroundColor = back;// GetSysColor(COLOR_WINDOWFRAME);

	unsigned int fontColorB = ((fontColor & 0x00FF0000) >> 16);
	unsigned int fontColorG = ((fontColor & 0x0000FF00) >> 8);
	unsigned int fontColorR = (fontColor & 0x000000FF);

	unsigned int backgroundColorB = ((backgroundColor & 0x00FF0000) >> 16);
	unsigned int backgroundColorG = ((backgroundColor & 0x0000FF00) >> 8);
	unsigned int backgroundColorR = (backgroundColor & 0x000000FF);

	unsigned int aR = fontColorR - backgroundColorR;
	unsigned int aG = fontColorG - backgroundColorG;
	unsigned int aB = fontColorB - backgroundColorB;

	float opacity;
	float opacity2;

	COLORREF col[256];

	for (int i = 0; i < 256; i++)
	{
		int ii = 255 - i;

		int r = cal_table[fontColorR] * i + ii * cal_table[backgroundColorR];

		int g = cal_table[fontColorG] * i + ii * cal_table[backgroundColorG];

		int b = cal_table[fontColorB] * i + ii * cal_table[backgroundColorB];

		col[i] = RGB(b, g, r);
		//col[ii] = RGB(255-r, 255-g, 255-b);
	}
	int Ymax = std::min(height, y + y_max);
	int Xmax = std::min(width, x + x_max);
	int buffer_length = height * width;
	int start_cl =0 ,row = 0;
	if (y < 0)
		row = -y;
	if (x < 0)
		start_cl = -x;
	for (; row < y_max && row + y < height; row++){        /* For each horizontal pixel..        */
		for (int cl= start_cl; cl < x_max && x + cl < width; cl++){    /* ...in each row of the font bitmap. */
			int grey = bitmap->buffer[row * x_max + cl];
			
			int index = width * (height - 1 - row - y) + x + cl;
			assert(index < buffer_length);
			if (buffer[index] != 0)
				buffer[index] = (unsigned int)col[grey];
		}
	}

}
void FreeTypeDrawBitmap256(HDC hdc, FT_Bitmap *bitmap, int x, int y, unsigned fore, unsigned back){
	/*
	http://web.comhem.se/~u34598116/content/FreeType2/main.html
	*/
	//return;
	/*
	Note that the coloured bitmap isn't cached, which means that the
	transparency calculation is performed on a character bitmap every time
	a character is written to the video buffer. This isn't good if you are
	concerned with performance.
	*/
	int x_max = bitmap->width;
	int y_max = bitmap->rows;
	//int x, y;
	unsigned int fontColor = fore;
	unsigned int backgroundColor = back;// GetSysColor(COLOR_WINDOWFRAME);

	unsigned int fontColorB = ((fontColor & 0x00FF0000) >> 16);
	unsigned int fontColorG = ((fontColor & 0x0000FF00) >> 8);
	unsigned int fontColorR = (fontColor & 0x000000FF);

	unsigned int backgroundColorB = ((backgroundColor & 0x00FF0000) >> 16);
	unsigned int backgroundColorG = ((backgroundColor & 0x0000FF00) >> 8);
	unsigned int backgroundColorR = (backgroundColor & 0x000000FF);

	unsigned int aR = fontColorR - backgroundColorR;
	unsigned int aG = fontColorG - backgroundColorG;
	unsigned int aB = fontColorB - backgroundColorB;

	float opacity;
	float opacity2;

	COLORREF col[256];

	for (int i = 0; i < 256; i++)
	{
		int ii = 255 - i;

		int r = cal_table[fontColorR] * i + ii * cal_table[backgroundColorR];

		int g = cal_table[fontColorG] * i + ii * cal_table[backgroundColorG];

		int b = cal_table[fontColorB] * i + ii * cal_table[backgroundColorB];

		col[i] = RGB(r, g, b);
		//col[ii] = RGB(255-r, 255-g, 255-b);
	}

	for (int i = 0; i < x_max; i++){        /* For each horizontal pixel..        */
		for (int j = 0; j < y_max; j++){    /* ...in each row of the font bitmap. */
			int grey = bitmap->buffer[j * x_max + i];
			//if (grey != 0)
				SetPixelV(hdc, x + i, y + j, col[grey]);
		}
	}
}
void FreeTypeDrawBitmap(unsigned int* buffer, int width, int height, int draw_mode, FT_Bitmap *bitmap, int x, int y, unsigned fore, unsigned back)
{

	if (x >= width || x + bitmap->width < 0 || y >= height || y + bitmap->rows < 0)
		return;
	else
	{
		FreeTypeDrawBitmap256(buffer, width, height, bitmap, x, y, fore, back);
	}

}
void FreeTypeDrawBitmap(HDC hdc, int draw_mode, FT_Bitmap *bitmap, int x, int y, unsigned fore, unsigned back){

	RECT rect;
	GetClipBox(hdc, &rect);

	if (x >= rect.right || x + bitmap->width < rect.left || y >= rect.bottom || y + bitmap->rows < rect.top)
		return;
	else
	{
		switch (bitmap->pixel_mode){
		case FT_PIXEL_MODE_GRAY:
			FreeTypeDrawBitmap256(hdc, bitmap, x, y, fore, back);
			break;
		case FT_PIXEL_MODE_MONO:
			FreeTypeDrawBitmapMono(hdc, bitmap, x, y);
			break;
		}
	}

}

//
void HBDrawText(HDC dc, int draw_mode, const FontOption* fontOption, int x, int y, const wchar_t* text, UScriptCode icuScriptCode)
{

	//const char		*	fontFilePath = "../msyh.ttf";// "../MongolianWhite.ttf";
	//const WCHAR		*	lFontFilePath = L"../msyh.ttf"; //L"../MongolianWhite.ttf";

	//FT_Library			ft_library;
	//FT_Face				ft_face;

	//FT_Init_FreeType(&ft_library); /* initialize library */
	//FT_New_Face(ft_library, fontFilePath, 0, &ft_face); /* create face object */
	//FT_Set_Pixel_Sizes(ft_face, 0, 54); /* set character size */

	//hb_font_t		*	font	= hb_ft_font_create(ft_face, NULL);
	hb_buffer_t		*	buffer	= hb_buffer_create();

	int* charWidthList = new int[std::max(lstrlen(text), 2)];
	for (int i = 0; i < std::max(lstrlen(text), 2); i++)
		charWidthList[i] = 0;
	//
	//set buffer
	//	
	hb_script_t scriptTag = hb_icu_script_to_script(icuScriptCode);
	hb_buffer_add_utf16(buffer, (uint16_t*)text, lstrlen(text), 0, lstrlen(text));
	hb_buffer_set_direction(buffer, HB_DIRECTION_TTB); /* or LTR */
	hb_buffer_set_script(buffer, scriptTag); /* see hb-unicode.h */
	hb_buffer_set_flags(buffer, (hb_buffer_flags_t)(HB_BUFFER_FLAG_PRESERVE_DEFAULT_IGNORABLES | HB_BUFFER_FLAG_EOT));
	//hb_buffer_guess_segment_properties(buffer);
	//
	//
	//
	const int fontIndex = icuScriptCode == USCRIPT_MONGOLIAN ? MONGOL_FONT : OTHER_FONT;
	hb_shape(fontOption->hb_ft_font[fontIndex], buffer, NULL, 0);

	int					glyph_count		= hb_buffer_get_length(buffer);
	hb_glyph_info_t *	glyph_info		= hb_buffer_get_glyph_infos(buffer, 0);
	hb_glyph_position_t *glyph_pos		= hb_buffer_get_glyph_positions(buffer, 0);


	WCHAR				dbg_info[6];
	int					pen_x			= x;
	int					pen_y			= y;
	int					yBaseLine		= y;
	int					glyphPosX		= 0;
	int					glyphPosY		= 0;
	int					lastClstIndex	= 0;

	for (int i = 0; i < glyph_count; ++i)
	{
		FT_UInt glyph_index = glyph_info[i].codepoint;
		FT_Load_Glyph(fontOption->ft_face[fontIndex], glyph_index, FT_LOAD_DEFAULT);

		wsprintf(dbg_info, L"%d,", glyph_info[i].cluster/* + 87*/);
		OutputDebugString(dbg_info);

		/* convert to an anti-aliased bitmap */
		FT_Render_Glyph(fontOption->ft_face[fontIndex]->glyph, FT_RENDER_MODE_NORMAL);
		int offset_y = glyph_pos[i].y_offset / 64;
		int offset_x = glyph_pos[i].x_offset / 64;
		FreeTypeDrawBitmap(dc, draw_mode, &fontOption->ft_face[fontIndex]->glyph->bitmap, pen_x + fontOption->ft_face[fontIndex]->glyph->bitmap_left + offset_x,
			pen_y - fontOption->ft_face[fontIndex]->glyph->bitmap_top - offset_y, fontOption->fore, fontOption->back);
		pen_y -= glyph_pos[i].y_advance / 64 ;// ft_face->glyph->advance.x >> 6;

		{
			const ULONG U63 = 63;
			int glyfWidth = fontOption->ft_face[fontIndex]->glyph->advance.x >> 6;
			UINT fraction = fontOption->ft_face[fontIndex]->glyph->advance.x & U63;
			float fWidth = (float)glyfWidth + (float)fraction / 64.0f;
			float diff = fWidth - (int)fWidth;
			if (fraction != 0)
			{
				int stop = 1;
			}
			int charCount = glyph_info[i].cluster - lastClstIndex;
			if (charCount > 1)
			{
				int clustWidth = charWidthList[lastClstIndex];
				int charWidth = clustWidth / charCount;
				for (int j = lastClstIndex; j < glyph_info[i].cluster; j++)
				{
					charWidthList[j] = charWidth;
				}
			}
			charWidthList[glyph_info[i].cluster] += glyfWidth;
			lastClstIndex = glyph_info[i].cluster;
		}
	}
	if (glyph_info[glyph_count].cluster < lstrlen(text) - 1)
	{
		int clustWidth = charWidthList[glyph_count - 1];
		int charCount = lstrlen(text) - glyph_info[glyph_count].cluster;
		int charWidth = clustWidth / charCount;
		for (int j = lastClstIndex; j < lastClstIndex + charCount; j++)
		{
			charWidthList[j] = charWidth;
		}
	}
	int dx = x;
	for (int i = 0; i < lstrlen(text); i++)
	{
		dx += charWidthList[i];
		MoveToEx(dc, dx, y, 0);
		LineTo(dc, dx, y - 40);
	}

	OutputDebugString(L"\n"); 
	delete[] charWidthList;
	hb_buffer_destroy(buffer);
}

void HBDrawTextB(HDC dc, int draw_mode, const FontOption* fontOption, int x, int y, const wchar_t* text, UScriptCode icuScriptCode)
{
	bw = 'b';
	HBDrawText(dc, draw_mode, fontOption, x, y, text, icuScriptCode);
}

void HBDrawTextW(HDC dc, int draw_mode, const FontOption* fontOption, int x, int y, const wchar_t* text, UScriptCode icuScriptCode)
{
	bw = 'w';
	HBDrawText(dc, draw_mode, fontOption, x, y, text, icuScriptCode);
}
}}