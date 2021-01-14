#include "framework.h"

#include "Banana2D.h"


Banana2D::Banana2D() :
	m_hWnd(nullptr),
	m_pFeatureLevels(),
	m_pWICFactory(nullptr),
	m_ppBitmap(nullptr),
	m_pD2DFactory(nullptr),
	m_pRenderTarget(nullptr),
	m_pLightSlateGrayBrush(nullptr)
{
}

Banana2D::~Banana2D()
{
	SafeRelease(&m_pD2DFactory);
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pLightSlateGrayBrush);
}

HRESULT Banana2D::Initialize()
{
	HRESULT	hr = S_OK;

	hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr))
	{
		/*
		* style
		* CS_HREDRAW : �̵� �Ǵ� ũ�� �������� Ŭ���̾�Ʈ ������ �ʺ� ����Ǹ� ��ü â�� �ٽ� �׸��ϴ�.
		* CS_VREDRAW : �̵� �Ǵ� ũ�� �������� Ŭ���̾�Ʈ ������ ���̰� ����Ǹ� ��ü â�� �ٽ� �׸��ϴ�.
		*
		* HINST_THISCOMPONENT
		* https://devblogs.microsoft.com/oldnewthing/20041025-00/?p=37483
		*
		* Window Class Styles
		* https://docs.microsoft.com/en-us/windows/win32/winmsg/window-class-styles
		*
		* WNDCLASSA
		* https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassa
		*
		*/
		WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Banana2D::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = HINST_THISCOMPONENT;
		wcex.hCursor = LoadCursor(0, IDC_ARROW);
		wcex.hbrBackground = nullptr;
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = L"Banana";

		/* ���ø����̼� ���� Ŭ���� ���
		* Using Window Classes
		* https://docs.microsoft.com/en-us/windows/win32/winmsg/using-window-classes
		*/
		RegisterClassEx(&wcex);

		m_hWnd = CreateWindowEx(
			0,
			wcex.lpszClassName,
			L"BananaDirect2D",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			nullptr,
			nullptr,
			HINST_THISCOMPONENT,
			this);

		hr = m_hWnd ? S_OK : E_FAIL;

		if (SUCCEEDED(hr))
		{
			ShowWindow(m_hWnd, SW_SHOWNORMAL);
			UpdateWindow(m_hWnd);
		}
	}
	return hr;
}

void Banana2D::RunMessageLoop()
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{

		}
	}
}

HRESULT Banana2D::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	/* Direct2D ���ҽ��� ����� �� ����� �� �ִ� Factory ��ü�� ����
	*
	* D2D1_FACTORY_TYPE_SINGLE_THREADED	: �̱� �������
	* D2D1_FACTORY_TYPE_MULTI_THREADED	: ��Ƽ �������
	*
	* D2D1CreateFactory
	* https://docs.microsoft.com/ko-kr/windows/win32/api/d2d1/nf-d2d1-d2d1createfactory
	*
	* D2D1_FACTORY_TYPE
	* https://docs.microsoft.com/en-us/windows/win32/api/d2d1/ne-d2d1-d2d1_factory_type
	*
	*/
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	// https://docs.microsoft.com/ko-kr/cpp/cpp/uuidof-operator?view=vs-2019
	// https://m.blog.naver.com/PostView.nhn?blogId=ljc8808&logNo=220456743162&proxyReferer=https:%2F%2Fwww.google.com%2F
	// hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_pDxgiFactory);

	if (FAILED(hr))
	{
		return hr;
	}

	/* --------------------------------------------------
	* 
	* ��ġ�� ����� �� ���Ǵ� �Ű� ����
	* 
	* https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_create_device_flag
	* 
	* D3D11_CREATE_DEVICE_BGRA_SUPPORT: BGRA ������ �����ϴ� ��ġ�� ����ϴ�.
	* 
	* API �⺻���� �ٸ� ���� ä�� ������ ���� ǥ�鿡 ���� ������ �߰��մϴ�.
	* Direct3D ���ҽ��� Direct2D ��ȣ ��뼺�� ���� �ʿ��մϴ�.
	* 
	* -------------------------------------------------- */
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	/* --------------------------------------------------
	* 
	* ���� �����ϴ� DirectX �ϵ���� ��� ���� ������ �����մϴ�.
	* 
	* https://docs.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_feature_level
	* 
	* ������� �Է��ϴ� ���� �߿��մϴ�.
	* ���� �ּ� �ʼ� ��� ������ �������� ������ ��� ���� 9.1�� �����ϴ� ������ �����մϴ�.
	* 
	* -------------------------------------------------- */
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_1,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_3
	};

	// DX11 API ����̽� ������Ʈ ���� �� �ش� ����̽��� �����ɴϴ�.
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	DX::ThrowIfFailed(
		D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			creationFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&device,
			&m_pFeatureLevels,
			&context
		)
	);

	// Direct3D11 ��ġ�� �⺻ DXGI ��ġ�� �����ɴϴ�.
	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
	DX::ThrowIfFailed(
		device.As(&dxgiDevice)
	);

	// 2D �������� ���� Direct2D ����̽��� ����ϴ�.
	DX::ThrowIfFailed(
		m_pD2DFactory->CreateDevice(dxgiDevice.Get(), &m_pD2DDevice)
	);


	// Direct2D ��ġ�� �ش� ��ġ ���ؽ�Ʈ ��ü�� �����ɴϴ�.
	DX::ThrowIfFailed(
		m_pD2DDevice->CreateDeviceContext(
			D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
			&m_pD2DDeviceContext
		)
	);


	// DXGI_SWAP_CHAIN_DESC1 ������ �Ҵ��ϰ� ���� ü�ο� ���� ������ �����մϴ�.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.Width = 0;
	swapChainDesc.Height = 0;		// ������ �ڵ� ����
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_NONE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.Flags = 0;

	// ����̽��� ����Ǵ� ������ �����(GPU �Ǵ� ī��)�� �ĺ��մϴ�.
	// Direct3D ��ġ �� DXGI ��ġ�� �������� ����͸� �����ͼ� ����� IDXGIFactory ��ü�� �����ɴϴ�.
	Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
	DX::ThrowIfFailed(
		dxgiDevice->GetAdapter(&dxgiAdapter)
	);

	Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
	DX::ThrowIfFailed(
		dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory))
	);




	hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));

	return hr;
}

