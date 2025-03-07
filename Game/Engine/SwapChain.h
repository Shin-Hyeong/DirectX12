#pragma once

// SwapChain
// 
// [외주 과정]
// - 메모리에 있는 상황을 묘사
// - 어떤 물체를 어떤 공식으로 어떻게 계산할지 알려줌
// - GPU가 열심히 계산(외주 진행)
// - 결과물을 받아서 화면에 그려줌
//
// [외주 결과물]을 어디에 받는가?
// - 종이(Buffer)에 그려서 건내달라고 주문함
// - 외주를 보낼 때 종이(Buffer)을 만들음 -> GPU에 외주와 함께 종이(Buffer)를 넘김
//		-> 결과물을 해당 종이(Buffer)에 받음
// - 화면에 종이(외주 결과물이 그려진 Buffer)을 출력한다
//
// 계속 외주를 보내야하는데?
// - 매 Frame마다 외주를 보내야 다음 Frame을 연속적으로 그려짐
// - 화면에 받아온 결과물을 출력하는 와중에, 다음 화면도 외주를 보내야함
// - Buffer를 2개를 만들어서, 하나는 현재 화면을 출력하고 하나는 외주를 맡김
// - Double Buffering이라고 함
//
// [1 Buffer] [2 Buffer]
// - 현재 화면 [1 Buffer], GPU 작업 중 [2 Buffer]
// - GPU 작업 완료, [1][2] Buffer 교체
// - 현재 화면 [2 Buffer], GPU 작업 중 [1 Buffer]
// - 이 과정을 SwapChain이라고 함
// https://learn.microsoft.com/ko-kr/windows/win32/direct3d12/swap-chains

class SwapChain
{
public:
	void Init(const WindowInfo& info, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);

	// 지정된 Buffer을 화면에 출력 시킴
	void Present();
	// GPU에서 작업 중인 버퍼의 인덱스 변경
	// 0 or 1
	void SwapIndex();

	// GET / SET
	ComPtr<IDXGISwapChain> GetSwapChain() { return _swapChain; }
	// 요청한 Buffer 인덱스를 반환
	ComPtr<ID3D12Resource> GetRenderTarget(int32 index) { return _rtvBuffer[index]; }

	// 현재 GPU에서 작업중인 Buffer
	ComPtr<ID3D12Resource> GetBackRTVBuffer() { return _rtvBuffer[_backBufferIndex]; }

	/**
		Description
	*/
	D3D12_CPU_DESCRIPTOR_HANDLE GetBackRTV() { return _rtvHandle[_backBufferIndex]; }

private:
	// SwapChain 생성
	void CreateSwapChain(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);

	// RTV 생성
	void CreateRTV(ComPtr<ID3D12Device> device);

private:
	// COM(Component Object Model)
	// - DX의 프로그래밍 언어 독립성과 하위 호환성을 가능하게 하는 기술
	// - COM 객체(COM 인터페이스)를 사용. 세부사항은 우리한테 숨겨짐
	// - ComPtr : Com객체를 받아주는  일종의 스마트 포인터
	// -- shared_ptr은 DiretX에서 함수 파라미터로 요구하는 Double Pointer를 매개변수로
	//	  전달하기 어려움이 있어 ComPtr을 사용해야한다.
	// - ComPtr로 되어 있으면 GPU에게 무언가를 요청하는 것이고 GPU내에서 실질적 객체가 생성됨
	// 
	ComPtr<IDXGISwapChain>	_swapChain;
	// Buffer가 저장됨.
	// RenderTargetView(RTV) = Buffer의 View(Description)
	ComPtr<ID3D12Resource>	_rtvBuffer[SWAP_CHAIN_BUFFER_COUNT]; // 2
	// 현재 GPU에서 작업중인 Buffer의 인덱스
	uint32					_backBufferIndex = 0;


	/**
		Description Heap
	*/
	// [기안서] 
	// DirectX12에 추가됨. 기존엔 View라고 불림
	// 외주를 맡길 떄 이런 저런 정보들을 같이 넘겨줘야 하는데,
	// 아무 현태로나 요청하면 이해하지 못함
	// - 각종 리소스를 어떤 용도로 사용하는지 꼼꼼하게 적어서 넘겨주는 용도
	// - GPU에게 리소스를 보낼 때 해당 리소스가 무엇인지 서술해서 보냄
	//
	// DescriptorHeap[[View][View][ ... ] ..] -> DH안에 여려가지 View가 있음
	// View : 하나의 Resource에 대한 정보를 가지고 있음. 
	// - CPU 입장에서 GPU에게 건내준 Resource를 사용할려 할때 Resource 원본이 아닌 View를 참조함.
	// - CPU가 View를 통해 요구하면 GPU는 View에 있는 정보
	//		(어떠한 정보를 가르키는지, 어디에 저장되어 있는지)를 통해 전달함
	//
	// 필요한 View를 사용해야하는 원본 리소스에 붙여 같이 관리하기 위해
	// RTV를 swapChain과 합침.
	// https://learn.microsoft.com/ko-kr/windows/win32/direct3d12/descriptor-heaps-overview
	// 
	// 전달하는 리소스가 추가될 때마다 추가됨
	ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHandle[SWAP_CHAIN_BUFFER_COUNT];

};

