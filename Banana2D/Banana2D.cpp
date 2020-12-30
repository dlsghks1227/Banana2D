#include "framework.h"

#include "Banana2D.h"


Banana2D::Banana2D() :
	m_hWnd(nullptr),
	m_pDxgiFactory(nullptr),
	m_pDirect2dFactory(nullptr),
	m_pRenderTarget(nullptr),
	m_pLightSlateGrayBrush(nullptr)
{
}

Banana2D::~Banana2D()
{
	SafeRelease(&m_pDirect2dFactory);
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
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

	// https://docs.microsoft.com/ko-kr/cpp/cpp/uuidof-operator?view=vs-2019
	// https://m.blog.naver.com/PostView.nhn?blogId=ljc8808&logNo=220456743162&proxyReferer=https:%2F%2Fwww.google.com%2F
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_pDxgiFactory);
	
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
		hr = m_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hWnd, size),
			&m_pRenderTarget);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::LightSlateGray),
				&m_pLightSlateGrayBrush);
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

		for (int x = 0; x < width; x += 10)
		{
			m_pRenderTarget->DrawLine(
				D2D1::Point2F(sin(static_cast<FLOAT>(x) * 2.0f * 3.14f) * 10.0f, 10.0f),
				D2D1::Point2F(rtSize.width, 10.0f),
				m_pLightSlateGrayBrush, 2.0f);
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
			::GetWindowLongW(hWnd, GWLP_USERDATA)));

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