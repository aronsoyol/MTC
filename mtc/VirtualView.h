#ifndef __MTC_VIRTUAL_VIEW_H
#define __MTC_VIRTUAL_VIEW_H
#include <vector>
#include <stdint.h>
#include <assert.h>
#include "TextDocument.h"
#include "TextLayout.h"
#include "util.h"
#include "noncopyable.hpp"
namespace MTC{ namespace VIRTUALVIEW{
	using namespace MTC::DOC;
	using namespace MTC::Util;
	using namespace MTC::LayoutEngine;

	struct Line
	{
		uint32_t start;
		uint32_t end;
		Line(uint32_t st, uint32_t en)
			:start(st), end(en){}
		uint32_t length(){return end - start;}
	};
	struct Para
	{
		std::vector<Line> lines;
		uint32_t LineCount()const{return lines.size();}
	};
	class GlyphCache :public ParaLayout
	{
	public:
		uint32_t mLineNo;
		void Init(uint32_t lineNo, const char16_t* start, const char16_t* end)
		{
			mLineNo = lineNo;
			this->set_text(std::u16string(start, end));
		}
	};
	class VirtualView :public noncopyable
	{
		typedef std::vector<Para> ParaCacheVector;
		typedef std::vector<ParaLayout*> GlyphCacheVector;
		typedef std::vector<uint32_t> AccessVector;
	private:
		TextDocument* pDoc;
		FontOption * pFontOption;
		ParaCacheVector mParaCacheVector;
		GlyphCacheVector mGlyphCacheVector;
		uint32_t mMaxLineHeight;
		AccessVector mAccessVector;

	public:
		VirtualView();
		virtual ~VirtualView();
		uint32_t Width()const 
		{
			uint32_t width = 0;
			uint32_t lineHeight = pFontOption->LineHeight();
			ParaCacheVector::const_iterator itor = mParaCacheVector.cbegin();

			for (; itor != mParaCacheVector.cend(); ++itor)
			{
				width += (itor->LineCount() * lineHeight);
			}
			
			return width;
		}
		void SetLineHeight(uint32_t max_line_height)
		{
			assert(max_line_height > 0);
			if (mMaxLineHeight == max_line_height)
				return;
		}
		uint32_t GetLineCount();
		bool GetCharLocation(uint32_t line_no, uint32_t pos, bool trailing, Point * charLoc);
		uint32_t GetCharPosition(uint32_t x, uint32_t y);

		uint32_t GetLineNo(uint16_t x, uint16_t y);

		void	Draw(int* buffer, uint32_t width, uint32_t height, int x, int y);

	};

}}
#endif /*__MTC_VIRTUAL_VIEW_H*/

