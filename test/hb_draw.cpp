#include "../hb/hb-ft.h"
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
	FT_Library ft_library;
	FT_Face ft_face;

	hb_font_t *font;
	hb_buffer_t *buffer;
	int glyph_count;
	hb_glyph_info_t *glyph_info;

	hb_glyph_position_t *glyph_pos;
	hb_bool_t fail;

	FT_UInt glyph_index;
	FT_GlyphSlot slot;
	FT_Error error;
	int charHeight = 40;
	char* fontFilePath;
	//jboolean iscopy;
	//const jchar *text;
	const wchar_t *text = L"ᠺᠣᠮᠫᠢᠦ᠋ᠲ᠋ᠧᠷ ᠠᠬ᠎ᠠ";
	int num_chars, i;

	int pen_x,yBaseLine = y;
	int glyphPosX, glyphPosY;

	if(bw == 'b')
		fontFilePath = "../hb_test_ms_baiti.ttf";
	else
		fontFilePath = "../hb_test_almas_white.ttf";
		//	"mongolianwhite.ttf";
	//text = (*env)->GetStringChars(env, unicodeText, &iscopy);
	num_chars = lstrlen(text);

	error = FT_Init_FreeType(&ft_library); /* initialize library */
	if (error) {
		//__android_log_print(6, "drawIndicText",
		//		"Error initializing FreeType library\n");
		return;
	}
	//__android_log_print(2, "drawIndicText",
	//		"Successfully initialized FreeType library\n");

	error = FT_New_Face(ft_library, fontFilePath, 0, &ft_face); /* create face object */
	if (error == FT_Err_Unknown_File_Format) {
		//__android_log_print(6, "drawIndicText",
				//"The font file could be opened and read, but it appears that its font format is unsupported");
		return;
	} else if (error) {
		//__android_log_print(6, "drawIndicText",
		//		"The font file could not be opened or read, or it might be broken");
		return;
	}
	//__android_log_print(2, "drawIndicText",
	//		"Successfully created font-face object\n");

	font = hb_ft_font_create(ft_face, NULL);

	error = FT_Set_Pixel_Sizes(ft_face, 0, charHeight); /* set character size */
	/* error handling omitted */
	//__android_log_print(2, "drawIndicText",
	//		"Successfully set character size to %d\n", charHeight);

	//__android_log_print(2, "drawIndicText", "Text being rendered = %s\n", text);
	slot = ft_face->glyph;
	pen_x = x;

	/* Create a buffer for harfbuzz to use */
	buffer = hb_buffer_create();
	
	//hb_buffer_set_unicode_funcs(buffer, hb_icu_get_unicode_funcs());
	//alternatively you can use hb_buffer_set_unicode_funcs(buffer, hb_glib_get_unicode_funcs());

	hb_buffer_set_direction(buffer, HB_DIRECTION_TTB); /* or LTR */
	hb_buffer_set_script(buffer, HB_SCRIPT_MONGOLIAN); /* see hb-unicode.h */
	//hb_buffer_set_language(buffer, hb_language_from_string("ka"));

	/* Layout the text */
	hb_buffer_add_utf16(buffer, (uint16_t *)text, num_chars, 0, num_chars);
	//__android_log_print(2, "drawIndicText", "Before HarfBuzz shape()\n");
	hb_shape(font, buffer, NULL, 0);
	//__android_log_print(2, "drawIndicText", "After HarfBuzz shape()\n");

	glyph_count = hb_buffer_get_length(buffer);
	glyph_info	= hb_buffer_get_glyph_infos(buffer, 0);
	glyph_pos	= hb_buffer_get_glyph_positions(buffer, 0);

	for (i = 0; i < glyph_count; i++) {
		glyph_index = glyph_info[i].codepoint;
		//__android_log_print(2, "drawIndicText", "Glyph%d = %x", i, glyph_index);
		error = FT_Load_Glyph(ft_face, glyph_index, FT_LOAD_DEFAULT);
		if (error) {
			continue; /* ignore errors */
		}
		WCHAR info[6];
		wsprintf(info,L"%X,",glyph_index);
		OutputDebugString(info);
		/* convert to an anti-aliased bitmap */
		error = FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL);
		if (error) {
			continue;
		}

		/* now, draw to our target surface (convert position) */
		FreeTypeDrawBitmap(dc, &slot->bitmap, pen_x + slot->bitmap_left,
				yBaseLine - slot->bitmap_top);
		//FreeTypeDrawBitmap(dc, pen_x, y);
		//glyphPosX = pen_x + glyph_pos[i].x_offset;
		//glyphPosY = yBaseLine - glyph_pos[i].y_offset;
		//draw_bitmap(&slot->bitmap, glyphPosX, glyphPosY, env, thiz, lock);

		/* increment pen position */
		pen_x += slot->advance.x >> 6;
		//pen_x += glyph_pos[i].x_advance / 64;
		//__android_log_print(2, "drawIndicText",
		//		"\tx_offset = %d\ty_offset = %d\tx_advance = %d\ty_advance = %d\n",
		//		glyph_pos[i].x_offset / 64, glyph_pos[i].y_offset / 64,
		//		glyph_pos[i].x_advance / 64, glyph_pos[i].y_advance / 64);
		//__android_log_print(2, "drawIndicText",
		//		"\tbitmap_left = %d\tbitmap_top = %d\tadvance.x = %d\tadvance.y = %d\n",
		//		slot->bitmap_left, slot->bitmap_top, slot->advance.x >> 6,
		//		slot->advance.y >> 6);
	}
	OutputDebugString(L"\n");
	hb_buffer_destroy(buffer);

	//(*env)->ReleaseStringChars(env, unicodeText, text);
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
