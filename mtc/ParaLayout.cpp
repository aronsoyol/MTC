#include "ParaLayout.h"
#include <hb-ft.h>

#include "util.h"
#include "breaker.h"
#include "scrptitemizer.h"
#include <vector>
#include <cassert>
#if defined ICU

#include <hb-icu.h>
#include <unicode/uscript.h>
#include <unicode/brkiter.h>

#endif
#ifdef JNI
#include <jni.h>
#endif
namespace MTC{	namespace LayoutEngine{
#if defined ICU
	static inline const uint16_t * uchar_to_utf16(const UChar* src)
	{
		static_assert(sizeof(UChar) == sizeof(uint16_t), "UChar is eq size to uint16_t");
#if defined(_MSC_VER)
		return reinterpret_cast<const uint16_t *>(src);
#else
		return src;
#endif
	}
	
	static inline const UChar * utf16_to_uchar(const uint16_t* src)
	{
		static_assert(sizeof(UChar) == sizeof(uint16_t), "UChar is eq size to uint16_t");
#if defined(_MSC_VER)
		return reinterpret_cast<const UChar *>(src);
#else
		return src;
#endif
	}

	static void icu_breaker(const uint16_t* text, int length, std::vector<Break>& breakList)
	{
		Breaker *brkItor = new IcuBreaker(BT_LINE);
		//UnicodeString utext((const UChar*));
		brkItor->setText(text, length);
		breakList.clear();
		int start = brkItor->first();
		int end = 0;
		while ((end = brkItor->next()) != BreakIterator::DONE)
		{
			breakList.emplace_back(start, end, -1);
			start = end;
		}
		delete brkItor;
	}
#endif

	static inline void mtc_itemizer(const uint16_t *text, int length, std::vector<Run>& runList)
	{
		runList.clear();

		MTC::ITEMIZER::ScriptItemIterator runIter(text, 0, length);

		while (runIter.next())
		{
			int32_t     start = runIter.getScriptStart();
			int32_t     end = runIter.getScriptEnd();
			hb_script_t script = runIter.get_hb_script();
			runList.emplace_back(start, end - start, script);
		}
	}
#if defined JNI  
#if !defined(ANDROID)
	struct JNILoader
	{
		/*
		jvm只能启动一次
		http://simple-asta.blogspot.jp/p/jni2.html
		*/
		JNIEnv *env = 0;
		JavaVM *jvm = 0;

		
		JNILoader()
		{
			JavaVMOption options[1];
			options[0].optionString = "-Djava.class.path=.";

			JavaVMInitArgs vm_args;
			vm_args.version = JNI_VERSION_1_6;
			vm_args.options = options;
			vm_args.nOptions = 1;
			int res = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
			if (res != JNI_OK){
				//std::cout << "cannot run JavaVM : " << res << endl;
				int failed = 1;
			}

		}
		~JNILoader()
		{
			int ret = jvm->DestroyJavaVM();
		}
	};
	JNILoader jniLoader;
#endif
	void jni_breaker(JNIEnv *env, const uint16_t* text, int length, std::vector<Break>& breakList)
	{
		Breaker *brkItor = new JNIBreaker(env, BT_LINE);
		//UnicodeString utext((const UChar*));
		brkItor->setText(text, length);
		breakList.clear();
		int start = brkItor->first();
		int end = 0;
		while ((end = brkItor->next()) != -1)
		{
			if (end > 0 && text[end - 1] == (uint16_t)0x180E)
				continue;
			breakList.emplace_back(start, end, -1);
			start = end;
		}
		delete brkItor;
	}
#endif

#if defined(ANDROID)
	ParaLayout::ParaLayout(JNIEnv * env, const MTC::Util::FontOption  *fontOption_)
		: _jni_env(env), fontOption(fontOption_)
	{
	}
#else
	ParaLayout::ParaLayout(const FontOption  *fontOption_) : fontOption(fontOption_)
	{
	}
#endif

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
		_line_list.clear();
#if defined (ICU)
		icu_breaker((const uint16_t*)&_text[0], _text.length(), _lineBreakList);
#else
#if defined (ANDROID)
		jni_breaker(_jni_env, (const uint16_t*)&_text[0], _text.length(), _lineBreakList);
#else
		jni_breaker(jniLoader.env, (const uint16_t*)&_text[0], _text.length(), _lineBreakList);
#endif
#endif	
		BreakVector::iterator itor = _lineBreakList.begin();
		
