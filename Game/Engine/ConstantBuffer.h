#pragma once

// GPU에 할당되는 Buffer

class ConstantBuffer
{
public:
	ConstantBuffer();
	// _cbvBuffer를 UnMap하고 메모리 해제
	~ConstantBuffer();

	// 처음 요구한 사이즈와 개수만큼 Buffer생성
	void Init(uint32 size, uint32 count);

	// 0번 인덱스 Buffer부터 다시 사용함
	void Clear();
	void PushData(int32 rootParamIndex, void* buffer, uint32 size);

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(uint32 index);

private:
	// 요구한 만큼의 사이즈와 개수의 Buffer 생성
	void CreateBuffer();

private:
	// COM(Component Object Model)
	// - DX의 프로그래밍 언어 독립성과 하위 호환성을 가능하게 하는 기술
	// - COM 객체(COM 인터페이스)를 사용. 세부사항은 우리한테 숨겨짐
	// - ComPtr : Com객체를 받아주는  일종의 스마트 포인터(GPU쪽 데이터)
	// -- shared_ptr은 DiretX에서 함수 파라미터로 요구하는 Double Pointer를 매개변수로
	//	  전달하기 어려움이 있어 ComPtr을 사용해야한다.
	ComPtr<ID3D12Resource>	_cbvBuffer;
	// CPU에서 GPU로 데이터 전송할 때 걸쳐가는 임시 Buffer
	// - Mesh에서 vertexDataBuffer와 같은 역할
	BYTE*					_mappedBuffer = nullptr; 
	uint32					_elementSize = 0;	// Buffer의 크기
	uint32					_elementCount = 0;	// Buffer의 개수
	uint32					_currentIndex = 0;	// 어디까지 사용했는지

};

