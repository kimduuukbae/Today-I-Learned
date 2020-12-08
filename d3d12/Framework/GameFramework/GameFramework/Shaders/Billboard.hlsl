#include "Common.hlsli"

Texture2D BillboardTexture[2] : register(t0);

struct VertexIn
{
	float3 PosW    : POSITION;
	float3 Complexity : NORMAL;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float3 CenterW : POSITION;
	float2 SizeW : SIZE;
	uint TextureIndex : TINDEX;
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
	output.CenterW = vin.PosW;
	output.SizeW = vin.Complexity.xy;
	output.TextureIndex = uint(vin.Complexity.z);
	return output;
}

[maxvertexcount(4)] // �ϳ��� ���� ������ 4���� Ȯ����
void GS(point VertexOut gin[1], // �Ѿ���� primitive�� point��
    inout TriangleStream<GeoOut> triStream) // TriangleStrip���� ��ȯ��Ű�� Stream
{
    float3 up = float3(0.0f, 1.0f, 0.0f); // up vector�� ����
    float3 look = gEyePosW - gin[0].CenterW;
    look.y = 0.0f;  // y �� ����
    look = normalize(look);
    float3 right = cross(up, look);

    float halfWidth = 0.5f * gin[0].SizeW.x;
    float halfHeight = 0.5f * gin[0].SizeW.y;

    float4 v[4];
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
    for (int i = 0; i < 4; ++i) {   // �Ѿ���� vertex�� �� world �������� ��ġ������
        gout.PosH = mul(v[i], gViewProj);
        gout.TexC = texC[i];
        gout.TextureIndex = gin[0].TextureIndex;
        triStream.Append(gout);
    }
}

float4 PS(GeoOut pin) : SV_Target
{
    float4 image = BillboardTexture[pin.TextureIndex % 2].Sample(gSamplerLinearWrap, pin.TexC);
    clip(image.a - 0.3f);
    return image;
}