// test.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "test.h"
//#include "ParaLayout.h"
//#include "Canvas.h"
//#include <hb-ft.h>
//#include "util.h"
#include "mtc.h"
#include <string>
#include <vector>
#include <png.h>
#include <pngstruct.h>
#include <pnginfo.h>
#include <jni.h>

#define MAX_LOADSTRING 100
#define PADDING 5
// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

wchar_t *text = L"ᠪᠢᠳᠡ ᠪᠣᠯ ᠬᠥᠮᠦᠨ ᠲᠥᠷᠦᠯᠬᠢᠲᠡᠨ ᠦ ᠰᠢᠨ᠎ᠡ ᠲᠡᠦᠬᠡ ᠶᠢ 今日は中华人asdfasdasdf民共和国 ᠨᠡᠭᠡᠭᠡᠵᠦ ᠪᠠᠢᠭ᠎ᠠ ᠭᠡᠷᠡᠯ ᠰᠠᠴᠤᠷᠠᠩᠭᠤᠢ ᠴᠠᠭ ᠦᠶ᠎ᠡ ᠳᠦ ᠠᠮᠢᠳᠤᠷᠠᠵᠤ ᠪᠠᠢᠭ᠎ᠠ ᠶᠤᠮ᠃ ᠡᠨᠡ ᠬᠦ ᠴᠠᠭ ᠦᠶ᠎ᠡ ᠳᠦ ᠮᠠᠰᠢ ᠣᠯᠠᠨ ᠬᠡᠷᠡᠭ ᠪᠣᠳᠠᠰ ᠲᠤ ᠬᠥᠮᠦᠰ ᠰᠢᠨ᠎ᠡ ᠪᠣᠳᠤᠯ ᠰᠢᠨ᠎ᠡ ᠲᠥᠷᠦᠭᠳᠡᠯ ᠣᠷᠤᠵᠤ ᠪᠠᠢᠨ᠎ᠠ᠃ ᠲᠤᠶᠠᠭ᠎ᠠ ᠰᠠᠴᠤᠷᠠᠭᠤᠯᠤᠭᠰᠠᠨ ᠤᠷᠭᠤᠬᠤ ᠨᠠᠷᠠ᠂ ᠮᠥᠩᠬᠡ ᠨᠣᠭᠤᠭᠠᠨ ᠨᠠᠷᠠᠰᠤ ᠮᠠᠢᠯᠠᠰᠤ᠂ ᠰᠡᠬᠦᠢᠨ ᠰᠦᠨᠳᠡᠷᠯᠡᠭᠰᠡᠨ ᠬᠠᠳᠠ ᠣᠷᠭᠢᠯ᠂ ᠣᠢᠯᠠᠰᠤᠨ ᠳᠣᠯᠭᠢᠶᠠᠯᠠᠭᠰᠠᠨ ᠠᠨᠠᠪᠠᠳ ᠳᠠᠯᠠᠢ ᠶᠢ ᠴᠥᠭᠡᠨ ᠪᠤᠰᠤ ᠬᠥᠮᠦᠰ ᠮᠠᠭᠲᠠᠨ ᠳᠠᠭᠤᠯᠠᠵᠤ ᠪᠠᠢᠬᠤ ᠪᠢᠰᠢ ᠦᠦ？ ᠢᠳᠡᠷᠡᠭᠦᠨ ᠰᠦᠷᠯᠢᠭ ᠴᠠᠭᠠᠨ ᠤᠯᠢᠶᠠᠰᠤ᠂ ᠭᠡᠭᠡᠨ ᠰᠠᠷᠠᠭᠤᠯ ᠳ᠋ᠧᠩ ᠦᠨ ᠭᠡᠷᠡᠯ᠂ ᠠᠭᠰᠤᠮᠨᠠᠨ ᠳᠠᠪᠬᠢᠬᠤ ᠲᠡᠷᠭᠡ᠂ ᠴᠣ᠋ ᠰᠢᠨ᠎ᠡ ᠤᠯᠠᠷᠢᠯ ᠤᠨ ᠪᠢᠴᠢᠭ᠌ ᠢ ᠴᠥᠭᠡᠨ ᠪᠤᠰᠤ ᠬᠥᠮᠦᠰ ᠪᠠᠬᠠᠳᠤᠨ ᠱᠠᠭᠰᠢᠵᠤ ᠪᠠᠢᠬᠤ ᠪᠢᠰᠢ ᠦᠦ？ ᠶᠠᠭᠤᠮᠠᠨ ᠠᠴᠠ ᠨᠢ ᠬᠥᠮᠦᠨ ᠢ ᠨᠢ ᠳᠤᠷᠠᠰᠤᠨ᠎ᠠ ᠭᠡᠭᠴᠢ ᠪᠡᠷ ᠡᠳᠡᠭᠡᠷ ᠵᠦᠢᠯᠡᠰ ᠨᠢ ᠬᠥᠮᠦᠰ ᠲᠦ ᠬᠡᠴᠢᠨᠡᠨ ᠡᠯᠪᠡᠭ ᠪᠠᠶᠠᠯᠢᠭ᠂ ᠭᠦᠨ ᠰᠡᠳᠭᠢᠯᠲᠦ ᠲᠠᠭᠠᠮᠠᠭᠯᠠᠯ ᠢ ᠲᠥᠷᠦᠭᠳᠡᠭᠦᠯᠦᠭᠰᠡᠨ ᠦᠭᠡᠢ ᠪᠤᠢ？";
// L"例えばDLLの初期化でjvmを起動し、アンロード時にDestoryJavaVM()を呼ぶような作り方が多いと思う。何も悪くないプログラムだが、もしもFreeLibrary( dllHandle ); のようなとこでフリーズしていたら、jvm->DestoryJavaVM()でフリーズしていないか調べてみよう。";
void *pLayout;
void *font;
void readpng(LPBYTE* png, int *width, int *height);
void DrawBitmap(HDC hdc, HBITMAP hBmp)
{
	HBITMAP	hBmpPrev;
	HDC	hDCBmp;


	hDCBmp = CreateCompatibleDC(hdc);
	hBmpPrev = (HBITMAP)SelectObject(hDCBmp, hBmp);
	BitBlt(hdc, 0, 0, 100, 100, hDCBmp, 0, 0, SRCCOPY);
	SelectObject(hDCBmp, hBmpPrev);
	DeleteDC(hDCBmp);

}
HBITMAP LoadPng(const std::wstring &strFilename);
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	
	int ret = 0;
	{
		int p = sizeof(jlong);
		p&= 0x80000000;

		//MTC::Util::FontOption fo(50, 0x000000, RGB(255, 255, 255));
		//MTC::LayoutEngine::ParaLayout layout(&fo);
		//pLayout = &layout;
		//pFontOption = &fo;
		font = mtc_font_create(40, 0x000000, RGB(255, 255, 255));
		pLayout = mtc_create_layout(font);

		UNREFERENCED_PARAMETER(hPrevInstance);
		UNREFERENCED_PARAMETER(lpCmdLine);
		//readpng();
 		// TODO: 在此放置代码。
		MSG msg;
		HACCEL hAccelTable;

		// 初始化全局字符串
		LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		LoadString(hInstance, IDC_TEST, szWindowClass, MAX_LOADSTRING);

		mtc_set_text(pLayout, (char16_t*)text, lstrlen(text));
		//layout.set_text((char16_t*)text);

		MyRegisterClass(hInstance);

		// 执行应用程序初始化:
		if (!InitInstance (hInstance, nCmdShow))
		{
			return FALSE;
		}

		hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEST));

		// 主消息循环:
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		ret = (int)msg.wParam;
		mtc_destroy_layout(pLayout);
		mtc_font_destroy(font);
	}
	_CrtDumpMemoryLeaks();
	return ret;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)0;// GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= 0;// MAKEINTRESOURCE(IDC_TEST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	std::vector<unsigned int> bitmap_buffer;
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SIZE:
	{
		int height = HIWORD(lParam);
		mtc_break_line(pLayout, height - 2 * PADDING);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		int  mx = (short)LOWORD(lParam) - PADDING, my = (short)HIWORD(lParam) - PADDING;
		unsigned char trailling = 0;
		int pos = mtc_get_char_position(pLayout, mx, my, (unsigned char*)&trailling);
	   
		int x, y;
		mtc_get_char_location(pLayout, pos, trailling, &x, &y);
		SetCaretPos(x + PADDING, y + PADDING);
		return 0;
	}
	case  WM_SETFOCUS:
	{
						 
		CreateCaret(hWnd, 0, mtc_font_line_height(font), 2);
		SetCaretPos(0, 0);
		ShowCaret(hWnd);
		return 0;
	}
	case WM_KILLFOCUS:
	{
		HideCaret(hWnd);
		DestroyCaret();
	}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		{

			RECT rect;

			GetClientRect(hWnd, &rect);

			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;
			int buff_size = width * height;
			if (buff_size != 0)
			{
				bitmap_buffer.assign(buff_size, 0xffffff);

				BITMAPINFO bmpInfo;

				//DIBの情報を設定する
				bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmpInfo.bmiHeader.biWidth = width;
				bmpInfo.bmiHeader.biHeight = height;
				bmpInfo.bmiHeader.biPlanes = 1;
				bmpInfo.bmiHeader.biBitCount = 32;
				bmpInfo.bmiHeader.biCompression = BI_RGB;

				int base_line = 50;
				mtc_draw(pLayout, &bitmap_buffer[0], width, height, PADDING, PADDING);

				HBITMAP bmp = LoadPng(L"face/f000.png");
				
				int ret = SetDIBitsToDevice(ps.hdc, 0, 0, width, height, 0, 0, 0, height, &bitmap_buffer[0], &bmpInfo, DIB_RGB_COLORS);
				DrawBitmap(ps.hdc, bmp);
				DeleteObject(bmp);

			}

		}
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
void readpng(LPBYTE* png, int *width, int *height)
{
	// ファイルを開く
	FILE *fp;
	::fopen_s(&fp, "face/f000.png", "rb");

	png_structp pPng;
	png_infop pInfo;
	DWORD dwWidth, dwHeight;
	int nBitDepth, nColorType, nInterlaceType;

	// ここからファイルを読み込み
	pPng = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		NULL, NULL, NULL);
	pInfo = png_create_info_struct(pPng);
	png_init_io(pPng, fp);
	png_read_info(pPng, pInfo);
	png_get_IHDR(pPng, pInfo, (png_uint_32 *)&dwWidth, (png_uint_32 *)&dwHeight,
		&nBitDepth, &nColorType, &nInterlaceType,
		NULL, NULL);
	// ここまでで必要な情報が一通り取得できます

	// ここからPngファイルの情報を元にBitmapファイル用の情報を作成
	//BITMAPFILEHEADER bf;
	//BITMAPINFOHEADER bi;

	//bi.biSize = sizeof(BITMAPINFOHEADER);
	*width = pInfo->width;
	*height = pInfo->height;
	//bi.biPlanes = 1;
	//bi.biBitCount = pInfo->pixel_depth;
	//bi.biCompression = BI_RGB;
	int biSizeImage = pInfo->height * (((pInfo->width * pInfo->pixel_depth / 8) + 3) & ~3);
	//bi.biClrUsed = 0;

	//bf.bfType = ('M' << 8) | 'B';
	//bf.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bi.biSizeImage;
	//bf.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	// ここまででBitmapの必要な情報が作成される

	png_set_bgr(pPng);

	BYTE *byBmp = (BYTE*)::malloc(biSizeImage);
	*png = byBmp;
	// 必要そうなチャンクの読み込み処理
	// おまじないみたいなもの
