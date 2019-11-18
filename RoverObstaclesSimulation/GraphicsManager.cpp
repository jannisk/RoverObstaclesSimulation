#include "stdafx.h"
#include "GraphicsManager.h"


GraphicsManager::GraphicsManager()
{
	m_pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	m_hPurpleBrush = CreateSolidBrush(RGB(255, 0, 255));
}

GraphicsManager::GraphicsManager(HWND hWnd)
{
	m_hWnd = hWnd;
	GraphicsManager();
	
}

void GraphicsManager::SetBkgColor(HWND hWindow)
{
	HDC hdc = GetDC(hWindow);
	SetBkColor(hdc, RGB(222,231,249));

}

void GraphicsManager::FieldPaint(HWND hWindow)
{
	HDC hdc = GetDC(hWindow);
	TextOut(hdc, 45, 45, TEXT("Michael Morrison"), 16);
}


GraphicsManager::~GraphicsManager()
{
}