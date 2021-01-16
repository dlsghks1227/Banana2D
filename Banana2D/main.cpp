﻿#include "framework.h"
#include "header.h"

#define MAX_LOADSTRING 100

namespace
{
	std::unique_ptr<Banana2D> g_game;
}

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

	if (!DirectX::XMVerifyCPUSupport())
	{
		std::cout << "Cpu Not Supported\n";
		return 1;
	}

	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
		return 1;

	g_game = std::make_unique<Banana2D>();

	{
		WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW) };
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
		}
	}

	// Message loop
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_game->Tick();
		}
	}

	g_game.reset();

	CoUninitialize();

	//// COM 초기화
	//if (SUCCEEDED(CoInitialize(nullptr)))
	//{
	//    {
	//        Banana2D D2D;

	//        if (SUCCEEDED(D2D.Initialize()))
	//            D2D.RunMessageLoop();
	//    }
	//    CoUninitialize();
	//}

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool s_in_sizemove = false;

	// 일시정지
	static bool s_in_suspend = false;

	static bool s_minimized = false;
	static bool s_fullscreen = false;

	auto game = reinterpret_cast<Banana2D*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

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

	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			if (!s_minimized)
			{
				s_minimized = true;
				if (!s_in_suspend && game)
				{

				}
				s_in_suspend = true;
			}
		}
		else if (s_minimized)
		{
			s_minimized = false;
			if (s_in_suspend && game)
			{

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