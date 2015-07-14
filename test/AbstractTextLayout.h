#pragma once 
#include <utility>
#include "util.h"

namespace Aqitai{ namespace LayoutEngine{

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