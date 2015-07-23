// test.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "test.h"
#include "hb_draw.h"
#include "scrptrun.h"
#include "ParaLayout.h"
#include "Canvas.h"
#include <hb-ft.h>
#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
wchar_t *text = L"ᠴᠢᠭᠤᠯᠭᠠᠨ?";
//wchar_t *text = L"ᠲᠦᠪ";

MTC::LayoutEngine::ParaLayout *pLayout;
MTC::Util::FontOption *pFontOption;
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	
	int ret = 0;
	{
		MTC::Util::FontOption fo(50, RGB(0, 0, 0), RGB(255, 255, 255));
		MTC::LayoutEngine::ParaLayout layout(&fo);
		pLayout = &layout;
		pFontOption = &fo;

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TEST, szWindowClass, MAX_LOADSTRING);

	layout.set_text(text, lstrlen(text));

	//layout.set_text(text, lstrlen(text));

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
		pLayout->break_line(height-100);
		return 0;
	}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		{
			//int c= toupper((int)'c');
			//wchar_t *text = L"2015 インタラクティブ、プレイできるゲーム、再生できる動画、遊べるおもちゃをすべて確認します。据新华社电昨天，金砖国家领导人第七次会晤在俄罗斯乌法举行。中国国家主席习近平、俄罗斯总统普京、巴西总统罗塞夫、印度总理莫迪、南非总统祖马出席。习近平在题为《共建伙伴关系共创美好未来》的主旨讲话中，就加强金砖国家伙伴关系提出四点主张。";
			//ScriptRunIterator runIter(text, 0, lstrlen(text));
			//int y = 0, x = 50;
			//while (runIter.next())
			//{
			//	int32_t     start	= runIter.getScriptStart();
			//	int32_t     end		= runIter.getScriptEnd();
			//	UScriptCode code	= runIter.getScriptCode();
			//	wchar_t subText[1024];
			//	int i = 0;
			//	x += 50;
			//	for (; i < end - start; i++)
			//	{
			//		subText[i] = text[start + i];
			//	}
			//	subText[i] = '\0';
			//	//TextOut(ps.hdc, 10, y, subText, lstrlen(subText));
			//	fontOption.fore = RGB(0, 0, 0);
			//	fontOption.back = RGB(255, 0, 0);
			//	HBDrawTextW(ps.hdc, DARW_MODE_TRANSPARENT, &fontOption, x, 50, subText, code);
			//}
			RECT rect;

			GetClientRect(hWnd, &rect);
			/*
			HBITMAP bmp = CreateCompatibleBitmap(ps.hdc, rect.right - rect.left, rect.bottom - rect.top);
			HDC memdc = CreateCompatibleDC(ps.hdc);
			HBRUSH brush = CreateSolidBrush(RGB(255,255,255));

			SelectObject(memdc, bmp);
			*/
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;
			int buff_size = width * height;
			if (buff_size != 0)
			{
			
				std::vector<unsigned int> bitmap_buffer;
				bitmap_buffer.assign(buff_size, 0xffffff);

				BITMAPINFO bmpInfo;

				//DIBの情報を設定する
				bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmpInfo.bmiHeader.biWidth = width;
				bmpInfo.bmiHeader.biHeight = height;
				bmpInfo.bmiHeader.biPlanes = 1;
				bmpInfo.bmiHeader.biBitCount = 32;
				bmpInfo.bmiHeader.biCompression = BI_RGB;

				//memset(bitmap_buffer, 0xFF , buff_size * sizeof(DWORD));

				//layout.vdraw(ps.hdc, 0, 10);
				int base_line = 50;

				for (int i = 0; i < width; i++)
				{
					bitmap_buffer[(height -1 - 50) * width + i] = 0;
					bitmap_buffer[50 * width + i] = 0;
				}
					
				pLayout->draw(&bitmap_buffer[0], width, height, 50, base_line);
				
				for (int i = 0; i < width; i++)
				{
					bitmap_buffer[(height - 1) * width + i] = 0;
					bitmap_buffer[i] = 0;
				}

				for (int i = 0; i < height; i++)
				{
					bitmap_buffer[(height - 1 - i) * width] = 0;
					bitmap_buffer[(height - 1 - i) * width + 50] = 0;
					bitmap_buffer[(height - 1 - i) * width + width - 1] = 0;
					assert((height - 1 - i) * width + width - 1 < buff_size);
					assert((height - 1 - i) * width + 500);
				}

				int ret = SetDIBitsToDevice(ps.hdc, 0, 0, width, height, 0, 0, 0, height, &bitmap_buffer[0], &bmpInfo, DIB_RGB_COLORS);

				//delete[] bitmap_buffer;
			}
			//HBDrawTextW(ps.hdc, 50, y, text);
			//HBDrawTextB(ps.hdc, 50, 200);
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
