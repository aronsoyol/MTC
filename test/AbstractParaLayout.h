#pragma once 
#include <utility>
#include "util.h"



namespace Aqitai{namespace LayoutEngine{
		
struct Rect;
struct Run;
class AbstractParaLayoutEngeine
{
	typedef  int result;
public:
	/*
	line_no - indicate which line to be paint
	*/
	virtual void	paint_line(int line_no) = 0;

	/*
	max_line_width - indicate max line width,
	*/
	virtual void	break_text(int max_line_width) = 0;

	/*
	start - 
	*/
	virtual void	select_text(int start, int length) = 0;

	virtual void	itemize() = 0;

	virtual void	shape() = 0;

	virtual void	set_text(const wchar_t* text, int length);
};

}}