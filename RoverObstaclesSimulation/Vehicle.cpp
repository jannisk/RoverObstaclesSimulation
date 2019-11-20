#include "stdafx.h"
#include "Vehicle.h"


Vehicle::Vehicle(void)
{

}


Vehicle::~Vehicle(void)
{
}

std::shared_ptr<CWICImagingFactory> CWICImagingFactory::m_pInstance;

CWICImagingFactory::CWICImagingFactory()
    : m_pWICImagingFactory(nullptr)
{
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
        IID_IWICImagingFactory, (LPVOID*)&m_pWICImagingFactory);
    assert(SUCCEEDED(hr));
}
 
IWICImagingFactory* CWICImagingFactory::GetFactory()  const
{
    assert(m_pWICImagingFactory);
    return m_pWICImagingFactory;
}