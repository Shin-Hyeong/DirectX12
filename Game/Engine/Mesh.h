#pragma once

// Mesh : 정점으로 이루어진 물체

class Mesh
{
public:
	void Init(vector<Vertex>& vec);
	void Render();

	// Mesh의 위치 정보 설정
	void SetTransform(const Transform& t) { _transform = t; }

private:
	// COM(Component Object Model)
	// - DX의 프로그래밍 언어 독립성과 하위 호환성을 가능하게 하는 기술
	// - COM 객체(COM 인터페이스)를 사용. 세부사항은 우리한테 숨겨짐
	// - ComPtr : Com객체를 받아주는  일종의 스마트 포인터(GPU쪽 데이터)
	// -- shared_ptr은 DiretX에서 함수 파라미터로 요구하는 Double Pointer를 매개변수로
	//	  전달하기 어려움이 있어 ComPtr을 사용해야한다.
	// - ComPtr로 되어 있으면 GPU에게 무언가를 요청하는 것이고 GPU내에서 실질적 객체가 생성됨
	ComPtr<ID3D12Resource>		_vertexBuffer;	// GPU 메모리에 저장됨
	D3D12_VERTEX_BUFFER_VIEW	_vertexBufferView = {}; // View를 통해 Resource를 지정하고 설명함
	uint32						_vertexCount = 0;

	// Mesh의 위치 정보
	Transform _transform = {};
};

