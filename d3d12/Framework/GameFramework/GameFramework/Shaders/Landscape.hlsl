#include "Common.hlsli"

float CalculateTessFactor(float3 position)
{
	float d = distance(position, gEyePosW + float3(0.0f, 15.0f, -50.0f));
	float s = saturate((d - 10.0f) / (1000.0f - 10.0f)); // 선형 안개랑 똑같은 식임

	return(lerp(64.0f, 1.0f, s));
	//	return(pow(2, lerp(20.0f, 4.0f, s)));
}

Texture2D gLandScapeTexture : register(t0);
Texture2D gLandScapeDetailTexture : register(t1);

Texture2D gShadowMap : register(t2);


struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float3 PosL    : POSITION;
	float3 PosW    : POSITION1;
	float2 TexCoord : TEXCOORD;
	float2 DetailTexCoord : DTEXCOORD;
	float3 NormalW : NORMAL;
};

VertexOut VS(VertexIn vin, uint vID : SV_VertexID)
{
	VertexOut vout;

	vout.PosL = vin.PosL;
	vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz; // Transform to world space.
	vout.TexCoord = vin.TexC;
	vout.DetailTexCoord = vin.TexC;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

	return vout;
}

struct HullControlPointOut
{
	float3 PosL    : POSITION;
	float3 PosW    : POSITION1;
	float2 TexCoord : TEXCOORD;
	float2 DetailTexCoord : DTEXCOORD;
	float3 NormalW : NORMAL;
};

// HSControlPoint
[domain("quad")] // 들어오는 버텍스는 어떤 타입?
[partitioning("fractional_even")] // 나누는걸 어떻게 처리?
//[partitioning("integer")]
[outputtopology("triangle_cw")] // 감는순서
[outputcontrolpoints(25)] // 출력되는 ControlPoint
[patchconstantfunc("HSConstant")] // constant 함수 이름
[maxtessfactor(64.0f)] // 최대 계수
HullControlPointOut HSControlPoint(InputPatch<VertexOut, 25> input, uint id : SV_OutputControlPointID)
{
	HullControlPointOut hout;

	hout.PosL = input[id].PosL;
	hout.PosW = input[id].PosW;
	hout.TexCoord = input[id].TexCoord;
	hout.DetailTexCoord = input[id].DetailTexCoord;
	hout.NormalW = input[id].NormalW;

	return hout;
}

struct HullConstantOut
{
	float fTessEdges[4] : SV_TessFactor;
	float fTessInsides[2] : SV_InsideTessFactor;
};

HullConstantOut HSConstant(InputPatch<VertexOut, 25> input)
{
	HullConstantOut hout;

	float3 e0 = 0.5f * (input[0].PosW + input[4].PosW);
	float3 e1 = 0.5f * (input[0].PosW + input[20].PosW);
	float3 e2 = 0.5f * (input[4].PosW + input[24].PosW);
	float3 e3 = 0.5f * (input[20].PosW + input[24].PosW);

	hout.fTessEdges[0] = CalculateTessFactor(e0);
	hout.fTessEdges[1] = CalculateTessFactor(e1);
	hout.fTessEdges[2] = CalculateTessFactor(e2);
	hout.fTessEdges[3] = CalculateTessFactor(e3);

	float3 sum = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 25; i++) sum += input[i].PosW;
	float3 center = sum / 25.0f;
	hout.fTessInsides[0] = hout.fTessInsides[1] = CalculateTessFactor(center);

	return hout;
}

struct DomainOut
{
	float4 PosH : SV_POSITION;
	float4 PosW : POSITION;
	float2 TexCoord : TEXCOORD;
	float2 DetailTexCoord : DETAILTEXCOORD;
	float3 NormalW : NORMAL;
};

void BernsteinCoeffcient5x5(float t, out float fBernstein[5])
{
	float tInv = 1.0f - t;
	fBernstein[0] = tInv * tInv * tInv * tInv;
	fBernstein[1] = 4.0f * t * tInv * tInv * tInv;
	fBernstein[2] = 6.0f * t * t * tInv * tInv;
	fBernstein[3] = 4.0f * t * t * t * tInv;
	fBernstein[4] = t * t * t * t;
}

float3 CubicBezierSum5x5(OutputPatch<HullControlPointOut, 25> patch, float uB[5], float vB[5])
{
	float3 f3Sum = float3(0.0f, 0.0f, 0.0f);
	f3Sum = vB[0] * (uB[0] * patch[0].PosW + uB[1] * patch[1].PosW + uB[2] * patch[2].PosW + uB[3] * patch[3].PosW + uB[4] * patch[4].PosW);
	f3Sum += vB[1] * (uB[0] * patch[5].PosW + uB[1] * patch[6].PosW + uB[2] * patch[7].PosW + uB[3] * patch[8].PosW + uB[4] * patch[9].PosW);
	f3Sum += vB[2] * (uB[0] * patch[10].PosW + uB[1] * patch[11].PosW + uB[2] * patch[12].PosW + uB[3] * patch[13].PosW + uB[4] * patch[14].PosW);
	f3Sum += vB[3] * (uB[0] * patch[15].PosW + uB[1] * patch[16].PosW + uB[2] * patch[17].PosW + uB[3] * patch[18].PosW + uB[4] * patch[19].PosW);
	f3Sum += vB[4] * (uB[0] * patch[20].PosW + uB[1] * patch[21].PosW + uB[2] * patch[22].PosW + uB[3] * patch[23].PosW + uB[4] * patch[24].PosW);

	return f3Sum;
}

[domain("quad")]
DomainOut DS(HullConstantOut patchConstant, float2 uv : SV_DomainLocation, OutputPatch<HullControlPointOut, 25> patch)
{
	DomainOut dout;

	float uB[5], vB[5];
	BernsteinCoeffcient5x5(uv.x, uB);
	BernsteinCoeffcient5x5(uv.y, vB);

	dout.TexCoord = lerp(lerp(patch[0].TexCoord, patch[4].TexCoord, uv.x), lerp(patch[20].TexCoord, patch[24].TexCoord, uv.x), uv.y);
	dout.DetailTexCoord = lerp(lerp(patch[0].DetailTexCoord, patch[4].DetailTexCoord, uv.x), lerp(patch[20].DetailTexCoord, patch[24].DetailTexCoord, uv.x), uv.y);
	dout.NormalW = lerp(lerp(patch[0].NormalW, patch[4].NormalW, uv.x), lerp(patch[20].NormalW, patch[24].NormalW, uv.x), uv.y);

	float3 PosW = CubicBezierSum5x5(patch, uB, vB);
	dout.PosW = float4(PosW, 1.0f);
	dout.PosH = mul(float4(PosW, 1.0f), gViewProj);
	return dout;
}

float4 PS(DomainOut pin) : SV_Target
{
	float4 base = gLandScapeTexture.Sample(gSamplerLinearWrap, pin.TexCoord);
	float4 detail = gLandScapeDetailTexture.Sample(gSamplerLinearWrap, pin.DetailTexCoord);
	
	float4 color = saturate(lerp(base, detail, 0.75f));

	pin.NormalW = normalize(pin.NormalW);
	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
	//shadowFactor = CalcShadowFactor(pin.ShadowPosH);

	float4 pixelColor = ComputeLighting(gLight, color, pin.PosW,
		pin.NormalW, toEyeW, shadowFactor);

	//float4 ambient = gAmbient * color;
	//float4 litColor = ambient + pixelColor;
	//litColor.a = color.a;

	return pixelColor;
}