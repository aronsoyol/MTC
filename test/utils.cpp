#include "util.h"
#include <vector>
#include <algorithm>
namespace MTC{	namespace Util{
#if  defined (ANDROID)
	const char* fontName[] = { 
		"/data/data/org.iisc.mile.indictext.android/files/MongolianWhite.ttf", 
		"/data/data/org.iisc.mile.indictext.android/files/msyh.ttf" 
	};
#else
	const char* fontName[] = { "../MongolianWhite.ttf", "../msyh.ttf" };
#endif
	int FontOption::Ascender() const
	{
		return std::max(ft_face[0]->ascender, ft_face[1]->ascender) >> 6;
	}
	int FontOption::Descender() const
	{
		return std::min(ft_face[0]->descender, ft_face[1]->descender) >> 6;
	}
	int FontOption::LineHeight() const
	{
		int max_ascender = std::max(ft_face[0]->ascender, ft_face[1]->ascender) >> 6;
		int min_descender = std::min(ft_face[0]->descender, ft_face[1]->descender) >> 6;

		/*
		http://lists.gnu.org/archive/html/freetype/2001-08/msg00066.html
		*/
		int line_gap0 = (ft_face[0]->size->metrics.height - ft_face[0]->size->metrics.ascender) >> 6;
		int line_gap1 = (ft_face[1]->size->metrics.height - ft_face[1]->size->metrics.ascender) >> 6;
		return max_ascender - min_descender + std::max(line_gap0, line_gap1);
	}
	FontOption::FontOption(int size, int fore_, int back_)
	{
		FT_Init_FreeType(&ft_library); /* initialize library */
		fore = fore_;
		back = back_;
		//GetFullPathName(fontName[0], MAX_PATH, fullPath, 0);
		for (int i = 0; i < 2; i++)
		{
			FT_New_Face(ft_library, fontName[i], 0, &ft_face[i]); /* create face object */
			FT_Set_Pixel_Sizes(ft_face[i], 0, size); /* set character size */
			hb_ft_font[i] = hb_ft_font_create(ft_face[i], NULL);
		}
	}
	FontOption::~FontOption()
	{
		for (int i = 0; i < 2; i++)
		{
			hb_font_destroy(hb_ft_font[i]);
			FT_Done_Face(ft_face[i]);
		}
		FT_Done_FreeType(ft_library);
	}
	int FontOption::font_index_from_script(hb_script_t script) const
	{
		if (script == HB_SCRIPT_MONGOLIAN)
			return MONGOL;
		else
			return OTHER;
	}
}}