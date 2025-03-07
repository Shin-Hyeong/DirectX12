#include "pch.h"
#include "SwapChain.h"

void SwapChain::Init(const WindowInfo& info, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue)
{
	CreateSwapChain(info, dxgi, cmdQueue);
	CreateRTV(device);
}
// 지정된 Buffer을 화면에 출력 시킴
void SwapChain::Present()
{
	_swapChain->Present(0, 0);
}

void SwapChain::SwapIndex()
{
	// GPU에서 작업 중인 버퍼의 인덱스 변경
	// 0 or 1
	_backBufferIndex = (_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
}

// swapChain Init
void SwapChain::CreateSwapChain(const WindowInfo& info, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue)
{
	// 이전에 만든 정보를 날림
	_swapChain.Reset();

	// 사용할 버퍼의 정보를 담을 변수
	DXGI_SWAP_CHAIN_DESC sd;

	// 버퍼를 화면에 출력해야하니까 화면의 비율과 같은 해상도로 설정함
	sd.BufferDesc.Width = static_cast<uint32>(info.width); // 버퍼의 해상도 너비
	sd.BufferDesc.Height = static_cast<uint32>(info.height); // 버퍼의 해상도 높이

	sd.BufferDesc.RefreshRate.Numerator = 60; // 화면 갱신 비율
	sd.BufferDesc.RefreshRate.Denominator = 1; // 화면 갱신 비율

	// R = 8bit, G = 8bit, B = 8bit, A = 8bit -> 32bit로 이루어짐
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 버퍼의 디스플레이 형식

	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1; // 멀티 샘플링 OFF
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 후면 버퍼에 렌더링 할것
	sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT; // 전면 + 후면 버퍼 = 2
	sd.OutputWindow = info.hwnd;
	sd.Windowed = info.windowed; // 윈도우 모드(전체화면, 창모드)
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 전면 후면 버퍼 교체 시 이전 프레임 정보 버림
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// SwapChain 생성
	// _device가 아닌 dxgi를 통해서 생성됨
	// dxgi : device의 출력과 관련된 기능이 들어감
	// 요청한 정보(sd)대로 만들어진 결과물을 _swapChain에 저장
	dxgi->CreateSwapChain(cmdQueue.Get(), &sd, &_swapChain);

	// SwapChain이 생성되면서 간접적으로 Buffer도 같이 생성됨
	// SwapChaind에서 Buffer를 따로 꺼내서 _renderTargets에 저장함
	for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
		_swapChain->GetBuffer(i, IID_PPV_ARGS(&_rtvBuffer[i]));
}

// DescriptionHeap의 기능을 swapChain에 붙여서 같이 관리함
// 
// GPU에게 Buffer을 건낼때 필요한 정보를 넣는 RTV을 생성
// 여러 View을 통합한 DescriptionHeap 생성
void SwapChain::CreateRTV(ComPtr<ID3D12Device> device)
{
	// DescriptorHeap(DX12) == View(~DX11)
	// [서술자의 힙]으로 RTV 생성
	// DescriptorHeap은 DX11의 RTV(RenderTargetView), DSV(DepthStencilView),
	// CBV(ConstantBufferView), SRV(ShaderResourceView), UAV(UnorderedAccessView)을 포함함
	// - DX11까진 각각의 View를 따로 관리해야하지만 DX12에선 DescriptorHeap으로 하나로 묶음


	// 1개의 RenderTargetView의 사이즈 계산
	int32 _rtvHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);


	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc; // RTV의 DescrptorHeap 설정
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // Desc의 타입 = RTV
	rtvDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT; // Desc 생성 개수 = 2
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDesc.NodeMask = 0;

	// 같은 종류의 데이터끼리 배열로 관리함
	// RTV 목록 : [ ] [ ]
	// 요청한 정보대로 Desc생성 -> RTV를 담을 수 있는 2개짜리 배열이 생성됨
	device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&_rtvHeap));

	// RTV타입의 Desc의 시작 주소를 가져옴
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = _rtvHeap->GetCPUDescriptorHandleForHeapStart();

	// 비어있는 RTV타입의 Desc를 초기화시킴
	for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
	{
		// d3dx12의 편의 기능을 사용함
		// _rtvHandle[i].ptr = rtvheapBegin.ptr + i * _rtvHeapSize;
		// 0번 인덱스 = rtvHeapBegin
		// 1번 인덱스 = rtvHeapBegin + _rtvHeapSize(RTV 1개의 사이즈)
		// 
		// Handle : ptr처럼 원격으로 접근해서 제어하는 느낌의 단어
		_rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * _rtvHeapSize);

		// swapChain에 있는 원본 리소스를 가져와서 RTV를 생성함
		device->CreateRenderTargetView(_rtvBuffer[i].Get(), nullptr, _rtvHandle[i]);
	}
}
