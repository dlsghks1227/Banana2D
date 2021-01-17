#include "framework.h"
#include "DeviceResources.h"

DX::DeviceResources::DeviceResources() noexcept :
	m_window(nullptr),
	m_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1),
	m_outputSize{ 0, 0, 1, 1 },
	m_deviceNotify(nullptr)
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
	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
	// D3DDeviceContext 객체를 생성하지만 필요 X
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

	DX::ThrowIfFailed(device.As(&dxgiDevice));
	DX::ThrowIfFailed(device.As(&m_d3dDevice));

	DX::ThrowIfFailed(m_d2dFactory->CreateDevice(
		dxgiDevice.Get(),
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
	m_d2dTargetBitmap.Reset();
	m_textFormat.Reset();

	m_d2dDeviceContext->SetTarget(nullptr);
	m_d2dDeviceContext->Flush();

	const UINT backBufferWidth = std::max<UINT>(static_cast<UINT>(m_outputSize.right - m_outputSize.left), 1u);
	const UINT backBufferHeight = std::max<UINT>(static_cast<UINT>(m_outputSize.bottom - m_outputSize.top), 1u);

	if (m_dxgiSwapChain)
	{
		// ResizeBuffers 하기전에 RenderTarget을 초기화 해야한다.
		// https://github.com/Microsoft/DirectX-Graphics-Samples/issues/48
		// https://docs.microsoft.com/ko-kr/windows/win32/direct2d/direct2d-and-direct3d-interoperation-overview?redirectedfrom=MSDN#resizing-a-dxgi-surface-render-target

		HRESULT hr = m_dxgiSwapChain->ResizeBuffers(
			2,
			backBufferWidth,
			backBufferHeight,
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0u);

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
		// 스왑 체인에 대한 설정을 정의
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = backBufferWidth;
		swapChainDesc.Height = backBufferHeight;
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Flags = 0u;

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

	//DX::ThrowIfFailed(m_dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(m_renderTarget.ReleaseAndGetAddressOf())));

	Microsoft::WRL::ComPtr<IDXGISurface> surface;
	DX::ThrowIfFailed(m_dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(surface.ReleaseAndGetAddressOf())));

	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
			96.0f,
			96.0f
		);

	// Direct2D의 렌더링 비트맵 표면을 생성
	DX::ThrowIfFailed(m_d2dDeviceContext->CreateBitmapFromDxgiSurface(
		surface.Get(),
		&bitmapProperties,
		m_d2dTargetBitmap.ReleaseAndGetAddressOf()
	));

	// Direct2D 디바이스 컨텍스트가 렌더링 할 비트맵 설정
	m_d2dDeviceContext->SetTarget(m_d2dTargetBitmap.Get());

	// DirectWrite 포맷 설정
	DX::ThrowIfFailed(m_dwFactory->CreateTextFormat(
		L"Arial",
		nullptr,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		21.0f,
		L"en-us",
		m_textFormat.ReleaseAndGetAddressOf()
	));

	// IDXGISwapChain에서 surface를 가져와 표면 RenderTarget을 생성합니다.
	//Microsoft::WRL::ComPtr<IDXGISurface> surface;
	//DX::ThrowIfFailed(m_dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(surface.ReleaseAndGetAddressOf())));


	//D2D1_RENDER_TARGET_PROPERTIES props =
	//	D2D1::RenderTargetProperties(
	//		D2D1_RENDER_TARGET_TYPE_DEFAULT,
	//		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
	//		0, 0
	//	);

	//DX::ThrowIfFailed(m_d2dFactory->CreateDxgiSurfaceRenderTarget(
	//	surface.Get(),
	//	&props,
	//	m_d2dRenderTarget.ReleaseAndGetAddressOf()
	//));

	//m_d2dDeviceContext->SetTarget()
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

	m_d2dTargetBitmap.Reset();
	m_dxgiSwapChain.Reset();

	m_d2dDeviceContext.Reset();

	m_d2dDevice.Reset();
	m_d3dDevice.Reset();

	m_d2dFactory.Reset();
	m_dxgiFactory.Reset();

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
#ifdef _DEBUG
		char buff[64] = {};
		sprintf_s(buff, "Device Lost on Present: Reason code 0x%08X\n",
			static_cast<unsigned int>((hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr));
		std::wcout << buff << '\n';
#endif
		HandleDeviceLost();
	}
	else
	{
		DX::ThrowIfFailed(hr);

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
	DX::ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, m_d2dFactory.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(m_dwFactory.ReleaseAndGetAddressOf())));
}