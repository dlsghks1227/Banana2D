#include "DeviceResources.h"

using namespace DX;

std::unique_ptr<DeviceResources>		g_deviceResources = std::make_unique<DeviceResources>();

DeviceResources::DeviceResources() noexcept :
	m_window(nullptr),
	m_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1),
	m_outputSize{ 0, 0, 1, 1 },
	m_deviceNotify(nullptr)
{

}

void DeviceResources::CreateDeviceResources()
{
	// API �⺻���� �ٸ� ���� ä�� ������ ���� ǥ�鿡 ���� ������ �߰��մϴ�.
	// Direct3D ���ҽ��� Direct2D ��ȣ ȣȯ���� ���� �ʿ�
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

	Microsoft::WRL::ComPtr<IDXGIDevice>				dxgiDevice;

	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		deviceContext;

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
		deviceContext.GetAddressOf()
	));

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

void DeviceResources::CreateWindowSizeDependentResources()
{
	if (!m_window)
	{
		throw std::logic_error("Call SetWindow with a valid Win32 window handle");
	}

	// ���� â ũ�� Ư�� ���ؽ�Ʈ �ʱ�ȭ
	m_d2dTargetBitmap.Reset();
	m_dwTextFormat.Reset();

	m_d2dDeviceContext->SetTarget(nullptr);
	m_d2dDeviceContext->Flush();

	const UINT backBufferWidth = std::max<UINT>(static_cast<UINT>(m_outputSize.right - m_outputSize.left), 1u);
	const UINT backBufferHeight = std::max<UINT>(static_cast<UINT>(m_outputSize.bottom - m_outputSize.top), 1u);

	if (m_dxgiSwapChain)
	{
		// ResizeBuffers �ϱ����� RenderTarget�� �ʱ�ȭ �ؾ��Ѵ�.
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
		// ���� ü�ο� ���� ������ ����
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc	= {};
		swapChainDesc.Width					= backBufferWidth;
		swapChainDesc.Height				= backBufferHeight;
		swapChainDesc.Format				= DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount			= 2;
		swapChainDesc.SampleDesc.Count		= 1;
		swapChainDesc.SampleDesc.Quality	= 0;
		swapChainDesc.Scaling				= DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.AlphaMode				= DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Flags					= 0u;

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

	Microsoft::WRL::ComPtr<IDXGISurface> surface;
	DX::ThrowIfFailed(m_dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(surface.ReleaseAndGetAddressOf())));

	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
			96.0f,
			96.0f
		);

	// Direct2D�� ������ ��Ʈ�� ǥ���� ����
	DX::ThrowIfFailed(m_d2dDeviceContext->CreateBitmapFromDxgiSurface(
		surface.Get(),
		&bitmapProperties,
		m_d2dTargetBitmap.ReleaseAndGetAddressOf()
	));

	// Direct2D ����̽� ���ؽ�Ʈ�� ������ �� ��Ʈ�� ����
	m_d2dDeviceContext->SetTarget(m_d2dTargetBitmap.Get());

	// DirectWrite ���� ����
	DX::ThrowIfFailed(m_dwFactory->CreateTextFormat(
		L"Arial",
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		21.0f,
		L"",
		m_dwTextFormat.ReleaseAndGetAddressOf()
	));

	DX::ThrowIfFailed(m_dwTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
	DX::ThrowIfFailed(m_dwTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
}

void DeviceResources::SetWindow(HWND window, int width, int height) noexcept
{
	m_window = window;

	m_outputSize.left = m_outputSize.top = 0;
	m_outputSize.right = width;
	m_outputSize.bottom = height;
}

bool DeviceResources::WindowSizeChanged(int width, int height)
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

void DeviceResources::HandleDeviceLost()
{
	if (m_deviceNotify)
	{
		m_deviceNotify->OnDeviceLost();
	}

	m_d2dTargetBitmap.Reset();
	m_dwTextFormat.Reset();

	m_dxgiSwapChain.Reset();

	m_d2dDeviceContext.Reset();

	m_d2dDevice.Reset();
	m_d3dDevice.Reset();

	m_dwFactory.Reset();
	m_wicFactory.Reset();
	m_d2dFactory.Reset();
	m_dxgiFactory.Reset();

	CreateDeviceResources();
	CreateWindowSizeDependentResources();

	if (m_deviceNotify)
	{
		m_deviceNotify->OnDeviceRestored();
	}
}

void DeviceResources::Present()
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

void DeviceResources::CreateFactory()
{
	DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));
	DX::ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, m_d2dFactory.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(m_dwFactory.ReleaseAndGetAddressOf())));
	DX::ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(m_wicFactory.ReleaseAndGetAddressOf())));
}

void DX::DeviceResources::FontLoadFromFile(std::wstring const& path)
{
	Microsoft::WRL::ComPtr<IDWriteFontSetBuilder1>		fontSetBuilder;
	Microsoft::WRL::ComPtr<IDWriteFontFile>				fontFile;
}
