#include "stdafx.h"
#include "FieldArea.h"
#include <d2d1.h>
#include <dwrite.h>


D2D1::ColorF::Enum colors[] = { D2D1::ColorF::Yellow, D2D1::ColorF::Salmon, D2D1::ColorF::LimeGreen };

FieldArea::FieldArea(void)
{
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
	FieldWidth = (m_rect.right - m_rect.left) ;
	FieldLength =  abs(m_rect.top - m_rect.bottom);
	NrOfXScanPoints = round(FieldWidth / VEHICLE_WIDTH_IN_CMS) + 1;  
	NrOfYScanPoints = round(FieldLength / (LIDAR_RANGE_IN_CMS / 2)) + 1;  
	XScanRange = FieldWidth / (NrOfXScanPoints-1);
	YScanRange = FieldLength / (NrOfYScanPoints-1);
	TotalNrOfScanPoints = NrOfXScanPoints * NrOfYScanPoints;
	int temp = NrOfYScanPoints;
	int ScanPtNumber;
	int iCounter = 0;
	int NumOfReachPoints ;
	for (int iCountX = 1; iCountX < NrOfXScanPoints; iCountX++)
	{
		for (int iCountY=0; iCountY < NrOfYScanPoints; iCountY++)
		{
			int Xcord = (iCountX  * XScanRange);
			//Start to generate points with an offset downward the Y axis
			int Ycord = iCountY == 0 ? ( iCountY +1) * YScanRange : iCountY * YScanRange;

			if (iCountX % 2 != 0)
				ScanPtNumber = iCounter + NrOfYScanPoints - (iCountY*2) - 1;
			else
				ScanPtNumber = iCounter;
			ScanPtArr [ScanPtNumber].ScanPtNum = ScanPtNumber;
			ScanPtArr [ScanPtNumber].Source.X = Xcord; 
			ScanPtArr [ScanPtNumber].Source.Y = Ycord;
			InsertEllipse(Xcord, Ycord);
			iCounter++;
			ScanPtArr[ScanPtNumber].NumOfReachPts = 0;
		}
	}
	TotalNrOfScanPoints = iCounter;
	for (int icount = 0; icount < TotalNrOfScanPoints; icount++)
	{
		ScanPtArr[icount].Goal.X = ScanPtArr[icount+1].Source.X;
		ScanPtArr[icount].Goal.Y = ScanPtArr[icount+1].Source.Y;
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
		BeginPaint(m_texthwnd, &ps);
		m_RenderTarget->BeginDraw();
		//m_RenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		m_RenderTarget->Clear( D2D1::ColorF(D2D1::ColorF::Coral));

		// Retrieve the size of the render target.
		const D2D1_SIZE_F renderTargetSize = m_RenderTarget->GetSize();

		int iCounter = 1;
		for (auto i = ellipses.begin(); i != ellipses.end(); ++i)
		{
			D2D1_ELLIPSE  ellipse = (*i)->ellipse;
			(*i)->Draw(m_RenderTarget, m_Brush);
			WCHAR info[20];
			_itow_s(iCounter, info, 10);	
			m_RenderTarget->DrawText(info, wcslen(info), m_textFormat, D2D1::RectF((*i)->ellipse.point.x, (*i)->ellipse.point.y, (*i)->ellipse.point.x + 9, (*i)->ellipse.point.y-40), m_Brush);
			iCounter++;
		}
		m_RenderTarget->EndDraw();
		EndPaint(m_texthwnd, &ps);

	}
}

HRESULT FieldArea::InsertEllipse(float x, float y)
{
	try
	{
		selection = ellipses.insert(
			ellipses.end(), 
			shared_ptr<ScanPtEllipse>(new ScanPtEllipse()));

		Selection()->ellipse.point = D2D1::Point2F(x, y);
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
	if(GetWindowRect(hParent, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}
	m_texthwnd = CreateWindowEx(0, __T("STATIC"), __T("RAM: 99%"),
		WS_CHILD | WS_VISIBLE | SS_SUNKEN,
		10, 10, width-50, height - 80, hParent, 0, 0, 0);
	GetClientRect(m_texthwnd, &m_rect);
	Initialize();
	/* HDC hdc = GetDC(texthwnd);
	SetBkColor(hdc, RGB(222,231,249));*/
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
			L"SampleImage",
			L"Image",
			200,
			0,
			&m_vehicle
			);

		RECT rc;

		GetClientRect(m_texthwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
		m_RenderTarget->CreateBitmap(size,D2D1::BitmapProperties(), &m_vehicle);

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
		GetClientRect(m_texthwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = m_factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_texthwnd, size),
			&m_RenderTarget);

		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(255.0f, 1.0f, 0);
			hr = m_RenderTarget->CreateSolidColorBrush(color, &m_Brush);
			hr = CreateVehicle();
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
	//hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_writeFactory);

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
		m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
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
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICBitmapScaler *pScaler = NULL;

	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	void *pImageFile = NULL;
	DWORD imageFileSize = 0;

	// Locate the resource.
	imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);
	HRESULT hr = imageResHandle ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		// Load the resource.
		imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);
		hr = imageResDataHandle ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr))
	{
		// Lock it to get a system memory pointer.
		pImageFile = LockResource(imageResDataHandle);
		hr = pImageFile ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr))
	{
		// Calculate the size.
		imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);
		hr = imageFileSize ? S_OK : E_FAIL;

	}
	if (SUCCEEDED(hr))
	{
		// Create a WIC stream to map onto the memory.
		hr = pIWICFactory->CreateStream(&pStream);
	}
	if (SUCCEEDED(hr))
	{
		// Initialize the stream with the memory pointer and size.
		hr = pStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImageFile),
			imageFileSize
			);
	}

	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream.
		hr = pIWICFactory->CreateDecoderFromStream(
			pStream,
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
			);
	}
	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}
	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{           
		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
			);
	}
	if (SUCCEEDED(hr))
	{
		//create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
			);

	}

	SafeRelease(&pDecoder);
	SafeRelease(&pSource);
	SafeRelease(&pStream);
	SafeRelease(&pConverter);
	SafeRelease(&pScaler);

	return hr;
}


