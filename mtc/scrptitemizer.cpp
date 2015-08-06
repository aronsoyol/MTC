#include "scrptitemizer.h"

namespace MTC{namespace ITEMIZER{
	static const hb_script_t hb_script_from_ucdn_script[] =
	{
		HB_SCRIPT_COMMON,
		HB_SCRIPT_LATIN,
		HB_SCRIPT_GREEK,
		HB_SCRIPT_CYRILLIC,
		HB_SCRIPT_ARMENIAN,
		HB_SCRIPT_HEBREW,
		HB_SCRIPT_ARABIC,
		HB_SCRIPT_SYRIAC,
		HB_SCRIPT_THAANA,
		HB_SCRIPT_DEVANAGARI,
		HB_SCRIPT_BENGALI,
		HB_SCRIPT_GURMUKHI,
		HB_SCRIPT_GUJARATI,
		HB_SCRIPT_ORIYA,
		HB_SCRIPT_TAMIL,
		HB_SCRIPT_TELUGU,
		HB_SCRIPT_KANNADA,
		HB_SCRIPT_MALAYALAM,
		HB_SCRIPT_SINHALA,
		HB_SCRIPT_THAI,
		HB_SCRIPT_LAO,
		HB_SCRIPT_TIBETAN,
		HB_SCRIPT_MYANMAR,
		HB_SCRIPT_GEORGIAN,
		HB_SCRIPT_HANGUL,
		HB_SCRIPT_ETHIOPIC,
		HB_SCRIPT_CHEROKEE,
		HB_SCRIPT_CANADIAN_SYLLABICS,
		HB_SCRIPT_OGHAM,
		HB_SCRIPT_RUNIC,
		HB_SCRIPT_KHMER,
		HB_SCRIPT_MONGOLIAN,
		HB_SCRIPT_HIRAGANA,
		HB_SCRIPT_KATAKANA,
		HB_SCRIPT_BOPOMOFO,
		HB_SCRIPT_HAN,
		HB_SCRIPT_YI,
		HB_SCRIPT_OLD_ITALIC,
		HB_SCRIPT_GOTHIC,
		HB_SCRIPT_DESERET,
		HB_SCRIPT_INHERITED,
		HB_SCRIPT_TAGALOG,
		HB_SCRIPT_HANUNOO,
		HB_SCRIPT_BUHID,
		HB_SCRIPT_TAGBANWA,
		HB_SCRIPT_LIMBU,
		HB_SCRIPT_TAI_LE,
		HB_SCRIPT_LINEAR_B,
		HB_SCRIPT_UGARITIC,
		HB_SCRIPT_SHAVIAN,
		HB_SCRIPT_OSMANYA,
		HB_SCRIPT_CYPRIOT,
		HB_SCRIPT_BRAILLE,
		HB_SCRIPT_BUGINESE,
		HB_SCRIPT_COPTIC,
		HB_SCRIPT_NEW_TAI_LUE,
		HB_SCRIPT_GLAGOLITIC,
		HB_SCRIPT_TIFINAGH,
		HB_SCRIPT_SYLOTI_NAGRI,
		HB_SCRIPT_OLD_PERSIAN,
		HB_SCRIPT_KHAROSHTHI,
		HB_SCRIPT_BALINESE,
		HB_SCRIPT_CUNEIFORM,
		HB_SCRIPT_PHOENICIAN,
		HB_SCRIPT_PHAGS_PA,
		HB_SCRIPT_NKO,
		HB_SCRIPT_SUNDANESE,
		HB_SCRIPT_LEPCHA,
		HB_SCRIPT_OL_CHIKI,
		HB_SCRIPT_VAI,
		HB_SCRIPT_SAURASHTRA,
		HB_SCRIPT_KAYAH_LI,
		HB_SCRIPT_REJANG,
		HB_SCRIPT_LYCIAN,
		HB_SCRIPT_CARIAN,
		HB_SCRIPT_LYDIAN,
		HB_SCRIPT_CHAM,
		HB_SCRIPT_TAI_THAM,
		HB_SCRIPT_TAI_VIET,
		HB_SCRIPT_AVESTAN,
		HB_SCRIPT_EGYPTIAN_HIEROGLYPHS,
		HB_SCRIPT_SAMARITAN,
		HB_SCRIPT_LISU,
		HB_SCRIPT_BAMUM,
		HB_SCRIPT_JAVANESE,
		HB_SCRIPT_MEETEI_MAYEK,
		HB_SCRIPT_IMPERIAL_ARAMAIC,
		HB_SCRIPT_OLD_SOUTH_ARABIAN,
		HB_SCRIPT_INSCRIPTIONAL_PARTHIAN,
		HB_SCRIPT_INSCRIPTIONAL_PAHLAVI,
		HB_SCRIPT_OLD_TURKIC,
		HB_SCRIPT_KAITHI,
		HB_SCRIPT_BATAK,
		HB_SCRIPT_BRAHMI,
		HB_SCRIPT_MANDAIC,
		HB_SCRIPT_CHAKMA,
		HB_SCRIPT_MEROITIC_CURSIVE,
		HB_SCRIPT_MEROITIC_HIEROGLYPHS,
		HB_SCRIPT_MIAO,
		HB_SCRIPT_SHARADA,
		HB_SCRIPT_SORA_SOMPENG,
		HB_SCRIPT_TAKRI,
		HB_SCRIPT_UNKNOWN,
		HB_SCRIPT_BASSA_VAH,
		HB_SCRIPT_CAUCASIAN_ALBANIAN,
		HB_SCRIPT_DUPLOYAN,
		HB_SCRIPT_ELBASAN,
		HB_SCRIPT_GRANTHA,
		HB_SCRIPT_KHOJKI,
		HB_SCRIPT_KHUDAWADI,
		HB_SCRIPT_LINEAR_A,
		HB_SCRIPT_MAHAJANI,
		HB_SCRIPT_MANICHAEAN,
		HB_SCRIPT_MENDE_KIKAKUI,
		HB_SCRIPT_MODI,
		HB_SCRIPT_MRO,
		HB_SCRIPT_NABATAEAN,
		HB_SCRIPT_OLD_NORTH_ARABIAN,
		HB_SCRIPT_OLD_PERMIC,
		HB_SCRIPT_PAHAWH_HMONG,
		HB_SCRIPT_PALMYRENE,
		HB_SCRIPT_PAU_CIN_HAU,
		HB_SCRIPT_PSALTER_PAHLAVI,
		HB_SCRIPT_SIDDHAM,
		HB_SCRIPT_TIRHUTA,
		HB_SCRIPT_WARANG_CITI,
	};
	hb_script_t ScriptItemIterator::get_hb_script()
	{
		if (scriptCode >= 0 && scriptCode < sizeof(hb_script_from_ucdn_script) / sizeof(hb_script_t))
		{
			return hb_script_from_ucdn_script[scriptCode];
		}
		else
			return HB_SCRIPT_INVALID;
	}
#define ARRAY_SIZE(array) (sizeof array  / sizeof array[0])

