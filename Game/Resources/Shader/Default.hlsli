// C++이 아닌 Shader 문법을 사용해야함

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
    output.color = input.color;

    return output;
}

// Pixel Shader 단계
// - Pixel의 색상을 정하는 단계
float4 PS_Main(VS_OUT input) : SV_Target
{
    return input.color;
}