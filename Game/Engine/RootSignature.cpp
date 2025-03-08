#include "pch.h"
#include "RootSignature.h"

void RootSignature::Init(ComPtr<ID3D12Device> device)
{
	// Root Signature를 생성하고 Register를 할당할 뿐 어떻게 사용할진 내용이 없음
	// - Mesh.cpp -> Render()에서 설정함
	CD3DX12_ROOT_PARAMETER param[2]; // 파라미터를 2개 생성
	// InitAsConstantBufferView 인자 중 
	// D3D12_SHADER_VISIBILITY : 각 세이더 단계 중 어느 단계에서만 Root Signature가 적용되고 소멸될것인지
	// - D3D12_SHADER_VISIBILITY_ALL : 모든 단계에서 Root Signature이 적용됨(기본 값)
	param[0].InitAsConstantBufferView(0); // API bind slot 0번을 가지고 register b0를 CBV로 사용한다
	param[1].InitAsConstantBufferView(1); // API bind slot 1번을 가지고 register b1를 CBV로 사용한다
	// CD3DX12 : d3dx12의 라이브러리
	// 2개의 CBV를 사용하겠다는 Root Signature
	D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(2, param);
	// 입력 조립기 단계(INPUT_ASSEMBLER))를 허용함
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;
	::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
	// RootSignature 생성 -> _signature에 저장
	device->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&_signature));
}