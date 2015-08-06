#include "Breaker.h"
#ifdef ANDROID
#include <android/log.h>
#endif
namespace MTC{

#ifdef ICU
	IcuBreaker::IcuBreaker(BreakType type)
	{
		UErrorCode status;
		brkItor = BreakIterator::createLineInstance(Locale::getUS(), status);
		int i = 0;
	}
	void IcuBreaker::setText(const uint16_t* text, int length)
	{
		UnicodeString utext((const UChar*)text, length);
		brkItor->setText(utext);
	}
	int IcuBreaker::next()
	{
		return brkItor->next();
	}
	int IcuBreaker::first()
	{
		return brkItor->first();
	}
	IcuBreaker::~IcuBreaker(){
		delete brkItor;
	};

#endif

#ifdef JNI
	JNIBreaker::JNIBreaker(JNIEnv *env, BreakType type) :
		_env(env), clsBreakIterator(0), method_setText(0), jnibreaker(0)
	{
		_env = env;
		clsBreakIterator = _env->FindClass("java/text/BreakIterator");
#ifdef ANDROID
		if (clsBreakIterator == 0)
		{

			__android_log_print(2, "mtc", "could not find clsBreakIterator");
		}
		else
		{
			__android_log_print(2, "mtc", "find clsBreakIterator ok");
		}
#endif
		method_setText = _env->GetMethodID(clsBreakIterator, "setText", "(Ljava/lang/String;)V");
#ifdef ANDROID
		if (method_setText == 0)
		{

			__android_log_print(2, "mtc", "method_setText failed");

		}
		else
		{
			__android_log_print(2, "mtc", "method_setText success");
		}
#endif
		method_next = _env->GetMethodID(clsBreakIterator, "next", "()I");
#ifdef ANDROID
		if (method_next == 0)
		{

			__android_log_print(2, "mtc", "method_next failed");

		}
		else
		{
			__android_log_print(2, "mtc", "method_next success");
		}
#endif
		method_createLineInstance = _env->GetStaticMethodID(clsBreakIterator, "getLineInstance", "()Ljava/text/BreakIterator;");
#ifdef ANDROID
		if (method_createLineInstance == 0)
		{

			__android_log_print(2, "mtc", "method_createLineInstance failed");

		}
		else
		{
			__android_log_print(2, "mtc", "method_createLineInstance success");
		}
#endif
		method_first = _env->GetMethodID(clsBreakIterator, "first", "()I");
#ifdef ANDROID
		if (method_first == 0)
		{

			__android_log_print(2, "mtc", "method_first failed");

		}
		else
		{
			__android_log_print(2, "mtc", "method_first success");
		}
#endif
		jnibreaker = _env->CallStaticObjectMethod(clsBreakIterator, method_createLineInstance);
#ifdef ANDROID
		if (jnibreaker == 0)
		{

			__android_log_print(2, "mtc", "jnibreaker failed");

		}
		else
		{
			__android_log_print(2, "mtc", "jnibreaker success");
		}
#endif
		int failed = 1;
	}
	void JNIBreaker::setText(const uint16_t* text, int length)
	{
		jtext = _env->NewString(text, length);
		_env->CallVoidMethod(jnibreaker, method_setText, jtext);
		_env->DeleteLocalRef(jtext);
	}
	int JNIBreaker::next()
	{
		return _env->CallIntMethod(jnibreaker, method_next);
	}
	int JNIBreaker::first()
	{
		return _env->CallIntMethod(jnibreaker, method_first);
	}
	JNIBreaker::~JNIBreaker(){
		_env->DeleteLocalRef(jnibreaker);
		_env->DeleteLocalRef(clsBreakIterator);

	};

#endif /*JNI*/
}
