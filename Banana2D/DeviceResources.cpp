#include "framework.h"

#include "DeviceResources.h"

DX::DeviceResources::DeviceResources() noexcept :
	m_window(nullptr),
	m_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1),
	m_outputSize{ 0, 0, 1, 1 }
{

}

void DX::DeviceResources::CreateDeviceResources()
{
	// API 기본값과 다른 색상 채널 순서를 가진 표면에 대한 지원을 추가합니다.
	// Direct3D 리소스와 Direct2D 상호 호환성을 위해 필요
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	CreateFactory();

	static const D3D_FEATURE_LEVEL s_featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;

	DX::ThrowIfFailed(D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		s_featureLevels,
		ARRAYSIZE(s_featureLevels),
		D3D11_SDK_VERSION,
		device.GetAddressOf(),
		&m_d3dFeatureLevel,
		deviceContext.GetAddressOf()));

	DX::ThrowIfFailed(device.As(&m_dxgiDevice));
	DX::ThrowIfFailed(device.As(&m_d3dDevice));
	DX::ThrowIfFailed(deviceContext.As(&m_d3dDeviceContext));

	DX::ThrowIfFailed(m_d2dFactory->CreateDevice(
		m_dxgiDevice.Get(),
		m_d2dDevice.ReleaseAndGetAddressOf()
	));

	DX::ThrowIfFailed(m_d2dDevice->CreateDeviceContext(
		D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
		m_d2dDeviceContext.ReleaseAndGetAddressOf()
	));
}

void DX::DeviceResources::CreateWindowSizeDependentResources()
{
	if (!m_window)
	{
		throw std::logic_error("Call SetWindow with a valid Win32 window handle");
	}

	// 이전 창 크기 특정 컨텍스트 초기화
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_d3dDeviceContext->OMGetRenderTargets(static_cast<UINT>(std::size(nullViews)), nullViews, nullptr);
	m_backBuffer.Reset();
	m_d2dTargetBitmap.Reset();
	m_d3dDeviceContext->Flush();

	if (m_dxgiSwapChain)
	{
		HRESULT hr = m_dxgiSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			HandleDeviceLost();
			return;
		}
		else
		{
			DX::ThrowIfFailed(hr);
		}
	}
	else
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = 0;
		swapChainDesc.Height = 0;
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.Stereo = false;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Flags = 0;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
		fsSwapChainDesc.Windowed = TRUE;

		DX::ThrowIfFailed(m_dxgiFactory->CreateSwapChainForHwnd(
			m_d3dDevice.Get(),
			m_window,
			&swapChainDesc,
			&fsSwapChainDesc,
			nullptr,
			m_dxgiSwapChain.ReleaseAndGetAddressOf()
		));

		DX::ThrowIfFailed(m_dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
	}

	DX::ThrowIfFailed(m_dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(m_backBuffer.ReleaseAndGetAddressOf())));

	Microsoft::WRL::ComPtr<IDXGISurface> dxgiBackBuffer;
	DX::ThrowIfFailed(m_dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(dxgiBackBuffer.ReleaseAndGetAddressOf())));

	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
			96.0f,
			96.0f
		);

	DX::ThrowIfFailed(m_d2dDeviceContext->CreateBitmapFromDxgiSurface(
		dxgiBackBuffer.Get(),
		&bitmapProperties,
		m_d2dTargetBitmap.ReleaseAndGetAddressOf()
	));

	m_d2dDeviceContext->SetTarget(m_d2dTargetBitmap.Get());
}

void DX::DeviceResources::SetWindow(HWND window, int width, int height) noexcept
{
	m_window = window;

	m_outputSize.left = m_outputSize.top = 0;
	m_outputSize.right = width;
	m_outputSize.bottom = height;
}

bool DX::DeviceResources::WindowSizeChanged(int width, int height)
{
	RECT newRc;
	newRc.left = newRc.top = 0;
	newRc.right = width;
	newRc.bottom = height;
	if (newRc == m_outputSize)
	{
		return false;
	}

	m_outputSize = newRc;
	CreateWindowSizeDependentResources();
	return true;
}

void DX::DeviceResources::HandleDeviceLost()
{
	if (m_deviceNotify)
	{
		m_deviceNotify->OnDeviceLost();
	}

	m_backBuffer.Reset();
	m_d2dTargetBitmap.Reset();
	m_dxgiSwapChain.Reset();
	m_d3dDeviceContext.Reset();
	m_d2dDeviceContext.Reset();

	m_d3dDevice.Reset();
	m_d2dDevice.Reset();

	m_dxgiFactory.Reset();
	m_d2dFactory.Reset();

	CreateDeviceResources();
	CreateWindowSizeDependentResources();

	if (m_deviceNotify)
	{
		m_deviceNotify->OnDeviceRestored();
	}
}

void DX::DeviceResources::Present()
{
	HRESULT hr = E_FAIL;
	hr = m_dxgiSwapChain->Present(1, 0);

	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		HandleDeviceLost();
	}
	else
	{
		ThrowIfFailed(hr);

		if (!m_dxgiFactory->IsCurrent())
		{
			CreateFactory();
		}
	}
}

void DX::DeviceResources::CreateFactory()
{

	// https://github.com/microsoft/Windows-classic-samples/blob/master/Samples/InteractionContextProduceTouchInput/cpp/DeviceResources.cpp
	DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));
	DX::ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_d2dFactory.ReleaseAndGetAddressOf()));
}