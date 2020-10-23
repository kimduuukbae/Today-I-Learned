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

	// 월드 공간으로 이동
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;

	// 균일하지 않은 Scaling 가정, 그렇지 않으면 월드 행렬의 역변환을 사용해야함.
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
	
	// 물체 공간 T벡터를 월드 공간으로 이동
	vout.TangentW = mul(vin.TangentU, (float3x3)gWorld);

	// 동차 절단 공간으로 이동
	vout.PosH = mul(posW, gViewProj);

	float4 texC = mul(float4(vin.TexCoord, 0.0f, 1.0f), gTexTransform);
	vout.TexCoord = mul(texC, gMatTransform).xy;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 albedo = gTexture.Sample(gSampler, pin.TexCoord) * gDiffuseAlbedo;

	// 법선을 보간하면, 단위 벡터가 아닐수도 있으니 다시 정규화 함
	pin.NormalW = normalize(pin.NormalW);

	float4 normalMapSample = gNormal.Sample(gSampler, pin.TexCoord);
	float3 bumpedNormalW = NormalMapToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW);

	//bumpedNormalW = pin.NormalW;

	// 월드 공간에서, 한 점에서 눈으로 가는 방향 벡터
	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// 간접광
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
	float toEyeVDist = length(toEyeW); // v1 - v0 의 length는 곧 distance임.
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
	// range == 안개 시작 지점에서 물체가 안개에 완전히 묻혀서 보이지 않게 되는 지점
	float fogAmount = saturate((toEyeVDist - 5.0f) / 10.0f);
	litColor = lerp(litColor, float4(0.0f, 0.0f, 0.0f, 1.0f), fogAmount);
	litColor.a = albedo.a;

	return litColor;
}
