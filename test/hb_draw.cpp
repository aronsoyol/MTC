#include "../hb/hb-ft.h"
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
			SetPixel(hdc, x + i, y + j, RGB(c,c,c));
		}
	}
}

void FreeTypeDrawBitmap256(HDC hdc, FT_Bitmap *bitmap, int x, int y){
	int i, j;
	int width, height;
	int c;

	width = bitmap->width;
	height = bitmap->rows;

	for(j = 0;j < height;++j){
		for(i = 0;i < width;++i){
			c = 255 - bitmap->buffer[j * width + i];
			SetPixel(hdc, x + i, y + j, RGB(c,c,c));
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
	int					pen_x		= x;
	int					yBaseLine	= y;
	int					glyphPosX	= 0;
	int					glyphPosY	= 0;

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
	}
	OutputDebugString(L"\n");
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