	UChar32 ScriptItemIterator::pairedChars[] = {
		0x0028, 0x0029, // ascii paired punctuation
		0x003c, 0x003e,
		0x005b, 0x005d,
		0x007b, 0x007d,
		0x00ab, 0x00bb, // guillemets
		0x2018, 0x2019, // general punctuation
		0x201c, 0x201d,
		0x2039, 0x203a,
		0x3008, 0x3009, // chinese paired punctuation
		0x300a, 0x300b,
		0x300c, 0x300d,
		0x300e, 0x300f,
		0x3010, 0x3011,
		0x3014, 0x3015,
		0x3016, 0x3017,
		0x3018, 0x3019,
		0x301a, 0x301b
	};

	const int32_t ScriptItemIterator::pairedCharCount = ARRAY_SIZE(pairedChars);
	const int32_t ScriptItemIterator::pairedCharPower = 1 << highBit(pairedCharCount);
	const int32_t ScriptItemIterator::pairedCharExtra = pairedCharCount - pairedCharPower;

	int8_t ScriptItemIterator::highBit(int32_t value)
	{
		if (value <= 0) {
			return -32;
		}

		int8_t bit = 0;

		if (value >= 1 << 16) {
			value >>= 16;
			bit += 16;
		}

		if (value >= 1 << 8) {
			value >>= 8;
			bit += 8;
		}

		if (value >= 1 << 4) {
			value >>= 4;
			bit += 4;
		}

		if (value >= 1 << 2) {
			value >>= 2;
			bit += 2;
		}

		if (value >= 1 << 1) {
			value >>= 1;
			bit += 1;
		}

		return bit;
	}

