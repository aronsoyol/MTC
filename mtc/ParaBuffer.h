#ifndef __MTC_LINE_BUFFER_H
#define __MTC_LINE_BUFFER_H

#include <cassert>
#include <vector>
#include <stdint.h>
#include "MnDebug.h"
#include "CharIterator.h"



namespace MTC{ namespace DOC{ namespace PARA{

struct LINE_BREAKER
{
	uint32_t	lb;
	uint16_t	len;
};
//管理行起始位置和长度的行缓冲区
//创建后行数为0，所以即使文本是空的，使用前也必须调用Init**，让段落(Paragraph)缓冲器至少有一行，否则不能用
class ParaBuffer
{
	static const int	INIT_SIZE	= 1024;
	static const int	GROW_STEP	= 10;
private:
	typedef std::vector<int> BUFFER;
	BUFFER				m_Buffer;
	int					m_nLastModifyLine;
	int					m_nLastModifyDiff;

	void		CommitModify()
	{

		for(uint32_t i = m_nLastModifyLine + 1; i < m_Buffer.size(); i++)
		{
			m_Buffer[i] +=  m_nLastModifyDiff;
		}
		m_nLastModifyDiff = 0;
	}
public:
	static bool IsLineBreaker(const char16_t*, int * length);
	static int	IsLineBreaker(CharIterator itor, CharIterator end);
	ParaBuffer(void);
	virtual		~ParaBuffer(void);
	//获取行的开始位置
	int			operator [](const int nLineNo) const;
	
	//
	//改变某行的长度
	//diff为负数的时候绝对值不能大于等于当前行的长度
	//所以要在某行上进行删除操作时先要判断本次操作是否会删除整个行
	//
	int			ModifyLine(int nLineNo, int diff);
	
	//
	//
	//
	int			InsertNonBreakerTextAt(const int nOffset, const int length);
	//
	//从文本缓冲区初始化行信息
	//
	bool		InitFromBuffer(const char16_t* pBuffer,const uint32_t nLength);
	
	bool		InitFromIterator(CharIterator begin, 
								CharIterator end);

	//
	//添加新行
	//
	bool		InsertLine(int nLineNo, int nLength);		
	
	//
	//在末尾添加行
	//
	bool		AppendLine(int nLength);

	//
	//删除某行
	//
	bool		RemoveLine(int nLineNo);
	
	//
	//返回某行的长度
	//
	int			LineLength(int nLineNo) const
	{
		const ParaBuffer& lb = *this;
		return (lb[nLineNo + 1] - lb[nLineNo ]);
	}

	//
	//返回当前行的数量
	//
	int			ParaCount() const
	{
		return m_Buffer.size() - 1;
	}

	//
	//
	//
	void		PrintInfo()
	{
		for (int i = 0; i < ParaCount(); i++)
		{
			mndebug(1, "[%d]", m_Buffer[i+ 1] - m_Buffer[i]);
		}
		mndebug(1, "[length:%d]\n", m_Buffer[LineCount()]);
	}
	//
	//清理当前行信息,清理后行数为0，再次使用前必须调用Init***
	//
	void		Clear()
	{
		m_Buffer.clear();
		//添加两个空项，当文本内容为空的时候，行管理其至少要有一个空的行

		m_Buffer.push_back(0);
		m_nLastModifyDiff = 0;
		m_nLastModifyLine = 0;
	}
	int			GetLineFromOffset(const int nOffset)const ;

	int			EraseText(int nOffset, int nLength);


};

}}}

#endif/*__MTC_LINE_BUFFER_H*/