#include "mndebug.h"
#include "stdio.h"
#include <stdarg.h>
#include <wchar.h>
#include <Windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
//void mndebug(/*int level,*/ const wchar_t* fmt, ...)
//{
//	__restrict 
//	va_list varg;
//	va_start(varg, fmt);
//	wchar_t buf[512];
//	
//	wsprintf(buf, fmt, varg);
//	OutputDebugString(buf);
//	va_end(varg);
//}
//libc_hidden_proto(vfwprintf)
void _mndebug(int level, const char * format, ...)
{
	if (level < DEBUG_LEVEL)
		return;
	va_list arg;
	va_start(arg, format);
	char buf[1024];
	
	vsprintf(buf, format, arg);
	OutputDebugStringA(buf);
	va_end(arg);

}