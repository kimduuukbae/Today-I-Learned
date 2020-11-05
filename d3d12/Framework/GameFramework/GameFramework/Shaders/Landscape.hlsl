#include "Common.hlsli"

Texture2D gLandScapeTexture : register(t0);
Texture2D gHeightTexture : register(t1);

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float3 PosW    : POSITION;
	float2 TexCoord : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	float offset = gHeightTexture.SampleLevel(gSamplerLinearWrap, vin.TexC, 0).r;

	offset = 2.0f * offset - 1.0f;

	vin.PosL.y += 140.0f * offset;

	// Transform to world space.
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

	vout.PosW = posW.xyz;

	vout.PosH = mul(posW, gViewProj);

	vout.TexCoord = vin.TexC;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return gLandScapeTexture.Sample(gSamplerLinearWrap, pin.TexCoord);
}