// OpencvDemo.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "OpencvDemo.h"
#include "RecognizeColor.h"
#include "EdgeDetection.h"

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

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OPENCVDEMO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OPENCVDEMO));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OPENCVDEMO));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	/*wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_OPENCVDEMO);*/
    wcex.lpszMenuName	= NULL;
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

   int sWidth = GetSystemMetrics(SM_CXSCREEN);
   int sHeight = GetSystemMetrics(SM_CYSCREEN);

   int width = 300;
   int height = 200;

   int x = (sWidth - width) / 2.0f;
   int y = (sHeight - height) / 2.0f;

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      x, y, width, height, NULL, NULL, hInstance, NULL);

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
    case WM_SYSCOMMAND:
        if (wParam == SC_MAXIMIZE)
            return 0;
        else
            return DefWindowProc(hWnd, message, wParam, lParam);
        break;
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
        case 10001:
            {
                Mat source = imread("./test.jpg");
                namedWindow("原图", WINDOW_NORMAL);
                imshow("原图", source);

                RecognizeColor *pRecgColor = new RecognizeColor();
               
                Mat reslut = pRecgColor->recoginze(source, Red);
                namedWindow("结果", WINDOW_NORMAL);
                imshow("结果", reslut);
                delete pRecgColor;

            }
            break;
        case 10002:
            {
                Mat source = imread("./test.jpg");
                namedWindow("原图", WINDOW_NORMAL);
                imshow("原图", source);

                EdgeDetection *pEdge = new EdgeDetection();
                Mat result = pEdge->canny(source);
                namedWindow("算子1", WINDOW_NORMAL);
                imshow("算子1", result);

                result = pEdge->laplacian(source);
                namedWindow("算子2", WINDOW_NORMAL);
                imshow("算子2", result);

                result = pEdge->sobel(source);
                namedWindow("算子3", WINDOW_NORMAL);
                imshow("算子3", result);
                delete pEdge;
            }
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
    case WM_CREATE:
        {
            CreateWindow(
                _TEXT("button"),
                _TEXT("色块检测"), 
                WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
                100, 20, 80, 40,
                hWnd,
                (HMENU)10001,
                hInst,
                NULL);

            CreateWindow(
                _TEXT("button"),
                _TEXT("边缘检测"), 
                WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
                100, 80, 80, 40,
                hWnd,
                (HMENU)10002,
                hInst,
                NULL);
        }
        break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
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
