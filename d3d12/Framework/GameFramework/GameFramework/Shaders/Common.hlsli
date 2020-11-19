#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 1
#endif

#include "LightingUtil.hlsl"


cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    float2 gTexTransform;
};

cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gProj;
    float4x4 gViewProj;
    float3 gEyePosW;
    float gTotalTime;
    Light gLight[1];
    float4 gAmbient;
};

/*cbuffer cbMaterial : register(b1)
{
	float4 gDiffuseAlbedo;
    float3 gFresnelR0;
    float  gRoughness;
	float4x4 gMatTransform;
};*/
 
SamplerState gSamplerPointWrap        : register(s0);
SamplerState gSamplerPointClamp       : register(s1);
SamplerState gSamplerLinearWrap       : register(s2);
SamplerState gSamplerLinearClamp      : register(s3);
SamplerState gSamplerAnisotropicWrap  : register(s4);
SamplerState gSamplerAnisotropicClamp : register(s5);

float3 NormalMapToWorldSpace(float3 normalMapSample, float3 normalW, float3 tangentW)
{
    // 샘플링된 Normal을 [-1, 1] 사이로 사상함
    float3 normalT = 2.0f * normalMapSample - 1.0f;

    float3 N = normalW;
    float3 T = normalize(tangentW - dot(tangentW, N) * N);  // N은 단위벡터라서 크기가 1임. v2 - dot(u, v) / length(u, u) * u
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);
    
    // TBN공간에서 월드 공간으로 변경
    float3 bumpedNormalW = mul(normalT, TBN);

    return bumpedNormalW;
}

float3x3 CalcTBN(float3 normal, float3x3 world)
{
    // N
    float3 worldNorm = normalize(mul(normal, world));
    
    // T
    float3 tangent = cross(float3(0.0f, 1.0f, 0.0f), normal);
    float3 worldTant = normalize(mul(tangent, world));

    //B
    float3 biNorm = cross(normal, tangent);
    float3 worldBino = normalize(mul(biNorm, world));

    float3x3 TBN = float3x3(worldTant, worldBino, worldNorm);

    return TBN;
}