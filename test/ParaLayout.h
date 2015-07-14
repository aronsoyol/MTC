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
			int g_start;
			int g_length;
			UScriptCode script;
			Run(int start, int length, UScriptCode script) :
				start(start), length(length), script(script){}
		};
		class ParaLayout
		{
			struct text_line
			{
				int start;
				int end;
				text_line(int start_, int end_) 
					:start(start_), end(end_){}
			};
			class LineBreaker
			{
			private:
				BreakIterator* boundary;// = BreakIterator::createLineInstance(Locale::getUS(), status);
				UErrorCode status;
				const int *char_width_list;
				int max_line_width;
				int current_width;
				int start;
				int end;
				int length;
				int get_chars_width(int start, int end)
				{
					int total = 0;
					for (int i = start; i < end; i++)
						total += char_width_list[i];
					return total;
				}
			public:
				LineBreaker(const wchar_t * text, int length_, int max_line_width_, const int * char_width_list_) :
					max_line_width(max_line_width_),
					length(length_),
					current_width(0),
					start(0), end(0),
					char_width_list(char_width_list_),
					status(U_ZERO_ERROR)
				{
					boundary = BreakIterator::createLineInstance(Locale::getUS(), status);
					boundary->setText(text);
				}
				~LineBreaker()
				{
					delete boundary;
				}
				int first()
				{
					start = 0;
					end = 0;
					return boundary->first();
				}
				int next()
				{
					if (start == length)
						return -1;
					do{
						end = boundary->next();
						if (end == length)
						{
							start = length;
							return start;
						}
							
						int break_width = get_chars_width(start, end);
						if (current_width + break_width > max_line_width)
						{
							current_width = break_width;
							int old_start = start;
							start = end;
							return old_start;
						}
						else
						{
							current_width += break_width;
						}
						start = end;// , end = boundary->next();
					} while (end != BreakIterator::DONE);
					if (end == BreakIterator::DONE)
						return length;
				}
			};
			std::wstring text;
			std::vector<Run> run_list;
			std::vector<int> char_width_list;
			std::vector<text_line> line_list;
			using glyph_index = int;
			using glyph_vector = std::vector<glyph>;
			glyph_vector glyph_list;
			const FontOption  *fontOption;
		private:
			virtual void	itemize();

			virtual void	shape();
			virtual int		ShapeRun(int i);

			int  DrawRun(const Run& run, int x, int y, HDC dc);
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
			void draw(unsigned int* buffer, int width, int height, int x, int y);
			/**/
			int draw_chars(HDC dc, int start, int end, int x, int y);
			int draw_chars(unsigned int* buffer, int width, int height, int start, int end, int x, int y);
			int get_chars_width(int start, int end);
		};

	}
}