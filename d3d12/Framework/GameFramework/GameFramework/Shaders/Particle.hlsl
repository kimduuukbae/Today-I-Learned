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

[maxvertexcount(4)] // �ϳ��� ���� ������ 4���� Ȯ����
void GS(point VertexOut gin[1], // �Ѿ���� primitive�� point��
    inout TriangleStream<GeoOut> triStream) // TriangleStrip���� ��ȯ��Ű�� Stream
{
    // �����尡 xz ��鿡 �پ y�������� ������ ���¿��� ī�޶�
    // ���ϰ� ����� World ���� ���� ������ ��ǥ�� ���
    float3 up = float3(0.0f, 1.0f, 0.0f); // up vector�� ����
    float3 look = gEyePosW - gin[0].CenterW;
    look.y = 0.0f;  // y �� ����
    look = normalize(look);
    float3 right = cross(up, look);

    float halfWidth = 0.5f * 16.0f;
    float halfHeight = 0.5f * 16.0f;

    float4 v[4];
    // float3 + float * float3 - float * float3 == float3
    // �� x�� ���ͷ� halfWidth��ŭ, y�� ���ͷ� halfHeight��ŭ �̵��ϴ� ��.
    // 0, 1, 2, 1, 3, 2 winding order �� ����
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