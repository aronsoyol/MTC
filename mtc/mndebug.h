#ifndef MNDEBUG_H
#define MNDEBUG_H
#ifdef __cplusplus
extern "C" {
#endif
#define DEBUG_LEVEL 1 
	//#include "stdio.h"
	//#include <stdarg.h>
	//#include <wchar.h>
#include <Windows.h>
	//#include <stdarg.h>
	//#include <stdio.h>
	//#include <iostream>

	void _mndebug(int level, const char * format, ...);
	//{
	//	if (level < DEBUG_LEVEL)
	//		return;
	//
	//	va_list arg;
	//	va_start(arg, format);
	//	wchar_t buf[512];
	//
	//	wsprintf(buf, format, arg);
	//	OutputDebugString(buf);
	//	va_end(arg);
	//
	//}
#ifdef	MN_DEBUG
#define mndebug _mndebug
#else	
#define mndebug() 
#endif
#ifdef __cplusplus
}//extern "C" {
#endif
#endif// MNDEBUG_H