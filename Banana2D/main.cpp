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


	// HeapSetInformation
	// https://docs.microsoft.com/en-us/windows/win32/api/heapapi/nf-heapapi-heapsetinformation
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	// COM 초기화
	if (SUCCEEDED(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED)))
	{
		{
			Banana2D D2D;

			try
			{
				D2D.Initialize();
				D2D.RunMessageLoop();
			}
			catch (DX::com_exception e)
			{
				MessageBoxA(nullptr, e.what(), e.what(), MB_OK);
			}
		}
		CoUninitialize();
	}

	return 0;
}