#pragma once

class SwapChain;
class DescriptorHeap;

// DirectX 9 시절에는 Device를 통해서 바로 그래픽카드에 요청을 하면 되었음
// DirectX 12로 오면서 CommandQueue과정이 추가됨.
// GPU에게 명령을 할떄 명령이 생길때마다 보내는 것이 아닌 Queue에 한번에 모아서 일괄로 보냄
// https://learn.microsoft.com/ko-kr/windows/win32/direct3d12/design-philosophy-of-command-queues-and-command-lists

class CommandQueue
{
public:
	~CommandQueue();

	void Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain);
	void WaitSync();


	// 실질적으로 게임이 렌더링되는 함수
	// _cmdList에 실행할 명령을 저장
	void RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect);
	// _cmdList에 저장된 명령을 GPU에 전달 및 실행
	void RenderEnd();

	// GET / SET
	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetCmdList() { return _cmdList; }

private:
	// COM(Component Object Model)
	// - DX의 프로그래밍 언어 독립성과 하위 호환성을 가능하게 하는 기술
	// - COM 객체(COM 인터페이스)를 사용. 세부사항은 우리한테 숨겨짐
	// - ComPtr : Com객체를 받아주는  일종의 스마트 포인터(GPU쪽 데이터)
	// -- shared_ptr은 DiretX에서 함수 파라미터로 요구하는 Double Pointer를 매개변수로
	//	  전달하기 어려움이 있어 ComPtr을 사용해야한다.
	// - ComPtr로 되어 있으면 GPU에게 무언가를 요청하는 것이고 GPU내에서 실질적 객체가 생성됨
	// 
	// CommandQueue : DX12에 등장
	// 외주를 요청할 때, 하나씩 요청하면 비효율적
	// [외주 목록]에 일감을 차곡차곡 기록했다가 한 방에 요청하는 것
	ComPtr<ID3D12CommandQueue>			_cmdQueue;
	ComPtr<ID3D12CommandAllocator>		_cmdAlloc;	// 일감을 저장하는 메모리할당
	ComPtr<ID3D12GraphicsCommandList>	_cmdList;	// 일감 List

	// Fence : 울타리
	// CPU / GPU 동기화를 위한 간단한 도구
	// 상황에 따라 일감이 처리되어야 다음 단계를 진행할 수 있는 상황이 있음
	// 외주로 준 일감이 끝날 때까지 기다리겠다
	// CommandQueue와 연동해서 사용함
	// - ex) 105번 일감이 끝날 때까지 대기하겠다고 할때 fence를 사용함
	ComPtr<ID3D12Fence>		_fence;
	uint32					_fenceValue = 0;
	HANDLE					_fenceEvent = INVALID_HANDLE_VALUE;

	shared_ptr<SwapChain>		_swapChain;
};

