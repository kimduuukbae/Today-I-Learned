#include "Common.hlsli"

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

    vout.PosH = mul(posW, gViewProj);
	
    return vout;
}

// 어짜피 Null render target이라 void 로 해놓고, 이대로 끝내면 됨.
// 깊이버퍼에는 쓰여질 것
void PS(VertexOut pin) 
{

}


