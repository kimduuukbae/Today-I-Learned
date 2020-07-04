cbuffer cbGameObjectInfo : register(b0){
	matrix gmtxWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1){
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

struct INSTANCEDGAMEOBJECTINFO {
	matrix m_mtxGameObject;
	float4 m_cColor;
};

StructuredBuffer<INSTANCEDGAMEOBJECTINFO> gGameObjectInfos : register(t0);

struct VS_INPUT{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT {
	float4 position : SV_POSITION;
};

VS_OUTPUT VSMain(VS_INPUT input, uint nInstanceID : SV_InstanceID){
	VS_OUTPUT output;
	output.position = mul(mul(mul(float4(input.position, 1.0f),
		gGameObjectInfos[nInstanceID].m_mtxGameObject),
		gmtxView),
		gmtxProjection);
	return output;
}

float4 PSMain(VS_OUTPUT output) : SV_TARGET{
	return vec4(0.0, 0.0, 0.0, 1.0);
}