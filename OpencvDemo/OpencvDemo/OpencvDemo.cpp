// OpencvDemo.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "OpencvDemo.h"
#include "RecognizeColor.h"
#include "EdgeDetection.h"

#define MAX_LOADSTRING 100

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������:
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

 	// TODO: �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OPENCVDEMO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OPENCVDEMO));

	// ����Ϣѭ��:
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
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
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
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

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
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
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
		// �����˵�ѡ��:
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
                namedWindow("ԭͼ", WINDOW_NORMAL);
                imshow("ԭͼ", source);

                RecognizeColor *pRecgColor = new RecognizeColor();
               
                Mat reslut = pRecgColor->recoginze(source, Red);
                namedWindow("���", WINDOW_NORMAL);
                imshow("���", reslut);
                delete pRecgColor;

            }
            break;
        case 10002:
            {
                Mat source = imread("./test.jpg");
                namedWindow("ԭͼ", WINDOW_NORMAL);
                imshow("ԭͼ", source);

                EdgeDetection *pEdge = new EdgeDetection();
                Mat result = pEdge->canny(source);
                namedWindow("����1", WINDOW_NORMAL);
                imshow("����1", result);

                result = pEdge->laplacian(source);
                namedWindow("����2", WINDOW_NORMAL);
                imshow("����2", result);

                result = pEdge->sobel(source);
                namedWindow("����3", WINDOW_NORMAL);
                imshow("����3", result);
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
                _TEXT("ɫ����"), 
                WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
                100, 20, 80, 40,
                hWnd,
                (HMENU)10001,
                hInst,
                NULL);

            CreateWindow(
                _TEXT("button"),
                _TEXT("��Ե���"), 
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
		// TODO: �ڴ���������ͼ����...
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

// �����ڡ������Ϣ�������
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
