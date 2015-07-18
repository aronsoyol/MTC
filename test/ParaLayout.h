#pragma once 

#include <string>
#include <algorithm>
#include <vector>
#include <unicode/brkiter.h>
#include "hb_draw.h"
#include "AbstractParaLayout.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
namespace Aqitai{
	namespace LayoutEngine{
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
			std::wstring text_;
			int g_start;
			int g_length;
			UScriptCode script;
			Run(const wchar_t* text, int start, int length, UScriptCode script) :
				text_(text + start, length),
				start(start), length(length), script(script){}
			//Run(int start, int length, UScriptCode script) :
			//	start(start), length(length), script(script)
			//{
			//	int found = 1;
			//}
			hb_direction_t dir() const
			{
				if (script == USCRIPT_HAN || script == USCRIPT_HIRAGANA || script == USCRIPT_KATAKANA)
					return HB_DIRECTION_TTB;
				else
					return HB_DIRECTION_LTR;
			}
		};
		class VerticalScriptBreaker
		{
		private:
			UErrorCode error;
			int start;
			int end;
			int length;
			//UScriptCode script; //next script
			//UScriptCode last_script;
			const wchar_t* text;
			bool vertical;

		public:
			VerticalScriptBreaker(const wchar_t * text_, int length_)
				:text(text_),start(0), end(0), length(length_), vertical(false){}
			void first()
			{
				start = 0;
				end = 0;

			}
			bool next()
			{
				if (start == length || length == 0)
					return false;
				
					

				start = end;

				vertical = CanBeVertRun(uscript_getScript(text[start], &error));

				for (int i = start + 1; i < length; i++)
				{
					UScriptCode script = uscript_getScript(text[i], &error);
					if (vertical != CanBeVertRun(script))
					{
						end = i;
						return true;
					}
				}
				end = length;
				return true;
			}
			int Start(){ return start; }
			int End(){ return end; }
			bool Vertical(){ return vertical; }
		private:
			bool CanBeVertRun(UScriptCode script)
			{
				if (script == USCRIPT_HAN || script == USCRIPT_HIRAGANA
					|| script == USCRIPT_KATAKANA)
					return true;
				else 
					return false;
			}
		};
	
		
		class ParaLayout
		{
			struct text_line
			{
				int start;
				int end;
				std::wstring _text;
				text_line(int start_, int end_) 
					:start(start_), end(end_){}
				text_line(const wchar_t* text, int start_, int end_)
					:start(start_), end(end_), _text(text + start, end_ - start_){}
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
			std::wstring text;
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
			const FontOption  *fontOption;
		private:
			virtual void	itemize();
			virtual void	shape();
			virtual void	place();

			virtual int		ShapeRun(int i);

			int  DrawRun(const Run& run, int x, int y, HDC dc);
			int get_one_line(int start, int max_width);
		public:

			/**/
			ParaLayout(const FontOption  *fontOption);
			
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

			
			virtual void	set_text(const wchar_t* text, int length);

			/**/

			void draw(HDC dc, int x, int y);
			void vdraw(HDC hdc, int x, int y);
			void hdraw(HDC hdc, int x, int y);
			void draw(unsigned int* buffer, int width, int height, int x, int y);
			/**/
			int draw_chars(HDC dc, int start, int end, int x, int y);
			int draw_chars(unsigned int* buffer, int width, int height, int start, int end, int x, int y);
			int get_chars_width(int start, int end);
		};

	}
}