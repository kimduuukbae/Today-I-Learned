#include "Common.hlsli"

Texture2D gLandScapeTexture : register(t0);
Texture2D gLandScapeDetailTexture : register(t1);

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
	float2 DetailTexCoord : DTEXCOORD;
};

VertexOut VS(VertexIn vin, uint vID : SV_VertexID)
{
	VertexOut vout;

	// Transform to world space.
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

	vout.PosW = posW.xyz;

	vout.PosH = mul(posW, gViewProj);

	vout.TexCoord = vin.TexC;

	vout.DetailTexCoord = float2((float(vID) / 257.0f) * 0.25f, float(vID % 257) * 0.25f);

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 base = gLandScapeTexture.Sample(gSamplerLinearWrap, pin.TexCoord);
	float4 detail = gLandScapeDetailTexture.Sample(gSamplerLinearWrap, pin.DetailTexCoord);
	
	float4 color = saturate(lerp(base, detail, 0.5f));
	return color;
}