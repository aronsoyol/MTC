#include "mndebug.h"
#include "stdio.h"
#include <stdarg.h>

#if defined (_WIN32)
#include <Windows.h>
#elif defined(ANDROID)
#include <android/log.h>
#endif
#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>

void _mndebug(int level, const char * format, ...)
{
	if (level < DEBUG_LEVEL)
		return;
	va_list arg;
	va_start(arg, format);
	
#if defined (_MSC_VER)
	char buf[1024];
	vsprintf(buf, format, arg);
	OutputDebugStringA(buf);
#elif defined(ANDROID)
	__android_log_print(2, "mtc", format, arg);
#else
	printf(format, arg);
#endif
	va_end(arg);

}