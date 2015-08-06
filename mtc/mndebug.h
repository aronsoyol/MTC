#ifndef MNDEBUG_H
#define MNDEBUG_H
#ifdef __cplusplus
extern "C" {
#endif
#define DEBUG_LEVEL 1 


void _mndebug(int level, const char * format, ...);

#ifdef	MN_DEBUG
#define mndebug(level, format, ...) _mndebug((level), (format), __VA_ARGS__)
#else	
//#define assert(_Expression)     ((void)0)
#define mndebug(level, format, ...) ((void)0)
#endif
#ifdef __cplusplus
}//extern "C" {
#endif
#endif// MNDEBUG_H