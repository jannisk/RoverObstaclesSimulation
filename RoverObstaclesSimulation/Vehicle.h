#pragma once
#include "stdafx.h"
#include <atlbase.h>
#include <assert.h>
#include <memory>
#include <wincodec.h>
#include <wincodecsdk.h>

class Vehicle
{
public:
	Vehicle(void);
	~Vehicle(void);
};

class CWICImagingFactory
{
public:
    inline static CWICImagingFactory& GetInstance()
    {
        if (nullptr == m_pInstance.get())
            m_pInstance.reset(new CWICImagingFactory());
        return *m_pInstance;
    }
 
    virtual IWICImagingFactory* GetFactory() const;
 
protected:
    CComPtr<IWICImagingFactory> m_pWICImagingFactory;
 
private:
    CWICImagingFactory();   // Private because singleton
	static std::shared_ptr<CWICImagingFactory> m_pInstance;
};