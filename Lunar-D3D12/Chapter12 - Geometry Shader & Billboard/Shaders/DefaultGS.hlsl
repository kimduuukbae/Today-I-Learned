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

Texture2DArray gTextureArray : register(t0);
SamplerState gStaticSamplerWrap : register(s0);
SamplerState gStaticSamplerAnis : register(s1);

// Constant data that varies per material.
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

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    Light gLights[MaxLights];
};
cbuffer cbTime : register(b3)
{
    float time;
};
 
struct VertexIn
{
	float3 PosW    : POSITION;
    float2 SizeW : SIZE;
};

struct VertexOut
{
    float3 CenterW : POSITION;
    float2 SizeW : SIZE;
};

struct GeoOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexC : TEXCOORD;
    uint PrimID : SV_PrimitiveID;
};

VertexOut VS(VertexIn vin)
{
    VertexOut output;
    output.CenterW = vin.PosW;
    output.SizeW = vin.SizeW;
    return output;
}

[maxvertexcount(4)] // �ϳ��� ���� ������ 4���� Ȯ����
void GS(point VertexOut gin[1], // �Ѿ���� primitive�� point��
    uint primID : SV_PrimitiveID,
    inout TriangleStream<GeoOut> triStream) // TriangleStrip���� ��ȯ��Ű�� Stream
{
    // �����尡 xz ��鿡 �پ y�������� ������ ���¿��� ī�޶�
    // ���ϰ� ����� World ���� ���� ������ ��ǥ�� ���
    float3 up = float3(0.0f, 1.0f, 0.0f); // up vector�� ����
    float3 look = gEyePosW - gin[0].CenterW;
    look.y = 0.0f;  // y �� ����
    look = normalize(look);
    float3 right = cross(up, look);

    float halfWidth = 0.5f * gin[0].SizeW.x;
    float halfHeight = 0.5f * gin[0].SizeW.y;

    float4 v[4];
    // float3 + float * float3 - float * float3 == float3
    // �� x�� ���ͷ� halfWidth��ŭ, y�� ���ͷ� halfHeight��ŭ �̵��ϴ� ��.
    // 0, 1, 2, 1, 3, 2 winding order �� ����
    v[0] = float4(gin[0].CenterW + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(gin[0].CenterW + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(gin[0].CenterW - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(gin[0].CenterW - halfWidth * right + halfHeight * up, 1.0f);

    GeoOut gout;
    float2 texC[4] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f)
    };

    [unroll]
    for (int i = 0; i < 4; ++i) {   // �Ѿ���� vertex�� �� world �������� ��ġ������
        gout.PosH = mul(v[i], gViewProj);
        gout.PosW = v[i].xyz;
        gout.NormalW = look;    // object���� ī�޶�� ���ϴ� ����
        gout.TexC = texC[i];
        gout.PrimID = primID;

        triStream.Append(gout);
    }
}

float4 PS(GeoOut pin, uint prim : SV_PrimitiveID) : SV_Target
{
    float3 uvw = float3(pin.TexC, prim % 3);
    float4 albedo = gTextureArray.Sample(gStaticSamplerAnis, uvw) * gDiffuseAlbedo;

    clip(albedo.a - 0.1f);

    // Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// Indirect lighting.
    float4 ambient = gAmbientLight*albedo;

    const float shininess = 1.0f - gRoughness;
    Material mat = { albedo, gFresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW, 
        pin.NormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

    // Common convention to take alpha from diffuse material.
    litColor.a = albedo.a;

    return litColor;
}
