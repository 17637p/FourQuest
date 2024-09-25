
/*=============================================================================
		Constant
=============================================================================*/
static const float PI = 3.141592;
static const float Epsilon = 0.00001;
static const float3 Fdielectric = 0.04;
static const int MaxDirectionalShadowCount = 3;
static const int CascadeCount = 3;
static const float ShadowMapWidth = 1024.f;

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
    float3 color;
    float intensity;
    float3 direction;
    uint lightMode; // 조명 타입{ Mixed, RealTime }에 따른 분기를 위해 추가_홍지환
};

struct PointLight
{
    float3 color;
    float intensity;
    float3 position;
    float range;
    float3 attenuation;
    uint lightMode; // 조명 타입{ Mixed, RealTime }에 따른 분기를 위해 추가_홍지환
};

struct SpotLight
{
    float3 color;
    float intensity;
    float3 position;
    float range;
    float3 direction;
    float spot;
    float3 attenuation;
    uint lightMode; // 조명 타입{ Mixed, RealTime }에 따른 분기를 위해 추가_홍지환
};

struct Material
{
    float3 albedo;
    float roughness;
    float metallic;
};

// unity attenuation func
float DistanceAttenuation(float distanceSqr, float2 distanceAttenuation)
{
    // Calculate the basic distance attenuation
    float lightAtten = 1.0 / distanceSqr; // Using reciprocal function to avoid division

    // Extract the attenuation parameters from the float2 vector
    float distanceAttenuationX = distanceAttenuation.x;
    float distanceAttenuationY = distanceAttenuation.y;

    // Compute the factor and smoothFactor for distance attenuation
    float factor = distanceSqr * distanceAttenuationX;
    float smoothFactor = saturate(1.0 - factor * factor);
    smoothFactor = smoothFactor * smoothFactor;

    // Apply the smooth factor to the basic distance attenuation
    return lightAtten * smoothFactor;
}

float AngleAttenuation(float3 spotDirection, float3 lightDirection, float2 spotAngles)
{
    // Calculate the dot product between the spotlight direction and light direction
    float SdotL = dot(spotDirection, lightDirection);
    
    // Convert the spotAngles from degrees to radians
    float innerAngleRad = spotAngles.x * 3.141592 / 180; //0.0174532925; // Degrees to radians (PI/180)
    float outerAngleRad = spotAngles.y * 3.141592 / 180; //0.0174532925; // Degrees to radians (PI/180)

    // Compute the cosines of the inner and outer angles
    float cosInnerAngle = cos(innerAngleRad);
    float cosOuterAngle = cos(outerAngleRad);

    // Calculate the inverse angle range
    float invAngleRange = 1.0 / (cosInnerAngle - cosOuterAngle);

    // Calculate the attenuation based on the dot product and angle range
    float attenuation = (SdotL - cosOuterAngle); // * invAngleRange;

    // Clamp the attenuation between 0 and 1
    float clampedAtten = saturate(attenuation);

    // Square the clamped attenuation for smooth falloff
    return clampedAtten * clampedAtten;
}

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
        
    //Todo: 터레인 노말은 멀쩡한데 라이팅 연산을 하면 값이 inf가 나올 때가 있다 확인해봐야 하는 것
    float3 result = (diffuse + specular) * NdotL * directionLight.color * directionLight.intensity;
    result = clamp(result, 0.0, 1.0);
    return result;
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

    // float att = DistanceAttenuation(d * d, float2(pointLight.attenuation.yz));
    float att = 1.0f / dot(pointLight.attenuation, float3(1.0f, d, d * d));
    resultColor *= att;
    
    resultColor = clamp(resultColor, 0.0, 1.0);
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
    spotLight.direction = normalize(spotLight.direction);

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

    // 이전 방식
    float spot = pow(max(dot(-lightVector, spotLight.direction), 0.0f), spotLight.spot);
    float att = spot / dot(spotLight.attenuation, float3(1.0f, d, d * d));
    
    // 유니티 방식
    //float angleAttenuation = AngleAttenuation(spotLight.direction, -lightVector, float2(spotLight.spot, spotLight.spot + 1));
    //float distanceAttenuation = DistanceAttenuation(d * d, spotLight.attenuation.yz); // 함수 형태 이상해서 일단 보류
    //float att = angleAttenuation * 1 / dot(spotLight.attenuation, float3(1.0f, d, d * d));
    resultColor *= att;
    
    resultColor = clamp(resultColor, 0.0, 1.0);
    return resultColor;
}