HRESULT Banana2D::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hWnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
		);

		// &renderTargetProperties		: ����Ÿ���� ����
		// &hwndRenderTargetProperties	: ������ ����Ÿ���� ����(������ �ڵ�, ������ ũ�� ���� ��)
		// **hwndRenderTarget			: ����� ������ ������
		hr = m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hWnd, size),
			&m_pRenderTarget);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::LightSlateGray),
				&m_pLightSlateGrayBrush);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromFile(L"Image/default.png", 200, 200);
		}
	}
	return hr;
}

void Banana2D::DiscardDeviceResources()
{
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pLightSlateGrayBrush);
}

HRESULT Banana2D::OnRender()
{
	HRESULT hr = S_OK;

	hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		// BeginDraw �޼��带 ȣ���Ͽ� �׸��� ����
		m_pRenderTarget->BeginDraw();

		// ���� ��ķ� ��ȯ
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		// â �ʱ�ȭ
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		// ������ ũ�⸦ �˻�
		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

		// DrawLine �޼��带 �̿��Ͽ� �׸��� �׸���
		int width = static_cast<int>(rtSize.width);
		int height = static_cast<int>(rtSize.height);

		for (int x = 0; x < width; x += 10)
		{
			m_pRenderTarget->DrawLine(
				D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
				D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
				m_pLightSlateGrayBrush, 0.5f);
		}

		for (int y = 0; y < height; y += 10)
		{
			m_pRenderTarget->DrawLine(
				D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
				D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
				m_pLightSlateGrayBrush, 0.5f);
		}

		if (m_ppBitmap != nullptr) {
			D2D1_SIZE_F size = m_ppBitmap->GetSize();


			m_pRenderTarget->DrawBitmap(m_ppBitmap, D2D1::RectF(
				1.0f,
				1.0f,
				1.0f + size.width,
				1.0f + size.height
			));
		}

		hr = m_pRenderTarget->EndDraw();
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceResources();
	}

	return hr;
}

void Banana2D::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget)
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
}

HRESULT Banana2D::LoadBitmapFromFile(PCWSTR uri, UINT destinationWidth, UINT destinationHeight)
{
	IWICBitmapDecoder* pDecoder = nullptr;
	IWICBitmapFrameDecode* pSource = nullptr;
	IWICStream* pStream = nullptr;
	IWICFormatConverter* pConverter = nullptr;
	IWICBitmapScaler* pScaler = nullptr;

	HRESULT hr = S_OK;

	hr = m_pWICFactory->CreateDecoderFromFilename(uri, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);

	if (SUCCEEDED(hr))
	{
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			nullptr,
			0.f,
			WICBitmapPaletteTypeCustom);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			nullptr,
			&m_ppBitmap);
	}

	SafeRelease(&pDecoder);
	SafeRelease(&pSource);
	SafeRelease(&pStream);
	SafeRelease(&pConverter);
	SafeRelease(&pScaler);

	return hr;
}


/// <summary>
/// â���� ���� �� �޽����� ó���ϴ� Application-define function
/// </summary>
/// <param name="hWnd"></param>
/// <param name="message"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <returns></returns>
/// 
/// WindowProc callback function
/// 'https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms633573(v=vs.85)'
LRESULT Banana2D::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		Banana2D* pD2D = (Banana2D*)pcs->lpCreateParams;

		// reinterpret_cast : ������ ������ Ÿ�Գ��� ��ȯ�� ����ϴ� ĳ��Ʈ ������
		::SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pD2D));
		result = 1;
	}
	else
	{
		Banana2D* pD2D = reinterpret_cast<Banana2D*>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(hWnd, GWLP_USERDATA)));

		bool wasHandled = false;

		if (pD2D)
		{
			switch (message)
			{
			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				pD2D->OnResize(width, height);
			}
			result = 0;
			wasHandled = true;
			break;
			case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hWnd, nullptr, FALSE);
			}
			result = 0;
			wasHandled = true;
			break;
			case WM_PAINT:
			{
				pD2D->OnRender();
				ValidateRect(hWnd, nullptr);
			}
			result = 0;
			wasHandled = true;
			break;
			case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			result = 1;
			wasHandled = true;
			break;
			}
		}

		if (!wasHandled)
			result = DefWindowProc(hWnd, message, wParam, lParam);
	}
	return result;
}