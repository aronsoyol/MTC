#pragma once
//void HBDrawText(HDC dc, int x, int y);
#include "../hb/hb-ft.h"
#include "../hb/hb-buffer-private.hh"
#include "../hb/hb-icu.h"

#define MONGOL_FONT 0
#define OTHER_FONT	1

struct FontOption{

	FT_Library		ft_library;
	FT_Face			ft_face[2];
	hb_font_t *		hb_ft_font[2];
	COLORREF		fore;
	COLORREF		back;
	FontOption(int size, int fore, int baack);
	virtual ~FontOption();
};
#define DARW_MODE_OPAQUE		1
#define DARW_MODE_TRANSPARENT	2
void HBDrawTextB(HDC dc, int draw_mode, const FontOption* fontOption, int x, int y, const wchar_t* text, UScriptCode icuScriptCode);
void HBDrawTextW(HDC dc, int draw_mode, const FontOption* fontOption, int x, int y, const wchar_t* text, UScriptCode icuScriptCode);
void FreeTypeDrawBitmap(HDC hdc, int draw_mode, FT_Bitmap *bitmap, int x, int y, unsigned fore, unsigned back);
void FreeTypeDrawBitmap(unsigned int * buffer, int width, int height, int draw_mode, FT_Bitmap *bitmap, int x, int y, unsigned fore, unsigned back);