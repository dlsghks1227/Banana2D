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
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(nCmdShow);

    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    // COM 초기화
    if (SUCCEEDED(CoInitialize(nullptr)))
    {
        {
            Banana2D D2D;

            if (SUCCEEDED(D2D.Initialize()))
                D2D.RunMessageLoop();
        }
        CoUninitialize();
    }

    return 0;
}