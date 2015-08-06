#include "ParaLayout.h"
#include "mtc.h"

void* mtc_font_create(int font_size, int fore, int back)
{
	MTC::Util::FontOption* font = new MTC::Util::FontOption(font_size, fore, back);
	return (void*)font;
}
void  mtc_font_destroy(void *font)
{
	delete((MTC::Util::FontOption*) font);
}

int   mtc_font_line_height(void* font)
{
	return ((MTC::Util::FontOption*) font)->LineHeight();
}
#if defined(ANDROID)
void*mtc_create_layout(JNIEnv* env, void* font)
{
	MTC::LayoutEngine::ParaLayout * layout = new MTC::LayoutEngine::ParaLayout(env, (MTC::Util::FontOption*) font);
	return layout;
}
#else
void*mtc_create_layout(void* font)
{
	MTC::LayoutEngine::ParaLayout * layout = new MTC::LayoutEngine::ParaLayout((MTC::Util::FontOption*) font);
	return layout;
}
#endif
void mtc_destroy_layout(void* layout)
{
	MTC::LayoutEngine::ParaLayout * pLayout = (MTC::LayoutEngine::ParaLayout *)layout;
	delete pLayout;
}
void mtc_set_text(void* layout, const uint16_t* text, long length)
{
	std::u16string text16(text, text + length);
	((MTC::LayoutEngine::ParaLayout *)layout)->set_text(text16);
}
int mtc_break_line(void* layout, int height)
{
	return ((MTC::LayoutEngine::ParaLayout *)layout)->break_line(height);
}
void mtc_draw(const void* layout, unsigned int* buffer, int width, int height, int x, int y)
{
	((MTC::LayoutEngine::ParaLayout *)layout)->draw(buffer, width, height, x, y);
}
int mtc_get_char_position(const void* layout, int x, int y, unsigned char* trailling)
{
	bool t = false;
	int pos = ((MTC::LayoutEngine::ParaLayout *)layout)->get_char_position(x, y, &t);

	*trailling = t ? 1 : 0;
	return pos;
}
void mtc_get_char_location(const void* layout, int char_pos, unsigned char trailling, int* x, int* y)
{
	MTC::Util::Point point;
	if (((MTC::LayoutEngine::ParaLayout *)layout)->get_char_location(char_pos, trailling, &point))
	{
		unsigned long ret = 0;
		static_assert(sizeof(long) == 4, "size of long not eq 4");

		*x = point.x;
		*y = point.y;
	}
}
