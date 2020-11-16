#include "Common.hlsli"

Texture2DArray ParticleTexture : register(t0);

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NOTHING : NORMAL;
	float2 NOTHING2: TEXCOORD;
};

struct VertexOut
{
	float3 CenterW : POSITION;
    uint IndexW : INDEX;
};

struct GeoOut
{
    float4 PosH : SV_POSITION;
    float2 TexC : TEXCOORD;
    uint TextureIndex : TINDEX;
};

VertexOut VS(VertexIn vin)
{
	VertexOut output;
    output.CenterW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
    output.IndexW = uint(gTexTransform.x);
	return output;
}

[maxvertexcount(4)] // 하나의 점을 가지고 4개로 확장함
void GS(point VertexOut gin[1], // 넘어오는 primitive는 point임
    inout TriangleStream<GeoOut> triStream) // TriangleStrip으로 변환시키는 Stream
{
    // 빌보드가 xz 평면에 붙어서 y방향으로 세워진 상태에서 카메라를
    // 향하게 만드는 World 공간 기준 빌보드 좌표계 계산
    float3 up = float3(0.0f, 1.0f, 0.0f); // up vector는 고정
    float3 look = gEyePosW - gin[0].CenterW;
    look.y = 0.0f;  // y 축 정렬
    look = normalize(look);
    float3 right = cross(up, look);

    float halfWidth = 0.5f * 16.0f;
    float halfHeight = 0.5f * 16.0f;

    float4 v[4];
    // float3 + float * float3 - float * float3 == float3
    // 즉 x축 벡터로 halfWidth만큼, y축 벡터로 halfHeight만큼 이동하는 것.
    // 0, 1, 2, 1, 3, 2 winding order 를 취함
    v[0] = float4(gin[0].CenterW + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(gin[0].CenterW + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(gin[0].CenterW - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(gin[0].CenterW - halfWidth * right + halfHeight * up, 1.0f);

    float2 texC[4] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f)
    };

    GeoOut gout;
    [unroll]
    for (int i = 0; i < 4; ++i) {   // 넘어오는 vertex는 다 world 공간에서 위치해있음
        gout.PosH = mul(v[i], gViewProj);
        gout.TexC = texC[i];
        gout.TextureIndex = gin[0].IndexW;
        triStream.Append(gout);
    }
}

float4 PS(GeoOut pin) : SV_Target
{
    float4 image = ParticleTexture.Sample(gSamplerLinearWrap, float3(pin.TexC, pin.TextureIndex));
    clip(image.a - 0.99f);
    return image;
}