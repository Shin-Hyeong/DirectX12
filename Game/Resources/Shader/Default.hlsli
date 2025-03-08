// C++이 아닌 Shader 문법을 사용해야함

// cbuffer : Constant Buffer
cbuffer TEST_BO : register(b0) // Constant Buffer를 사용하기 위해 register b0을 사용
{
    // 외부에서 offset 값을 설정해주도록 함
    float4 offset0;
}

cbuffer TEST_B1 : register(b1) // Constant Buffer를 사용하기 위해 register b1을 사용
{
    float4 offset1;
}

struct VS_IN
{
    // float3 : (float, float, float)
    float3 pos : POSITION;
    // float4 : (float, float, float, float)
    float4 color : COLOR;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

// Vertex Shader 단계
// - 정점을 어떻게 변화할지 정하는 단계
VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // (input.pos, 1.f)으로 float4으로 바꾸고 output.pos에 저장함
    output.pos = float4(input.pos, 1.f);
    output.pos += offset0;      // 설정된 offset만큼 변화를 줌
    output.color = input.color;
    output.color += offset1;    // 설정된 offset만큼 변화를 줌

    return output;
}

// Pixel Shader 단계
// - Pixel의 색상을 정하는 단계
float4 PS_Main(VS_OUT input) : SV_Target
{
    return input.color;
}