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

//
//	TextDocument constructor
//
TextDocument::TextDocument(): 
	mDocLength_bytes(0),
	mFileFormat(NCP_UTF8),
	mHeaderSize(0),
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
//	Parse the file lo
//
//
//	From the unicode.org FAQ:
//
//	00 00 FE FF			UTF-32, big-endian 
//	FF FE 00 00			UTF-32, little-endian 
//	FE FF				UTF-16, big-endian 
//	FF FE				UTF-16, little-endian 
//	EF BB BF			UTF-8 
//
//	Match the first x bytes of the file against the
//  Byte-Order-Mark (BOM) lookup table
//
int TextDocument::detect_file_format(const unsigned char* buffer,int buffer_length,  int *header_size)
{
	unsigned char header[4]	= { 0 };
	int header_len	= std::min(4, buffer_length);
	for(int i = 0; i < header_len; i++)
	{
		header[i] = buffer[i];
	}
	for(int i = 0; BOMLOOKUP[i].len; i++)
	{
		if(header_len >= BOMLOOKUP[i].len &&
		   memcmp(header, &BOMLOOKUP[i].bom, BOMLOOKUP[i].len) == 0)
		{
			*header_size = BOMLOOKUP[i].len;
			return BOMLOOKUP[i].type;
		}
	}

	*header_size = 0;
	return NCP_UTF8;	// default to ASCII
}
int TextDocument::detect_file_format(int *mHeaderSize)
{
	unsigned char header[4] = { 0 };
	mSeq.render(0, (SEQ::seqchar*)header, sizeof(header) / sizeof(SEQ::seqchar));

	for(int i = 0; BOMLOOKUP[i].len; i++)
	{
		if(mDocLength_bytes >= BOMLOOKUP[i].len &&
		   memcmp(header, &BOMLOOKUP[i].bom, BOMLOOKUP[i].len) == 0)
		{
			*mHeaderSize = BOMLOOKUP[i].len;
			return BOMLOOKUP[i].type;
		}
	}

	*mHeaderSize = 0;
	return NCP_ASCII;	// default to ASCII
}


//
//	Empty the data-TextDocument
//
bool TextDocument::Clear()
{
	mSeq.clear();
	mDocLength_bytes	= 0;
	mHeaderSize		= 0;
	ResetLineBuffer();
	assert(mParaBuffer.ParaCount() == 1);
	return true;
}




//
//	Return a UTF-32 character value
//
int TextDocument::getUtf32(uint32_t offset, uint32_t lenbytes, uint32_t *pch32)
{
//	unsigned char	*rawdata   = (unsigned char *)(buffer + offset + mHeaderSize);
	unsigned char	rawdata[16];

	lenbytes = std::min(16u, lenbytes);
	mSeq.render(offset + mHeaderSize, rawdata, lenbytes);


	UTF16   *rawdata_w = (UTF16 *)rawdata;//(char16_t*)(buffer + offset + mHeaderSize);
	char16_t     ch16;
	size_t   ch32len = 1;
	return utf16_to_utf32(rawdata_w, lenbytes / 2, pch32, &ch32len) * sizeof(char16_t);
}

