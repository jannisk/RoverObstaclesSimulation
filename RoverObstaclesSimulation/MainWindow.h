#pragma once
#include "MyWindow.h"
#include "Resource.h"
#include "FieldArea.h"
#define MAX_LOADSTRING 100

class MainWindow : public BaseWindow<MainWindow>
{
	// Global Variables:
	HINSTANCE hInst;								// current instance
	TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
	TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
	
	ID2D1Factory *pFactory;
    ID2D1HwndRenderTarget *pRenderTarget;
    ID2D1SolidColorBrush *pBrush;
	D2D1_POINT_2F m_ptMouse;
	D2D1_ELLIPSE  m_ellipse;


	HPEN m_pen;
	HBRUSH m_hPurpleBrush;
	FieldArea *m_fieldArea;
	int m_globalCommandId;

	void OnPaint(HDC, LPPAINTSTRUCT);
	void OnCreate(HWND);
	void OnLButtonDown(int pixelX, int pixelY, DWORD flags);
	void OnLButtonUp();;
	void OnMouseMove(int pixelX, int pixelY, DWORD flags);;
	
public:
	MainWindow();
	~MainWindow() {}
	PCWSTR  ClassName()  { return szWindowClass; }
	HICON   Icon() { return LoadIcon(hInst, MAKEINTRESOURCE(IDI_ROVEROBSTACLESSIMULATION)); }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	HRESULT CreateGraphicsResources();
	void DiscardGraphicsResources();
	void Resize();
};


