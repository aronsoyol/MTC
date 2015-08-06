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
	using ParaBuffer = MTC::DOC::PARA::ParaBuffer;
	//friend class TextIterator;
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
	uint32_t	GetPara(uint32_t nParaNo, char16_t *buffer, uint32_t buflen, uint32_t *off_chars, int* breaker_len);
	bool		GetText(uint32_t offset_chars, uint32_t length_chars, char16_t *buffer, uint32_t *buflen);
	
	uint32_t	TextLength() const
	{
		return mSeq.size();
	}
	
	uint32_t	ParaCount() const
	{
		return mParaBuffer.ParaCount();
	}

	uint32_t	GetParaNo(uint32_t offset_chars) const;

	bool	GetParaInfo(uint32_t nLineNo, LINE_INFO* pLineInfo = NULL) const;
	bool	GetParaInfo(LINE_INFO* pLineInfo) const;
	bool	GetParaInfoFromOffset(uint32_t offset, LINE_INFO* pLineInfo = NULL)const;

	Iterator GetIterator(uint32_t offset)
	{
		return Iterator(this, offset);
	}

	int		CodePage()const{ return mFileFormat; }

	int		SetCodePage(SCP cp){ mFileFormat = cp;}

	void	ResetParaBuffer()
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
	/*
	Para 信息缓冲器
	*/
	ParaBuffer		mParaBuffer;
	
	/*
	外部提供的一个ParaBuffer的监视器，每当文本变化时需要重新扫面全部文	本
	以获取关于段落的信息，这是非常低效率的做法。所以，为了提高效率而引入了
	ParaBuffer，BufferWathcer可以及时	地把ParaBuffer的变化通知给前台。
	*/
	BufferWatcher * mBufferWatcher;

	/*
	PieceTable数据结构的内部缓冲
	*/
	SEQ::sequence	mSeq;

	/*
	文件在内部只用UTF16表示
	mFileFormat指的是文件的输入输出代码页
	*/
	SCP				mFileFormat;

	/*
	清除缓冲区
	*/
	bool			Clear();
};

}}

#endif /*__MTC_TEXTDOCUMENT_H*/