/*=============================================================================
		etc.
=============================================================================*/


struct DirectionalShadow
{
    float4x4 cShadowViewProj[9];
    int ShadowCount;
    int3 bShadowUsed;
};

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

float CalculateShadowRatio(SamplerComparisonState samShadow, Texture2D shadowMap, float4 shadowPosTex, float shadowSize)
{
    shadowPosTex.xyz /= shadowPosTex.w;
    
    float delta = 1.f / shadowSize; // 그림자 맵 사이즈
    
    float shadowRatio = 0.f;
    
    // 3x3 커널
    const float2 offsets[9] =
    {
        float2(-delta, -delta),
        float2(0, -delta),
        float2(+delta, -delta),
        
        float2(-delta, 0),
        float2(0, 0),
        float2(+delta, 0),
        
        float2(-delta, +delta),
        float2(0, +delta),
        float2(+delta, +delta)
    };
    
    // 판정 결과를 보간해서 그림자 비율을 구함
    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        shadowRatio += shadowMap.SampleCmpLevelZero(samShadow, shadowPosTex.xy + offsets[i], shadowPosTex.z).r;
    }
    
    return shadowRatio /= 9.f;
}

float CalculateCascadeShadowRatio(SamplerComparisonState samShadow, Texture2DArray shadowMap, float4 shadowPosTex, uint index, float shadowSize)
{
    if (shadowPosTex.x < -1.f
        || shadowPosTex.x > 1.f
        || shadowPosTex.y < -1.f
        || shadowPosTex.y > 1.f
        || shadowPosTex.z < -1.f
        || shadowPosTex.z > 1.f)
    {
        return 1.f;
    }
    
    float delta = 1.f / shadowSize; // 그림자 맵 사이즈
    
    float shadowRatio = 0.f;
    
    // 3x3 커널
    const float2 offsets[9] =
    {
        float2(-delta, -delta),
        float2(0, -delta),
        float2(+delta, -delta),
        
        float2(-delta, 0),
        float2(0, 0),
        float2(+delta, 0),
        
        float2(-delta, +delta),
        float2(0, +delta),
        float2(+delta, +delta)
    };
    
    // 판정 결과를 보간해서 그림자 비율을 구함
    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        shadowRatio += shadowMap.SampleCmpLevelZero(samShadow, float3(shadowPosTex.xy + offsets[i], index), shadowPosTex.z).r;
    }
    
    return shadowRatio /= 9.f;
}

// https://learnopengl.com/Guest-Articles/2020/OIT/Weighted-Blended 참고
bool IsApproximatelyEqual(float a, float b)
{
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * Epsilon;
}

uint GetEmitterIndex(uint emitterProperties)
{
    return emitterProperties & 0xffff;
}

float GetTextureOffset(uint emitterProperties)
{
    uint index = (emitterProperties & 0x000f0000) >> 16;

    return (float) index / 2.0f;
}

bool IsStreakEmitter(uint emitterProperties)
{
    return (emitterProperties >> 24) & 0x01 ? true : false;
}

uint WriteEmitterProperties(uint emitterIndex, uint textureIndex, bool isStreakEmitter)
{
    uint properties = emitterIndex & 0xffff;

    properties |= textureIndex << 16;

    if (isStreakEmitter)
    {
        properties |= 1 << 24;
    }

    return properties;
}

// Function to calculate the streak radius in X and Y given the particles radius and velocity
float2 calcEllipsoidRadius(float radius, float2 viewSpaceVelocity)
{
    float minRadius = radius * max(1.0, 0.1 * length(viewSpaceVelocity));
    return float2(radius, minRadius);
}

// this creates the standard Hessian-normal-form plane equation from three points, 
// except it is simplified for the case where the first point is the origin
float3 CreatePlaneEquation(float3 b, float3 c)
{
    return normalize(cross(b, c));
}

// point-plane distance, simplified for the case where 
// the plane passes through the origin
float GetSignedDistanceFromPlane(float3 p, float3 eqn)
{
    // dot( eqn.xyz, p.xyz ) + eqn.w, , except we know eqn.w is zero 
    // (see CreatePlaneEquation above)
    return dot(eqn, p);
}

