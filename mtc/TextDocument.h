#ifndef __MTC_TEXTDOCUMENT_H
#define __MTC_TEXTDOCUMENT_H

//汉字在SourceTree里显示乱码
//每行的最大的字符数，超过这个数字则强制换行
//#define MAX_LINE_LENGTH  102400



#include <algorithm>
#include <string.h>
#include <stdint.h>
#include <cassert>

#if !defined (ANDROID) && defined (_WIN32)
#include <windows.h>
#endif

#include "sequence.h"
#include "CmpIterator.h"
#include "ParaBuffer.h"

namespace MTC{ namespace DOC{

bool IsLineBreaker(char16_t);
bool IncludeLineBreaker(char16_t* buffer, int length);

//#define LBA_NEW_EMPTY_LINE	0
//#define LBA_REMOVE_LINE		1
//#define LBA_CHANGE_LINE		2 
//#define LBA_CHANGE_ALL		3

typedef enum SUPPORTED_CODE_PAGE{
	NCP_ASCII	= 0,
	NCP_UTF8	= 1,
	NCP_UTF16	= 2,
	NCP_UTF16BE = 3,
	NCP_UTF32	= 4,
	NCP_UTF32BE = 5,
	NCP_UNKNOWN = 6
} SCP;

typedef struct ByteOrderMark
{
	uint32_t	bom;
	uint32_t	len;
	int			type;
} BOM;

typedef struct _LINE_INFO
{
	uint32_t	nParaNo;
	uint32_t	nOffset;
	uint32_t	nLength;
} LINE_INFO, Para;

/*interface*/
class BufferWatcher
{
	virtual void NewPara(int nParaNo) = 0;
	virtual void ParaChanged(int nParaNo) = 0;
	virtual void ParaCountChanged(int nParaCount) = 0;
};
class TextDocument
{
	friend class TextIterator;
	friend class CharCmpIterator;
	typedef CharCmpIterator Iterator;

public:
	//
	//单个行的最大的字符数，超过这个数字则强制换行
	//
	static const int MaxLineLength = 102400;


	TextDocument();
	~TextDocument();

	bool    Init(){
		//assert(0); 
		return Clear() && mSeq.init();
	}
			
	bool	Init(const char16_t * buffer, long length);
					
public:
	bool	Undo(uint32_t* offset_start, uint32_t* offset_end);
	bool	Redo(uint32_t* offset_start, uint32_t* offset_end);

	bool	CanUndo(){ return mSeq.canundo() ? true : false; }
	bool	CanRedo(){ return mSeq.canredo() ? true : false; }
	void	BreakOptimize(){ mSeq.breakopt(); }

	bool		InsertText(const uint32_t offset_chars, const char16_t *text, uint32_t length);
	uint32_t	EraseText(uint32_t offset_chars, uint32_t length);
	uint32_t	GetPara(uint32_t nParaNo, char16_t *buffer, uint32_t buflen, uint32_t *off_chars, bool fIncludeBreaker = false);
	bool		GetText(uint32_t offset_chars, uint32_t length_chars, char16_t *buffer, uint32_t *buflen);

	uint32_t	ParaCount() const
	{
		return mParaBuffer.ParaCount();
	}
	uint32_t	TextLength() const
	{
		return byteoffset_to_charoffset(mDocLength_bytes - mHeaderSize);
	}

	uint32_t	GetLineNo(uint32_t offset_chars) const;

	bool	GetLineInfo(uint32_t nLineNo, LINE_INFO* pLineInfo = NULL) const;
	bool	GetLineInfo(LINE_INFO* pLineInfo) const;
	bool	GetLineInfoFromOffset(uint32_t offset, LINE_INFO* pLineInfo = NULL)const;

	Iterator GetIterator(uint32_t offset)
	{
		return Iterator(this, offset);
	}

	int		CodePage()const{ return mFileFormat; }

	int		SetCodePage(SCP cp){ mFileFormat = cp;}

