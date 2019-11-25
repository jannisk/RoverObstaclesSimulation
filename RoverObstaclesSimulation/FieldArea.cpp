#include "stdafx.h"
#include "FieldArea.h"
#include <d2d1.h>
#include <dwrite.h>
#include "Vehicle.h"

#pragma comment(lib, "Windowscodecs.lib")


D2D1::ColorF::Enum colors[] = { D2D1::ColorF::Yellow, D2D1::ColorF::Salmon, D2D1::ColorF::LimeGreen };

FieldArea::FieldArea(void): m_ptMouse(D2D1::Point2F()), m_ellipse(D2D1::Ellipse(D2D1::Point2F(), 0, 0))
{
	// Initialize COM
	CoInitialize(nullptr);
	VEHICLE_WIDTH_IN_CMS = 32;
	LIDAR_RANGE_IN_CMS = 180;
	m_factory = NULL;
	m_textFormat = NULL;
}


FieldArea::~FieldArea(void)
{
}


FieldArea::FieldArea(HWND mainWindow, ID2D1Factory *factory)
{
	*this = FieldArea::FieldArea();
	m_factory= factory;
	m_RenderTarget = NULL;
}

void FieldArea::Initialize()
{
	FieldWidth = (m_viewRect.right - m_viewRect.left) ;
	FieldLength =  abs(m_viewRect.top - m_viewRect.bottom);
	NrOfXScanPoints = round(FieldWidth / VEHICLE_WIDTH_IN_CMS) + 1;  
	NrOfYScanPoints = round(FieldLength / (LIDAR_RANGE_IN_CMS / 2)) + 1;  
	XScanRange = FieldWidth / (NrOfXScanPoints-1);
	YScanRange = FieldLength / (NrOfYScanPoints-1);
	TotalNrOfScanPoints = NrOfXScanPoints * NrOfYScanPoints;
	int temp = NrOfYScanPoints;
	int scanPtNumber;
	int iCounter = 0;
	int NumOfReachPoints ;
	for (int iCountX = 0; iCountX < NrOfXScanPoints; iCountX++)
	{
		for (int iCountY=0; iCountY < NrOfYScanPoints; iCountY++)
		{
			int Xcord = iCountX  * XScanRange + COORD_OFFSET; 
			//Start to generate points with an offset downward the Y axis
			int Ycord = iCountY * YScanRange + COORD_OFFSET;

			if (iCountX % 2 != 0)
				scanPtNumber = iCounter + NrOfYScanPoints - (iCountY*2) - 1;
			else
				scanPtNumber = iCounter;
			m_ScanPtArr [scanPtNumber].ScanPtNum = scanPtNumber;
			m_ScanPtArr [scanPtNumber].Source.X = Xcord; 
			m_ScanPtArr [scanPtNumber].Source.Y = Ycord;
			
			InsertEllipse(Xcord, Ycord, scanPtNumber);
			
			iCounter++;
			m_ScanPtArr[scanPtNumber].NumOfReachPts = 0;
		}
	}
	TotalNrOfScanPoints = iCounter;
	for (int icount = 0; icount < TotalNrOfScanPoints; icount++)
	{
		m_ScanPtArr[icount].Goal.X = m_ScanPtArr[icount+1].Source.X;
		m_ScanPtArr[icount].Goal.Y = m_ScanPtArr[icount+1].Source.Y;
	}
}

void FieldArea::DrawScanPoints()
{
	static const WCHAR sc_helloWorld[] = L"Hello, World!";

	HRESULT hr = CreateGraphicsResources();
	hr = CreateDeviceIndependentResources();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		//BeginPaint(m_fieldhwnd, &ps);
		m_RenderTarget->BeginDraw();
		//m_RenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_RenderTarget->Clear( D2D1::ColorF(D2D1::ColorF::OliveDrab));

		// Retrieve the size of the render target.
		const D2D1_SIZE_F renderTargetSize = m_RenderTarget->GetSize();

		// Retrieve the size of the bitmap.
		D2D1_SIZE_F size = m_vehicle->GetSize();

        D2D1_POINT_2F upperLeftCorner = D2D1::Point2F(100.f, 10.f);
		// Draw a bitmap.
		m_RenderTarget->DrawBitmap(
			m_vehicle,
			D2D1::RectF(
			upperLeftCorner.x,
			upperLeftCorner.y,
			upperLeftCorner.x + size.width,
			upperLeftCorner.y + size.height)
			);

		//count nr of points
		int iCounter = 1;
		for (auto i = ellipses.begin(); i != ellipses.end(); ++i)
		{
			D2D1_ELLIPSE  ellipse = (*i)->ellipse;
			(*i)->Draw(m_RenderTarget, m_Brush);
			WCHAR info[20];
			_itow_s((*i)->ScanPtNumber, info, 10);	
			m_RenderTarget->DrawText(info, wcslen(info), m_textFormat, D2D1::RectF((*i)->ellipse.point.x, (*i)->ellipse.point.y, 
				(*i)->ellipse.point.x + 9, (*i)->ellipse.point.y-40), m_Brush);
			
			iCounter++;
		}
      
		D2D1_SIZE_F aSize;
		aSize.width = (m_rect.right - m_rect.left) - 50;
		aSize.height = (m_rect.bottom - m_rect.top) - 50;

		m_RenderTarget->DrawRectangle(D2D1::RectF(m_rect.left + 50, m_rect.top + 50, m_rect.left + aSize.width, m_rect.top + aSize.height ), m_Brush);
		m_RenderTarget->EndDraw();
		
		m_RenderTarget->DrawEllipse(m_ellipse, m_Brush, 2);

		//EndPaint(m_fieldhwnd, &ps);
	}
}

