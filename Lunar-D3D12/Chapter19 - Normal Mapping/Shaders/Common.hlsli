
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 1
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

#include "LightingUtil.hlsl"


cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gTexTransform;
};

cbuffer cbMaterial : register(b1)
{
	float4 gDiffuseAlbedo;
    float3 gFresnelR0;
    float  gRoughness;
	float4x4 gMatTransform;
};

cbuffer cbPass : register(b2)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
    float4 gAmbientLight;
    Light gLights[MaxLights];
};
 
SamplerState gSampler : register(s0);

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