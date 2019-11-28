#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include "MainWindow.h"
#include "Resource.h"
#include "FieldArea.h"


float DPIScale::scaleX = 1.0f;
float DPIScale::scaleY = 1.0f;

MainWindow::MainWindow():BaseWindow(), 	m_ptMouse(D2D1::Point2F()), m_ellipse(D2D1::Ellipse(D2D1::Point2F(), 0, 0))
{
	hInst = GetModuleHandle(NULL);
	// Initialize global strings
	LoadString(hInst, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInst, IDC_ROVEROBSTACLESSIMULATION, szWindowClass, MAX_LOADSTRING);
	m_pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	m_hPurpleBrush = CreateSolidBrush(RGB(128, 0, 255));
	pFactory = NULL;
	pRenderTarget = NULL;
	pBrush = NULL;
}


LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HMENU hmenu;         // handle to main menu  
	
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_SIZE:
		Resize();
		return 0;
	case WM_CREATE:
		hmenu = GetMenu(m_hwnd); 
		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
		{
			return -1;  // Fail CreateWindowEx.
		}
		DPIScale::Initialize(pFactory);
		OnCreate(m_hwnd);
		return 0;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(m_hwnd, &ps);
			OnPaint(hdc, &ps);
		}
		return 0;
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDM_CREATE:
				CheckUnchekMenuItem((BYTE)LOWORD(wParam), hmenu);
				break;
			default:
				break;

			};
		}

		return 0;
	case WM_LBUTTONDOWN: 
		OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;

	case WM_LBUTTONUP: 
		OnLButtonUp();
		return 0;

	case WM_MOUSEMOVE: 
		OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;

	default:
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	}
	return TRUE;
}


/// <summary>
/// Checks Unchecks menu items
/// </summary>
/// <param name="bMenuItemID">The b menu item ID.</param>
/// <param name="hmenu">The hmenu.</param>
void MainWindow::CheckUnchekMenuItem(BYTE bMenuItemID, HMENU hmenu)
{  
	DWORD fdwMenu; 
	switch (bMenuItemID) 
	{ 
		case IDM_CREATE:
			fdwMenu = CheckMenuItem(hmenu, IDM_CREATE, MF_BYCOMMAND | MF_CHECKED); 
			if(!(fdwMenu & MF_CHECKED))
				m_globalCommandId = IDM_CREATE;
			else
			{
				CheckMenuItem(hmenu, (UINT) bMenuItemID, 
					MF_BYCOMMAND | MF_UNCHECKED); 
				m_globalCommandId = 0;
			}
			return;
	}
}



void MainWindow::OnPaint(HDC hdc,  LPPAINTSTRUCT ps)
{
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);
		pRenderTarget->BeginDraw();
		pRenderTarget->Clear( D2D1::ColorF(D2D1::ColorF::SkyBlue));
		m_fieldArea->DrawScanPoints();

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}
		EndPaint(m_hwnd, &ps);

	}
}

void MainWindow::OnCreate(HWND hwnd)
{
	
}

HRESULT MainWindow::CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	if (pRenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTarget);

		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0);
			hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);
		}

		m_fieldArea = new FieldArea(m_hwnd, pFactory, pRenderTarget);
		m_fieldArea->Create(m_hwnd);
	}
	return hr;
}

void MainWindow::DiscardGraphicsResources()
{
	SafeRelease(&pRenderTarget);
	SafeRelease(&pBrush);
}

void MainWindow::Resize()
{
	if (pRenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		pRenderTarget->Resize(size);

		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

void MainWindow::OnLButtonDown(int pixelX, int pixelY, DWORD flags)
{
	if (m_globalCommandId != IDM_CREATE) return;
	/*SetCapture(m_hwnd);
	m_ellipse.point = m_ptMouse = DPIScale::PixelsToDips(pixelX, pixelY);
	m_ellipse.radiusX = m_ellipse.radiusY = 1.0f; 
	InvalidateRect(m_hwnd, NULL, FALSE);*/
	m_fieldArea->OnLButtonDown(pixelX, pixelY, flags);
	
}

void MainWindow::OnLButtonUp()
{
	if (m_globalCommandId != IDM_CREATE) return;
	m_fieldArea->OnLButtonUp();

}

void MainWindow::OnMouseMove(int pixelX, int pixelY, DWORD flags)
{
	if (m_globalCommandId != IDM_CREATE) return;
	m_fieldArea->OnMouseMove(pixelX, pixelY, flags);


	//if (flags & MK_LBUTTON) 
	//{ 
	//	const D2D1_POINT_2F dips = DPIScale::PixelsToDips(pixelX, pixelY);

	//	const float width = (dips.x - m_ptMouse.x) / 2;
	//	const float height = (dips.y - m_ptMouse.y) / 2;
	//	const float x1 = m_ptMouse.x + width;
	//	const float y1 = m_ptMouse.y + height;

	//	m_ellipse = D2D1::Ellipse(D2D1::Point2F(x1, y1), width, height);

	//	InvalidateRect(m_hwnd, NULL, FALSE);
	//}
}
