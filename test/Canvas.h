#ifndef __MTC_CANVAS_H
#define __MTC_CANVAS_H
#include "noncopyable.hpp"
#include <stdio.h>
namespace  MTC{namespace Canvas{
	class ImplCanvas : Util::noncopyable
	{
		int _height;
		int _width;
		unsigned int *buffer;

		ImplCanvas(int width, int height) :_width(width), _height(height)
		{
			buffer = new unsigned int[_width * _height];
			printf("canvas create\n");
		}
	public:
		static ImplCanvas * Create(int width, int height)
		{
			return new ImplCanvas(width, height);
		}

		static void Delete(ImplCanvas*canvas)
		{
			delete canvas;
		}
		virtual ~ImplCanvas()
		{
			delete[] buffer;
			printf("canvas delete\n");
		}
	};

	class Canvas :Util::noncopyable
	{
		ImplCanvas * canvas_;
	public:
		Canvas(int width, int height)
		{
			canvas_ = ImplCanvas::Create(width, height);
		}
		virtual ~Canvas()
		{
			ImplCanvas::Delete(canvas_);
		}
	};
}}
#endif /*MTC_CANVAS_H*/