#define screen_gamma 2.2
	double image_gamma;
	png_color_16p image_background;

	int intent;
	if (png_get_sRGB(pPng, pInfo, &intent))
	{
		png_set_gamma(pPng, screen_gamma, 0.45455);
	}
	else
	if (png_get_gAMA(pPng, pInfo, &image_gamma))
	{
		png_set_gamma(pPng, screen_gamma, image_gamma);
	}
	if ((pInfo->color_type & PNG_COLOR_MASK_ALPHA)
		|| png_get_valid(pPng, pInfo, PNG_INFO_tRNS))
	{
		if (png_get_bKGD(pPng, pInfo, &image_background))
		{
			png_set_background(pPng, image_background,
				PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
		}
		else
		{
			png_color_16 my_background = { 0, 0, 0, 0, };
			png_set_background(pPng, &my_background,
				PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);
		}
	}

	// ここでパレット登録
	int number_passes;

	number_passes = png_set_interlace_handling(pPng);
	png_read_update_info(pPng, pInfo);

	//if (pInfo->color_type == PNG_COLOR_TYPE_PALETTE)
	{
		int num_palette;
		png_colorp palette;

		png_get_PLTE(pPng, pInfo, &palette, &num_palette);

		for (int i = 0; i < num_palette; i++)
		{
			byBmp[i * 4 + 0] = palette[i].blue;
			byBmp[i * 4 + 1] = palette[i].green;
			byBmp[i * 4 + 2] = palette[i].red;
		}
	}

	// で、ここから実データの処理
	int row_size;
	int image_size;
	int i, j;

	row_size = (png_get_rowbytes(pPng, pInfo) + 3) & ~3;
	image_size = row_size * pInfo->height;

	BYTE *pRow;
	for (i = 0; i < number_passes; i++)
	{
		for (j = pInfo->height - 1; j >= 0; j--)
		{
			pRow = &byBmp[j*row_size];
			png_read_row(pPng, pRow, NULL);
		}
	}
	png_read_end(pPng, NULL);

	// ファイルを閉じる
	::fclose(fp);

	//// ここまでで byBmp に Bitmap画像としてデータが入ります
	//// あとは好きなように処理できます
	//// とりあえず、画像として保存してみたり
	//::fopen_s(&fp, "a.bmp", "wb");

	//size_t size = ::fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, fp);
	//size = ::fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, fp);
	//size = ::fwrite(byBmp, sizeof(BYTE), bi.biSizeImage, fp);

	//::fclose(fp);

	//// ビットマップハンドルに渡してみたり
	//BITMAPINFO bmpInfo;
	//bmpInfo.bmiHeader = bi;

	//LPBYTE lpBuf;
	//// ビットマップハンドル取得
	//HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmpInfo,
	//	DIB_RGB_COLORS,
	//	(void**)&lpBuf, NULL, 0);
	//for (int i = 0; i < (int)bi.biSizeImage; i++)
	//	lpBuf[i] = byBmp[i];

	// と、いろいろできるかも

	// 確保したメモリの開放は忘れずに
	::free(byBmp);
}
//---------------------------------------------------------------------------
// LoadPng用ファイル読み込みプロシージャ
void PngReadProc(png_structp png_ptr, png_bytep data, png_size_t length) {
	ReadFile(png_get_io_ptr(png_ptr), data, length, (DWORD*)&length, NULL);
}

