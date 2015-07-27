#include "Breaker.h"
#include <windows.h>
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
	JNIBreaker::JNIBreaker(JNIEnv *env, BreakType type)
	{
		_env = env;
		clsBreakIterator = _env->FindClass("Ljava/text/BreakIterator;");
		
		method_setText = _env->GetMethodID(clsBreakIterator, "setText", "(Ljava/lang/String;)V");
		method_next = _env->GetMethodID(clsBreakIterator, "next", "()I");
		method_createLineInstance = _env->GetStaticMethodID(clsBreakIterator, "getLineInstance", "()Ljava/text/BreakIterator;");
		method_first = _env->GetMethodID(clsBreakIterator, "first", "()I");

		jnibreaker = _env->CallStaticObjectMethod(clsBreakIterator, method_createLineInstance);

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
		_env->DeleteLocalRef(jtext);
		_env->DeleteLocalRef(jnibreaker);
		_env->DeleteLocalRef(clsBreakIterator);

	};

#endif /*JNI*/
}