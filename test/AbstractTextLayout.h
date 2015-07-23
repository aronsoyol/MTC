#ifndef __MTC_LAYOUTENGINE_ABSTRACTTEXTLAYOUT_H
#define __MTC_LAYOUTENGINE_ABSTRACTTEXTLAYOUT_H

#include <utility>
#include "util.h"

namespace MTC{ namespace LayoutEngine{

struct Rect;
class AbstractTextLayoutEngeine
{
	typedef  int result;
public :
	virtual result	append_text(const wchar_t* text) = 0;
	virtual void	paint_text(int start, const Rect & rect) = 0;
	virtual void	break_tet(int max_line_width) = 0;
	virtual void	select_text(int start, int length) = 0;
};

}}
#endif /*__MTC_LAYOUTENGINE_ABSTRACTTEXTLAYOUT_H*/