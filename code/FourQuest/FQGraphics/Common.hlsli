/*=============================================================================
		Constant
=============================================================================*/
static const float PI = 3.141592;
static const float Epsilon = 0.00001;

/*=============================================================================
		PBR - BRDF
=============================================================================*/
float ndfGGX(float ndoth, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (ndoth * ndoth) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

float gaSchlickGGX(float ndotl, float ndotv, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return gaSchlickG1(ndotl, k) * gaSchlickG1(ndotv, k);
}

float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

/*=============================================================================
		Light
=============================================================================*/
struct DirectionalLight
{
    float3  color;
    float   intensity;
    float3  direction;
    float   pad;
};

struct PointLight
{
    float3  color;
    float   intensity;
    float3  position;
    float   range;
    float3  attenuation;
    float   pad;
};

struct SpotLight
{
    float3  color;
    float   intensity;
    float3  position;
    float   range;
    float3  direction;
    float   spot;
    float3  attenuation;
    float   pad;
};

struct Material
{
    float3 albedo;
    float roughness;
    float metallic;
};

float3 ComputeDirectionLight(
    DirectionalLight directionLight,
    Material material,
    float3 toEye,
    float3 normal)
{
    // Specular coefficiant - fixed reflectance value for non-metals
    static const float kSpecularCoefficient = 0.04;
    const float3 c_spec = lerp(kSpecularCoefficient, material.albedo, material.metallic);

    float3 lightVector = -directionLight.direction;
    lightVector = normalize(lightVector);
    
    // Half vector
    const float3 halfVector = normalize(lightVector + toEye);

    // products
    const float NdotV = saturate(dot(normal, toEye));
        
    const float NdotL = saturate(dot(normal, lightVector));
    const float LdotH = saturate(dot(lightVector, halfVector));
    const float NdotH = saturate(dot(normal, halfVector));
        
    // Specular
    float3 F = fresnelSchlick(c_spec, max(0.0, dot(halfVector, toEye))); // 프레넬
    float D = ndfGGX(NdotH, max(Epsilon, material.roughness)); // 정반사 분포도
    float G = gaSchlickGGX(NdotL, NdotV, material.roughness); // 기하 감쇠율
        
    float3 specular = (F * D * G) / max(Epsilon, 4.0 * NdotL * NdotV);
        
    // Diffuse 
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), material.metallic);
    float3 diffuse = kd * material.albedo / PI;
        
    return (diffuse + specular) * NdotL * directionLight.color * directionLight.intensity;
}

float3 ComputePointLight(
    PointLight pointLight,
    Material material,
    float3 toEye,
    float3 normal,
    float3 positionW)
{
    // Specular coefficiant - fixed reflectance value for non-metals
    static const float kSpecularCoefficient = 0.04;
    const float3 c_spec = lerp(kSpecularCoefficient, material.albedo, material.metallic);

    float3 lightVector = pointLight.position - positionW;
    // atteunation 에 필요 
    float d = length(lightVector);
    if (d > pointLight.range)
    {
        return float3(0, 0, 0);
    }
    
    lightVector = normalize(lightVector);

    // Half vector
    const float3 halfVector = normalize(lightVector + toEye);

    // products
    const float NdotV = saturate(dot(normal, toEye));
        
    const float NdotL = saturate(dot(normal, lightVector));
    const float LdotH = saturate(dot(lightVector, halfVector));
    const float NdotH = saturate(dot(normal, halfVector));
        
    // Specular
    float3 F = fresnelSchlick(c_spec, max(0.0, dot(halfVector, toEye))); // 프레넬
    float D = ndfGGX(NdotH, max(Epsilon, material.roughness)); // 정반사 분포도
    float G = gaSchlickGGX(NdotL, NdotV, material.roughness); // 기하 감쇠율
        
    float3 specular = (F * D * G) / max(Epsilon, 4.0 * NdotL * NdotV);
        
    // Diffuse 
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), material.metallic);
    float3 diffuse = kd * material.albedo / PI;
    
    // PointLight attenuation
    float3 resultColor = (diffuse + specular) * NdotL * pointLight.color * pointLight.intensity;

    float att = 1.0f / dot(pointLight.attenuation, float3(1.0f, d, d * d));
    resultColor *= att;
    
    return resultColor;
}

float3 ComputeSpotLight(
    SpotLight spotLight,
    Material material,
    float3 toEye,
    float3 normal,
    float3 positionW)
{
    // Specular coefficiant - fixed reflectance value for non-metals
    static const float kSpecularCoefficient = 0.04;
    const float3 c_spec = lerp(kSpecularCoefficient, material.albedo, material.metallic);

    float3 lightVector = spotLight.position - positionW;
    // atteunation 에 필요 
    float d = length(lightVector);
    if (d > spotLight.range)
    {
        return float3(0, 0, 0);
    }
    
    lightVector = normalize(lightVector);

    // Half vector
    const float3 halfVector = normalize(lightVector + toEye);

    // products
    const float NdotV = saturate(dot(normal, toEye));
        
    const float NdotL = saturate(dot(normal, lightVector));
    const float LdotH = saturate(dot(lightVector, halfVector));
    const float NdotH = saturate(dot(normal, halfVector));
        
    // Specular
    float3 F = fresnelSchlick(c_spec, max(0.0, dot(halfVector, toEye))); // 프레넬
    float D = ndfGGX(NdotH, max(Epsilon, material.roughness)); // 정반사 분포도
    float G = gaSchlickGGX(NdotL, NdotV, material.roughness); // 기하 감쇠율
        
    float3 specular = (F * D * G) / max(Epsilon, 4.0 * NdotL * NdotV);
        
    // Diffuse 
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), material.metallic);
    float3 diffuse = kd * material.albedo / PI;
    
    // SpotLight attenuation
    float3 resultColor = (diffuse + specular) * NdotL * spotLight.color * spotLight.intensity;

    float spot = pow(max(dot(-lightVector, spotLight.direction), 0.0f), spotLight.spot);
    float att = spot / dot(spotLight.attenuation, float3(1.0f, d, d * d));
    resultColor *= att;
    
    return resultColor;
}

/*=============================================================================
		etc.
=============================================================================*/
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
	// Uncompress each component from [0,1] to [-1,1].
    float3 normalT = normalize(2.0 * normalMapSample - 1.0);

	// Build orthonormal basis.
    float3 N = normalize(unitNormalW);
    float3 T = normalize(tangentW - dot(tangentW, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

	// Transform from tangent space to world space.
    float3 bumpedNormalW = mul(normalT, TBN);

    return bumpedNormalW;
}

uint QueryTextureLevels(TextureCube cubeTexture)
{
    uint width, height, levels;
    cubeTexture.GetDimensions(0, width, height, levels);
    return levels;
}