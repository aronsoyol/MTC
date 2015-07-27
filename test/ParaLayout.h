#ifndef __MTC_PARA_LAYOUT_H 
#define __MTC_PARA_LAYOUT_H
#include <string>
#include <algorithm>
#include <vector>
#include "AbstractParaLayout.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
namespace MTC{
	namespace LayoutEngine{
		using namespace Util;
		struct glyph
		{
			int index;
			int x_advance;
			int y_advance;
			int x_offset;
			int y_offset;
			int cluster;
			Run* run;
			FT_Glyph image;
			
			glyph(int index_, 
				int x_advance_, 
				int y_advance_, 
				int x_offset_,
				int y_offset_, 
				int cluster_,
				Run *run_)
				:index(index_), 
				x_advance(x_advance_), 
				y_advance(y_advance_), 
				x_offset(x_offset_), 
				y_offset(y_offset_), 
				cluster(cluster_),
				run(run_),
				image(0)
			{}
			operator FT_Glyph()
			{
				return image;
			}
			~glyph()
			{
				FT_Done_Glyph(image);
			}
		};
		struct  Run
		{
			int start;
			int length;
			int g_start;
			int g_length;
			hb_script_t script;
			Run(int start, int length, hb_script_t hb_script) :
				start(start), length(length), script(hb_script){}

			hb_direction_t dir() const
			{
				if (script == HB_SCRIPT_HAN || script == HB_SCRIPT_HIRAGANA || script == HB_SCRIPT_KATAKANA)
					return HB_DIRECTION_TTB;
				else
					return HB_DIRECTION_LTR;
			}
		};

		class ParaLayout
		{
			struct text_line
			{
				int _start;
				int _end;
				int _width;
				UnicodeString _text;
				text_line(int start_, int end_) 
					:_start(start_), _end(end_){}
				text_line(const UnicodeString &text, int start, int end, int width)
					:_start(start), _end(end), _text(text.getBuffer() + start, end - start), _width(width){}
			};
			struct Break
			{
				int _start;
				int _end;
				int _width;
				Break(int start, int end, int width) :
					_start(start), _end(end), _width(width){} 
			};
			std::vector<Break> _lineBreakList;
			std::vector<Break> _charBreakList;
			UnicodeString _text;
			std::vector<Run> run_list;
			
			/*
			char_width_list只能用于对光标，插入符的定位。不能用于换行位置的判断
			例如两个字符由一个字形显示的时候，插入符应在这个字形的中间显示，
			但是这两个字符的中间是不能换行的，行的宽度的计算应该使用字形宽度
			*/
			std::vector<int> char_width_list;
			
			std::vector<text_line> line_list;
			using glyph_index = int;
			using glyph_vector = std::vector<glyph>;
			glyph_vector glyph_list;
			const MTC::Util::FontOption  *fontOption;
		private:
			virtual void	itemize();
			virtual void	shape();
			//virtual void	place();

			virtual int		ShapeRun(int i);

			int  DrawRun(const Run& run, int x, int y, HDC dc);
			int get_one_line(int start, int max_width);
		public:

			/**/
			ParaLayout(const MTC::Util::FontOption  *fontOption);
			
			/**/
			~ParaLayout();
			/*
			line_no - indicate which line to be paint
			*/
			virtual void	paint_line(int line_no);

			/*
			max_line_width - indicate max line width,
			*/
			virtual int		iterate_break_line(int max_line_width, int start);
			
			/*
			*/
			int		break_line(int max_line_width);
			/*
			start -
			*/
			virtual void	select_text(int start, int length);

			
			virtual void	set_text(const UnicodeString& text);


			int get_char_position(int x, int y, bool* trailling);
			int get_char_position_from_line(int line_no, int y, bool* trailling);

			bool get_char_location(int char_pos, bool trailling, Point * point);

			/**/
			void draw(unsigned int* buffer, int width, int height, int x, int y);
			/**/

			int draw_chars(unsigned int* buffer, int width, int height, int start, int end, int x, int y);
			int get_chars_width(int start, int end);
		};

	}
}
#endif /*__MTC_PARA_LAYOUT_H*/