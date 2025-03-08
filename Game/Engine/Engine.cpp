#include "pch.h"
#include "Engine.h"
#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "ConstantBuffer.h"

void Engine::Init(const WindowInfo& info)
{
	_window = info;

	// 초기 윈도우창 크기 설정
	ResizeWindow(info.width, info.height);

	// 그려질 화면 크기를 설정
	// TopLeftX, TopLeftY, Width, Height, MinDepth, MaxDepth
	_viewport = { 0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f };
	// Left, Top, Right, Bottom
	_scissorRect = CD3DX12_RECT(0, 0, info.width, info.height); 

	// class 생성
	_device = make_shared<Device>();
	_cmdQueue = make_shared<CommandQueue>();
	_swapChain = make_shared<SwapChain>();
	_signature = make_shared<RootSignature>();
	_constantBuffer = make_shared<ConstantBuffer>();

	// class 초기화
	// GPU설정, 디스플레이 화면 설정
	_device->Init();
	// GPU에 전달할 명령List 메모리 할당
	_cmdQueue->Init(_device->GetDevice(), _swapChain);
	// 윈도우 정보에 맞게 SwapChain(+ Buffer)생성
	_swapChain->Init(info, _device->GetDevice(), _device->GetDXGI(), _cmdQueue->GetCmdQueue());
	// GPU에 명령(외주) 보낼때 사용될 기본적인 계약서 생성
	_signature->Init(_device->GetDevice());
	// GPU의 메모리에 Buffer 공간 할당 및 데이터 전달
	// Root Signature를 통해 Buffer와 Register 연결
	// 보통 위치 정보를 넘겨주기 위함이기 때문에 Transform 사이즈로 Buffer를 생성함
	_constantBuffer->Init(sizeof(Transform), 256);
}

void Engine::Render()
{
	RenderBegin();

	// TODO : 나머지 물체들 그려줄 예약 작업
	
	
	RenderEnd();
}

void Engine::RenderBegin()
{
	_cmdQueue->RenderBegin(&_viewport, &_scissorRect);
}

void Engine::RenderEnd()
{
	_cmdQueue->RenderEnd();
}

void Engine::ResizeWindow(int width, int height)
{
	_window.width = width;
	_window.height = height;

	RECT rect = { 0, 0, width, height };
	// :: -> c++의 표준 라이브러리 제공 기능 암시
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false); // 윈도우 크기 조절
	::SetWindowPos(_window.hwnd, 0, 100, 100, width, height, 0); // 윈도우 포지션 설정
}
