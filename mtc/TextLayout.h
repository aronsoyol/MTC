#ifndef __MTC_TEXT_LAYOUT_H 
#define __MTC_TEXT_LAYOUT_H

#include "ParaLayout.h"
#include <hb-ft.h>

#include "util.h"

#include <vector>
#include <cassert>
#if defined ICU

#include <hb-icu.h>
#include <unicode/uscript.h>
#include <unicode/brkiter.h>

#endif
#ifdef JNI
#include <jni.h>
#endif

 
namespace MTC{namespace LayoutEngine{

	class TextLayout
	{
	private:
	public:
		virtual void	set_text(const std::u16string& text);
	};
}}
#endif /*__MTC_TEXT_LAYOUT_H*/