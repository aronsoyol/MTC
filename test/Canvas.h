#ifndef __MTC_CANVAS_H
#define __MTC_CANVAS_H
#include "noncopyable.hpp"
#include <stdio.h>
namespace  MTC{namespace Canvas{
	class ImplCanvas : Util::noncopyable
	{
		int height_;
		int width_;
		unsigned int *buffer;

		ImplCanvas(int width, int height) :width_(width), height_(height)
		{
			buffer = new unsigned int[width_ * height_];
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