// Banana2D.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Banana2D.h"
#include "DeviceResources.h"
#include "Game.h"
#include "InputManager.h"

#define MAX_LOADSTRING 100

// 전역 변수:
namespace
{
	std::unique_ptr<Game>                       g_game;
}

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	// C4100 Warning을 발생시키지 않는 매크로
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.
	if (!DirectX::XMVerifyCPUSupport())
	{
#ifdef _DEBUG
		std::wcout << L"Cpu not Supported\n";
#endif
		return 1;
	}

	HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);

	HRESULT hr = S_OK;
	hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
		return 1;

	// 애플리케이션 초기화를 수행합니다:

	g_game = std::make_unique<Game>();

	{
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = HINST_THISCOMPONENT;
		wcex.hCursor = LoadCursor(0, IDC_ARROW);
		wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = L"Banana";

		if (!RegisterClassExW(&wcex))
			return 1;

		int w, h;
		g_game->GetDefaultSize(w, h);

		RECT rc = { 0, 0, static_cast<LONG>(w), static_cast<LONG>(h) };

		HWND hwnd = CreateWindowExW(
			0,
			wcex.lpszClassName,
			L"Banana",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rc.right - rc.left,
			rc.bottom - rc.top,
			nullptr,
			nullptr,
			HINST_THISCOMPONENT,
			nullptr);

		hr = hwnd ? S_OK : E_FAIL;

		if (SUCCEEDED(hr))
		{
			ShowWindow(hwnd, nCmdShow);

			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_game.get()));

			GetClientRect(hwnd, &rc);

			g_game->Initialize(hwnd, rc.right - rc.left, rc.bottom - rc.top);
			g_inputManager->Initialize();
		}
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BANANA2D));

	MSG msg = {};

	// 기본 메시지 루프입니다:
	while (msg.message != WM_QUIT)
	{
		// GetMessage와 PeekMessage의 차이
		// GetMessage - 메시지가 있을 때까지 대기
		// PeekMessage - 메시지가 올때까지 기다리지 않음
		// https://stackoverflow.com/questions/2850186/why-peekmessage-before-getmessage
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Game Loop
			g_game->Tick();
			g_inputManager->KeyBoardUpdate();
		}
	}

	g_game.reset();
	g_deviceResources.reset();

	CoUninitialize();

	return static_cast<int>(msg.wParam);
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool s_in_sizemove = false;
	static bool s_in_suspend = false;
	static bool s_minimized = false;
	static bool s_fullscreen = false;

	auto game = reinterpret_cast<Game*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (message)
	{
	case WM_PAINT:
		if (s_in_sizemove && game)
		{
			g_game->Tick();
		}
		else
		{
			PAINTSTRUCT ps;
			(void)BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_MOVE:
		if (game)
		{
			game->OnWindowMoved();
		}
		break;

	case WM_ACTIVATEAPP:
		if (game)
		{
			if (wParam)
			{
				game->OnActivated();
			}
			else
			{
				game->OnDeactivated();
			}
		}
		break;

	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			if (!s_minimized)
			{
				s_minimized = true;
				if (!s_in_suspend && game)
				{
					game->OnSuspending();
				}
				s_in_suspend = true;
			}
		}
		else if (s_minimized)
		{
			s_minimized = false;
			if (s_in_suspend && game)
			{
				game->OnResuming();
			}
			s_in_suspend = false;
		}
		else if (!s_in_sizemove && game)
		{
			game->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
		}
		break;

	case WM_ENTERSIZEMOVE:
		s_in_sizemove = true;
		break;

	case WM_EXITSIZEMOVE:
		s_in_sizemove = false;
		if (game)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);

			game->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
		}
		break;

		// 최소 사이즈 및 최대 사이즈 조절
	case WM_GETMINMAXINFO:
		if (lParam)
		{
			auto info = reinterpret_cast<MINMAXINFO*>(lParam);
			info->ptMinTrackSize.x = 800;
			info->ptMinTrackSize.y = 600;
		}
		break;
	case WM_MOUSEMOVE:
		g_inputManager->OnMouseMove(
			D2D1::Point2L(static_cast<INT32>(LOWORD(lParam)), static_cast<INT32>(HIWORD(lParam)))
		);
		break;
	case WM_LBUTTONDOWN:
		g_inputManager->OnLeftMousePressed(
			D2D1::Point2L(static_cast<INT32>(LOWORD(lParam)), static_cast<INT32>(HIWORD(lParam)))
		);
		break;
	case WM_LBUTTONUP:
		g_inputManager->OnLeftMouseReleased(
			D2D1::Point2L(static_cast<INT32>(LOWORD(lParam)), static_cast<INT32>(HIWORD(lParam)))
		);
		break;
	case WM_RBUTTONDOWN:
		g_inputManager->OnRightMousePressed(
			D2D1::Point2L(static_cast<INT32>(LOWORD(lParam)), static_cast<INT32>(HIWORD(lParam)))
		);
		break;

	case WM_RBUTTONUP:
		g_inputManager->OnRightMouseReleased(
			D2D1::Point2L(static_cast<INT32>(LOWORD(lParam)), static_cast<INT32>(HIWORD(lParam)))
		);
		break;

	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			if (s_fullscreen)
			{
				SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
				SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

				int width = 800;
				int height = 600;
				if (game)
					game->GetDefaultSize(width, height);

				ShowWindow(hWnd, SW_SHOWNORMAL);

				SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
			}
			else
			{
				SetWindowLongPtr(hWnd, GWL_STYLE, 0);
				SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

				SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

				ShowWindow(hWnd, SW_SHOWMAXIMIZED);
			}

			s_fullscreen = !s_fullscreen;
		}
		break;
		// 종료
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}