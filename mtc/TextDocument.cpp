//
//	MODULE:		TextDocument.cpp
//
//	PURPOSE:	Basic implementation of a text data-sequence class
//
//
#include <cassert>
#include <algorithm>
#include <iostream>
#include <string.h>
#include "Unicode.h"
#include "ParaBuffer.h"
#include "TextDocument.h"

namespace MTC{ namespace DOC{
using namespace PARA;
BOM BOMLOOKUP[] = 
{
	// define longest headers first
	{ 0x0000FEFF, 4, NCP_UTF32    },
	{ 0xFFFE0000, 4, NCP_UTF32BE  },
	{ 0xBFBBEF,	  3, NCP_UTF8	  },
	{ 0xFFFE,	  2, NCP_UTF16BE  },
	{ 0xFEFF,	  2, NCP_UTF16    },
	{ 0,          0, NCP_ASCII	  },
};
char16_t LB[] = { 0x000A, 0x000B, 0x000C, 0x000D, 0x0085, 0x2028, 0x2029, 0x000D, 0x000A };

bool IsLineBreaker(char16_t ch)
{
	for (int i = 0; i < sizeof(LB) / sizeof(char16_t); i++)
	{
		if (LB[i] == ch)
			return true;
	}
	return false;
}

bool IncludeLineBreaker(const char16_t* buffer, int length)
{
	for (int i = 0; i < length; i++)
	{
		if (IsLineBreaker(buffer[i]))
			return true;
	}
	return false;
}

int CRLF_size(char16_t *szText, int nLength)
{
	if (nLength >= 2)
	{
		if (szText[nLength - 2] == '\r' && szText[nLength - 1] == '\n')
			return 2;
	}

	if (nLength >= 1)
	{
		if (szText[nLength - 1] == '\r' || szText[nLength - 1] == '\n' ||
			szText[nLength - 1] == '\x0b' || szText[nLength - 1] == '\x0c' ||
			szText[nLength - 1] == '\x85' || szText[nLength - 1] == 0x2028 ||
			szText[nLength - 1] == 0x2029)
			return 1;
	}

	return 0;
}
bool FillHeader(int format, DWORD* pHeader, int * pHeader_length)
{
	if (pHeader == 0 || pHeader_length == 0) return false;
	int count = sizeof(BOMLOOKUP) / sizeof(BOM);

	for (int i = 0; i < count; i++)
	{
		if (BOMLOOKUP[i].type == format)
		{
			*pHeader = BOMLOOKUP[i].bom;
			*pHeader_length = BOMLOOKUP[i].len;
			return true;
		}
	}
	return false;
}
//
//	TextDocument constructor
//
TextDocument::TextDocument(): 
	mFileFormat(NCP_UTF8),
	mBufferWatcher(0)
{
	Init();
}

//
//	TextDocument destructor
//
TextDocument::~TextDocument()
{
	Clear();
}



//
//	Empty the data-TextDocument
//
bool TextDocument::Clear()
{
	mSeq.clear();
	ResetLineBuffer();
	assert(mParaBuffer.ParaCount() == 1);
	return true;
}

//
//	Return the number of lines
//
bool TextDocument::GetText(uint32_t offset, uint32_t length, char16_t *buffer, uint32_t *buflen)
{
	assert(buflen && *buflen > length);
	assert(offset + length <= mSeq.size());

	SEQ::size_w copied_len = mSeq.render(offset, buffer, length);

	if (copied_len > 0 && buflen)
	{
		*buflen = copied_len;
		return true;
	}
	return false;
}

//
//	Return the number of lines
//

bool TextDocument::GetLineInfo(__inout LINE_INFO* pLineInfo)const
{
	return GetLineInfo(pLineInfo->nParaNo, pLineInfo);
}
bool TextDocument::GetLineInfo(uint32_t nLineNo,  LINE_INFO* pLineInfo) const
{
	if (nLineNo >= mParaBuffer.ParaCount() || pLineInfo == 0)
		return false;

	pLineInfo->nParaNo = nLineNo;
	pLineInfo->nOffset = mParaBuffer[nLineNo];
	pLineInfo->nLength = mParaBuffer.LineLength(nLineNo);
	return true;
}
uint32_t TextDocument::GetLineNo(uint32_t offset_chars)const
{
	int tmp = mParaBuffer[mParaBuffer.ParaCount()];
	assert(TextLength() >= offset_chars);
	assert(TextLength() == tmp);

	if (offset_chars == TextLength())
	{
		return mParaBuffer.ParaCount() - 1;
	}
	long low  = 0;
	long high = mParaBuffer.ParaCount() - 1;
	uint32_t para = 0;

	int modify_offset = 0;
	int modify_length = 0;
	while (low <= high)
	{
		assert(high < mParaBuffer.ParaCount());
		para = (high + low) / 2;
		//当文本的最后为换行时，会在文本的最后形成一个空行
		if (offset_chars >= mParaBuffer[para] && offset_chars < mParaBuffer[para + 1])
		{
			return para;
		}
		else if (offset_chars < mParaBuffer[para])
		{
			high = para - 1;
		}
		else
		{
			low = para + 1;
		}
	}
	return 0;
}

bool TextDocument::GetLineInfoFromOffset(uint32_t offset,  LINE_INFO* pLineInfo ) const
{
	assert(offset <= TextLength());
	assert(pLineInfo != NULL);

	pLineInfo->nParaNo = GetLineNo(offset);
	return GetLineInfo(pLineInfo);
}

//
//	Retrieve an entire line of text
//	
uint32_t TextDocument::GetPara(uint32_t nParaNo, char16_t *buffer, uint32_t buflen, uint32_t *off_chars, int* breaker_len)
{
	assert(nParaNo == 0 || nParaNo < mParaBuffer.ParaCount());
	
	uint32_t offset;
	uint32_t length;

	offset	= mParaBuffer[nParaNo];
	length	= mParaBuffer.ParaCount() > 0 ? mParaBuffer.LineLength(nParaNo) : 0;
	assert(offset + length <= mSeq.size());
	length	= std::min(length, buflen);
	length	= mSeq.render(offset, buffer, length);
	
	if (off_chars) 
		*off_chars = offset;

	if (breaker_len)
		*breaker_len = CRLF_size(buffer, buflen);
	return length;
}
//
//	Insert text at specified character-offset
//
bool TextDocument::InsertText(const uint32_t offset_chars, const char16_t *text, uint32_t length)
{
	assert(0 < mParaBuffer.ParaCount());
	assert(TextLength() >= offset_chars);
	if(text == 0 || length == 0)
		return false;
	int charCount1 = mParaBuffer[mParaBuffer.ParaCount()];

	//如果第n行的内容为
	//[位置]0 1 2 3 4 5
	//[内容] a b c d e
	//则，在位置[0，5]中插入内容时无需更新ParaBuffer
	int tmpLineNo  = 0;
	if (TextLength() > offset_chars)
		tmpLineNo = mParaBuffer.GetLineFromOffset(offset_chars);
	else
		tmpLineNo = ParaCount() - 1;

	const uint32_t nLineNo	= tmpLineNo;

	length = std::min(length, SEQ::MAX_SEQUENCE_LENGTH - mSeq.size());

	mSeq.insert(offset_chars, text, length);

	int lb_len = length;

	//第一种情况：插入的就是换行符
	if (ParaBuffer::IsLineBreaker(text, &lb_len) && lb_len == length)
	{
		//插入
		//#1.处理新行的位置和长度
		//
		int tmpLen1 = offset_chars - mParaBuffer[nLineNo];
		int tmpLen2 = mParaBuffer[nLineNo + 1] - offset_chars;
		if (nLineNo == ParaCount() - 1)
			mParaBuffer.AppendLine(tmpLen2);
		else
			mParaBuffer.InsertLine(nLineNo + 1, tmpLen2);
		//#2.处理当前行
		//当前行的新长度应该等于 插入位置 - 当前行的开始位置 + 换行符的长度
		mParaBuffer.ModifyLine(nLineNo, lb_len - tmpLen2);
		//int nNextLineStart	= mParaBuffer[nLineNo + 1] - offset_chars + lb_len;
		//int nNewLineLength = offset_chars - nLineStart + lb_len;
		if (mBufferWatcher != 0)
		{

		}
	}
	////第二种情况：插入的是包含换行符的文本
	else 
	if(IncludeLineBreaker(text, length))
		//mParaBuffer.InitFromBuffer
	{
		//
		//复杂的处理
		//
	}
	//第三种情况：插入的是不包含换行符的文本
	else
	{
		mParaBuffer.ModifyLine(nLineNo, length);
		assert(TextLength() == mParaBuffer[mParaBuffer.ParaCount()]);
	}
	int charCount2 = mParaBuffer[mParaBuffer.ParaCount()];
	if (charCount2 != charCount1 + length)
	{
		int stop = 1;
	}
	assert(charCount2 == charCount1 + length);

	return length > 0 ? true : false;
}

//
//	Erase text at specified character-offset
//
uint32_t TextDocument::EraseText(uint32_t offset_chars, uint32_t length)
{
	assert(offset_chars > 0 ? offset_chars < TextLength(): true);
	assert(offset_chars + length <= mSeq.size());
	//判断删除的内容中是否包含换行符
	const int nLineStart	= mParaBuffer.GetLineFromOffset(offset_chars);
	const int nLineEnd		= TextLength() == offset_chars + length ? ParaCount() - 1 : mParaBuffer.GetLineFromOffset(offset_chars + length );
	bool bNeadResetLineBuffer	= false;
	if(nLineStart != nLineEnd || nLineStart == nLineEnd && mParaBuffer.LineLength(nLineStart) < length)
	{
		bNeadResetLineBuffer = true;
	}

	length = std::min(length, mSeq.size() - offset_chars);

	mSeq.erase(offset_chars, length);
	if(bNeadResetLineBuffer)
	{
		ResetLineBuffer();
		assert(TextLength() == mParaBuffer[mParaBuffer.ParaCount()]);
		//m_pTextView->OnLineBufferChange(LBA_CHANGE_ALL, 0);
	}
	else
	{
		mParaBuffer.ModifyLine(nLineStart, -length);
		assert(TextLength() == mParaBuffer[mParaBuffer.ParaCount()]);
		//m_pTextView->OnLineBufferChange(LBA_CHANGE_LINE, nLineStart);
	}
	
	return length;
}

bool TextDocument::Undo(uint32_t *offset_start, uint32_t *offset_end)
{
	uint32_t start, length;

	if(!mSeq.undo())
		return false;

	start				= mSeq.event_index();
	length				= mSeq.event_length();

	*offset_start		= start;
	*offset_end			= start + length;

	return true;
}

bool TextDocument::Redo(uint32_t *offset_start, uint32_t *offset_end)
{
	uint32_t start, length;

	if(!mSeq.redo())
		return false;

	start				= mSeq.event_index() ;
	length				= mSeq.event_length();

	*offset_start		= start;
	*offset_end			= start + length;
	
	return true;
}

}}