	void	ResetLineBuffer()
	{
		Iterator begin = GetIterator(0);
		Iterator end = GetIterator(TextLength());
		mParaBuffer.InitFromIterator(begin, end);
	}
		
	void	SetParaBufferListener(BufferWatcher* watcher)
	{
		mBufferWatcher = watcher;
	}
private:
	typedef MTC::DOC::PARA::ParaBuffer ParaBuffer;

	ParaBuffer		mParaBuffer;
	BufferWatcher * mBufferWatcher;
	
	TextIterator	iterate(uint32_t offset);
	TextIterator	iterate_line(uint32_t lineno, uint32_t *linestart = 0, uint32_t *linelen = 0);
	TextIterator	iterate_line_offset(uint32_t offset_chars, uint32_t *lineno, uint32_t *linestart = 0);
	bool			InitLineBuffer();//new one

	/*
	size in bytes
	*/
	uint32_t		Size() const
	{
		return mDocLength_bytes;
	}

	bool	fillheader(int format, DWORD* pHeader, int * pHeader_length)const;

	bool	Clear();


	uint32_t	lineno_from_offset(uint32_t offset);
	uint32_t	offset_from_lineno(uint32_t lineno);

	uint32_t	count_chars(uint32_t offset_bytes, uint32_t length_chars) const;


	int			file_to_utf16(int format, unsigned char* buffer, size_t buffer_length,  char16_t *utf16str,  size_t *utf16len);


	int		CRLF_size(char16_t *szText, int nLength)const;

	bool	lineinfo_from_offset(uint32_t offset_chars, uint32_t *lineno, uint32_t *lineoff_chars, uint32_t *linelen_chars, uint32_t *lineoff_bytes, uint32_t *linelen_bytes);
	//bool	lineinfo_from_lineno(uint32_t lineno,                      uint32_t *lineoff_chars,  uint32_t *linelen_chars, uint32_t *lineoff_bytes, uint32_t *linelen_bytes);


	SEQ::sequence	mSeq;

	uint32_t	mDocLength_bytes;  //include header size

	SCP			mFileFormat;
	int			mHeaderSize;

	
};

class TextIterator
{

public:
	// default constructor sets all members to zero
	TextIterator()
		: text_doc(0), off_bytes(0), len_bytes(0)
	{
	}

	TextIterator(uint32_t off, uint32_t len, TextDocument *td)
		: text_doc(td), off_bytes(off), len_bytes(len)
	{

	}

	// default copy-constructor
	TextIterator(const TextIterator &ti)
		: text_doc(ti.text_doc), off_bytes(ti.off_bytes), len_bytes(ti.len_bytes)
	{
	}

	// assignment operator
	TextIterator & operator= (TextIterator &ti)
	{
		text_doc = ti.text_doc;
		off_bytes = ti.off_bytes;
		len_bytes = ti.len_bytes;
		return *this;
	}
	char16_t operator *()
	{
		char16_t buf;
		uint32_t buflen = sizeof(char16_t);
		//text_doc->gettext(off_bytes, len_bytes, &buf, &buflen);
		uint32_t len = text_doc->mSeq.render(off_bytes, (SEQ::seqchar*)&buf, sizeof(char16_t) / sizeof(SEQ::seqchar));;

		return (len == sizeof(SEQ::seqchar)) ? buf : '\0';
	}


	uint32_t gettext(char16_t *buf, uint32_t buflen)
	{
		if (text_doc)
		{
			// get text from the TextDocument at the specified byte-offset
			uint32_t len = text_doc->gettext(off_bytes, len_bytes, buf, &buflen);

			// adjust the iterator's internal position
			off_bytes += len;
			len_bytes -= len;

			return buflen;
		}
		else
		{
			return 0;
		}
	}

	operator bool()
	{
		return text_doc ? true : false;
	}

private:

	TextDocument *	text_doc;
	uint32_t		off_bytes;
	uint32_t		len_bytes;
};

}}

#endif /*__MTC_TEXTDOCUMENT_H*/