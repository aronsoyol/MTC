#ifndef __MTC_BREAKER_H
#define __MTC_BREAKER_H

#include <vector>
#include <string>
#include <stdint.h>

#ifdef ICU
#include <unicode/uscript.h>
#include <unicode/brkiter.h>
#endif

#ifdef JNI
#include <jni.h>
#endif


namespace MTC{

enum BreakType
{
	BT_LINE = 0,
	BT_WORD
};
class Breaker
{
	static const int DONE = -1;
public:
	virtual void setText(const uint16_t*, int) = 0;
	virtual int first() = 0;
	virtual int next() = 0;
	virtual ~Breaker(){};
};

#ifdef ICU

class IcuBreaker :public Breaker
{
	BreakIterator *brkItor;
public:
	IcuBreaker(BreakType type);
	virtual void setText(const uint16_t*, int);
	virtual int first();
	virtual int next();
	virtual ~IcuBreaker();
};
#endif

#ifdef JNI
class JNIBreaker :public Breaker
{
	JNIEnv *_env;

	jobject jnibreaker;
	jclass clsBreakIterator;
	jstring jtext;
	jmethodID method_setText;
	jmethodID method_next;
	jmethodID method_createLineInstance;
	jmethodID method_first;
public:
	JNIBreaker(JNIEnv *env, BreakType type);
	virtual void setText(const uint16_t*, int);
	virtual int first();
	virtual int next();
	virtual ~JNIBreaker();
};
#endif/*JNI*/
}
#endif /*__MTC_BREAKER_H*/