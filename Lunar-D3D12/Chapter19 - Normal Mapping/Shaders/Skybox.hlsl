#include "Common.hlsli"

TextureCube gCubeMap : register(t0);

struct VertexIn
{
	float3 Lookup    : POSITION;
	float3 NormalL : NORMAL;
	float3 TangentU : TANGENT;
	float2 TexCoord : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};


VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	//Local Lookup vector 를 Skybox 맵 Lookup 벡터로 사용
	vout.PosL = vin.Lookup; 

	// Lookup vector를  World space로 변경
	float4 posW = mul(float4(vin.Lookup, 1.0f), gWorld);

	// Skybox의 위치를 카메라의 위치로 
	posW.xyz += gEyePosW;

	vout.PosH = mul(posW, gViewProj).xyww;	// z/w == 1 되도록(즉, Skybox 구가 항상 먼 평면에 있도록) z = w로 설정

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return gCubeMap.Sample(gSampler, pin.PosL);
}