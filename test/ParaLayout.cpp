#pragma once 
#include "ParaLayout.h"
#include "scrptrun.h"
#include "../hb/hb-ft.h"
#include "../hb/hb-icu.h"
#include "hb_draw.h"

namespace Aqitai{
	namespace LayoutEngine{
		//struct  Run
		//{
		//	int start;
		//	int length;
		//	UScriptCode script;
		//	Run(int start, int length, UScriptCode script) :
		//		start(start), length(length), script(script){}
		//};
		//GetGlyphsWidth()
		ParaLayout::ParaLayout(const FontOption  *fontOption_) :fontOption(fontOption_)
		{
			//fontOption = new FontOption(50);
			this->run_list.emplace_back(0, 0, (UScriptCode)0);
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
		int		ParaLayout::break_line(int max_line_width)
		{
			line_list.clear();
			ParaLayout::LineBreaker breaker(text.c_str(), text.length(), max_line_width, &char_width_list[0]);

			int start = breaker.first();
			for (int32_t end = breaker.next();
				end != BreakIterator::DONE;
				start = end, end = breaker.next())
			{
				//printTextRange(boundary, start, end);
				line_list.emplace_back(start, end);

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
			ScriptRunIterator runIter(text.c_str(), 0, text.length());
			run_list.clear();
			while (runIter.next())
			{
				int32_t     start	= runIter.getScriptStart();
				int32_t     end		= runIter.getScriptEnd();
				UScriptCode code	= runIter.getScriptCode();
				run_list.emplace_back(start, end - start, code);
			}
		}

		void	ParaLayout::shape()
		{
			glyph_list.reserve(text.length());
		
			int g_start = 0;
			for (int i = 0; i < run_list.size(); i++)
			{
				run_list[i].g_start = g_start;
				int g_len = ShapeRun(i);
				run_list[i].g_length = g_len;
				g_start += g_len;
			}
		}
		int ParaLayout::DrawRun(const Run& run, int x, int y, HDC dc)
		{
			const int fontIndex = run.script == USCRIPT_MONGOLIAN ? MONGOL_FONT : OTHER_FONT;
			int pen_x = x;
			int pen_y = y;
			
			RECT rect;
			GetClipBox(dc, &rect);
			for (int i = 0; i < run.g_length; i++)
			{
				FT_UInt glyph_index = glyph_list[run.g_start + i].index;

				FT_Load_Glyph(fontOption->ft_face[fontIndex], glyph_index, FT_LOAD_DEFAULT);
				FT_Render_Glyph(fontOption->ft_face[fontIndex]->glyph, FT_RENDER_MODE_NORMAL);

				if (pen_x >rect.right || pen_y > rect.bottom)
					break;
				int offset_y = glyph_list[run.g_start + i].y_offset / 64;
				int offset_x = glyph_list[run.g_start + i].x_offset / 64;
				FreeTypeDrawBitmap(dc, DARW_MODE_TRANSPARENT, &fontOption->ft_face[fontIndex]->glyph->bitmap,
					pen_x + fontOption->ft_face[fontIndex]->glyph->bitmap_left + offset_x,
					pen_y - fontOption->ft_face[fontIndex]->glyph->bitmap_top - offset_y, fontOption->fore, fontOption->back);
				pen_x += glyph_list[run.g_start + i].x_advance/64;
			}
			return pen_x;
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
			hb_script_t scriptTag = hb_icu_script_to_script(run.script);
			hb_buffer_add_utf16(buffer, (uint16_t*)text.c_str(), text.length(), run.start, run.length);
			hb_buffer_set_direction(buffer, HB_DIRECTION_LTR); /* or LTR */
			hb_buffer_set_script(buffer, scriptTag); /* see hb-unicode.h */
			hb_buffer_set_flags(buffer, (hb_buffer_flags_t)(HB_BUFFER_FLAG_PRESERVE_DEFAULT_IGNORABLES | HB_BUFFER_FLAG_EOT));
			//hb_buffer_guess_segment_properties(buffer);
			//
			//
			//
			const int fontIndex = run.script == USCRIPT_MONGOLIAN ? MONGOL_FONT : OTHER_FONT;
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
				FT_Load_Glyph(fontOption->ft_face[fontIndex], glyph_index, FT_LOAD_DEFAULT);

				//wsprintf(dbg_info, L"%d,", glyph_info[i].cluster/* + 87*/);
				//OutputDebugString(dbg_info);

				/* convert to an anti-aliased bitmap */
				//FT_Render_Glyph(fontOption.ft_face[fontIndex]->glyph, FT_RENDER_MODE_NORMAL);
				int glyfWidth = fontOption->ft_face[fontIndex]->glyph->advance.x >> 6;
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
					glyph_pos[i].y_advance, 
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
		void	ParaLayout::set_text(const wchar_t* text_, int length)
		{
			text = std::wstring(text_, length);
			this->run_list.reserve(length);
			itemize();
			shape();
		}
		void	ParaLayout::draw(HDC dc, int x, int y)
		{
			std::vector<text_line>::iterator itor = line_list.begin();
			int l = 0;
			for (; itor != line_list.end(); ++itor)
			{
				draw_chars(dc, (*itor).start,(*itor).end, x, y+l);
				l += 50;
			}

		}

		void	ParaLayout::draw(unsigned int* buffer, int width, int height, int x, int y)
		{
			std::vector<text_line>::iterator itor = line_list.begin();
			int l = 0;
			for (; itor != line_list.end(); ++itor)
			{
				draw_chars(buffer, width, height, (*itor).start, (*itor).end, x, y + l);
				l += 50;
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
			glyph_vector::iterator itor = glyph_list.begin();

			int pen_x = x;
			int pen_y = y;


			int g_index = 0;
			for (; itor != glyph_list.end() && (*itor).cluster < end; ++itor, g_index++)
			{
				if ((*itor).cluster < start)
					continue;

				const int fontIndex = (*itor).run->script == USCRIPT_MONGOLIAN ? MONGOL_FONT : OTHER_FONT;
				FT_UInt glyph_index = (*itor).index;

				if ((*itor).image == 0)
				{
					FT_Load_Glyph(fontOption->ft_face[fontIndex], glyph_index, FT_LOAD_DEFAULT);
					FT_Get_Glyph(fontOption->ft_face[fontIndex]->glyph, &((*itor).image));
				}

				if ((*itor).image->format != FT_GLYPH_FORMAT_BITMAP)
				{
					FT_Glyph_To_Bitmap(&(*itor).image, FT_RENDER_MODE_NORMAL, 0, 0);
				}
				else
				{
					int stop = 1;
				}
				FT_GlyphSlot;
				FT_Glyph;
				
				int offset_y = (*itor).y_offset / 64;
				int offset_x = (*itor).x_offset / 64;
				FT_BitmapGlyph  bit = (FT_BitmapGlyph)(*itor).image;
				if (pen_x > width || pen_y - bit->top - offset_y > height)
					break;
				
				FreeTypeDrawBitmap(buffer, width, height, DARW_MODE_TRANSPARENT, &bit->bitmap,
					pen_x + bit->left + offset_x,
					pen_y - bit->top - offset_y, fontOption->fore, fontOption->back);
				pen_x += (*itor).x_advance / 64;
			}
			return 0;
		}
		int ParaLayout::draw_chars(HDC dc, int start, int end, int x, int y)
		{
			glyph_vector::iterator itor = glyph_list.begin();
			
			int pen_x = x;
			int pen_y = y;

			RECT rect;
			GetClipBox(dc, &rect);
			int g_index = 0;
			for (; itor != glyph_list.end() && (*itor).cluster < end; ++itor, g_index++)
			{
				if ((*itor).cluster < start)
					continue;

				const int fontIndex = (*itor).run->script == USCRIPT_MONGOLIAN ? MONGOL_FONT : OTHER_FONT;
				FT_UInt glyph_index = (*itor).index;

				if ((*itor).image == 0)
				{
					FT_Load_Glyph(fontOption->ft_face[fontIndex], glyph_index, FT_LOAD_DEFAULT);
					FT_Get_Glyph(fontOption->ft_face[fontIndex]->glyph, &((*itor).image));
				}

				if ((*itor).image->format != FT_GLYPH_FORMAT_BITMAP)
				{
					FT_Glyph_To_Bitmap(&(*itor).image, FT_RENDER_MODE_NORMAL, 0, 0);
				}
				else
				{
					int stop = 1;
				}
				FT_GlyphSlot; 
				FT_Glyph;
				if (pen_x > rect.right || pen_y > rect.bottom)
					break;
				int offset_y = (*itor).y_offset / 64;
				int offset_x = (*itor).x_offset / 64;
				FT_BitmapGlyph  bit = (FT_BitmapGlyph)(*itor).image;
				FreeTypeDrawBitmap(dc, DARW_MODE_TRANSPARENT, &bit->bitmap,
					pen_x + bit->left + offset_x,
					pen_y - bit->top - offset_y, fontOption->fore, fontOption->back);
				pen_x += (*itor).x_advance / 64;
				//FT_Done_Glyph((*itor).image);
				//(*itor).image = 0;
			}
			return 0;
		}

		
	}
}