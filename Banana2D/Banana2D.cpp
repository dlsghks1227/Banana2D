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
		* CS_HREDRAW : 이동 또는 크기 조정으로 클라이언트 영역의 너비가 변경되면 전체 창을 다시 그립니다.
		* CS_VREDRAW : 이동 또는 크기 조정으로 클라이언트 영역의 높이가 변경되면 전체 창을 다시 그립니다.
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

		/* 어플리케이션 로컬 클래스 등록
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

	/* Direct2D 리소스를 만드는 데 사용할 수 있는 Factory 개체를 생성
	*
	* D2D1_FACTORY_TYPE_SINGLE_THREADED	: 싱글 스레드용
	* D2D1_FACTORY_TYPE_MULTI_THREADED	: 멀티 스레드용
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
	* 장치를 만드는 데 사용되는 매개 변수
	* 
	* https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_create_device_flag
	* 
	* D3D11_CREATE_DEVICE_BGRA_SUPPORT: BGRA 형식을 지원하는 장치를 만듭니다.
	* 
	* API 기본값과 다른 색상 채널 순서를 가진 표면에 대한 지원을 추가합니다.
	* Direct3D 리소스와 Direct2D 상호 운용성을 위해 필요합니다.
	* 
	* -------------------------------------------------- */
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	/* --------------------------------------------------
	* 
	* 앱이 지원하는 DirectX 하드웨어 기능 수준 집합을 정의합니다.
	* 
	* https://docs.microsoft.com/en-us/windows/win32/api/d3dcommon/ne-d3dcommon-d3d_feature_level
	* 
	* 순서대로 입력하는 것이 중요합니다.
	* 앱의 최소 필수 기능 수준을 선언하지 않으면 모든 앱은 9.1을 지원하는 것으로 간주합니다.
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

	// DX11 API 디바이스 오브젝트 생성 및 해당 디바이스를 가져옵니다.
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

	// Direct3D11 장치의 기본 DXGI 장치를 가져옵니다.
	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
	DX::ThrowIfFailed(
		device.As(&dxgiDevice)
	);

	// 2D 렌더링을 위한 Direct2D 디바이스를 얻습니다.
	DX::ThrowIfFailed(
		m_pD2DFactory->CreateDevice(dxgiDevice.Get(), &m_pD2DDevice)
	);


	// Direct2D 장치의 해당 장치 컨텍스트 개체를 가져옵니다.
	DX::ThrowIfFailed(
		m_pD2DDevice->CreateDeviceContext(
			D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
			&m_pD2DDeviceContext
		)
	);


	// DXGI_SWAP_CHAIN_DESC1 구조를 할당하고 스왑 체인에 대한 설정을 정의합니다.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.Width = 0;
	swapChainDesc.Height = 0;		// 사이즈 자동 설정
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_NONE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.Flags = 0;

	// 디바이스가 실행되는 물리적 어댑터(GPU 또는 카드)를 식별합니다.
	// Direct3D 장치 및 DXGI 장치가 실행중인 어댑터를 가져와서 연결된 IDXGIFactory 개체를 가져옵니다.
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

		// &renderTargetProperties		: 렌더타겟의 정보
		// &hwndRenderTargetProperties	: 윈도우 렌더타겟의 정보(윈도우 핸들, 윈도우 크기 등의 값)
		// **hwndRenderTarget			: 결과를 저장할 포인터
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
		// BeginDraw 메서드를 호출하여 그리기 시작
		m_pRenderTarget->BeginDraw();

		// 단위 행렬로 변환
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		// 창 초기화
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		// 도면의 크기를 검색
		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

		// DrawLine 메서드를 이용하여 그리드 그리기
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
/// 창으로 전송 된 메시지를 처리하는 Application-define function
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

		// reinterpret_cast : 임의의 포인터 타입끼리 변환을 허용하는 캐스트 연산자
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