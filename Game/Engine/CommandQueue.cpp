#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "Engine.h"

CommandQueue::~CommandQueue()
{	
	// 동적 할당된 _fenceEvent 해제
	::CloseHandle(_fenceEvent);
}

// device를 받아서 CommandQueue을 작업함
void CommandQueue::Init(ComPtr<ID3D12Device> device, shared_ptr<SwapChain> swapChain)
{
	_swapChain = swapChain;

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	// cmdQueue위치에 queueDesc형태로 만들어달라고 주문함
	device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));

	// - D3D12_COMMAND_LIST_TYPE_DIRECT : GPU가 직접 실행하는 명령 목록
	// 메모리 할당
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAlloc));

	// GPU가 하나인 시스템에서는 0으로
	// DIRECT or BUNDLE
	// Allocator
	// 초기 상태 (그리기 명령은 nullptr 지정)
	// _cmdList을 내용을 clear함. 할당된 메모리는 그대로 유지
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&_cmdList));

	// CommandList는 Close / Open 상태가 있는데
	// Open 상태에서 Command를 넣다가 Close한 다음 제출하는 개념
	// - Open 상태에서 Command를 받다가 Close되면 일괄로 제출
	_cmdList->Close();

	// CreateFence
	// - CPU와 GPU의 동기화 수단으로 쓰인다
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

// CPU와 GPU의 동기화하는 함수
// 어떠한 값을 설정하고 해당 값이 완료 될때까지 기다리는 것
// CPU가 GPU의 할일을 끝날 때까지 기다리게 만드는것
// 효율적이진 않음
void CommandQueue::WaitSync()
{
	// GPU에게 전달할 명령에 번호를 부여함
	// _fenceValue에 FencePoint을 표시함
	_fenceValue++;

	// _cmdQueue에 _fenceValue를 전달함
	// GPU가 모든 명령 처리를 완료할 때까지 새로운 _fencePoint가 설정되지 않는다
	_cmdQueue->Signal(_fence.Get(), _fenceValue);

	// GPU가 _fencePoint를 완료할때까지 기다림
	if (_fence->GetCompletedValue() < _fenceValue)
	{
		// GPU가 전달된 명령 중 _fenceValue까지 완료하였으면 _fenceEvent을 실행함
		_fence->SetEventOnCompletion(_fenceValue, _fenceEvent);

		// CPU는 _fenceEvent가 실행될때까지 기다림
		::WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

// _cmdList를 초기화하고 BackBuffer 설정
// _cmdList에 다음에 필요한 명령(그리고자 하는 것)을 추가함
void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect)
{
	// 이전에 만든 정보를 날림
	_cmdAlloc->Reset();
	_cmdList->Reset(_cmdAlloc.Get(), nullptr);


	// _swapChain에서 Buffer 리소스를 가져와서 
	// 현재 화면에 출력하고 있는 Buffer를 GPU에서 작업한 Buffer으로 교환함
	// - 교체가 바로 이루어지지 않고 Barrier형태로 만들어짐
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_swapChain->GetBackRTVBuffer().Get(), // 후면(GPU에서 작업중 인) Buffer를 가져옴
		D3D12_RESOURCE_STATE_PRESENT, // 화면 출력
		D3D12_RESOURCE_STATE_RENDER_TARGET); // 렌더링 결과물

	// Root Signature를 적용시킴
	_cmdList->SetGraphicsRootSignature(ROOT_SIGNATURE.Get());
	// Buffer를 0번 인덱스부터 다시 Write하도록 함
	GEngine->GetConstantBuffer()->Clear();

	// Barrier형태로 만들어진 결과물을 _cmdList에 저장함
	_cmdList->ResourceBarrier(1, &barrier);

	// _cmdList을 Reset했으면 설정해야함
	_cmdList->RSSetViewports(1, vp);
	_cmdList->RSSetScissorRects(1, rect);

	// 어떤 Buffer을 렌더링할지 정함
	// _descHeap을 통해 GPU가 렌더링할 Buffer을 찾음
	// GPU보고 해당 Buffer에 렌더링하라고 명령내림
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _swapChain->GetBackRTV(); // 후면 Buffer에 대한 View를 가져옴
	// 연한 하늘색을 렌더링 시킴 -> 테스트
	_cmdList->ClearRenderTargetView(backBufferView, Colors::LightSteelBlue, 0, nullptr);
	_cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, nullptr);
}

// GPU에 전달할 명령을 다 받으면 _cmdList에 있는 명령을 GPU에 전달하고 실행시킴
void CommandQueue::RenderEnd()
{
	// _swapChain에서 Buffer 리소스를 가져와서 
	// GPU에서 작업한 Buffer를 현재 화면에 출력하고 있는 Buffer으로 교환함
	// - 교체가 바로 이루어지지 않고 Barrier형태로 만들어짐
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_swapChain->GetBackRTVBuffer().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, // 렌더링 결과물
		D3D12_RESOURCE_STATE_PRESENT); // 화면 출력

	// Barrier형태로 만들어진 결과물을 _cmdList에 저장함
	_cmdList->ResourceBarrier(1, &barrier);
	// cmdList에 저장된 명령을 GPU에 전달함
	_cmdList->Close();

	// GPU가 cmdList에 있는 내용을 실행함
	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	// 화면 Buffer를 출력시킴
	_swapChain->Present();

	// 앞에 있는 명령들을 GPU가 완료될 때까지 CPU가 대기를 함
	// 해당 기능은 비효율적이지만 단순화를 위해 사용됨
	WaitSync();

	// Buffer 인덱스 교환 (0 <-> 1)
	_swapChain->SwapIndex();
}