HRESULT FieldArea::InsertEllipse(float x, float y, int scanNr)
{
	try
	{
		selection = ellipses.insert(
			ellipses.end(), 
			shared_ptr<ScanPtEllipse>(new ScanPtEllipse()));

		Selection()->ellipse.point = D2D1::Point2F(x, y);
		Selection()->ScanPtNumber = scanNr;
		Selection()->ellipse.radiusX = Selection()->ellipse.radiusY = 2.0f; 
		Selection()->color = D2D1::ColorF( colors[0] );
	}
	catch (std::bad_alloc)
	{
		return E_OUTOFMEMORY;
	}
	return S_OK;
}

void FieldArea::Create(HWND hParent)
{ 
	RECT rect;
	int width, height;
	m_hParent = hParent;
	////if(GetWindowRect(hParent, &rect))
	//{
	//	width = rect.right - rect.left;
	//	height = rect.bottom - rect.top;
	//}
	//m_fieldhwnd = CreateWindowEx(0, __T("STATIC"), __T("RAM: 99%"),
	//	WS_CHILD | WS_VISIBLE | SS_SUNKEN,
	//	10, 10, width-50, height - 80, hParent, 0, 0, 0);
	//GetClientRect(m_fieldhwnd, &m_rect);

	D2D1_SIZE_F size;
	size.width = (m_rect.right - m_rect.left) - COORD_OFFSET;
	size.height = (m_rect.bottom - m_rect.top) - COORD_OFFSET;

	m_viewRect.left = m_rect.left + COORD_OFFSET;
	m_viewRect.top = m_rect.top + COORD_OFFSET;
	m_viewRect.right = m_rect.left + size.width ;
	m_viewRect.bottom =  m_rect.top + size.height;

	Initialize();
}

HRESULT FieldArea::CreateVehicle()
{
	HRESULT hr = S_OK;
	if (m_RenderTarget != NULL)
	{
		//https://docs.microsoft.com/en-us/windows/win32/direct2d/how-to-draw-a-bitmap
		// Create a bitmap from an application resource.
		hr = LoadResourceBitmap(
			m_RenderTarget,
			m_WicFactory,
			L"IDB_CAR",
			MAKEINTRESOURCE(RT_BITMAP),
			20,
			10,
			&m_vehicle
			);
		/*RECT rc;

		GetClientRect(m_texthwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
		m_RenderTarget->CreateBitmap(size,D2D1::BitmapProperties(), &m_vehicle);*/

	}
	if (SUCCEEDED(hr))
	{
	}
	return hr;
}

HRESULT FieldArea::CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	if (m_RenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(m_hParent, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = m_factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hParent, size),
			&m_RenderTarget);
		
		m_WicFactory = CWICImagingFactory::GetInstance().GetFactory();

		hr = CreateVehicle();

		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(255.0f, 1.0f, 0);
			hr = m_RenderTarget->CreateSolidColorBrush(color, &m_Brush);
		}
	}
	return hr;
}

