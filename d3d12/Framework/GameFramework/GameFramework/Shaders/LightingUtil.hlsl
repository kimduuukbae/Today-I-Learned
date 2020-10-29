//***************************************************************************************
// LightingUtil.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Contains API for shader lighting.
//***************************************************************************************

#define MaxLights 16

struct Light
{
    float3 Strength;
    float FalloffStart; // 포인트/스팟 라이트 전용
    float3 Direction;   // 디렉셔널/스팟 전용
    float FalloffEnd;   // 포인트/스팟전용
    float3 Position;    // 포인트전용
    float SpotPower;    // 스팟전용
};

struct Material
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Shininess; // 광택도 = 1 - Roughness
};

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // Point light와 Spot light에 적용하는 선형 감쇠 계수 계산
    return saturate((falloffEnd-d) / (falloffEnd - falloffStart));
}

// 프레넬 방정식의 슐릭 근사를 구함
// 법선이 n인 표면에서 프레넬 효과에 의해 반사되는 빛의 비율을 빛 벡터 L과 표면 법선 n 사이의 각도에 근거해서 근사함.
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
    float cosIncidentAngle = saturate(dot(normal, lightVec));

    float f0 = 1.0f - cosIncidentAngle;
    float3 reflectPercent = R0 + (1.0f - R0)*(f0*f0*f0*f0*f0);

    return reflectPercent;
}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
    //Roughness 에서 얻은 Shininess 로부터 m을 유도
    const float m = mat.Shininess * 256.0f;
    float3 halfVec = normalize(toEye + lightVec);

    float roughnessFactor = (m + 8.0f)*pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
    float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);

    float3 specAlbedo = fresnelFactor*roughnessFactor;

    // 반영 반사율 공식이 [0, 1] 구간 바깥의 값을 낼 수도 있지만
    // 우리는 LDR 렌더링을 구현하므로, 반사율을 1 미만으로 낮춘다.
    specAlbedo = specAlbedo / (specAlbedo + 1.0f);

    return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStrength;
}
// 모든 빛 벡터는 빛을 받을 점 -> 빛 이다.
//---------------------------------------------------------------------------------------
// DirectionalLight 구현
//---------------------------------------------------------------------------------------
float3 ComputeDirectionalLight(Light L, Material mat, float3 normal, float3 toEye)
{
    // 빛 벡터는 광선들이 나아가는 방향의 반대 방향임
    float3 lightVec = -L.Direction;

    // 람베르트 코사인 법칙에 따라 빛의 세기를 줄인다.
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//---------------------------------------------------------------------------------------
// Evaluates the lighting equation for point lights.
//---------------------------------------------------------------------------------------
float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    // 위치(표면)에서 Light로의 벡터
    float3 lightVec = L.Position - pos;

    // 광원과 표면 사이의 거리
    float d = length(lightVec);

    // 범위 판정
    if(d > L.FalloffEnd)
        return 0.0f;

    // 빛 벡터를 정규화한다.
    lightVec /= d;

    // 람베르트 코사인 법칙에 다라 빛의 세기를 줄인다.
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

    // 거리에 따라 빛을 감쇠한다.
    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    lightStrength *= att;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//---------------------------------------------------------------------------------------
// Evaluates the lighting equation for spot lights.
//---------------------------------------------------------------------------------------
float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    // 표면에서 광원으로의 벡터
    float3 lightVec = L.Position - pos;

    // 광원과 표면사이의 거리
    float d = length(lightVec);

    // 범위 판정
    if(d > L.FalloffEnd)
        return 0.0f;

    // 빛 벡터를 정규화한다.
    lightVec /= d;

    // 람베르트 코사인 법칙에 따라 빛의 세기를 줄인다.
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

    // 거리에 따라 빛을 감쇠한다.
    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    lightStrength *= att;

    // Spot Light 계수로 비례한다.
    float spotFactor = pow(max(dot(-lightVec, L.Direction), 0.0f), L.SpotPower);
    lightStrength *= spotFactor;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

// 여러 빛들의 누적을 위한 계산
float4 ComputeLighting(Light gLights[MaxLights], Material mat,
                       float3 pos, float3 normal, float3 toEye,
                       float3 shadowFactor)
{
    float3 result = 0.0f;

    int i = 0;

#if (NUM_DIR_LIGHTS > 0)
    for(i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        result += shadowFactor[i] * ComputeDirectionalLight(gLights[i], mat, normal, toEye);
    }
#endif

#if (NUM_POINT_LIGHTS > 0)
    for(i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS+NUM_POINT_LIGHTS; ++i)
    {
        result += ComputePointLight(gLights[i], mat, pos, normal, toEye);
    }
#endif

#if (NUM_SPOT_LIGHTS > 0)
    for(i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        result += ComputeSpotLight(gLights[i], mat, pos, normal, toEye);
    }
#endif 

    return float4(result, 0.0f);
}


