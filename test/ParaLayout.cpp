#include "ParaLayout.h"
#include "scrptrun.h"
#include <hb-ft.h>
#include <hb-icu.h>
#include "util.h"
#include <vector>
namespace MTC{	namespace LayoutEngine{
	static inline const uint16_t * uchar_to_utf16(const UChar* src)
	{
		static_assert(sizeof(UChar) == sizeof(uint16_t), "UChar is eq size to uint16_t");
#if defined(_MSC_VER)
		return reinterpret_cast<const uint16_t *>(src);
#else
		return src;
#endif
	}
	class LineBreaker
	{
	private:
		const int *char_width_list;
		int max_line_width;
		int _start;
		int _length;
		int _current_width;
		UnicodeString _text;
		int get_chars_width(int start, int end)
		{
			int total = 0;
			for (int i = start; i < end; i++)
				total += char_width_list[i];
			return total;
		}
	public:
		LineBreaker(UnicodeString text, int length, int max_line_width_, const int * char_width_list_) :
			_text(text), max_line_width(max_line_width_), _current_width(0), _length(length),
			_start(0), char_width_list(char_width_list_)
		{

		}

		void first()
		{
			_start = 0;
		}
		int width()
		{
			return _current_width;
		}
		int next()
		{
			if (_start == _length)
				return -1;
			UErrorCode status;
			BreakIterator *boundary = BreakIterator::createLineInstance(Locale::getUS(), status);
			boundary->setText(UnicodeString( _text.getBuffer() + _start));
			boundary->first();
			int end = boundary->next();
			_current_width = get_chars_width(_start, _start + end);
			if (_current_width >= max_line_width)
			{
				delete boundary;
				boundary = 0;

				_start += end;
				return _start;
			}
			int break_width = 0;
			int tmp_start = end;
			while (_current_width + break_width <= max_line_width)
			{
				tmp_start = end;
				_current_width += break_width;
				if (_start + end == _length)
				{
					break;
				}
				end = boundary->next();

				break_width = get_chars_width(_start + tmp_start, _start + end);

			}
			if (boundary)
			{
				delete boundary;
				boundary = 0;
			}

			_start += tmp_start;
			return _start;
		}
	};
	ParaLayout::ParaLayout(const FontOption  *fontOption_) : fontOption(fontOption_)
	{
		//fontOption = new FontOption(50);
		//this->run_list.emplace_back(0, 0, 0, (UScriptCode)0);
		//fontOption->back = RGB(255, 0, 0);
		//fontOption->fore = RGB(0,0,0);
	}
	ParaLayout::~ParaLayout()
	{
		//delete fontOption;
	}
	/*
	line_no - indicate which line to be paint
	*/
	void	ParaLayout::paint_line(int line_no)
	{	
	}
	//int		ParaLayout::get_one_line(int start, int max_width)
	//{
	//	int line_width = 0;
	//	int break_width = ;
	//	while ()

	//}
	int		ParaLayout::break_line(int max_line_width)
	{
		line_list.clear();
		if (_text.length() == 0)
		{
			line_list.emplace_back(_text.getBuffer(), 0, 0, 0);
			return 1;
		}
		if (max_line_width <= 0)
		{
			int total_width = get_chars_width(0, _text.length());
			line_list.emplace_back(_text.getBuffer(), 0, _text.length(), total_width);

			return 1;
		}
		if (char_width_list.size() == 0)
			return 1;
		int start = 0;
		LineBreaker breaker(_text.getBuffer(), _text.length(), max_line_width, &char_width_list[0]);
		breaker.first();
		int end = 0;
		while ((end = breaker.next()) != -1)
		{
			line_list.emplace_back(_text.getBuffer(), start, end, breaker.width());
			start = end;
		}
		return line_list.size();
	}
	/*
	max_line_width - indicate max line width,
	*/
	int	ParaLayout::iterate_break_line(int max_line_width, int start_char_pos)
	{
		return 0;
		//if (start_char_pos >= text.length())
		//	return - 1;
		//UErrorCode status = U_ZERO_ERROR;
		//BreakIterator* boundary = BreakIterator::createLineInstance(Locale::getUS(), status);
		//boundary->setText(text.c_str() + start_char_pos);
		//int32_t start = boundary->first();
		//
		//int line_start = 0;
		//int line_width = 0;
		//int line = 0;

		//int32_t end;
		//do
		////for ()
		//{
		//	end = boundary->next();
		//	

		//	int break_width = get_chars_width(start, end);
		//	if (line_width + break_width > max_line_width)
		//	{
		//		return start;
		//	}
		//	else
		//	{
		//		line_width += break_width;
		//	}
		//	start = end;// , end = boundary->next();
		//} while (end != BreakIterator::DONE);
		//if (end == BreakIterator::DONE)
		//	return text.length();
	}

