#include "ParaBuffer.h"
#include "TextDocument.h"

using namespace MTC::DOC;
using namespace MTC::DOC::PARA;

//{ 0x000A, 0x000B, 0x000C, 0x000D, 0x0085, 0x2028, 0x2029, 0x000D, 0x000A };
LINE_BREAKER LB[] =
{
	{0x000A000D, 2},
	{0x0000000A, 1},
	{0x0000000B, 1},
	{0x0000000C, 1},
	{0x0000000D, 1},
	{0x00000085, 1},
	{0x00002028, 1},
	{0x00002029, 1}
};
int ParaBuffer::IsLineBreaker(CharCmpIterator itor, CharCmpIterator end)
{
	WCHAR sample[2];
	int sampleLen = 0;

	while(itor != end && sampleLen < 2)
	{
		sample[sampleLen] = *itor;
		++sampleLen;
		++itor;
	}
	int lbCount= sizeof(LB )/sizeof(LINE_BREAKER);
	for(int i = 0; i < lbCount; i++)
	{
		int cmplen= std::min<int>(LB[i].len, sampleLen);
		if(0 == memcmp(sample, &LB[i].lb, sizeof(WCHAR) * cmplen))
		{
			return cmplen;
		}
	}
	return 0;
}
bool ParaBuffer::IsLineBreaker(const char16_t* string, int * len)
{
	int lbCount= sizeof(LB )/sizeof(LINE_BREAKER);
	for(int i = 0; i < lbCount; i++)
	{
		if(0 == memcmp(string, &LB[i].lb, sizeof(WCHAR) * std::min<int>(LB[i].len, *len)))
		{
			* len = std::min<int>(LB[i].len, *len);
			return true;
		}
	}
	return false;
}

ParaBuffer::ParaBuffer(void):
m_nLastModifyLine(0),m_nLastModifyDiff(0)
{
	Clear();
}

ParaBuffer::~ParaBuffer(void)
{

}
int ParaBuffer::operator [](const int nLineNo) const
{
	if(nLineNo <= m_nLastModifyLine)
	{
		return m_Buffer[nLineNo];
	}
	else
	{
		return m_Buffer[nLineNo] + m_nLastModifyDiff;
	}
}
bool ParaBuffer::InitFromIterator(
	CharCmpIterator begin, 
	CharCmpIterator end)
{
	Clear();
	int last_line_start = 0;
	CharCmpIterator it	= begin;
	int i= 0;
	while(it != end)
	{
		int lb_len = 0;//
		int next_line_start  = 0;
		if(0 < (lb_len = ParaBuffer::IsLineBreaker(it, end)))
		{
			if(lb_len == 2)
				next_line_start = i + 2;
			else
				next_line_start = i + 1;
			AppendLine(next_line_start - last_line_start);
			last_line_start = next_line_start;
			while(i < last_line_start)
			{
				++it;
				++i;
			}
		}
		else
		{
			++it;
			++i;
		}
	}
	assert(i >= last_line_start);
	AppendLine(i - last_line_start);
	return true;
}
bool ParaBuffer::InitFromBuffer(const char16_t* pBuffer, const uint32_t nLength)
{

	Clear();
	int last_line_start = 0;
	for(ULONG i = 0; i < nLength;)
	{
		int lb_len = nLength - i;//初始化为剩余的文本长度
		int next_line_start  = 0;
		if (ParaBuffer::IsLineBreaker((char16_t*)&pBuffer[i], &lb_len))
		{
			if(lb_len == 2)
				next_line_start = i + 2;
			else
				next_line_start = i + 1;
			AppendLine(next_line_start - last_line_start);
			last_line_start = next_line_start;
			i = last_line_start;
		}
		else
		{
			i++;
		}
	}
	if(nLength > last_line_start)
	AppendLine(nLength - last_line_start);
	return true;
}
bool ParaBuffer::AppendLine(int nLength)
{
	assert(nLength >= 0);
	if(nLength < 0) 
		return false;
	mndebug(1, "[append %d, %d]\n", ParaCount(), nLength);
	//要保证尾部总有一个空行
	//所以m_nLength的最大值是m_nSize - 1 
	//如果已经达到最大值时加入新行则要重新申请内存
	int backValue = m_Buffer.back();
	m_Buffer.push_back(backValue + nLength);
	//m_Buffer[m_nLineCount - 1]	= m_Buffer[m_nLineCount - 2] + nLength;
	return true;
}

