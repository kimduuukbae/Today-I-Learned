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

	//Local Lookup vector �� Skybox �� Lookup ���ͷ� ���
	vout.PosL = vin.Lookup; 

	// Lookup vector��  World space�� ����
	float4 posW = mul(float4(vin.Lookup, 1.0f), gWorld);

	// Skybox�� ��ġ�� ī�޶��� ��ġ�� 
	posW.xyz += gEyePosW;

	vout.PosH = mul(posW, gViewProj).xyww;	// z/w == 1 �ǵ���(��, Skybox ���� �׻� �� ��鿡 �ֵ���) z = w�� ����

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return gCubeMap.Sample(gSampler, pin.PosL);
}