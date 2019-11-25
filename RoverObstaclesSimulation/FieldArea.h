#pragma once
#include <math.h>
#include "Structures.h"
#include <wincodec.h>
#include <memory>
#include <list>
#include <d2d1.h>

struct ScanPtEllipse
{

	D2D1_ELLIPSE    ellipse;
	D2D1_COLOR_F    color;
	int  ScanPtNumber;

	void Draw(ID2D1RenderTarget *pRT, ID2D1SolidColorBrush *pBrush)
	{
		pBrush->SetColor(color);
		pRT->FillEllipse(ellipse, pBrush);
		pBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
		pRT->DrawEllipse(ellipse, pBrush, 1.0f);
	}
};



#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif
using namespace std;

template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

class FieldArea
{
	static const int COORD_OFFSET= 50;

private:
	ID2D1Factory *m_factory;

	IDWriteTextFormat    * m_textFormat;
	ID2D1HwndRenderTarget *m_RenderTarget;
	ID2D1Bitmap *m_vehicle;
	ID2D1SolidColorBrush *m_Brush;
	IWICImagingFactory  *m_WicFactory;
	//HWND m_fieldhwnd;

	D2D1_POINT_2F m_ptMouse;
	D2D1_ELLIPSE  m_ellipse;

	RECT m_rect;
	Coordinates Curr, Next, Prev;

	int VEHICLE_WIDTH_IN_CMS ;
	int LIDAR_RANGE_IN_CMS ;

	int NrOfXScanPoints;
	int NrOfYScanPoints;
	int XScanRange;
	int YScanRange;
	int FieldWidth;
	int FieldLength;
	int TotalNrOfScanPoints;
	ScanPt m_ScanPtArr[1000];

	list<shared_ptr<ScanPtEllipse>>   ellipses;
	list<shared_ptr<ScanPtEllipse>>::iterator   selection;
	RECT m_viewRect;
	HWND m_hParent;
	HRESULT CreateGraphicsResources();
	HRESULT CreateDeviceIndependentResources();

	inline shared_ptr<ScanPtEllipse> Selection() 
	{
		if (selection == ellipses.end()) 
		{ 
			return nullptr;
		}
		else
		{
			return (*selection);
		}

	}

	inline double round(double number)
	{
		return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
	}

	HRESULT CreateVehicle();
	
	HRESULT LoadResourceBitmap(
    ID2D1RenderTarget *pRenderTarget,
    IWICImagingFactory *pIWICFactory,
    PCWSTR resourceName,
    PCWSTR resourceType,
    UINT destinationWidth,
    UINT destinationHeight,
    ID2D1Bitmap **ppBitmap
    );

public:
	FieldArea(void);
	FieldArea(RECT& aRect);
	FieldArea(HWND window, ID2D1Factory *);
	~FieldArea(void);
	void Create(HWND hParent);

	void Initialize();
	void DrawScanPoints();

	HRESULT InsertEllipse(float x, float y, int scanNr);
	void OnLButtonDown(int pixelX, int pixelY, DWORD flags);
	void OnMouseMove(int pixelX, int pixelY, DWORD flags);
	void OnLButtonUp();
};