//---------------------------------------------------------------------------
// LoadPng用4色形式→16色形式変換関数
void to4bpp(png_structp png_ptr, png_row_infop row_info, png_bytep data) {
	static const png_byte pix[] = {
		0x00, 0x01, 0x02, 0x03, 0x10, 0x11, 0x12, 0x13,
		0x20, 0x21, 0x22, 0x23, 0x30, 0x31, 0x32, 0x33,
	};
	png_uint_32 rowb;
	png_byte *p, *q, c;

	rowb = (row_info->width + 1) / 2;
	q = data + rowb;
	p = data + rowb / 2;

	if (rowb % 2 == 1) {
		c = *p;
		*(--q) = pix[c >> 4];
	}
	while (p > data) {
		c = *(--p);
		*(--q) = pix[c & 0x0f];
		*(--q) = pix[c >> 4];
	}
	row_info->bit_depth = 4;
	row_info->pixel_depth = 4;
	row_info->rowbytes = rowb;
}

//---------------------------------------------------------------------------
// PNGファイルを読み込む
HBITMAP LoadPng(const std::wstring &strFilename) {
	HANDLE hFile = CreateFile(strFilename.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return NULL;

	// PNG読み込み開始
	png_struct *png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		CloseHandle(hFile);
		return NULL;
	}

	png_info *info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		CloseHandle(hFile);
		return NULL;
	}
	png_info *end_info = png_create_info_struct(png_ptr);
	if (!end_info) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		CloseHandle(hFile);
		return NULL;
	}

	png_set_read_fn(png_ptr, hFile, PngReadProc);
	png_uint_32 nWidth, nHeight;
	int nDepth, nPal;
	int nPngDepth, nColorType, nInterlaceType, nCompType, nFilterType;

	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &nWidth, &nHeight, &nPngDepth, &nColorType, &nInterlaceType, &nCompType, &nFilterType);

	if (nColorType == PNG_COLOR_TYPE_RGB || nColorType == PNG_COLOR_TYPE_RGB_ALPHA) {
		nPngDepth = 24;
		nDepth = 24;
		nPal = 0;
	}
	else {
		switch (nPngDepth) {
		case 2:  nDepth = 4; break;
		case 16: nDepth = 8; break;
		default: nDepth = nPngDepth; break;
		}
		nPal = 1 << nDepth;
	}

	std::vector <png_color> vPalette;
	if (nPal > 0)
		vPalette.resize(nPal);
	int nRowBytes = (nWidth * nDepth + 31) / 32 * 4;
	int nImgBytes = nRowBytes * nHeight;
	BYTE *pImgPtr = (BYTE*)GlobalAlloc(GMEM_FIXED, nImgBytes);
	std::vector<BYTE*> vRowPtr;
	vRowPtr.reserve(nHeight);
	for (int y = nHeight - 1; y >= 0; --y)
		vRowPtr.push_back(pImgPtr + y * nRowBytes);

	if (nColorType & PNG_COLOR_MASK_ALPHA)
		png_set_strip_alpha(png_ptr);
	if (nPngDepth == 2)
		png_set_read_user_transform_fn(png_ptr, to4bpp);
	else if (nPngDepth == 16)
		png_set_strip_16(png_ptr);
	if (nColorType == PNG_COLOR_TYPE_RGB || nColorType == PNG_COLOR_TYPE_RGB_ALPHA)
		png_set_bgr(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	if (nPal > 0) {
		if (nColorType == PNG_COLOR_TYPE_PALETTE) {
			png_color *palette;
			int num_palette;
			png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
			if (num_palette > nPal)
				num_palette = nPal;
			memset(&vPalette[0], 0, nPal * sizeof png_color);
			memcpy(&vPalette[0], palette, num_palette * sizeof png_color);
		}
		else {
			int depth = nPngDepth == 16 ? 8 : nPngDepth;
			png_build_grayscale_palette(depth, &vPalette[0]);
		}
	}

	png_read_image(png_ptr, &vRowPtr[0]);
	png_read_end(png_ptr, end_info);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

	CloseHandle(hFile);

	BITMAPINFO* bi = (BITMAPINFO*)GlobalAlloc(GMEM_FIXED, sizeof BITMAPINFOHEADER + nPal * sizeof RGBQUAD);

	memset(bi, 0, sizeof BITMAPINFOHEADER);
	bi->bmiHeader.biSize = sizeof BITMAPINFOHEADER;
	bi->bmiHeader.biWidth = nWidth;
	bi->bmiHeader.biHeight = nHeight;
	bi->bmiHeader.biPlanes = 1;
	bi->bmiHeader.biBitCount = nDepth;
	bi->bmiHeader.biCompression = BI_RGB;
	bi->bmiHeader.biSizeImage = nImgBytes;
	bi->bmiHeader.biClrUsed = nPal;

	for (int i = 0; i < nPal; ++i) {
		bi->bmiColors[i].rgbRed = vPalette[i].red;
		bi->bmiColors[i].rgbGreen = vPalette[i].green;
		bi->bmiColors[i].rgbBlue = vPalette[i].blue;
	}

	HWND hwnd = GetDesktopWindow();
	HDC hdc = GetDC(hwnd);
	char *pBits;
	HBITMAP hBitmap = CreateDIBSection(hdc, bi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
	if (pBits)
		memcpy(pBits, pImgPtr, nImgBytes);
	ReleaseDC(hwnd, hdc);

	GlobalFree(pImgPtr);
	GlobalFree(bi);

	return hBitmap;
}