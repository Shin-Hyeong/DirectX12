#pragma once

// 계약서 / 결제

// CPU  ----- 명령(외주) ----> GPU
// CPU & RAM 서로 인접하여 데이터를 주고 받기 가능
// GPU & RAM 너무 멀어서 데이터 주고 받기에 좋지 않음
// - GPU에 명령(외주)를 보낼 때 데이터를 같이 보내야함
// - RAM처럼 GPU가 데이터를 저장해서 사용할 공간을 할당해야함
// - 레지스터(Registers)를 어떻게 활용할 것인지
// https://learn.microsoft.com/ko-kr/windows/win32/direct3d12/example-root-signatures

class RootSignature
{
public:
	void Init(ComPtr<ID3D12Device> device);

	ComPtr<ID3D12RootSignature> GetSignature() { return _signature; }


private:
	// COM(Component Object Model)
	// - DX의 프로그래밍 언어 독립성과 하위 호환성을 가능하게 하는 기술
	// - COM 객체(COM 인터페이스)를 사용. 세부사항은 우리한테 숨겨짐
	// - ComPtr : Com객체를 받아주는  일종의 스마트 포인터(GPU쪽 데이터)
	// -- shared_ptr은 DiretX에서 함수 파라미터로 요구하는 Double Pointer를 매개변수로
	//	  전달하기 어려움이 있어 ComPtr을 사용해야한다.
	// - ComPtr로 되어 있으면 GPU에게 무언가를 요청하는 것이고 GPU내에서 실질적 객체가 생성됨
	ComPtr<ID3D12RootSignature>		_signature;

};

