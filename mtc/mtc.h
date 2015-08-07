#ifndef __MTC_H
#define  __MTC_H
#include <stdint.h>

#if defined(ANDROID)
#include <jni.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif
	void* mtc_font_create(int font_size, int fore, int back);
	void  mtc_font_destroy(void *font);
	int   mtc_font_line_height(void* font);

#if defined(ANDROID)
	void*mtc_create_layout(JNIEnv* env, void* font);
#else
	void*mtc_create_layout(void* font);
#endif
	
	void mtc_destroy_layout(void* layout);
	void mtc_set_text(void* layout, const uint16_t* text, long length);
	int  mtc_break_line(void* layout, int height);
	void mtc_draw(const void* layout, unsigned int* buffer, int width, int height, int x, int y);
	int  mtc_get_char_position(const void* layout, int x, int y, unsigned char* trailing);
	void mtc_get_char_location(const void* layout, int char_pos, unsigned char trailing, int* x, int* y);
	
	
#ifdef __cplusplus
}
#endif
#endif