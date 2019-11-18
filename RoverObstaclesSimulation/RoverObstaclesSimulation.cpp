// RoverObstaclesSimulation.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "RoverObstaclesSimulation.h"
#include "MyWindow.h"
#include "MainWindow.h"

HWND      texthwnd = 0;
// Forward declarations of functions included in this code module:
//ATOM				MyRegisterClass(HINSTANCE hInstance);
//BOOL				InitInstance(HINSTANCE, int);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	//MyRegisterClass(hInstance);
	
	MainWindow win;

    if (!win.Create(L"Learn to Program Windows", WS_OVERLAPPEDWINDOW, CS_HREDRAW | CS_VREDRAW))
    {
        return FALSE;
    }

	// Perform application initialization:
	/*if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}*/

	ShowWindow(win.Window(), nCmdShow);

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ROVEROBSTACLESSIMULATION));

	// Main message loop:
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//ATOM MyRegisterClass(HINSTANCE hInstance)
//{
//	WNDCLASSEX wcex;
//
//	wcex.cbSize = sizeof(WNDCLASSEX);
//
//	wcex.style			= CS_HREDRAW | CS_VREDRAW;
//	wcex.lpfnWndProc	= WndProc;
//	wcex.cbClsExtra		= 0;
//	wcex.cbWndExtra		= 0;
//	wcex.hInstance		= hInstance;
//	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ROVEROBSTACLESSIMULATION));
//	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.hbrBackground	= (HBRUSH)(COLOR_BTNSHADOW+1);
//	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_ROVEROBSTACLESSIMULATION);
//	wcex.lpszClassName	= szWindowClass;
//	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
//
//	return RegisterClassEx(&wcex);
//}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
//BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//{
//   HWND hWnd;
//
//   hInst = hInstance; // Store instance handle in our global variable
//
//   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
//      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
//
//   if (!hWnd)
//   {
//      return FALSE;
//   }
// 
//   ShowWindow(hWnd, nCmdShow);
//   UpdateWindow(hWnd);
//
//   return TRUE;
//}

void OnCreate(HWND hParent)
{
 
	RECT rect;
	int width, height;
	if(GetWindowRect(hParent, &rect))
	{
		 width = rect.right - rect.left;
		 height = rect.bottom - rect.top;
	}

  texthwnd = CreateWindowEx(0, __T("STATIC"), __T("RAM: 99%"),
	  WS_CHILD | WS_VISIBLE | SS_SUNKEN,
	  10, 10, width-50, height - 50, hParent, 0, 0, 0);


  HDC hdc = GetDC(texthwnd);
  SetBkColor(hdc, RGB(222,231,249));
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	int wmId, wmEvent;
//	PAINTSTRUCT ps;
//	HDC hdc;
//
//	switch (message)
//	{
//	case WM_COMMAND:
//		wmId    = LOWORD(wParam);
//		wmEvent = HIWORD(wParam);
//		// Parse the menu selections:
//		switch (wmId)
//		{
//		case IDM_ABOUT:
//			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
//			break;
//		case IDM_EXIT:
//			DestroyWindow(hWnd);
//			break;
//		default:
//			return DefWindowProc(hWnd, message, wParam, lParam);
//		}
//		break;
//	case WM_CREATE:
//		OnCreate(hWnd);
//	
//	case WM_PAINT:
//		hdc = BeginPaint(texthwnd, &ps);
//		//// TODO: Add any drawing code here...
//		//graphicsManager->FieldPaint(texthwnd);
//		EndPaint(texthwnd, &ps);
//		hdc = BeginPaint(hWnd, &ps);
//		// TODO: Add any drawing code here...
//		EndPaint(hWnd, &ps);
//		
//		break;
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		break;
//	default:
//		return DefWindowProc(hWnd, message, wParam, lParam);
//	}
//	return 0;
//}

// Message handler for about box.
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