//Create resources which are not bound
// to any device. Their lifetime effectively extends for the
// duration of the app. These resources include the Direct2D and
// DirectWrite factories,  and a DirectWrite Text Format object
// (used for identifying particular font characteristics).
//
HRESULT FieldArea::CreateDeviceIndependentResources()
{
	const WCHAR msc_fontName[] = L"Verdana";
	const FLOAT msc_fontSize = 10;
	HRESULT hr = S_OK;
	IDWriteFactory *m_writeFactory;

	if (m_textFormat != NULL) return hr;
	// Create a Direct2D factory.
	//hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_WicFactory);

	if (SUCCEEDED(hr))
	{

		// Create a DirectWrite factory.
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_writeFactory),
			reinterpret_cast<IUnknown **>(&m_writeFactory)
			);
	}

	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite text format object.
		hr = m_writeFactory->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			msc_fontSize,
			L"", //locale
			&m_textFormat
			);
	}
	if (SUCCEEDED(hr))
	{
		// Center the text horizontally and vertically.
		m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_textFormat->SetReadingDirection(DWRITE_READING_DIRECTION_RIGHT_TO_LEFT);
		m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		m_textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	}
	return hr;
}


HRESULT FieldArea::LoadResourceBitmap(
	ID2D1RenderTarget *pRenderTarget,
	IWICImagingFactory *pIWICFactory,
	PCWSTR resourceName,
	PCWSTR resourceType,
	UINT destinationWidth,
	UINT destinationHeight,
	ID2D1Bitmap **ppBitmap
	)
{
	int errmsg;

	HBITMAP hbitmap;
	WICBitmapAlphaChannelOption wicalpha;
	IWICBitmap *pwicbitmap;
	IWICFormatConverter *pConverter;

	ID2D1Factory *d2dfactory;
	D2D1_BITMAP_PROPERTIES d2dbp;
	D2D1_PIXEL_FORMAT d2dpf;
	FLOAT dpiX;
	FLOAT dpiY;

	hbitmap = LoadBitmap( GetModuleHandle(NULL),resourceName );
	wicalpha = WICBitmapIgnoreAlpha; //no alpha channel in bitmaps

	errmsg = m_WicFactory->CreateBitmapFromHBITMAP( hbitmap, NULL, wicalpha, &pwicbitmap );
	if( !SUCCEEDED(errmsg) )
	{
		printf("LoadBitmapFromResource::CreateBitmapFromHBITMAP() error: %x\r\n", errmsg );
		return errmsg;
	}

	errmsg = m_WicFactory->CreateFormatConverter( &pConverter );
	if( !SUCCEEDED(errmsg) )
	{
		printf("LoadBitmapFromResource::CreateFormatConverter() error: %x\r\n", errmsg );
		return errmsg;
	}

	d2dpf.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	d2dpf.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	pRenderTarget->GetFactory( &d2dfactory );
	d2dfactory->GetDesktopDpi( &dpiX, &dpiY );
	d2dbp.pixelFormat = d2dpf;
	d2dbp.dpiX = dpiX;
	d2dbp.dpiY = dpiY;

	//not needed and used keep it for future cases
	pConverter->Initialize( pwicbitmap, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut );
	if( !SUCCEEDED(errmsg) )
	{
		printf("LoadBitmapFromResource::Initialize() error: %x\r\n", errmsg );
		return errmsg;
	}

	errmsg = pRenderTarget->CreateBitmapFromWicBitmap( pwicbitmap, ppBitmap );
	if( !SUCCEEDED(errmsg) )
	{
		printf("LoadBitmapFromResource::CreateBitmapFromWicBitmap() error: %x\r\n", errmsg );
		return errmsg;
	}

	pConverter->Release();
	pwicbitmap->Release();
	DeleteObject( hbitmap );
	
	return 0;
}

void FieldArea::OnLButtonDown(int pixelX, int pixelY, DWORD flags)
{
	SetCapture(m_hParent);
	m_ellipse.point = m_ptMouse = DPIScale::PixelsToDips(pixelX, pixelY);
	m_ellipse.radiusX = m_ellipse.radiusY = 1.0f; 
	InvalidateRect(m_hParent, NULL, FALSE);
}

void FieldArea::OnMouseMove(int pixelX, int pixelY, DWORD flags)
{
	if (flags & MK_LBUTTON) 
	{ 
		const D2D1_POINT_2F dips = DPIScale::PixelsToDips(pixelX, pixelY);

		const float width = (dips.x - m_ptMouse.x) / 2;
		const float height = (dips.y - m_ptMouse.y) / 2;
		const float x1 = m_ptMouse.x + width;
		const float y1 = m_ptMouse.y + height;

		m_ellipse = D2D1::Ellipse(D2D1::Point2F(x1, y1), width, height);

		InvalidateRect(m_hParent, NULL, FALSE);
	}
}

void FieldArea::OnLButtonUp()
{
	ReleaseCapture();
}


