#ifndef __MTC_UTIL_H
#define __MTC_UTIL_H
#include <windows.h>

namespace MTC{namespace Util{
	struct Point
	{
		long x;
		long y;
		Point();
		Point(long, long);
	};
	struct Size
	{
		long width;
		long height;
		Size();
		Size(long w, long h);
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
		Size&	Size();
	};
}}

#endif /*__MTC_UTIL_H*/