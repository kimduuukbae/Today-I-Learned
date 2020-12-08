#include "Common.hlsli"

Texture2D gDiffuse : register(t0);
Texture2D gNormal : register(t1);

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
    float3x3 TBN : TANBINNOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;
	
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

    vout.PosW = posW.xyz;

    vout.PosH = mul(posW, gViewProj);

    vout.TexCoord = vin.TexC;

    vout.TBN = CalcTBN(vin.NormalL,(float3x3)gWorld);

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float4 color = gDiffuse.Sample(gSamplerLinearWrap, pin.TexCoord);

    float4 normalMapSample = gNormal.Sample(gSamplerLinearWrap, pin.TexCoord);

    float3 normalT = 2.0f * (float3)normalMapSample - 1.0f;

    float3 bumpedNormalW = mul(normalT, pin.TBN);

   //  한 점에서 카메라로 가는 방향벡터
    float3 toEyeW = normalize(gEyePosW - pin.PosW);
  
    float3 shadowFactor = 1.0f;
    float4 pixelColor = ComputeLighting(gLight, color, pin.PosW,
        bumpedNormalW, toEyeW, shadowFactor);

    float4 ambient = gAmbient + color;
    float4 litColor = ambient + pixelColor;
    litColor.a = color.a;

    return color;
}


