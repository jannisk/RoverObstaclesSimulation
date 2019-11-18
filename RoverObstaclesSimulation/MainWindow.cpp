#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include "MainWindow.h"
#include "Resource.h"
#include "FieldArea.h"

template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}


MainWindow::MainWindow():BaseWindow()
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
	switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
     case WM_SIZE:
        Resize();
        return 0;
	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(
                D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
       {
            return -1;  // Fail CreateWindowEx.
        }
       // DPIScale::Initialize(pFactory);
		 OnCreate(m_hwnd);
		 return 0;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(m_hwnd, &ps);
			OnPaint(hdc, &ps);
            //EndPaint(m_hwnd, &ps);
        }
        return 0;

    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return TRUE;
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
		hr = pRenderTarget->EndDraw();
        if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        EndPaint(m_hwnd, &ps);
		m_fieldArea->DrawScanPoints();

    }

	/*FillRect(hdc, &ps->rcPaint, m_hPurpleBrush); 
	m_fieldArea->DrawScanPoints(hdc);

	TextOut(hdc, 45, 45, TEXT("Michael Morrison"), 16);*/
}

void MainWindow::OnCreate(HWND hwnd)
{
  m_fieldArea = new FieldArea(m_hwnd, pFactory);
  m_fieldArea->Create(m_hwnd);
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