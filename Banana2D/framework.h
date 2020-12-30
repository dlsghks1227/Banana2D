// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
#include <wincodec.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <iostream>

// Direct2D 헤더 파일입니다.
#include <d2d1.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dwrite.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxgi.lib")

// 인터페이스 해제
template<class Interface>
inline UINT32 SafeRelease(Interface** ppInterfaceToRelease)
{
	UINT32 ret = 0;
	::IUnknown* pUnknown = nullptr;

	if (*ppInterfaceToRelease == nullptr)
		return 0;

	pUnknown = dynamic_cast<::IUnknown*>(*ppInterfaceToRelease);

	if (pUnknown == nullptr)
		return 0;

	if (*ppInterfaceToRelease != nullptr)
	{
		ret = (*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = nullptr;
	}

	return ret;
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#define Assert(b) do {if (!(b)) { OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif // DEBUG || _DEBUG
#endif

// 모듈 기본 주소
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

