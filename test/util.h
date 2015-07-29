#ifndef __MTC_UTIL_H
#define __MTC_UTIL_H

#include <hb-ft.h>
#ifdef _WIN32 // Win32 コード
#include <windows.h>
#endif

/*
关于颜色的宏
*/
#if defined (WIN32) 
#define TORGB(r,g,b) ((((unsigned)(b) | (((unsigned)(g)) << 8)) | (((unsigned)(r)) << 16)))
#else
#define TORGB(r,g,b) ( 0xFF000000 | (((unsigned)(b) | (((unsigned)(g)) << 8)) | (((unsigned)(r)) << 16)))
#endif


namespace MTC{namespace Util{

	struct Point
	{
		long x;
		long y;
		Point() :x(0), y(0){};
		Point(long X, long Y) :x(X), y(Y){};
	};
	struct Size
	{
		long width;
		long height;
		Size() :width(0), height(0){};
		Size(long w, long h) :width(w), height(h){};
	};
	struct Rect{
		long	left;
		long	top;
		long	right;
		long	bottom;
		Rect(long l, long t, long r, long b);
#ifdef _WIN32 // Win32 コード
		Rect(RECT & rect);
#endif
		//WINDOWS
		long	width();// { right - left; }
		long	height();// { bottom - top; }
		Size&	size();
	};

	typedef struct FontOption{
		static const int MONGOL = 0;
		static const int OTHER = 1;
		FT_Library		ft_library;
		FT_Face			ft_face[2];
		hb_font_t *		hb_ft_font[2];
		unsigned int	fore;
		unsigned int	back;
		int				LineHeight() const;
		int				Descender() const;
		int				Ascender() const;
		int				font_index_from_script(hb_script_t) const;
		FontOption(int size, int fore, int back);
		virtual ~FontOption();
	} FixedFont;


	enum DRAW_MODE
	{
		DARW_MODE_OPAQUE = 1,
		DARW_MODE_TRANSPARENT = 2
	};

	void FreeTypeDrawBitmap(unsigned int * buffer, int width, int height, DRAW_MODE mode, FT_Bitmap *bitmap, int x, int y, unsigned fore, unsigned back);
	bool Rotate90Degree(const FT_Bitmap *src, FT_Bitmap *dest);
}}

#endif /*__MTC_UTIL_H*/