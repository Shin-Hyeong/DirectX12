#pragma once

// GetDevice(), GetCmdQueue(), GetSwapChain()을 통해
// 각 class의 기능을 Engine.h만 #include하여 사용 할 수 있도록 함
// 
// 통상적을 header 파일의 #include 내용이 방대하고 자주 값이 바꾸어지면 좋지 않음
// Header 파일에 Header파일을 하는 것은 좋지 않지만
// 원래 하나의 파일을 역할별로 분리해둔 것이기 때문에 괜찮음
#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "RootSignature.h"
#include "Shader.h"
#include "Mesh.h"

class Device;
class CommandQueue;
class SwapChain;

class Engine
{
public:
	void Init(const WindowInfo& info);

	void Render();
	// GPU에게 전달할 명령을 CommandQueue에 쌓아둠
	void RenderBegin();
	// 쌓아둔 CommandQueue를 GPU에게 전달함 -> GPU가 일을 처리하기 시작함
	void RenderEnd();


	// 윈도우 크기 조절
	// 외부에서 사이즈를 변경 요청을 받을 수 있게 사이즈를 매개변수로 만듦.
	void ResizeWindow(int width, int height);

public:	// Get / Set
	shared_ptr<Device> GetDevice() { return _device; }
	shared_ptr<CommandQueue> GetCmdQueue() { return _cmdQueue; }
	shared_ptr<SwapChain> GetSwapChain() { return _swapChain; }
	shared_ptr<RootSignature> GetRootSignature() { return _signature; }
	
private:
	// 그려질 화면 크기 관련
	WindowInfo		_window;
	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};

	// Engine의 기능을 별도의 class로 분리 관리함
	// Device : CPU와 GPU를 연결하여 GPU에 명령을 내릴 수 있도록 함
	shared_ptr<Device>				_device;
	// GPU에 명령를 하나씩 보내지 않고 한번에 모아서 일괄로 전달함
	shared_ptr<CommandQueue>		_cmdQueue;
	// Double-Buffer 생성 및 Buffer 교환 기능
	shared_ptr<SwapChain>			_swapChain;
	// GPU에 명령에 대한 데이터 공간 할당, 등
	shared_ptr<RootSignature>		_signature;
};