	/*
	start -
	*/
	void	ParaLayout::select_text(int start, int length){}

	void	ParaLayout::itemize()
	{
		run_list.clear();
		int item_start = 0;// vertBreaker.Start();
		int item_length = _text.length();// vertBreaker.End() - vertBreaker.Start();
		std::vector<std::wstring> lines;
		/*	while (vertBreaker.next())
			{
			item_start = vertBreaker.Start();
			item_length = vertBreaker.End() - vertBreaker.Start();
			*/
		ScriptRunIterator runIter(_text.getBuffer() + item_start, 0, item_length);

		while (runIter.next())
		{
			int32_t     start = runIter.getScriptStart();
			int32_t     end = runIter.getScriptEnd();
			UScriptCode code = runIter.getScriptCode();
			run_list.emplace_back(_text.getBuffer(), item_start + start, end - start, code);
			lines.emplace_back(_text.getBuffer() + item_start + start, end - start);
		}
	}

	void	ParaLayout::shape()
	{
		glyph_list.reserve(_text.length());
		
		int g_start = 0;
		for (int i = 0; i < run_list.size(); i++)
		{
			run_list[i].g_start = g_start;
			int g_len = ShapeRun(i);
			run_list[i].g_length = g_len;
			g_start += g_len;
		}
	}

	int	ParaLayout::ShapeRun(int run_index)
	{
		const Run &		run = run_list[run_index];

		hb_buffer_t * buffer	= hb_buffer_create();
		
		int * charWidthList = new int[std::max(run.length, 2)];

		for (int i = 0; i < std::max(run.length, 2); i++)
			charWidthList[i] = 0;
		//
		//set buffer
		//	
		hb_direction_t dir = run.dir();// run.script == USCRIPT_MONGOLIAN ? HB_DIRECTION_LTR : HB_DIRECTION_TTB;
		hb_script_t scriptTag = hb_icu_script_to_script(run.script);
		hb_buffer_add_utf16(buffer, uchar_to_utf16(_text.getBuffer()), _text.length(), run.start, run.length);
		hb_buffer_set_direction(buffer, dir); /* or LTR */
		hb_buffer_set_script(buffer, scriptTag); /* see hb-unicode.h */
		hb_buffer_set_flags(buffer, (hb_buffer_flags_t)(HB_BUFFER_FLAG_PRESERVE_DEFAULT_IGNORABLES | HB_BUFFER_FLAG_EOT));
		//hb_buffer_guess_segment_properties(buffer);
		//
		//
		//
		const int fontIndex = run.script == USCRIPT_MONGOLIAN ? FixedFont::MONGOL : FixedFont::OTHER;
		hb_shape(fontOption->hb_ft_font[fontIndex], buffer, NULL, 0);

		int					glyph_count = hb_buffer_get_length(buffer);
		hb_glyph_info_t *	glyph_info = hb_buffer_get_glyph_infos(buffer, 0);
		hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buffer, 0);


		WCHAR				dbg_info[1000];
		//int					pen_x = x;
		//int					pen_y = y;

		int					lastClstIndex = glyph_count > 0 ? glyph_info[0].cluster : 0;
		int					offset = lastClstIndex;
		for (int i = 0; i < glyph_count; ++i)
		{
			FT_UInt glyph_index = glyph_info[i].codepoint;
			//FT_Load_Glyph(fontOption->ft_face[fontIndex], glyph_index, FT_LOAD_DEFAULT);

			/* convert to an anti-aliased bitmap */
			//FT_Render_Glyph(fontOption->ft_face[fontIndex]->glyph, FT_RENDER_MODE_NORMAL);


			/*不管是TTB还是LTR 这个变量都是0*/
			//fontOption->ft_face[fontIndex]->glyph->advance.y
				

			/*
			字形旋转后90后宽度会有以像素左右变化，所以一下两种解决方案
			1.LTR方向的run就水平绘制后，在做旋转
			2.以旋转后的宽度为准

			选择2的话，Glyph的x_offset, y_offset就没有用了，所以要以Harfbuzz为准，不用FT_Matrix旋转Glyph
			*/
			int advance = std::abs(dir == HB_DIRECTION_TTB ? glyph_pos[i].y_advance : glyph_pos[i].x_advance);
			int glyfWidth = advance >> 6;
			{
				charWidthList[glyph_info[i].cluster - offset] += glyfWidth;
				int cluster_char_count = glyph_info[i].cluster - lastClstIndex;
				if (cluster_char_count > 1)
				{
					int average_width = charWidthList[lastClstIndex - offset] / cluster_char_count;
					for (int k = lastClstIndex; k < glyph_info[i].cluster; k++)
					{
						charWidthList[k - offset] = average_width;
					}
				}
				lastClstIndex = glyph_info[i].cluster;
			}

			glyph_list.emplace_back(
				glyph_info[i].codepoint,
				glyph_pos[i].x_advance,
				glyph_pos[i].y_advance,
				glyph_pos[i].x_offset,
				glyph_pos[i].y_offset,
				glyph_info[i].cluster,
				&run_list[run_index]
			);
		}

