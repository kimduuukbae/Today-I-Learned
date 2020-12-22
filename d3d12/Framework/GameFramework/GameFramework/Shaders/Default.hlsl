#include "Common.hlsli"

Texture2D gDiffuse : register(t0);
Texture2D gNormal : register(t1);

Texture2D gShadowMap : register(t2);

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
    float4 ShadowPosH : POSITION1;
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

    vout.ShadowPosH = mul(posW, gShadowTransform);

    return vout;
}

float CalcShadowFactor(float4 shadowPosH)
{
    // w로 나눈 투영 공간
    shadowPosH.xyz /= shadowPosH.w;

    // NDC 공간안의 depth 값을 가져옴
    float depth = shadowPosH.z;

    uint width, height, numMips;
    gShadowMap.GetDimensions(0, width, height, numMips);

    // 텍셀 크기
    float dx = 1.0f / (float)width;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += gShadowMap.SampleCmpLevelZero(gSamplerShadow,
            shadowPosH.xy + offsets[i], depth).r;
    }

    return percentLit / 9.0f;
}


float4 PS(VertexOut pin) : SV_Target
{
    float4 color = gDiffuse.Sample(gSamplerLinearWrap, pin.TexCoord);

    float4 normalMapSample = gNormal.Sample(gSamplerLinearWrap, pin.TexCoord);

    float3 normalT = 2.0f * normalMapSample.rgb - 1.0f;
    
    float3 bumpedNormalW = mul(normalT, pin.TBN);

   //  한 점에서 카메라로 가는 방향벡터
    float3 toEyeW = normalize(gEyePosW - pin.PosW);
  
    float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
    shadowFactor = CalcShadowFactor(pin.ShadowPosH);

    float4 pixelColor = ComputeLighting(gLight, color, pin.PosW,
        bumpedNormalW, toEyeW, shadowFactor);

    float4 ambient = gAmbient * color;
    float4 litColor = ambient + pixelColor;
    litColor.a = color.a;

    return litColor;
}