float3 RGBtoHSV(float3 rgb)
{
    float cmax = max(rgb.r, max(rgb.g, rgb.b));
    float cmin = min(rgb.r, min(rgb.g, rgb.b));
    float delta = cmax - cmin;

    float3 hsv;
    hsv.z = cmax; // value
    if (cmax != 0)
        hsv.y = delta / cmax; // saturation
    else
    {
        hsv.y = 0;
        hsv.x = -1;
        return hsv;
    }

    if (rgb.r == cmax)
        hsv.x = (rgb.g - rgb.b) / delta; // between yellow & magenta
    else if (rgb.g == cmax)
        hsv.x = 2 + (rgb.b - rgb.r) / delta; // between cyan & yellow
    else
        hsv.x = 4 + (rgb.r - rgb.g) / delta; // between magenta & cyan

    hsv.x /= 6;
    if (hsv.x < 0)
        hsv.x += 1;
    return hsv;
}

float3 HSVtoRGB(float3 hsv)
{
    float h = hsv.x * 6;
    float s = hsv.y;
    float v = hsv.z;

    int i = floor(h);
    float f = h - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    float3 rgb;
    if (i == 0)
        rgb = float3(v, t, p);
    else if (i == 1)
        rgb = float3(q, v, p);
    else if (i == 2)
        rgb = float3(p, v, t);
    else if (i == 3)
        rgb = float3(p, q, v);
    else if (i == 4)
        rgb = float3(t, p, v);
    else
        rgb = float3(v, p, q);

    return rgb;
}

float GetBrightness(float3 color)
{
    return dot(color.rgb, float3(0.299, 0.587, 0.114));
}

float4 OverlayMode(float4 src, float4 dst)
{
    float4 result;
    result.r = (dst.r < 0.5) ? (2.0 * src.r * dst.r) : (1.0 - 2.0 * (1.0 - src.r) * (1.0 - dst.r));
    result.g = (dst.g < 0.5) ? (2.0 * src.g * dst.g) : (1.0 - 2.0 * (1.0 - src.g) * (1.0 - dst.g));
    result.b = (dst.b < 0.5) ? (2.0 * src.b * dst.b) : (1.0 - 2.0 * (1.0 - src.b) * (1.0 - dst.b));
    result.a = src.a * dst.a; // Assuming alpha is multiplied
    return result;
}

float4 ColorMode(float4 src, float4 dst)
{
    float3 srcHsv = RGBtoHSV(src.rgb);
    float3 dstHsv = RGBtoHSV(dst.rgb);
    float3 resultRgb = HSVtoRGB(float3(srcHsv.r, srcHsv.g, dstHsv.b));
    return float4(resultRgb, src.a);
}

#define RENDER_MODE_ADDITIVE 0
#define RENDER_MODE_SUBTRACTIVE 1
#define RENDER_MODE_MODULATE 2
#define RENDER_MODE_ALPHA_BLEND 3

#define COLOR_MODE_MULTIPLY 0
#define COLOR_MODE_ADDITIVE 1
#define COLOR_MODE_SUBTRACT 2
#define COLOR_MODE_OVERLAY 3
#define COLOR_MODE_COLOR 4
#define COLOR_MODE_DIFFERENCE 5

struct ParticleMaterial
{
    float4 BaseColor;
    float4 EmssiveColor;
    float4x4 TexTransform;
    
    int RenderMode;
    int ColorMode;
    int bUseAlbedo;
    int bUseEmissive;
    
    int bUseMultiplyAlpha;
    float AlphaCutoff;
};

struct ModelMaterial
{
    float4x4 TexTransform;
    float4 BaseColor;
    float4 EmissiveColor;
    float4 DissolveOutlineStartColor;
    float4 DissolveOutlineEndColor;
    float4 DissolveOutlineStartEmissive;
    float4 DissolveOutlineEndEmissive;
    float4 RimColor;
    float4 InvRimColor;
    
    float Metalness;
    float Roughness;
    bool UseAlbedoMap;
    bool UseMetalnessMap;
  
    bool UseRoughnessMap;
    bool UseNormalMap;
    bool UseEmissiveMap;
    float AlphaCutoff;
    
    float EmissiveIntensity;
    bool UseMetalnessSmoothness;
    bool UseDissolve;
    float OutlineThickness;

    float DissolveCutoff;
    bool bUseRimLight;
    float RimPow;
    float RimIntensity;

    bool bUseInvRimLight;
    float InvRimPow;
    float InvRimIntensity;
    bool UseMulEmissiveAlpha;
};

#define DISSOLVE_ADDITIVE 0
#define DISSOLVE_SUBTRACTIVE 1
#define DISSOLVE_MODULATE 2

#define LIGHT_MODE_REALTIME 0
#define LIGHT_MODE_MIXED 1
#define LIGHT_MODE_BAKED 2