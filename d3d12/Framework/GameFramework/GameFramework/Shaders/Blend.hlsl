#include "Common.hlsli"

Texture2D gDiffuse : register(t0);
Texture2D gWaves : register(t1);

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
	VertexOut vout = (VertexOut)0.0f;
	
    vout.TexCoord = vin.TexC * gTexTransform;
    vout.TexCoord += (gTotalTime / 10.0f);

    float color = gWaves.SampleLevel(gSamplerLinearWrap, vout.TexCoord, 0).a;
    vin.PosL.y += color * 100.0f;
    

    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;

    vout.PosH = mul(posW, gViewProj);

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float4 color = gDiffuse.Sample(gSamplerLinearWrap, pin.TexCoord);
    color.a *= 0.25f;
    return color;
}


