﻿#include "../hb/hb-ft.h"
#include "../hb/hb-buffer-private.hh"
#include <windows.h>
#include <vector>

#include "hb_draw.h"

char bw  ;
void FreeTypeDrawBitmapMono(HDC hdc, FT_Bitmap *bitmap, int x, int y){
	int i, j;
	int width, height, pitch;
	int c;

	width = bitmap->width;
	height = bitmap->rows;
	pitch = bitmap->pitch;

	for(j = 0;j < height;++j){
		for(i = 0;i < width;++i){
			c = 255 - ((bitmap->buffer[j * pitch + (i / 8)] >> (7 - (i % 8))) & 1) * 255;
			//if (c == 0)
			SetPixel(hdc, x + i, y + j, RGB(c,c,c));
		}
	}
}

void FreeTypeDrawBitmap256(HDC hdc, FT_Bitmap *bitmap, int x, int y){
	/*
	http://web.comhem.se/~u34598116/content/FreeType2/main.html
	*/

	/*
	Note that the coloured bitmap isn't cached, which means that the
	transparency calculation is performed on a character bitmap every time
	a character is written to the video buffer. This isn't good if you are
	concerned with performance.
	*/
	int x_max = bitmap->width;
	int y_max = bitmap->rows;
	//int x, y;
	unsigned int fontColor = RGB(0, 0, 0);
	unsigned int backgroundColor = GetSysColor(COLOR_GRAYTEXT);

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
	for (int i = 0; i < x_max; i++){        /* For each horizontal pixel..        */
		for (int j = 0; j < y_max; j++){    /* ...in each row of the font bitmap. */

			int c = bitmap->buffer[j * x_max + i];
			//if (x >= WIDTH || y >= HEIGHT){ continue; }
			if (c == 0){
				/* Render background color. */
					//SetPixel(hdc, x + i, y + j, backgroundColor);
				continue;
			}
			else if (c == 255)
			{
				SetPixel(hdc, x + i, y + j, fontColor);
			}
			else {
				/* Calculate alpha (opacity). *////////////
				opacity = c / 255.0f;// bitmap->buffer[j * bitmap->width + i];
				opacity2 = 1 - opacity;

				int r = fontColorR * opacity + opacity2 * backgroundColorR;

				int g = fontColorG * opacity + opacity2 * backgroundColorG;

				int b = fontColorB * opacity + opacity2 * backgroundColorB;

				COLORREF col = RGB(r, g, b);

				SetPixel(hdc, x + i, y + j, RGB(r, g, b));
				
			}
		}
	}
}

void FreeTypeDrawBitmap(HDC hdc, FT_Bitmap *bitmap, int x, int y){
	switch(bitmap->pixel_mode){
	case FT_PIXEL_MODE_GRAY:
		FreeTypeDrawBitmap256(hdc, bitmap, x, y);
		break;
	case FT_PIXEL_MODE_MONO:
		FreeTypeDrawBitmapMono(hdc, bitmap, x, y);
		break;
	}
}

void HBDrawText(HDC dc, int x, int y)
{
	const char		*	fontFilePath = "../MongolianWhite.ttf";
	const WCHAR		*	lFontFilePath = L"../MongolianWhite.ttf";
	const WCHAR		*	text = L"ᠺᠣᠮᠫᠢᠦ᠋ᠲ᠋ᠧᠷ ᠠᠬ᠎ᠠ ᠡ ";
	WCHAR buffer_path[256];

	GetFullPathName(lFontFilePath, 256,buffer_path, 0);
	FT_Library			ft_library;
	FT_Face				ft_face;

	FT_Init_FreeType(&ft_library); /* initialize library */
	FT_New_Face(ft_library, fontFilePath, 0, &ft_face); /* create face object */
	FT_Set_Pixel_Sizes(ft_face, 0, 54); /* set character size */

	hb_font_t		*	font	= hb_ft_font_create(ft_face, NULL);
	hb_buffer_t		*	buffer	= hb_buffer_create();

	int* charWidthList = new int[max(lstrlen(text), 2)];
	for (int i = 0; i < max(lstrlen(text), 2); i++)
		charWidthList[i] = 0;
	//
	//set buffer
	//	
	hb_buffer_add_utf16(buffer, (uint16_t*)text, lstrlen(text), 0, lstrlen(text));
	hb_buffer_set_direction(buffer, HB_DIRECTION_TTB); /* or LTR */
	hb_buffer_set_script(buffer, HB_SCRIPT_MONGOLIAN); /* see hb-unicode.h */
	hb_buffer_set_flags(buffer, (hb_buffer_flags_t)(HB_BUFFER_FLAG_PRESERVE_DEFAULT_IGNORABLES | HB_BUFFER_FLAG_EOT));
	//hb_buffer_guess_segment_properties(buffer);
	//
	//
	//
	hb_shape(font, buffer, NULL, 0);

	int					glyph_count = hb_buffer_get_length(buffer);
	hb_glyph_info_t *	glyph_info	= hb_buffer_get_glyph_infos(buffer, 0);

	WCHAR				dbg_info[6];
	int					pen_x			= x;
	int					yBaseLine		= y;
	int					glyphPosX		= 0;
	int					glyphPosY		= 0;
	int					lastClstIndex	= 0;

	for (int i = 0; i < glyph_count; ++i) 
	{
		FT_UInt glyph_index = glyph_info[i].codepoint;
		FT_Load_Glyph(ft_face, glyph_index, FT_LOAD_DEFAULT);

		wsprintf(dbg_info, L"%d,", glyph_info[i].cluster/* + 87*/);
		OutputDebugString(dbg_info);

		/* convert to an anti-aliased bitmap */
		FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL);

		FreeTypeDrawBitmap(dc, &ft_face->glyph->bitmap, pen_x + ft_face->glyph->bitmap_left,
				yBaseLine - ft_face->glyph->bitmap_top);
		pen_x += ft_face->glyph->advance.x >> 6;

		{
			const ULONG U63 = 63;
			int glyfWidth = ft_face->glyph->advance.x >> 6;
			UINT fraction = ft_face->glyph->advance.x & U63;
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
	hb_font_destroy(font);
	FT_Done_Face(ft_face);
	FT_Done_FreeType(ft_library);
}

void HBDrawTextB(HDC dc, int x, int y)
{
	bw = 'b';
	HBDrawText( dc, x, y);
}

void HBDrawTextW(HDC dc, int x, int y)
{
	bw = 'w';
	HBDrawText( dc, x, y);
}
