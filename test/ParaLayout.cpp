#pragma once 
#include "ParaLayout.h"
#include "scrptrun.h"
#include "../hb/hb-ft.h"
#include "../hb/hb-icu.h"
#include "hb_draw.h"
#include <vector>
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
			if (text.length() == 0)
			{
				line_list.emplace_back(text.c_str(), 0, 0);
				return 1;
			}
			int start = 0;
			LineBreaker breaker(text.c_str(), text.length(), max_line_width, &char_width_list[0]);
			breaker.first();
			int end = 0;
			while ((end = breaker.next()) != -1)
			{
				line_list.emplace_back(text.c_str(), start, end);
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
			VerticalScriptBreaker vertBreaker(text.c_str(), text.length());
			vertBreaker.first();
			run_list.clear();
			int item_start = 0;// vertBreaker.Start();
			int item_length = text.length();// vertBreaker.End() - vertBreaker.Start();
			std::vector<std::wstring> lines;
			/*	while (vertBreaker.next())
				{
				item_start = vertBreaker.Start();
				item_length = vertBreaker.End() - vertBreaker.Start();
				*/
			ScriptRunIterator runIter(text.c_str() + item_start, 0, item_length);

			UErrorCode error;

			wchar_t ch0 = L'《';
			int ch1 = '?';
			char16_t code = '《';
			UBlockCode block0 = ublock_getCode(L'《');
			UBlockCode block1 = ublock_getCode(L'1');
			UScriptCode sc0 = uscript_getScript(L'《', &error);
			UScriptCode sc1 = uscript_getScript(L'a', &error);
			UScriptCode sc2 = uscript_getScript(L'1', &error);
			UScriptCode sc3 = uscript_getScript(L',', &error);
			UScriptCode sc4 = uscript_getScript(L'!', &error);
			while (runIter.next())
			{
				int32_t     start = runIter.getScriptStart();
				int32_t     end = runIter.getScriptEnd();
				UScriptCode code = runIter.getScriptCode();
				run_list.emplace_back(text.c_str(), item_start + start, end - start, code);
				lines.emplace_back(text.c_str() + item_start + start, end - start);
			}
		}
		void	ParaLayout::place()
		{

			_lineBreakList.clear();
			UErrorCode status;
			BreakIterator * breaker = BreakIterator::createLineInstance(Locale::getUS(), status);
			breaker->setText(text.c_str());
			breaker->first();
			int start = 0;
			while (breaker->next() != BreakIterator::DONE)
			{
				int end = breaker->current();
				int width = get_chars_width(start, end);
				_lineBreakList.emplace_back(start, end, width);
				start = end;
			}

			_charBreakList.clear();
			breaker = BreakIterator::createCharacterInstance(Locale::getUS(), status);
			breaker->setText(text.c_str());
			breaker->first();
			start = 0;
			while (breaker->next() != BreakIterator::DONE)
			{
				int end = breaker->current();
				int width = get_chars_width(start, end);
				_charBreakList.emplace_back(start, end, width);
				start = end;
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
			hb_direction_t dir = run.dir();// run.script == USCRIPT_MONGOLIAN ? HB_DIRECTION_LTR : HB_DIRECTION_TTB;
			hb_script_t scriptTag = hb_icu_script_to_script(run.script);
			hb_buffer_add_utf16(buffer, (uint16_t*)text.c_str(), text.length(), run.start, run.length);
			hb_buffer_set_direction(buffer, dir); /* or LTR */
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
				int advance = dir == HB_DIRECTION_TTB ? std::abs(glyph_pos[i].y_advance) : glyph_pos[i].x_advance;
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
		void	ParaLayout::set_text(const wchar_t* text_, int length)
		{
			text = std::wstring(text_, length);
			run_list.reserve(length);
			itemize();
			
			shape();
			place();
		}
		void	ParaLayout::vdraw(HDC hdc, int x, int y)
		{
			float line_height =
				fontOption->ft_face[0]->ascender - fontOption->ft_face[0]->descender;
			line_height = line_height * 50 / fontOption->ft_face[0]->units_per_EM;
			std::vector<text_line>::iterator itor = line_list.begin();
			int l = 0;
			for (; itor != line_list.end(); ++itor)
			{
				draw_chars(hdc, (*itor).start, (*itor).end, x + l, y);
				l += line_height;
			}
		}
		void	ParaLayout::hdraw(HDC hdc, int x, int y)
		{
			float line_height =
				fontOption->ft_face[0]->ascender - fontOption->ft_face[0]->descender;
			line_height = line_height * 50 / fontOption->ft_face[0]->units_per_EM;

			draw(hdc, x, y + line_height);
		}
		void	ParaLayout::draw(HDC dc, int x, int y)
		{
			float line_height =
				fontOption->ft_face[0]->ascender - fontOption->ft_face[0]->descender;
			line_height = line_height * 50 / fontOption->ft_face[0]->units_per_EM;
			std::vector<text_line>::iterator itor = line_list.begin();
			int l = 0;
			for (; itor != line_list.end(); ++itor)
			{
				draw_chars(dc, (*itor).start, (*itor).end, x, y + l);
				l += 50;
			}

		}

		void	ParaLayout::draw(unsigned int* buffer, int width, int height, int x, int y)
		{
			std::vector<text_line>::iterator itor = line_list.begin();
			int l = 0;
			for (; itor != line_list.end(); ++itor)
			{
				draw_chars(buffer, width, height, (*itor).start, (*itor).end, x + l, y );
				
				/*实验用的代码，画出字符边界*/
				int h = 0;
				for (int j = (*itor).start; j < (*itor).end; j++)
				{
					h += char_width_list[j];
					for (int k = 0; k < 25; k++)
					{
						buffer[(height - 1 - h - y) * width + x + k + l - 25] = 0;
					}
				}
				/*结束*/

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
				const int fontIndex = glyph_ptr->run->script == USCRIPT_MONGOLIAN ? MONGOL_FONT : OTHER_FONT;
				FT_UInt glyph_index = glyph_ptr->index;

				FT_Glyph image = glyph_ptr->image;
				if (image == 0)
				{
					FT_Load_Glyph(fontOption->ft_face[fontIndex], glyph_index, FT_LOAD_DEFAULT);
					FT_Get_Glyph(fontOption->ft_face[fontIndex]->glyph, &image);
					if (dir != HB_DIRECTION_TTB)
					{
						/*
						用FT_Matrix旋转Glyph后，Glyph的尺寸和harfbuzz计算的尺寸不完全一样，所以这个方法需要改进
						*/
#define PI 3.141592
						FT_Matrix     matrix;              /* transformation matrix */
						FT_Vector     pen;                 /* untransformed origin */
						pen.x = 0;
						pen.y = 0;
						/* set up matrix */
						//int degree = 3600;
						double deg = -90;
						double len = 2;
						double rad = deg * PI / 180.0;
						matrix.xx = (FT_Fixed)(cos(rad) * 0x10000L);
						matrix.xy = (FT_Fixed)(-sin(rad) * 0x10000L);
						matrix.yx = (FT_Fixed)(sin(rad) * 0x10000L);
						matrix.yy = (FT_Fixed)(cos(rad) * 0x10000L);
						FT_Glyph_Transform(image, &matrix, &pen);
					}
					
					
				}

				if (image->format != FT_GLYPH_FORMAT_BITMAP)
				{
					FT_Glyph_To_Bitmap(&image, FT_RENDER_MODE_NORMAL, 0, 0);
				}
				else
				{
					int stop = 1;
				}
				FT_GlyphSlot;
				FT_Glyph;

				int offset_y = (*itor).y_offset / 64;
				int offset_x = (*itor).x_offset / 64;

				if (dir == HB_DIRECTION_TTB)
				{
					//std::swap(offset_x, offset_y);
				}

				FT_BitmapGlyph bit = (FT_BitmapGlyph)image;

				//int advance = image->advance.y;// dir == HB_DIRECTION_TTB ? : -image->advance.y;
				//advance = (*itor).x_advance;
				int yy = pen_y - bit->top /*- offset_y*/;
				
	/*			int ascender = fontOption->ft_face[1]->ascender >> 6;*/
				int descender = 0;
				if (dir == HB_DIRECTION_TTB)
				{
					yy -= offset_y;
					offset_x = (*itor).x_offset / 64;
				}
				else
				{
					yy -= offset_x;
					offset_x = -(fontOption->ft_face[0]->ascender - fontOption->ft_face[0]->descender >> 6) / 2;
				}
				wchar_t dbg[100];
				//yy -= dir == HB_DIRECTION_TTB ? fontOption->ft_face[1]->ascender >> 6 : 0;
				//int tmp = bit->top + offset_y;
				//yy = pen_y;
				if (pen_x < width && yy < height)
				{
					FreeTypeDrawBitmap(buffer, width, height, DARW_MODE_TRANSPARENT, &bit->bitmap,
						pen_x + bit->left + offset_x ,
						yy, fontOption->fore, fontOption->back);
				}

				for (int k = 0; k < 25; k++)
				{
					buffer[(height - 1 - pen_y) * width + pen_x + k] = 0xff0000;
				}
				
				int dy1 = glyph_ptr->x_advance >> 6;
				int dy2 = image->advance.y >> 16;
				
				
				/*
				发现旋转后的字形的宽度比Harfbuzz获得的宽度大一点
				*/
				wsprintf(dbg, L"[hb:%d,ft:%d],", dy1, dy2);
				OutputDebugString(dbg);
				
				if (dir == HB_DIRECTION_TTB)
				{
					pen_x += glyph_ptr->x_advance >> 6;
					pen_y -= glyph_ptr->y_advance >> 6;
				}
				else
				{
					pen_x += image->advance.x >> 16;
					pen_y -= image->advance.y >> 16;
				}
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
			//pen_y += 50;
			for (; itor != glyph_list.end() && (*itor).cluster < end; ++itor, g_index++)
			{
				if ((*itor).cluster < start)
					continue;

				const Run * run = (*itor).run;
				hb_direction_t dir = run->dir();// (*itor).run->script == USCRIPT_MONGOLIAN ? HB_DIRECTION_LTR : HB_DIRECTION_TTB;
				
				const int fontIndex = (*itor).run->script == USCRIPT_MONGOLIAN ? MONGOL_FONT : OTHER_FONT;
				FT_UInt glyph_index = (*itor).index;

				if ((*itor).image == 0)
				{
					if (run->script == USCRIPT_HAN || run->script == USCRIPT_HIRAGANA || run->script == USCRIPT_KATAKANA)
					{
						FT_Matrix     matrix;              /* transformation matrix */
						FT_Vector     pen;                 /* untransformed origin */
						pen.x = 0;
						pen.y = 0;
						/* set up matrix */
						matrix.xx = (FT_Fixed)(cos(-90) * 0x10000L);
						matrix.xy = (FT_Fixed)(-sin(-90) * 0x10000L);
						matrix.yx = (FT_Fixed)(sin(-90) * 0x10000L);
						matrix.yy = (FT_Fixed)(cos(-90) * 0x10000L);
					}
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
				FT_Glyph glyph = (*itor).image;
				FT_BitmapGlyph bit = (FT_BitmapGlyph)glyph;
				//pen_y += 50;
				
				int advance = /*std::abs(glyph->advance.x >> 16);*/ (dir == HB_DIRECTION_TTB ? -(*itor).y_advance : (*itor).x_advance)/64;
				//int y = dir == HB_DIRECTION_TTB ? pen_y : pen_y - bit->top - offset_y;
				FreeTypeDrawBitmap(dc, DARW_MODE_TRANSPARENT, &bit->bitmap,
					pen_x + bit->left /*+ offset_x*/,
					pen_y - bit->top - offset_y, fontOption->fore, fontOption->back);
				
				pen_y += advance/* / 64*/;
			}
			return 0;
		}

		
	}
}