	int32_t ScriptItemIterator::getPairIndex(UChar32 ch)
	{
		int32_t probe = pairedCharPower;
		int32_t index = 0;

		if (ch >= pairedChars[pairedCharExtra]) {
			index = pairedCharExtra;
		}

		while (probe > (1 << 0)) {
			probe >>= 1;

			if (ch >= pairedChars[index + probe]) {
				index += probe;
			}
		}

		if (pairedChars[index] != ch) {
			index = -1;
		}

		return index;
	}

	bool ScriptItemIterator::sameScript(UCDN_SCRIPT scriptOne, UCDN_SCRIPT scriptTwo)
	{
		return scriptInherited(scriptOne) || scriptInherited(scriptTwo) || scriptOne == scriptTwo;
	}

	bool ScriptItemIterator::next()
	{
		int32_t startSP = parenSP;  // used to find the first new open character
		//UErrorCode error = U_ZERO_ERROR;

		// if we've fallen off the end of the text, we're done
		if (scriptEnd >= charLimit) {
			return false;
		}

		scriptCode = -1;

		for (scriptStart = scriptEnd; scriptEnd < charLimit; scriptEnd += 1) {
			UChar   high = charArray[scriptEnd];
			UChar32 ch = high;

			// if the character is a high surrogate and it's not the last one
			// in the text, see if it's followed by a low surrogate
			if (high >= 0xD800 && high <= 0xDBFF && scriptEnd < charLimit - 1)
			{
				UChar low = charArray[scriptEnd + 1];

				// if it is followed by a low surrogate,
				// consume it and form the full character
				if (low >= 0xDC00 && low <= 0xDFFF) {
					ch = (high - 0xD800) * 0x0400 + low - 0xDC00 + 0x10000;
					scriptEnd += 1;
				}
			}

			UCDN_SCRIPT sc = ucdn_get_script(ch);
			int32_t pairIndex = getPairIndex(ch);

			// Paired character handling:
			//
			// if it's an open character, push it onto the stack.
			// if it's a close character, find the matching open on the
			// stack, and use that script code. Any non-matching open
			// characters above it on the stack will be poped.
			if (pairIndex >= 0) {
				if ((pairIndex & 1) == 0) {
					parenStack[++parenSP].pairIndex = pairIndex;
					parenStack[parenSP].scriptCode = scriptCode;
				}
				else if (parenSP >= 0) {
					int32_t pi = pairIndex & ~1;

					while (parenSP >= 0 && parenStack[parenSP].pairIndex != pi) {
						parenSP -= 1;
					}

					if (parenSP < startSP) {
						startSP = parenSP;
					}

					if (parenSP >= 0) {
						sc = parenStack[parenSP].scriptCode;
					}
				}
			}

			if (sameScript(scriptCode, sc)) {
				//if (scriptCode <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
				if (scriptInherited(scriptCode) && !scriptInherited(sc)) {
					scriptCode = sc;

					// now that we have a final script code, fix any open
					// characters we pushed before we knew the script code.
					while (startSP < parenSP) {
						parenStack[++startSP].scriptCode = scriptCode;
					}
				}

				// if this character is a close paired character,
				// pop it from the stack
				if (pairIndex >= 0 && (pairIndex & 1) != 0 && parenSP >= 0) {
					parenSP -= 1;
					startSP -= 1;
				}
			}
			else {
				// if the run broke on a surrogate pair,
				// end it before the high surrogate
				if (ch >= 0x10000) {
					scriptEnd -= 1;
				}

				break;
			}
		}

		return true;
	}
}}