bool ParaBuffer::InsertLine(int nLineNo, int nLength)
{
	assert(nLength >= 0);
	printf("[insert %d, %d]\n", nLineNo, nLength);
	//要保证尾部总有一个空行
	//所以m_nLength的最大值是m_nSize - 1 
	//如果已经达到最大值时加入新行则要重新申请内存
	int nLineCount = ParaCount();

	assert(nLineNo >= 0);
	assert(nLineNo < ParaCount());
	CommitModify();
	if(nLength <= 0)
		return false;
	//0 5 7 10
	//insert at 2 nLength = 10
	//[0]0 5 _ 7 10
	//[1]0 5 7 7 10
	//[2]0 5 7 17 20
	int old_value = m_Buffer[nLineNo];

	BUFFER::iterator it		= m_Buffer.begin();
	std::advance(it, nLineNo );
	BUFFER::iterator it2	= m_Buffer.insert(it, old_value);

	for(++it2; it2 < m_Buffer.end(); ++it2)
	{
		(*it2) += nLength;
	}
	return true;
}
bool ParaBuffer::RemoveLine(int nLineNo)
{
	int nLength = 0;
	mndebug(1, "[remove %d, %d]\n", nLineNo, nLength = operator[nLineNo + 1] - operator[nLineNo]);
	int nLineCount = ParaCount();
	assert(nLineNo >= 0);
	assert(nLineNo < ParaCount());
	CommitModify();
	BUFFER::iterator it		= m_Buffer.begin();
	std::advance(it, nLineNo );
	BUFFER::iterator it2	= m_Buffer.erase(it);

	for( ; it2 < m_Buffer.end(); ++it2)
	{
		(*it2) -= nLength;
	}
	return true;
}
//改变某行的长度
//不处理删除字符数超过现有字符数的情况
int ParaBuffer::ModifyLine(int nLineNo, int diff)
{
	assert(0 <= nLineNo);
	assert(ParaCount() > nLineNo);
	mndebug(1, "[ModifyLine %d, %d]\n", nLineNo, diff);
	
	if(nLineNo != m_nLastModifyLine)
	{
		CommitModify();
		assert(0 == m_nLastModifyDiff);
	}
	
	assert(0 <= (operator[](nLineNo + 1) - operator[](nLineNo)) + diff);

	m_nLastModifyLine = nLineNo;
	m_nLastModifyDiff += diff;
	return 0;
}

/* 
      0    5    10 
      |    |    |  
0  0  ########LB
1  10 ###########LB
2  23 ######LB
3  31 #####LB
4  38 ##########
5  48 	

第一种情况，删除操作影响到其他行
EraseText(12, 20)
nStart	= 12
nLength	= 20
      0    5    10  
      |    |    |
0  0  ########LB
1  10 ##----------
2  23 --------
3  31 -####LB
4  38 ##########
5  48 	

step1. 
	lineStart	= GetLineNo(12);
	lineEnd		= GetLIneNo(12 + 20);
	在区间（lineStart, lineEnd]里的行全删掉

step2
	考虑lineStart 和lineEnd剩余部分合并后的长度是否安全(小于TextDocument::MaxLineLength)
	如果是的话拆分行

step3
	原来的第lineEnd行以后的所有行首 -20

	如果正好删除了换行符，则跟以上的处理一样

	EraseText(21, 3)
	  0    5    10 
	  |    |    |  
0  0  ########LB
1  10 ###########LB
2  23 ######LB
3  31 #####LB
4  38 ##########
5  48

第二种情况：删除操作不影响其他行的话

只需调用ModifyLine

*/
int ParaBuffer::EraseText(int nOffset, int nLength)
{
	assert(nOffset + nLength < operator[](ParaCount()));
	
	const int nLineStart = GetLineFromOffset(nOffset);
	const int nLineEnd	= GetLineFromOffset(nOffset + nLength);
	//所有受影响的行的总长度[nLineStart, nLineEnd] 区间的所有行的长度的和
	const long nTotalLength = operator[](nLineEnd + 1) - operator[](nLineStart);
	//const int nLineStartRemain	= nOffset - operator[](nLineStart);
	//const int nLineEndRemain	= nOffset + nLength - operator[](nLineEnd);
	//第一步，判断删除操作是否影响多行
	const int nEraseLineCount = nLineEnd - nLineStart;
	if(nEraseLineCount > 0)//yes
	{
		//#step1.
		if(m_nLastModifyDiff != 0)
			CommitModify();
		BUFFER::iterator itor = m_Buffer.begin();
		itor = m_Buffer.erase(itor + nLineStart + 1, itor + nLineEnd + 1);
		m_nLastModifyLine = nLineStart;
		//#step2.
		//nTotalLength - nLength是合并后的新行的长度
		if(nTotalLength - nLength > TextDocument::MaxLineLength)
		{
			m_Buffer.insert(itor, operator[](nLineStart) + TextDocument::MaxLineLength);
			m_nLastModifyLine++;
		}
		m_nLastModifyDiff = -nLength;
	}
	else//no
	{
		ModifyLine(nLineStart, -nLength);
	}
	return nEraseLineCount;
}

int	ParaBuffer::InsertNonBreakerTextAt(const int nOffset, const int nLength)
{
	//这个nOffset是否是m_nLastModifyLine，顺序输入文本的情况下，应该是
	if(nOffset >= operator[](m_nLastModifyLine) && nOffset <= (operator[](m_nLastModifyLine + 1)))
	{
		//如果前后行的起始位置一样，这个行只能是最后一行
		assert(operator[](m_nLastModifyLine) == operator[](m_nLastModifyLine + 1) ?  (ParaCount() - 1) == m_nLastModifyLine : false );
	}
	return -1;
}
//
//nOffset 必须小于operator[](ParaCount())
//否则返回-1
int ParaBuffer::GetLineFromOffset(const int nOffset)const
{
	assert(nOffset > 0 ? nOffset < operator[](ParaCount()): true);
	int low = 0;
	int high = ParaCount() - 1;
	int line = 0;
	while(low <= high)
	{
		line = (low + high) / 2;
		if(nOffset >= operator[](line) && nOffset < (operator[](line + 1)))
		{
			return line;
		}
		else if((ParaCount() - 1) == line && nOffset == operator[](line) && operator[](line) == operator[](line + 1))
		{
			//如果前后行的起始位置一样，这个行只能是最后一行
			return line;
		}
		else if(nOffset < operator[](line))
		{
			high = line - 1;
		}
		else
			low = line + 1;
	}
	//没找到就是错误！
	assert(0);
	return -1;
}
