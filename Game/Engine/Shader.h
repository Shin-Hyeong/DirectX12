#pragma once

// [기술서] : GPU가 뭘 해야할지 기술해둔 것

class Shader
{
public:
	void Init(const wstring& path);
	void Update();

private:
	void CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode);
	void CreateVertexShader(const wstring& path, const string& name, const string& version);
	void CreatePixelShader(const wstring& path, const string& name, const string& version);

private:
	// COM(Component Object Model)
	// - DX의 프로그래밍 언어 독립성과 하위 호환성을 가능하게 하는 기술
	// - COM 객체(COM 인터페이스)를 사용. 세부사항은 우리한테 숨겨짐
	// - ComPtr : Com객체를 받아주는  일종의 스마트 포인터
	// -- shared_ptr은 DiretX에서 함수 파라미터로 요구하는 Double Pointer를 매개변수로
	//	  전달하기 어려움이 있어 ComPtr을 사용해야한다.
	// - ComPtr로 되어 있으면 GPU에게 무언가를 요청하는 것이고 GPU내에서 실질적 객체가 생성됨
	ComPtr<ID3DBlob>					_vsBlob;
	ComPtr<ID3DBlob>					_psBlob;
	ComPtr<ID3DBlob>					_errBlob;

	ComPtr<ID3D12PipelineState>			_pipelineState;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC  _pipelineDesc = {};

};