		for (int i = 0; i < run.length; i++)
		{
			char_width_list.emplace_back(charWidthList[i]);
		}

		OutputDebugString(L"\n");
		delete[] charWidthList;
		hb_buffer_destroy(buffer);
		return glyph_count;
	}
	void	ParaLayout::set_text(const UnicodeString& text)
	{
		_text = text;
		const wchar_t* str = text.getBuffer();
		int length = text.length();
		itemize();
		shape();

	}

	int ParaLayout::get_char_position(int x, int y, bool* trailling)
	{
		if (y < 0)
			y = 0;
		int line_no = x / fontOption->LineHeight();
		return get_char_position_from_line(line_no, y, trailling);
	}
	int ParaLayout::get_char_position_from_line(int line_no, int y, bool* trailling)
	{
		if (line_no >= line_list.size())
		{
			return _text.length();
		}
		const text_line * line = &line_list[line_no];
		int dy = 0;
		if (y >= line->_width)
		{
			*trailling = true;
			return std::max(0, line->_end - 1);
		}
		for (int i = line->_start; i < line->_end; ++i)
		{
			char_width_list[i];
			if (y < dy + char_width_list[i] && y >= dy)
			{
				if (y >= dy + char_width_list[i] / 2)
				{
					*trailling = true;
				}

				return i;
			}
			dy += char_width_list[i];
		}
		return _text.length();
	}
	bool ParaLayout::get_char_location(int char_pos, bool trailling, Point * point)
	{
		std::vector<text_line>::const_iterator itor = line_list.cbegin();
		int line_no = 0;
		if (char_pos + (int)trailling == _text.length())
		{
			point->y = line_list[line_list.size() - 1]._width;
			point->x = (line_list.size() - 1) * fontOption->LineHeight();
			return true;
		}
		for (; itor < line_list.end() && itor->_end <= char_pos;
			++itor, ++line_no);
				
		if (itor != line_list.end() && itor->_end > char_pos && itor->_start <= char_pos)
		{
			point->y = get_chars_width(itor->_start, char_pos + (int)trailling);
			point->x = line_no * fontOption->LineHeight();

			return true;
		}
		return false;

	}
	//void	ParaLayout::vdraw(HDC hdc, int x, int y)
	//{
	//	float line_height =
	//		fontOption->ft_face[0]->ascender - fontOption->ft_face[0]->descender;
	//	line_height = line_height * 50 / fontOption->ft_face[0]->units_per_EM;
	//	std::vector<text_line>::iterator itor = line_list.begin();
	//	int l = 0;
	//	for (; itor != line_list.end(); ++itor)
	//	{
	//		draw_chars(hdc, (*itor).start, (*itor).end, x + l, y);
	//		l += line_height;
	//	}
	//}
	//void	ParaLayout::hdraw(HDC hdc, int x, int y)
	//{
	//	float line_height =
	//		fontOption->ft_face[0]->ascender - fontOption->ft_face[0]->descender;
	//	line_height = line_height * 50 / fontOption->ft_face[0]->units_per_EM;

	//	draw(hdc, x, y + line_height);
	//}


