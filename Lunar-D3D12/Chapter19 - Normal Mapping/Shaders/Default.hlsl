#include "Common.hlsli"

Texture2D gTexture : register(t0);
Texture2D gNormal : register(t1);

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float3 TangentU : TANGENT;
	float2 TexCoord : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float3 PosW    : POSITION;
	float3 NormalW : NORMAL;
	float2 TexCoord : TEXCOORD;
	float3 TangentW : TANGENT;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	// ���� �������� �̵�
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;

	// �������� ���� Scaling ����, �׷��� ������ ���� ����� ����ȯ�� ����ؾ���.
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
	
	// ��ü ���� T���͸� ���� �������� �̵�
	vout.TangentW = mul(vin.TangentU, (float3x3)gWorld);

	// ���� ���� �������� �̵�
	vout.PosH = mul(posW, gViewProj);

	float4 texC = mul(float4(vin.TexCoord, 0.0f, 1.0f), gTexTransform);
	vout.TexCoord = mul(texC, gMatTransform).xy;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 albedo = gTexture.Sample(gSampler, pin.TexCoord) * gDiffuseAlbedo;

	// ������ �����ϸ�, ���� ���Ͱ� �ƴҼ��� ������ �ٽ� ����ȭ ��
	pin.NormalW = normalize(pin.NormalW);

	float4 normalMapSample = gNormal.Sample(gSampler, pin.TexCoord);
	float3 bumpedNormalW = NormalMapToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW);

	//bumpedNormalW = pin.NormalW;

	// ���� ��������, �� ������ ������ ���� ���� ����
	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// ������
	float4 ambient = gAmbientLight * albedo;

	const float shininess = 1.0f - gRoughness;
	Material mat = { albedo, gFresnelR0, shininess };
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
		bumpedNormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	// Common convention to take alpha from diffuse material.
	litColor.a = albedo.a;

	return litColor;
}

float4 TransparencyPS(VertexOut pin) : SV_Target
{
	float4 albedo = gTexture.Sample(gSampler, pin.TexCoord) * gDiffuseAlbedo;

	// Interpolating normal can unnormalize it, so renormalize it.
	pin.NormalW = normalize(pin.NormalW);

	// Vector from point being lit to eye. 
	float3 toEyeW = gEyePosW - pin.PosW;
	float toEyeVDist = length(toEyeW); // v1 - v0 �� length�� �� distance��.
	toEyeW /= toEyeVDist;

	// Indirect lighting.
	float4 ambient = gAmbientLight * albedo;

	const float shininess = 1.0f - gRoughness;
	Material mat = { albedo, gFresnelR0, shininess };
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
		pin.NormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	// Common convention to take alpha from diffuse material.
	// distance - fogStart / fogRange
	// range == �Ȱ� ���� �������� ��ü�� �Ȱ��� ������ ������ ������ �ʰ� �Ǵ� ����
	float fogAmount = saturate((toEyeVDist - 5.0f) / 10.0f);
	litColor = lerp(litColor, float4(0.0f, 0.0f, 0.0f, 1.0f), fogAmount);
	litColor.a = albedo.a;

	return litColor;
}
