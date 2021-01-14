#include "framework.h"
#include "header.h"

#define MAX_LOADSTRING 100

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

    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr))
        return 1;

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

        HWND hwnd = CreateWindowExW(
            0,
            wcex.lpszClassName,
            L"Banana",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            nullptr,
            nullptr,
            HINST_THISCOMPONENT,
            nullptr);

        hr = hwnd ? S_OK : E_FAIL;

        if (SUCCEEDED(hr))
        {
            ShowWindow(hwnd, nCmdShow);

            //SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>());

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

        }
    }

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
    return DefWindowProc(hWnd, message, wParam, lParam);
}