	void	ParaLayout::draw(unsigned int* buffer, int width, int height, int x, int y)
	{
		std::vector<text_line>::iterator itor = line_list.begin();
		int buff_size = width * height;
		int l = 0;
		for (; itor != line_list.end(); ++itor)
		{
				
			draw_chars(buffer, width, height, itor->_start, itor->_end, x + l, y);
				
			/*实验用的代码，画出字符边界*/
			int h = 0;
			for (int j = itor->_start; j < itor->_end; j++)
			{
				h += char_width_list[j];
				for (int k = 0; k < 25; k++)
				{
					int index = (height - 1 - h - y) * width + x + k + l - 25;
					if (index < buff_size && index >= 0);
						//buffer[index] = 0;
				}
			}
			/*结束*/

			l += fontOption-> LineHeight();
		}
	}
	int ParaLayout::get_chars_width(int start, int end)
	{
		int total = 0;
		for (int i = start; i < end; i++)
			total += char_width_list[i];
		return total;
	}
	int ParaLayout::draw_chars(unsigned int* buffer, int width, int height, int start, int end, int x, int y)
	{
		/*
		对于CJK等字形方向和行的方向相对有旋转角度的文字使用通过Harfbuzz,TTB 计算的位置
		对于蒙文等字形相对于行没有旋转的run应该使用LTB方向设置harfbuzz，并旋转glyph
		*/
		glyph_vector::iterator itor = glyph_list.begin();

		int pen_x = x;
		int pen_y = y;


		int g_index = 0;
		for (; itor != glyph_list.end() && (*itor).cluster < end; ++itor, g_index++)
		{
			glyph * glyph_ptr = &(*itor);
			if (glyph_ptr->cluster < start)
				continue;
			const hb_direction_t dir	= glyph_ptr->run->dir();
			const int fontIndex = glyph_ptr->run->script == USCRIPT_MONGOLIAN ? FixedFont::MONGOL : FixedFont::OTHER;
			FT_UInt glyph_index = glyph_ptr->index;

			FT_Error error;
			if (glyph_ptr->image == 0)
			{
				error = FT_Load_Glyph(fontOption->ft_face[fontIndex], glyph_index, FT_LOAD_DEFAULT);
				error = FT_Get_Glyph(fontOption->ft_face[fontIndex]->glyph, &glyph_ptr->image);
			}
			FT_Glyph bmp_glyph = glyph_ptr->image;
			if (glyph_ptr->image->format != FT_GLYPH_FORMAT_BITMAP)
			{
				FT_Glyph_To_Bitmap(&bmp_glyph, FT_RENDER_MODE_NORMAL, 0, 0);
			}
			else
			{
				int stop = 1;
			}

			int offset_y = (*itor).y_offset / 64;
			int offset_x = (*itor).x_offset / 64;

/*			int ascender = fontOption->ft_face[1]->ascender >> 6;*/

			if (dir == HB_DIRECTION_TTB)
			{
				int line_height = fontOption->LineHeight();
				offset_x = (*itor).x_offset / 64 + line_height / 2;
			}
			else
			{
				std::swap(offset_y, offset_x);
			}

			wchar_t dbg[100];
			FT_BitmapGlyph bit = (FT_BitmapGlyph)bmp_glyph;

			FT_Bitmap* bmp_ptr = &bit->bitmap;
			FT_Bitmap newBmp;
			int bit_left = bit->left;
			int bit_top = bit->top;
			if (dir != HB_DIRECTION_TTB)
			{
				Rotate90Degree(&bit->bitmap, &newBmp);
				int newLeft = bit->top - newBmp.width;
				//std::swap(bit->top, bit->left);
				bit_top = -bit->left;
				bit_left = newLeft;
				bmp_ptr = &newBmp;
			}
			FreeTypeDrawBitmap(buffer, width, height, DARW_MODE_TRANSPARENT, bmp_ptr,
				pen_x + bit_left + offset_x - fontOption->Descender(),
				pen_y - bit_top - offset_y, fontOption->fore, fontOption->back);

			if (dir != HB_DIRECTION_TTB)
				delete[] newBmp.buffer;

			FT_Done_Glyph(bmp_glyph);
#ifdef _DEBUG
			for (int k = 0; k < 25; k++)
			{
				int buffer_length = width* height;
				int index = (height - 1 - pen_y) * width + pen_x + k;
				if(index < buffer_length && index >= 0)
					buffer[index] = 0xff0000;
			}
#endif
			int dy1 = glyph_ptr->x_advance >> 6;
			int dy2 = glyph_ptr->image->advance.x >> 16;
				
				
			/*
			发现旋转后的字形的宽度比Harfbuzz获得的宽度大一点
			*/
			//wsprintf(dbg, L"[hb:%d,ft:%d],", dy1, dy2);
			//OutputDebugString(dbg);
				
			if (dir == HB_DIRECTION_TTB)
			{
				pen_x += glyph_ptr->x_advance >> 6;
				pen_y -= glyph_ptr->y_advance >> 6;
			}
			else
			{
				pen_y += glyph_ptr->x_advance >> 6;
				pen_x += glyph_ptr->y_advance >> 6;
			}
		}
		return 0;
	}
			
}}