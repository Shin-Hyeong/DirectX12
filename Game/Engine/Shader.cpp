#include "pch.h"
#include "Shader.h"
#include "Engine.h"

void Shader::Init(const wstring& path)
{
	// 렌더링 파이프 라인의 Vertex Shader 생성
	// - _vsBlob에 VertexShader의 Resource 파일 저장
	CreateVertexShader(path, "VS_Main", "vs_5_0");
	// 렌더링 파이프 라인의 Pixel Shader 생성
	// - _psBlob에 PixelShader의 Resource 파일 저장
	CreatePixelShader(path, "PS_Main", "ps_5_0");

	D3D12_INPUT_ELEMENT_DESC desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// 렌더링 파이프라인에 대한 정보
	_pipelineDesc.InputLayout = { desc, _countof(desc) };
	// 어떤 정책(어떤 Buffer, 어떤 레지스터리)를 사용할 것인지
	_pipelineDesc.pRootSignature = ROOT_SIGNATURE.Get();

	_pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	_pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	_pipelineDesc.DepthStencilState.DepthEnable = FALSE;
	_pipelineDesc.DepthStencilState.StencilEnable = FALSE;
	_pipelineDesc.SampleMask = UINT_MAX;
	_pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	_pipelineDesc.NumRenderTargets = 1;
	_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	_pipelineDesc.SampleDesc.Count = 1;

	// 그래픽 카드에서 _pipelineDesc의 정보대로 렌더링 파이프라인을 생성하고 _pipelineState에 저장함
	DEVICE->CreateGraphicsPipelineState(&_pipelineDesc, IID_PPV_ARGS(&_pipelineState));
}

void Shader::Update()
{
	// cmdList에 파이프라인을 설정하는 명령어를 추가함
	CMD_LIST->SetPipelineState(_pipelineState.Get());
}

void Shader::CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode)
{
	uint32 compileFlag = 0;
#ifdef _DEBUG
	compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	if (FAILED(::D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, name.c_str(), version.c_str(), compileFlag, 0, &blob, &_errBlob)))
	{
		::MessageBoxA(nullptr, "Shader Create Failed !", nullptr, MB_OK);
	}

	shaderByteCode = { blob->GetBufferPointer(), blob->GetBufferSize() };
}

void Shader::CreateVertexShader(const wstring& path, const string& name, const string& version)
{
	// path 경로에 있는 파일을 읽어서 입력한 name과 version이 유효하면
	// _vsBlob에 name와 version에 맞게 VertexShader과 관련된 Resource가 저장됨
	CreateShader(path, name, version, _vsBlob, _pipelineDesc.VS);
}

void Shader::CreatePixelShader(const wstring& path, const string& name, const string& version)
{
	// path 경로에 있는 파일을 읽어서 입력한 name과 version이 유효하면
	// _psBlob에 name와 version에 맞게 PixelShdaer과 관련된 Resource가 저장됨
	CreateShader(path, name, version, _psBlob, _pipelineDesc.PS);
}