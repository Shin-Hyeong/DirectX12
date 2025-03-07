#include "pch.h"
#include "Mesh.h"
#include "Engine.h"

void Mesh::Init(vector<Vertex>& vec)
{
	// vec : 정점의 정보들이 담긴 vector -> 삼각형을 그리고자 하면 3개의 정보가 들어있음
	_vertexCount = static_cast<uint32>(vec.size()); // vec로 전달받은 정점의 갯수
	uint32 bufferSize = _vertexCount * sizeof(Vertex);

	// CD3DX12 : d3dx12 라이브러리 함수
	// Buffer에 어떤한 용도로 사용할지
	// D3D12_HEAP_TYPE_UPLOAD : CPU에서 작업한 것을 GPU로 전달하는 용도
	// - 원래는 D3D12_HEAP_TYPE_DEFAULT와 D3D12_HEAP_TYPE_UPLOAD 두개의 Buffer를 만들어서
	// - UPLOAD는 GPU에 전달하고 DEFAULT에 복사해서 GPU가 DEFAULT를 참고하여 활용해야함
	// - 문서에 따라 UPLOAD을 GPU에 전달하고 GPU가 UPLOAD를 참고해서 활용하는 2개의 역할로 사용하기도 함
	// - 해당 코드는 2개의 역할을 공용으로 사용함
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// Buffer에 할당될 크기
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	// GPU에 전달할 Buffer를 GPU 메모리에 저장하기 위하여 메모리 할당을 요청함
	// - GPU 메모리에 정점 데이터를 저장(복사)하기 위함
	//
	// _vertexBuffer 생성
	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer));

	// 삼각형 데이터를 정점 Buffer에 복사하는 과정
	// - 직접적으로 _vertexBuffer에 복사할 수 없음
	// - 1. _vertexBuffer에서 Map을 통해 임시의 데이터 저장소를 생성함
	// - 2. 임시의 데이터 저장소에 정점 데이터를 저장함
	// - 3. _vertexBuffer에서 Unmap을 통해 임시의 데이터 저장소에 더 이상 저장을 못하도록 하고 _vertexBuffer에 저장됨

	void* vertexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	// Map : _vertexBuffer의 임시의 저장소인 vertexDataBuffer를 생성함
	_vertexBuffer->Map(0, &readRange, &vertexDataBuffer);
	// vertexDataBuffer에 vec에 있는 정점 데이터를 복사함
	::memcpy(vertexDataBuffer, &vec[0], bufferSize);
	// Unmap : 더이상 임시의 저장소에 추가적인 데이터를 저장하지 못하도록 하고 
	//		_vertexBuffer에게 임시의 저장소에 있던 데이터를 넘김
	_vertexBuffer->Unmap(0, nullptr);

	// _vertexBuffer를 설명하는 View 초기화
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress(); // 어디에 위치하는지
	_vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개의 크기
	_vertexBufferView.SizeInBytes = bufferSize; // 버퍼의 크기	
}

// 실질적으로 활용할땐 View를 통해 전달하게 됨
// _cmdQueue의 RenderBegin()과 RenderEnd()의 사이에 실행됨
void Mesh::Render()
{
	// 정점들이 어떠한 형태로 연결되어 있는지
	// D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST : 삼각형 형태로 연결되어 있음
	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Slot이랑 여러가지 정보를 설정할 수 있음
	// Slot: (0~15), 현재 0번으로 설정되어 있음
	CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView);
	// 정점을 그리도록 예약
	// CmdQueue의 RenderEnd()를 통해 작업이 시작됨
	CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0);
}
