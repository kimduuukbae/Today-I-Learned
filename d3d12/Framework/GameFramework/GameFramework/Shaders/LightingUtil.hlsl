#define MaxLights 1

struct Light
{
    float3 strength;
    float pad1;
    float3 direction;
    float pad2;
};

/*struct Light
{
    float3 Strength;
    float FalloffStart; // ����Ʈ/���� ����Ʈ ����
    float3 Direction;   // �𷺼ų�/���� ����
    float FalloffEnd;   // ����Ʈ/��������
    float3 Position;    // ����Ʈ����
    float SpotPower;    // ��������
};
*/
struct Material
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Shininess; // ���õ� = 1 - Roughness
};

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // Point light�� Spot light�� �����ϴ� ���� ���� ��� ���
    return saturate((falloffEnd-d) / (falloffEnd - falloffStart));
}

// ������ �������� ���� �ٻ縦 ����
// ������ n�� ǥ�鿡�� ������ ȿ���� ���� �ݻ�Ǵ� ���� ������ �� ���� L�� ǥ�� ���� n ������ ������ �ٰ��ؼ� �ٻ���.
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
    float cosIncidentAngle = saturate(dot(normal, lightVec));

    float f0 = 1.0f - cosIncidentAngle;
    float3 reflectPercent = R0 + (1.0f - R0)*(f0*f0*f0*f0*f0);

    return reflectPercent;
}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, float4 baseColor)
{
    //Roughness ���� ���� Shininess �κ��� m�� ����
   // const float m = 256.0f * 1.0f;
    //float3 halfVec = normalize(toEye + lightVec);

//    float roughnessFactor = (m + 8.0f)*pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
  //  float3 fresnelFactor = SchlickFresnel(0.25f, halfVec, lightVec);

    //float3 specAlbedo = fresnelFactor*roughnessFactor;

    // �ݿ� �ݻ��� ������ [0, 1] ���� �ٱ��� ���� �� ���� ������
    // �츮�� LDR �������� �����ϹǷ�, �ݻ����� 1 �̸����� �����.
    //specAlbedo = specAlbedo / (specAlbedo + 1.0f);

    return baseColor.rgb /*+ specAlbedo)*/ * lightStrength;
}
// ��� �� ���ʹ� ���� ���� �� -> �� �̴�.
//---------------------------------------------------------------------------------------
// DirectionalLight ����
//---------------------------------------------------------------------------------------
float3 ComputeDirectionalLight(Light L, float4 baseColor, float3 normal, float3 toEye)
{
    // �� ���ʹ� �������� ���ư��� ������ �ݴ� ������
    float3 lightVec = -L.direction;

    // ������Ʈ �ڻ��� ��Ģ�� ���� ���� ���⸦ ���δ�.
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.strength * ndotl;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, baseColor);
}

//---------------------------------------------------------------------------------------
// Evaluates the lighting equation for point lights.
//---------------------------------------------------------------------------------------
/*float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    // ��ġ(ǥ��)���� Light���� ����
    float3 lightVec = L.Position - pos;

    // ������ ǥ�� ������ �Ÿ�
    float d = length(lightVec);

    // ���� ����
    if(d > L.FalloffEnd)
        return 0.0f;

    // �� ���͸� ����ȭ�Ѵ�.
    lightVec /= d;

    // ������Ʈ �ڻ��� ��Ģ�� �ٶ� ���� ���⸦ ���δ�.
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

    // �Ÿ��� ���� ���� �����Ѵ�.
    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    lightStrength *= att;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//---------------------------------------------------------------------------------------
// Evaluates the lighting equation for spot lights.
//---------------------------------------------------------------------------------------
float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    // ǥ�鿡�� ���������� ����
    float3 lightVec = L.Position - pos;

    // ������ ǥ������� �Ÿ�
    float d = length(lightVec);

    // ���� ����
    if(d > L.FalloffEnd)
        return 0.0f;

    // �� ���͸� ����ȭ�Ѵ�.
    lightVec /= d;

    // ������Ʈ �ڻ��� ��Ģ�� ���� ���� ���⸦ ���δ�.
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

    // �Ÿ��� ���� ���� �����Ѵ�.
    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    lightStrength *= att;

    // Spot Light ����� ����Ѵ�.
    float spotFactor = pow(max(dot(-lightVec, L.Direction), 0.0f), L.SpotPower);
    lightStrength *= spotFactor;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}
*/
// ���� ������ ������ ���� ���
float4 ComputeLighting(Light gLights[MaxLights], float4 baseColor,
                       float3 pos, float3 normal, float3 toEye,
                       float3 shadowFactor)
{
    float3 result = 0.0f;

    int i = 0;

#if (NUM_DIR_LIGHTS > 0)
    for(i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        result += shadowFactor[i] * ComputeDirectionalLight(gLights[i], baseColor, normal, toEye);
    }
#endif
    return float4(result, 0.0f);
}


