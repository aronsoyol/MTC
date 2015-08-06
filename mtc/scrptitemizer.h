#ifndef __SCRIPT_ITEMIZER_H
#define __SCRIPT_ITEMIZER_H

#include <stdint.h>
#include <hb.h>
#include <hb-ucdn/ucdn.h>

namespace MTC{ namespace ITEMIZER{

	typedef int32_t UCDN_SCRIPT;
	typedef uint32_t UChar32;
	typedef uint16_t UChar;

	struct ScriptRecord
	{
		uint32_t	startChar;
		uint32_t	endChar;
		UCDN_SCRIPT scriptCode;
	};

	struct ParenStackEntry
	{
		int32_t pairIndex;
		UCDN_SCRIPT scriptCode;
	};

	class ScriptItemIterator {

	public:
		ScriptItemIterator();

		ScriptItemIterator(const UChar chars[], int32_t length);

		ScriptItemIterator(const UChar chars[], int32_t start, int32_t length);

		void reset();

		void reset(int32_t start, int32_t count);

		void reset(const UChar chars[], int32_t start, int32_t length);

		int32_t getScriptStart();

		int32_t getScriptEnd();

		UCDN_SCRIPT getScriptCode();
		hb_script_t get_hb_script();
		bool next();

	private:
		static UCDN_SCRIPT uscript_getScript(UChar32 ch);
		static bool sameScript(UCDN_SCRIPT scriptOne, UCDN_SCRIPT scriptTwo);
		static bool scriptInherited(UCDN_SCRIPT scriptOne);

		int32_t charStart;
		int32_t charLimit;
		const UChar *charArray;

		int32_t scriptStart;
		int32_t scriptEnd;
		UCDN_SCRIPT scriptCode;

		ParenStackEntry parenStack[128];
		int32_t parenSP;

		static int8_t highBit(int32_t value);
		static int32_t getPairIndex(UChar32 ch);

		static UChar32 pairedChars[];
		static const int32_t pairedCharCount;
		static const int32_t pairedCharPower;
		static const int32_t pairedCharExtra;

	};

	inline ScriptItemIterator::ScriptItemIterator()
	{
		reset(0, 0, 0);
	}

	inline ScriptItemIterator::ScriptItemIterator(const UChar chars[], int32_t length)
	{
		reset(chars, 0, length);
	}

	inline ScriptItemIterator::ScriptItemIterator(const UChar chars[], int32_t start, int32_t length)
	{
		reset(chars, start, length);
	}

	inline int32_t ScriptItemIterator::getScriptStart()
	{
		return scriptStart;
	}

	inline int32_t ScriptItemIterator::getScriptEnd()
	{
		return scriptEnd;
	}

	inline UCDN_SCRIPT ScriptItemIterator::getScriptCode()
	{
		return scriptCode;
	}
	inline void ScriptItemIterator::reset()
	{
		scriptStart = charStart;
		scriptEnd = charStart;
		scriptCode = UCDN_SCRIPT_COMMON;// USCRIPT_INVALID_CODE;
		parenSP = -1;
	}

	inline void ScriptItemIterator::reset(int32_t start, int32_t length)
	{
		charStart = start;
		charLimit = start + length;

		reset();
	}

	inline void ScriptItemIterator::reset(const UChar chars[], int32_t start, int32_t length)
	{
		charArray = chars;

		reset(start, length);
	}

	inline bool ScriptItemIterator::scriptInherited(UCDN_SCRIPT script)
	{
		return script == UCDN_SCRIPT_INHERITED || script == UCDN_SCRIPT_COMMON || script == -1;
	}
}}

#endif /*__SCRIPT_ITEMIZER_H*/
