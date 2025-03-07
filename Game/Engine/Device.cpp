#include "pch.h"
#include "Device.h"

void Device::Init()
{
#ifdef _DEBUG // _DEBUG 가 Define 되어 있으면 하단 코드 컴파일
	// D3D12 디버그층 활성화
	// - VC++ 출력창에 상세한 디버깅 메시지 출력
	// - riid : 디바이스의 COM ID
	// - ppDevice : 생성된 장치가 매개변수에 설정
	// Debug 용도로만 사용, Debug할때 추가적인 정보를줌.
	//
	// IID_PPV_ARGS : 매개변수로 넘겨준 값의 어떠한 ID를 출력하고 추가적인 작업을 함
	// ID(riid), **ppvDebug(이중 포인터)을 출력함
	::D3D12GetDebugInterface(IID_PPV_ARGS(&_debugController));
	_debugController->EnableDebugLayer();

#endif // _DEBUG와 상관없이 컴파일
	// DXGI(DirectX Graphics Infrastructrue) : 화면 디스플레이 관련 기능
	// Direct3D와 함께 쓰이는 API
	// - 전체 화면 모드 전환
	// - 지원되는 디스플레이 모드 열거 등
	// 
	// CreateDXGIFactory
	// - riid : 디바이스의 COM ID
	// - ppDevice: 생성된 장치가 매개변수에 설정
	::CreateDXGIFactory(IID_PPV_ARGS(&_dxgi));

	// CreateDevice
	// - 디스플레이 어댑터(그래픽 카드)를 나타내는 객체
	// -- 그래픽카드에 어떠한 기능을 요청할때 _device를 통해서 요청됨.
	// 
	// - pAdapter : nullptr 지정하면 시스템 기본 디스플레이 어댑터
	// - MinimumFeatureLevel : 응용 프로그램이 요구하는 최소 기능 수준 (성능이 안좋으면 걸러짐)
	// -- D3D_FEATURE_LEVEL_11_0 = DirectX 11을 지원하는 그래픽카드를 요구
	// - riid : 디바이스의 COM ID
	// - ppDevice: 생성된 장치가 매개변수에 설정
	::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device));
}
