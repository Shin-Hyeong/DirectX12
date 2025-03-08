#include "pch.h"
#include "ConstantBuffer.h"
#include "Engine.h"

ConstantBuffer::ConstantBuffer()
{

}

// _cbvBuffer를 UnMap하고 메모리 해제
ConstantBuffer::~ConstantBuffer()
{
	if (_cbvBuffer)
	{
		if (_cbvBuffer != nullptr)
			_cbvBuffer->Unmap(0, nullptr);

		_cbvBuffer = nullptr;
	}
}

// 요구한 사이즈와 개수만큼 Buffer생성
void ConstantBuffer::Init(uint32 size, uint32 count)
{
	// 상수 버퍼는 256바이트 배수 크기로 만들어야 한다
	// 0 256 512 768
	// 255 : 0000 0000 1111 1111 
	// -> ~255 : 0000 0000 1111 1111를 반전 시킨 값 -> 1111 1111 0000 0000
	// & : 비트 단위로 둘다 1이면 1을 반환
	// (size + 255) & ~255
	// 만약 0000 0001 1011 0010 & 1111 1111 0000 0000 이면 0000 0001 0000 0000(256)이 됨
	_elementSize = (size + 255) & ~255;
	_elementCount = count;

	CreateBuffer();
}

// 0번 인덱스 Buffer부터 다시 사용함
void ConstantBuffer::Clear()
{
	_currentIndex = 0;
}

// rootParamIndex : RootTable의 인덱스
void ConstantBuffer::PushData(int32 rootParamIndex, void* buffer, uint32 size)
{
	// Debug Code
	// 사용할려는 Idx가 할당한 Buffer보다 크면 메모리를 벗어남으로 Crash를 발생시킴
	assert(_currentIndex < _elementSize);

	// _mappedBuffer의 지정된 인덱스에 전달할 데이터(buffer)를 복사함
	::memcpy(&_mappedBuffer[_currentIndex * _elementSize], buffer, size);

	// _cbvBuffer의 _curretnIndex 주소를 받음
	D3D12_GPU_VIRTUAL_ADDRESS address = GetGpuVirtualAddress(_currentIndex);
	// Root Table의 인덱스와 Buffer의 주소를 연결함
	// - Root Signature로 설정한 Register와 Buffer를 연결
	// - _cmdQueue가 나중에 일괄적으로 보내질때 실행됨
	CMD_LIST->SetGraphicsRootConstantBufferView(rootParamIndex, address);
	// 다음 Buffer로 인덱스 넘김
	_currentIndex++;
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGpuVirtualAddress(uint32 index)
{
	// _cbvBuffer의 GPUVirtualAddress를 가져옴(시작 주소)
	D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = _cbvBuffer->GetGPUVirtualAddress();
	// 시작주소 + (Buffer의 크기 * 요구 인덱스)의 주소를 반환받음
	objCBAddress += index * _elementSize;
	return objCBAddress;
}

// 요구한 만큼의 사이즈와 개수의 Buffer 생성
void ConstantBuffer::CreateBuffer()
{
	// Buffer 크기 * Buffer 개수
	uint32 bufferSize = _elementSize * _elementCount;
	// UPLOAD타입의 Buffer를 생성
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	// UPLOAD타입의 Buffer를 BufferSize만큼 GPU메모리에 할당받음
	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_cbvBuffer));

	// Resource를 완료될때까지 UnMap() 필요없음
	//  - 매 Frame마다 Buffer에서 GPU에서 사용하기 때문에
	// GPU에서 Resource를 사용하는 동안에는 Resource를 Write안됨
	// _mappedBuffer에 있는 데이터를 _cbvBuffer에 저장함
	// 동기화 기술을 사용해야함(해당 코드에선 cmdQueue에서 Fence를 통해 동기화함)
	_cbvBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_mappedBuffer));
}