		for (; itor != _lineBreakList.end(); ++itor)
		{
			itor->_width = get_chars_width(itor->_start, itor->_end);
		}

		int start = 0;
		int end = 0;
		std::vector<std::wstring> str_line_list;
		int i = 0;
		int line_width = 0;
		while ((end = break_get_one_line(start, max_line_width, line_width)) != -1)
		{
#if defined (_DEBUG)
			int temp_line_width = get_chars_width(start, end);
			assert(line_width == temp_line_width);
			str_line_list.emplace_back(_text.c_str() + start, _text.c_str() + end);
#endif
			_line_list.emplace_back(start, end, line_width);

			start = end;
			i++;
		}
		return _line_list.size();
	}

	int		ParaLayout::break_get_one_line(int start, int max_line_width, int& line_width)
	{
		if (start == _text.length())
			return -1;

		int end = start;
		int lineWidth = 0;
		int breakWidth = 0;
		//#1.find out the first break position behind start 

		BreakVector::iterator itor = _lineBreakList.begin();
		for ( ; itor != _lineBreakList.end(); ++itor)
		{
			if (start >= itor->_start && start < itor->_end)
			{
				end = itor->_end;
				++itor;
				break;
			}
				
		}
		lineWidth = get_chars_width(start, end);


		for (; itor != _lineBreakList.end(); ++itor)
		{
			if (lineWidth + itor->_width > max_line_width)
			{
				line_width = lineWidth;
				return itor->_start;
			}
			else  if (lineWidth + itor->_width == max_line_width)
			{
				line_width = lineWidth + itor->_width;
				return itor->_end;
			}else
			{
				lineWidth += itor->_width;
			}
		}
		line_width = lineWidth;
		return _text.length();
	}
	/*
	start -
	*/
	void	ParaLayout::select_text(int start, int length){}

	void	ParaLayout::itemize()
	{
		mtc_itemizer(const_char16_to_uint16(&_text[0]), _text.size(), _run_list);
	}

	void	ParaLayout::shape()
	{
		glyph_list.clear();
		char_width_list.clear();
		if (glyph_list.capacity() < _text.length())
		{
			glyph_list.reserve(_text.size());
		}
		
		int g_start = 0;
		for (int i = 0; i < _run_list.size(); i++)
		{
			_run_list[i].g_start = g_start;
			int g_len = ShapeRun(i);
			_run_list[i].g_length = g_len;
			g_start += g_len;
		}
	}

	int	ParaLayout::ShapeRun(int run_index)
	{
		const Run &		run = _run_list[run_index];

		hb_buffer_t * buffer	= hb_buffer_create();
		
		int * charWidthList = new int[std::max(run.length, 2)];

		for (int i = 0; i < std::max(run.length, 2); i++)
			charWidthList[i] = 0;
		//
		//set buffer
		//	
		
		const hb_direction_t dir = run.dir();// run.script == USCRIPT_MONGOLIAN ? HB_DIRECTION_LTR : HB_DIRECTION_TTB;
		hb_buffer_add_utf16(buffer, const_char16_to_uint16(&_text[0]), _text.length(), run.start, run.length);
		hb_buffer_set_direction(buffer, dir); /* or LTR */
		hb_buffer_set_script(buffer, run.script); /* see hb-unicode.h */
		hb_buffer_set_flags(buffer, (hb_buffer_flags_t)(HB_BUFFER_FLAG_PRESERVE_DEFAULT_IGNORABLES | HB_BUFFER_FLAG_EOT));
		//hb_buffer_guess_segment_properties(buffer);
		//
		//
		//
		const int fontIndex = fontOption->font_index_from_script(run.script);
		hb_shape(fontOption->hb_ft_font[fontIndex], buffer, NULL, 0);

		int					glyph_count = hb_buffer_get_length(buffer);
		hb_glyph_info_t *	glyph_info = hb_buffer_get_glyph_infos(buffer, 0);
		hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buffer, 0);


