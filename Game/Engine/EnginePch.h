#pragma once

// Engine, Client 둘다 자주 사용 될 라이브러리

// include
#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>

#include "d3dx12.h" // MS 비공식 지원 라이브러리
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>


// lib
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")



// typedef
using int8		= __int8;
using int16		= __int16;
using int32		= __int32;
using int64		= __int64;
using uint8		= unsigned __int8;
using uint16	= unsigned __int16;
using uint32	= unsigned __int32;
using uint64	= unsigned __int64;
using namespace std;

// 그래픽카드
#define DEVICE GEngine->GetDevice()->GetDevice()
// CommandList
#define CMD_LIST GEngine->GetCmdQueue()->GetCmdList()
// Root_Signature
#define ROOT_SIGNATURE GEngine->GetRootSignature()->GetSignature()

// DirectX
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;
using Vec2		= XMFLOAT2; // (float, float)
using Vec3		= XMFLOAT3; // (float, float, float)
using Vec4		= XMFLOAT4; // (float, float, float, float)
using Matrix	= XMMATRIX;

enum
{
	SWAP_CHAIN_BUFFER_COUNT = 2
};

struct WindowInfo
{
	HWND	hwnd;		// 출력 윈도우
	int32	width;		// 너비
	int32	height;		// 높이
	bool	windowed;	// 창모드 or 전체화면
};

struct Vertex
{
	Vec3	pos;	// (X, Y, Z)
	Vec4	color;  // (R, G, B, A)
};

// class Engine을 Client(다른 class)에서 사용하기 위한 extern 전역 변수로 선언함
extern unique_ptr<class Engine> GEngine;