//
//	Fetch a buffer of UTF-16 text from the specified byte offset - 
//  returns the number of characters stored in buf
//
//	Depending on how Neatpad was compiled (UNICODE vs ANSI) this function
//  will always return text in the "native" format - i.e. Unicode or Ansi -
//  so the necessary conversions will take place here.
//
//  TODO: make sure the CR/LF is always fetched in one go
//        make sure utf-16 surrogates kept together
//		  make sure that combining chars kept together
//		  make sure that bidirectional text kep together (will be *hard*) 
//
//	offset   - unsigned char offset within underlying data sequence
//	lenbytes - max number of bytes to process (i.e. to limit to a line)
//  buf		 - UTF16/ASCII output buffer
//	plen	 - [in] - length of buffer, [out] - number of code-units stored
//
//	returns  - number of bytes processed
//
uint32_t TextDocument::gettext(uint32_t offset, uint32_t lenbytes, char16_t *buf, uint32_t *buflen)
{
//	unsigned char	*rawdata = (unsigned char *)(buffer + offset + mHeaderSize);

	uint32_t chars_copied = 0;
	uint32_t bytes_processed = 0;
	//assert(lenbytes <= mDocLength_bytes);
	lenbytes = std::min(lenbytes, *buflen);
	lenbytes = std::min(lenbytes, mDocLength_bytes - offset);
	//assert(*buflen + 1 > lenbytes);
	if(offset >= mDocLength_bytes)
	{
		*buflen = 0;
		return 0;
	}

	while(lenbytes > 0 && *buflen > 0)
	{
		unsigned char   rawdata[0x100];
		size_t rawlen = std::min<uint32_t>(0x100UL, lenbytes);// min(min(lenbytes, 0x100), mDocLength_bytes - offset);

		// get next block of data from the piece-table
		size_t copied_len = mSeq.render(offset + mHeaderSize, rawdata, rawlen);

		// convert to UTF-16 
		size_t tmplen = *buflen;

		
		rawlen = rawdata_to_utf16(rawdata, copied_len, buf, &tmplen);

		lenbytes		-= rawlen;
		offset			+= rawlen;
		bytes_processed += rawlen;

		buf				+= tmplen;
		*buflen			-= tmplen;
		chars_copied	+= tmplen;
	}

	*buflen = chars_copied;
	return bytes_processed;
}
//buflen	-	in bytes
bool TextDocument::GetText(uint32_t char_offset, uint32_t char_length,  char16_t *buf, uint32_t *buflen)
{
	long start_bytes = charoffset_to_byteoffset(char_offset);
	long end_bytes = charoffset_to_byteoffset(char_offset + char_length);
	long length_bytes	= end_bytes - start_bytes;
	if (length_bytes > 0 && gettext(start_bytes, length_bytes, buf, buflen) > 0)
	{
		return true;
	}else
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

TextIterator TextDocument::iterate(uint32_t offset_chars)
{
	uint32_t off_bytes = charoffset_to_byteoffset(offset_chars);
	uint32_t len_bytes = mDocLength_bytes - off_bytes;

	return TextIterator(off_bytes, len_bytes, this);
}
//
//	Retrieve an entire line of text
//	
uint32_t TextDocument::GetPara(uint32_t nLineNo, char16_t *buf, uint32_t buflen, uint32_t *off_chars, bool fWithLineBreaker)
{
	assert(nLineNo == 0 || nLineNo < mParaBuffer.ParaCount());
	uint32_t offset_bytes;
	uint32_t length_bytes;
	uint32_t offset_chars;
	uint32_t length_chars;

	//if(!lineinfo_from_lineno(nLineNo, &offset_chars, &length_chars, &offset_bytes, &length_bytes))
	//{
	//	*off_chars = 0;	
	//	return 0;
	//}
	offset_chars	= mParaBuffer[nLineNo];
	length_chars	= mParaBuffer.ParaCount() > 0 ? mParaBuffer.LineLength(nLineNo) : 0;
	offset_bytes	= charoffset_to_byteoffset(offset_chars);
	length_bytes	= charoffset_to_byteoffset(offset_chars + length_chars) - offset_bytes; 

	gettext(offset_bytes, length_bytes, buf, &buflen);
	
	if(off_chars) *off_chars = offset_chars;

	if(!fWithLineBreaker)
		buflen -= CRLF_size(buf, buflen);
	return buflen;
}
int TextDocument::CRLF_size(char16_t *szText, int nLength)const
{
	if(nLength >= 2)
	{
		if(szText[nLength-2] == '\r' && szText[nLength-1] == '\n') 
			return 2;
	}

	if(nLength >= 1)
	{
		if( szText[nLength-1] == '\r'	|| szText[nLength-1] == '\n' || 
			szText[nLength-1] == '\x0b' || szText[nLength-1] == '\x0c' ||
			szText[nLength-1] == '\x85' || szText[nLength-1] == 0x2028 || 
			szText[nLength-1] == 0x2029)
			return 1;
	}	

	return 0;
}

int TextDocument::file_to_utf16(
								   __in  int		file_format, 
								   __in  unsigned char   *	buffer, 
								   __in  size_t		file_length, 
								    char16_t  *	utf16str, 
								    size_t *	utf16len
								)
{
	switch(file_format)
	{
	// convert from ANSI->UNICODE
	case NCP_ASCII:
		return ascii_to_utf16(buffer, file_length, (UTF16 *)utf16str, utf16len);
		
	case NCP_UTF8:
		return utf8_to_utf16(buffer, file_length, (UTF16 *)utf16str, utf16len);

	// already unicode, do a straight memory copy
	case NCP_UTF16:
		return 0;
	//	rawlen /= sizeof(char16_t);
	//	return copy_utf16((UTF16 *)rawdata, rawlen, (UTF16 *)utf16str, utf16len) * sizeof(char16_t);

	// need to convert from big-endian to little-endian
	case NCP_UTF16BE:
		file_length /= sizeof(char16_t);
		return swap_utf16((UTF16 *)buffer, file_length, (UTF16 *)utf16str, utf16len) * sizeof(char16_t);

	// error! we should *never* reach this point
	default:
		*utf16len = 0;
		return 0;	
	}
}
/*
Convert the RAW buffer in underlying file-format to UTF-16

	
utf16len	- [in/out]	on input holds size of utf16str buffer, 
						on output holds number of utf16 characters stored

returns bytes processed from rawdata
*/
size_t TextDocument::rawdata_to_utf16(const unsigned char *rawdata, size_t rawlen, char16_t *utf16str, size_t *utf16len)
{
	//rawdate是UTF16的，所以直接拷贝就行
	rawlen /= sizeof(char16_t);
	return copy_utf16((UTF16 *)rawdata, rawlen, (UTF16 *)utf16str, utf16len) * sizeof(char16_t);
}

uint32_t TextDocument::replace_raw(uint32_t offset_bytes, const char16_t *text, uint32_t length, uint32_t erase_chars)
{
	unsigned char  buf[0x100];
	size_t buflen;
	uint32_t copied;
	uint32_t rawlen = 0;
	uint32_t offset = offset_bytes + mHeaderSize;

	uint32_t erase_bytes	= count_chars(offset_bytes, erase_chars);
	mSeq.group();
	while(length)
	{
		buflen = 0x100;
		copied = utf16_to_rawdata(text, length, buf, (size_t *)&buflen);

		// do the piece-table replacement!
		if(!mSeq.replace(offset, buf, buflen, erase_bytes))
			break;

		text   += copied;
		length -= copied;
		rawlen += buflen;
		offset += buflen;

		erase_bytes = 0;
	}
	mSeq.ungroup();
	mDocLength_bytes = mSeq.size();
	return rawlen;
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

	return ret > 0 ? true : false;
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

	start				= mSeq.event_index() - mHeaderSize;
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

	start				= mSeq.event_index() - mHeaderSize;
	length				= mSeq.event_length();

	*offset_start		= start;
	*offset_end			= start + length;
	
	mDocLength_bytes	= mSeq.size();

	return true;
}

bool TextDocument::fillheader(int format, DWORD* pHeader, int * pHeader_length)const
{
	if(pHeader == 0 || pHeader_length == 0) return false;
	int count = sizeof(BOMLOOKUP) / sizeof(BOM);

	for(int i = 0; i < count; i++)
	{
		if(BOMLOOKUP[i].type == format)
		{
			*pHeader		= BOMLOOKUP[i].bom;
			*pHeader_length = BOMLOOKUP[i].len;
			return true;
		}
	}
	return false;
}

bool TextDocument::FindForward(uint32_t start_offset_chars, const char16_t *text, uint32_t length, uint32_t *find_offset)
{
	for(Iterator itor	= GetIterator(start_offset_chars);
		*itor != '\0' ;
		++ itor )
	{
		if(strcmp_(text, length, itor) == 0)
		{
			*find_offset = itor.Offset();
			return true;
		}
	}
	return false;
}
bool TextDocument::FindBackward(uint32_t start_offset_chars, const char16_t *text, uint32_t length, uint32_t *find_offset)
{
	for(Iterator itor	= GetIterator(start_offset_chars);
		*itor != (char16_t)0xFFFF ;
		-- itor )
	{
		if(strcmp_(text, length, itor) == 0)
		{
			*find_offset = itor.Offset();
			return true;
		}
	}
	return false;
}

int	TextDocument::Convert2Utf8(
								   __in  int		format, 
								   __in  unsigned char*		buffer, 
								   __in  size_t		buffer_length, 
								    unsigned char *		utf8str, 
								    size_t *	utf8len
							   )
{
	switch(format)
	{
	// convert from ANSI->UNICODE
	case NCP_ASCII:
		return 0;
		
	case NCP_UTF8:
		return 0;//utf8_to_utf16(buffer, file_length, (UTF16 *)utf16str, utf16len);

	// already unicode, do a straight memory copy
	case NCP_UTF16:
		return utf16_to_utf8((UTF16 *)buffer, buffer_length, utf8str,  utf8len);
		//return 0;
	//	rawlen /= sizeof(char16_t);
	//	return copy_utf16((UTF16 *)rawdata, rawlen, (UTF16 *)utf16str, utf16len) * sizeof(char16_t);

	// need to convert from big-endian to little-endian
	//case NCP_UTF16BE:
	//	buffer_length /= sizeof(char16_t);
	//	return swap_utf16((UTF16 *)buffer, buffer_length, (UTF16 *)utf16str, utf16len) * sizeof(char16_t);

	// error! we should *never* reach this point
	default:
		*utf8len = 0;
		return 0;	
	}
}

}}
