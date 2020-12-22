#include "Common.hlsli"

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);

    vout.PosH = mul(posW, gViewProj);
	
    return vout;
}

// ��¥�� Null render target�̶� void �� �س���, �̴�� ������ ��.
// ���̹��ۿ��� ������ ��
void PS(VertexOut pin) 
{

}