		//char16_t				dbg_info[1000];
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
					/*
					消除整数在除运算中产生的误差
					*/
					charWidthList[lastClstIndex - offset] -= (cluster_char_count - 1) * average_width;
					for (int k = lastClstIndex + 1; k < glyph_info[i].cluster; k++)
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
				&_run_list[run_index]
			);
		}

		for (int i = 0; i < run.length; i++)
		{
			char_width_list.emplace_back(charWidthList[i]);
		}
#ifdef _WIN32
		OutputDebugString(L"\n");
#endif
		delete[] charWidthList;
		hb_buffer_destroy(buffer);
		return glyph_count;
	}
	void	ParaLayout::set_text(const std::u16string& text)
	{
		_text = text;



#if defined(_WIN32) && defined(_DEBUG)
		const char16_t* str = (char16_t*)&text[0];
		int length = text.length();
#endif
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
		if (line_no >= _line_list.size())
		{
			return _text.length();
		}
		const text_line * line = &_line_list[line_no];

#if defined(_DEBUG) && defined(_WIN32)/*ony for debug*/
		std::wstring strLine(_text.c_str() + line->_start, _text.c_str() + line->_end);
#endif
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
		std::vector<text_line>::const_iterator itor = _line_list.cbegin();
		int line_no = 0;
		if (char_pos + (int)trailling == _text.length())
		{
			point->y = _line_list[_line_list.size() - 1]._width;
			point->x = (_line_list.size() - 1) * fontOption->LineHeight();
			return true;
		}
		for (; itor < _line_list.end() && itor->_end <= char_pos;
			++itor, ++line_no);
				
		if (itor != _line_list.end() && itor->_end > char_pos && itor->_start <= char_pos)
		{
			point->y = get_chars_width(itor->_start, char_pos + (int)trailling);
			point->x = line_no * fontOption->LineHeight();

			return true;
		}
		return false;
	}

	void	ParaLayout::draw(unsigned int* buffer, int width, int height, int x, int y)
	{
		std::vector<text_line>::iterator itor = _line_list.begin();
		int buff_size = width * height;
		int l = 0;
		int descender = fontOption->Descender();
		for (; itor != _line_list.end(); ++itor)
		{
				
			draw_chars(buffer, width, height, itor->_start, itor->_end, x + l - descender, y);
				
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
		对于蒙文等字形相对于行没有旋转的run应该使用LTB方向设置harfbuzz,并旋转glyph
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
			const hb_direction_t dir = glyph_ptr->run->dir();
			const int fontIndex = fontOption->font_index_from_script(glyph_ptr->run->script);
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
				int des = fontOption->ft_face[FontOption::MONGOL]->size->metrics.descender >> 6;
				int des2 = fontOption->ft_face[FontOption::OTHER]->size->metrics.descender >> 6;
				int fit_height = (fontOption->ft_face[FontOption::OTHER]->size->metrics.ascender - fontOption->ft_face[FontOption::OTHER]->size->metrics.descender) >> 6;
				offset_x = (*itor).x_offset / 64 - des2;
			}
			else
			{
				std::swap(offset_y, offset_x);
			}

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
				pen_x + bit_left + offset_x,
				pen_y - bit_top - offset_y, fontOption->fore, fontOption->back);

			if (dir != HB_DIRECTION_TTB)
				delete[] newBmp.buffer;

			FT_Done_Glyph(bmp_glyph);
#ifdef _DEBUG
			for (int k = 0; k < 25 && pen_x + k < width; k++)
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