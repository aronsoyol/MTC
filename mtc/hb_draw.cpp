#include <vector>
#include <algorithm>
#include <cassert>
#include "util.h"

namespace MTC{	namespace Util{
char bw  ;

float cal_table[] =
{ 
	0.00, 0.01, 0.01, 0.02, 0.02, 0.02, 0.03, 0.03, 0.04, 0.04, 0.04, 0.05, 0.05, 0.05, 0.06, 0.06,
	0.07, 0.07, 0.07, 0.08, 0.08, 0.09, 0.09, 0.09, 0.10, 0.10, 0.11, 0.11, 0.11, 0.12, 0.12, 0.13,
	0.13, 0.13, 0.14, 0.14, 0.14, 0.15, 0.15, 0.16, 0.16, 0.16, 0.17, 0.17, 0.18, 0.18, 0.18, 0.19,
	0.19, 0.20, 0.20, 0.20, 0.21, 0.21, 0.21, 0.22, 0.22, 0.23, 0.23, 0.23, 0.24, 0.24, 0.25, 0.25,
	0.25, 0.26, 0.26, 0.27, 0.27, 0.27, 0.28, 0.28, 0.29, 0.29, 0.29, 0.30, 0.30, 0.30, 0.31, 0.31,
	0.32, 0.32, 0.32, 0.33, 0.33, 0.34, 0.34, 0.34, 0.35, 0.35, 0.36, 0.36, 0.36, 0.37, 0.37, 0.38,
	0.38, 0.38, 0.39, 0.39, 0.39, 0.40, 0.40, 0.41, 0.41, 0.41, 0.42, 0.42, 0.43, 0.43, 0.43, 0.44,
	0.44, 0.45, 0.45, 0.45, 0.46, 0.46, 0.46, 0.47, 0.47, 0.48, 0.48, 0.48, 0.49, 0.49, 0.50, 0.50,
	0.50, 0.51, 0.51, 0.52, 0.52, 0.52, 0.53, 0.53, 0.54, 0.54, 0.54, 0.55, 0.55, 0.55, 0.56, 0.56,
	0.57, 0.57, 0.57, 0.58, 0.58, 0.59, 0.59, 0.59, 0.60, 0.60, 0.61, 0.61, 0.61, 0.62, 0.62, 0.63,
	0.63, 0.63, 0.64, 0.64, 0.64, 0.65, 0.65, 0.66, 0.66, 0.66, 0.67, 0.67, 0.68, 0.68, 0.68, 0.69,
	0.69, 0.70, 0.70, 0.70, 0.71, 0.71, 0.71, 0.72, 0.72, 0.73, 0.73, 0.73, 0.74, 0.74, 0.75, 0.75,
	0.75, 0.76, 0.76, 0.77, 0.77, 0.77, 0.78, 0.78, 0.79, 0.79, 0.79, 0.80, 0.80, 0.80, 0.81, 0.81,
	0.82, 0.82, 0.82, 0.83, 0.83, 0.84, 0.84, 0.84, 0.85, 0.85, 0.86, 0.86, 0.86, 0.87, 0.87, 0.88,
	0.88, 0.88, 0.89, 0.89, 0.89, 0.90, 0.90, 0.91, 0.91, 0.91, 0.92, 0.92, 0.93, 0.93, 0.93, 0.94,
	0.94, 0.95, 0.95, 0.95, 0.96, 0.96, 0.96, 0.97, 0.97, 0.98, 0.98, 0.98, 0.99, 0.99, 1.00, 1.00 
};

bool Rotate90Degree(const FT_Bitmap *src, FT_Bitmap *dest)
{
	//if (src->pixel_mode=)
	int size = src->width * src->rows;
	
	dest->buffer = new unsigned char[size];
	int height = src->width;
	int width = src->rows;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			assert(i * width + j < size);
			int index = (src->rows - 1 - j) * src->width + i;
			assert(index < size && index >= 0);
			dest->buffer[i * width + j] = src->buffer[index];
		}
	}
	dest->rows = src->width;
	dest->width = src->rows;
	return true;
}
void FreeTypeDrawBitmap256(unsigned int * buffer, int width, int height, DRAW_MODE mode, FT_Bitmap *bitmap, int x, int y, unsigned fore, unsigned back)
{
	int x_max = bitmap->width;
	int y_max = bitmap->rows;
	if (x_max == 0 || y_max == 0)
		return;
	//int x, y;
	unsigned int fontColor = fore;
	unsigned int backgroundColor = back;// GetSysColor(COLOR_WINDOWFRAME);

	unsigned int fontColorR = ((fontColor & 0x00FF0000) >> 16);
	unsigned int fontColorG = ((fontColor & 0x0000FF00) >> 8);
	unsigned int fontColorB = (fontColor & 0x000000FF);

	unsigned int backgroundColorR = ((backgroundColor & 0x00FF0000) >> 16);
	unsigned int backgroundColorG = ((backgroundColor & 0x0000FF00) >> 8);
	unsigned int backgroundColorB = (backgroundColor & 0x000000FF);

	unsigned int aR = fontColorR - backgroundColorR;
	unsigned int aG = fontColorG - backgroundColorG;
	unsigned int aB = fontColorB - backgroundColorB;

	float opacity;
	float opacity2;

	unsigned int grey_to_color[256];

	for (int i = 0; i < 256; i++)
	{
		int ii = 255 - i;

		unsigned r = cal_table[fontColorR] * i + ii * cal_table[backgroundColorR];

		unsigned g = cal_table[fontColorG] * i + ii * cal_table[backgroundColorG];

		unsigned b = cal_table[fontColorB] * i + ii * cal_table[backgroundColorB];

		grey_to_color[i] = TORGB(r, g, b);
		//grey_to_color[ii] = RGB(255-r, 255-g, 255-b);
	}
	int Ymax = std::min(height, y + y_max);
	int Xmax = std::min(width, x + x_max);
	int buffer_length = height * width;
	int start_cl = 0, row = 0;
	if (y < 0)
		row = -y;
	if (x < 0)
		start_cl = -x;

	int firstGrey = row * bitmap->width;
	int bmpBuffSize = bitmap->rows * bitmap->width;
	for (; firstGrey < bmpBuffSize && bitmap->buffer[firstGrey] == 0; firstGrey++)
	{
	}
	assert(bitmap->width > 0);
	row = firstGrey / bitmap->width;
	assert(row < bitmap->rows);
	for (; row < y_max && row + y < height; row++){        /* For each horizontal pixel..        */
		for (int cl = start_cl; cl < x_max && x + cl < width; cl++){    /* ...in each row of the font bitmap. */
			int grey = bitmap->buffer[row * x_max + cl];
#if defined(_WIN32)
			int yy = height - 1 - (y + row);
#else
			int yy = y + row;
#endif
			int xx = x + cl;
			int index = width * yy + xx;
			assert(yy >= 0 && yy < height);
			assert(xx >= 0 && xx < width);
			assert(index < buffer_length && index >= 0);
			//if (/*grey_to_color[grey] != back &&*/ buffer[index] == back)
			if (buffer[index] == back && grey == 0)
				continue;
			if (grey == 255 || buffer[index] == back)
				buffer[index] = grey_to_color[grey];
			else if (grey != 0 && buffer[index] != back)
			{
			//else
			//{
				unsigned int bR = ((buffer[index] & 0x00FF0000) >> 16);
				unsigned int bG = ((buffer[index] & 0x0000FF00) >> 8);
				unsigned int bB = (buffer[index] & 0x000000FF);

				unsigned int cR = ((grey_to_color[grey] & 0x00FF0000) >> 16);
				unsigned int cG = ((grey_to_color[grey] & 0x0000FF00) >> 8);
				unsigned int cB = (grey_to_color[grey] & 0x000000FF);

				unsigned int r = (bR + cR) / 2;
				unsigned int g = (bG + cG) / 2;
				unsigned int b = (bB + cB) / 2;

				buffer[index] = TORGB(r, g, b);
			}
		}
	}

	

}

void FreeTypeDrawBitmap(unsigned int* buffer, int width, int height, DRAW_MODE mode, FT_Bitmap *bitmap, int x, int y, unsigned fore, unsigned back)
{

	if (x >= width || x + bitmap->width < 0 || y >= height || y + bitmap->rows < 0)
		return;
	else
	{
		FreeTypeDrawBitmap256(buffer, width, height, mode, bitmap, x, y, fore, back);
	}

}

}}