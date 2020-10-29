cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
};
 
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
};

VertexOut VS(VertexIn vin, uint vid : SV_VertexID)
{
	VertexOut vout = (VertexOut)0.0f;
	
    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;
    if(vid == 0)
    vout.PosH = float4(-1.0f, 0.0f, 1.0f , 1.0f);

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}


