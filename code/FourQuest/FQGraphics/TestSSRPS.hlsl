Texture2D tex_normal_refl_mask : register(t0);
Texture2D tex_depth : register(t1);
Texture2D tex_scene_color : register(t2);

//RWTexture2D<float4> tex_output : register(u0);

SamplerState pointSampler : register(s0);

#define MAX_ITERATION 200
#define MAX_THICKNESS 0.000025 
// 크면 정밀하지 않지만 가까이 갔을 때 갈라지는 현상이 없음 
// 작으면 정밀하고 예쁘게 나오지만 성능이 떨어지고 가까이 갔을 때 갈라지는 현상이 있음

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 uv : Texcoord;
};

// Scene 정보들 
struct SceneInfo
{
    float4x4 ViewMat;
    float4x4 ProjMat;
    float4x4 InvProjMat;
    uint2 ViewSize; // 화면 해상도
};

struct ComputePosAndReflectionOutput
{
    float3 outSamplePosInTS; // 텍스트 공간에서 샘플링하는 위치
    float3 outReflDirInTS; // 텍스트 공간에서 ReflectionVector 방향
    float outMaxDistance; // 최대 이동 거리 (화면 밖으로 벗어나지 않는? Todo: 더 공부하기)
};

struct Inter
{
    float3 intersection; // 교차점
    float intensity; // 일단 bool 값임. 메탈 있 없
};

cbuffer cbSceneInfo
{
    SceneInfo sceneInfo;
};

// Compute the position, the reflection direction, maxTraceDistance of the sample in texture space.
// 반사 방향 계산, 위치 계산, 텍스트 공간에서 샘플의 최대추적거리 계산
ComputePosAndReflectionOutput ComputePosAndReflection(
                                float2 tid,
                                const SceneInfo sceneInfo,
                                float3 vSampleNormalInVS,
                                ComputePosAndReflectionOutput output)
{
    //float sampleDepth = tex_depth[tid].r;
    float sampleDepth = tex_depth.Sample(pointSampler, tid);
    float4 samplePosInCS =  float4((float2(tid))*2-1.0f, sampleDepth, 1);
    samplePosInCS.y *= -1;

    float4 samplePosInVS = mul(samplePosInCS, sceneInfo.InvProjMat);
    samplePosInVS /= samplePosInVS.w;

    float3 vCamToSampleInVS = normalize(samplePosInVS.xyz);
    float4 vReflectionInVS = float4(reflect(vCamToSampleInVS.xyz, vSampleNormalInVS.xyz),0);

    float4 vReflectionEndPosInVS = samplePosInVS + vReflectionInVS;
    vReflectionEndPosInVS /= (vReflectionEndPosInVS.z < 0 ? vReflectionEndPosInVS.z : 1);
    float4 vReflectionEndPosInCS = mul(float4(vReflectionEndPosInVS.xyz, 1), sceneInfo.ProjMat);
    vReflectionEndPosInCS /= vReflectionEndPosInCS.w;
    float3 vReflectionDir = normalize((vReflectionEndPosInCS - samplePosInCS).xyz);

    // Transform to texture space
    samplePosInCS.xy *= float2(0.5f, -0.5f);
    samplePosInCS.xy += float2(0.5f, 0.5f);
    
    vReflectionDir.xy *= float2(0.5f, -0.5f);
    
    output.outSamplePosInTS = samplePosInCS.xyz;
    output.outReflDirInTS = vReflectionDir;
    
	// Compute the maximum distance to trace before the ray goes outside of the visible area.
    output.outMaxDistance = output.outReflDirInTS.x>=0 ? (1 - output.outSamplePosInTS.x)/output.outReflDirInTS.x  : -output.outSamplePosInTS.x/output.outReflDirInTS.x;
    output.outMaxDistance = min(output.outMaxDistance, 
                            output.outReflDirInTS.y<0 ? (-output.outSamplePosInTS.y/output.outReflDirInTS.y)  : ((1-output.outSamplePosInTS.y)/output.outReflDirInTS.y));
    output.outMaxDistance = min(output.outMaxDistance, 
                            output.outReflDirInTS.z<0 ? (-output.outSamplePosInTS.z/output.outReflDirInTS.z) : ((1-output.outSamplePosInTS.z)/output.outReflDirInTS.z));

    return output;
}

Inter FindIntersection_Linear(ComputePosAndReflectionOutput posAndReflection)
{
    float3 vReflectionEndPosInTS = posAndReflection.outSamplePosInTS + posAndReflection.outReflDirInTS * posAndReflection.outMaxDistance;
    
    float3 dp = vReflectionEndPosInTS.xyz - posAndReflection.outSamplePosInTS.xyz;
    int2 sampleScreenPos = int2(posAndReflection.outSamplePosInTS.xy * sceneInfo.ViewSize.xy);
    int2 endPosScreenPos = int2(vReflectionEndPosInTS.xy * sceneInfo.ViewSize.xy);
    int2 dp2 = endPosScreenPos - sampleScreenPos;
    const int max_dist = max(abs(dp2.x), abs(dp2.y));
    dp /= max_dist;
    
    float4 rayPosInTS = float4(posAndReflection.outSamplePosInTS.xyz + dp, 0);
    float4 vRayDirInTS = float4(dp.xyz, 0);
	float4 rayStartPos = rayPosInTS;

    int hitIndex = -1;
    for(int i = 0;i<=max_dist && i<MAX_ITERATION;i += 4)
    {
        float depth0 = 0;
        float depth1 = 0;
        float depth2 = 0;
        float depth3 = 0;
        
        float4 rayPosInTS0 = rayPosInTS+vRayDirInTS*0;
        float4 rayPosInTS1 = rayPosInTS+vRayDirInTS*1;
        float4 rayPosInTS2 = rayPosInTS+vRayDirInTS*2;
        float4 rayPosInTS3 = rayPosInTS+vRayDirInTS*3;
        
        depth3 = tex_depth.SampleLevel(pointSampler, rayPosInTS3.xy, 0).x;
        depth2 = tex_depth.SampleLevel(pointSampler, rayPosInTS2.xy, 0).x;
        depth1 = tex_depth.SampleLevel(pointSampler, rayPosInTS1.xy, 0).x;
        depth0 = tex_depth.SampleLevel(pointSampler, rayPosInTS0.xy, 0).x;
        
        {
            float thickness = rayPosInTS3.z - depth3;
            hitIndex = (thickness>=0 && thickness < MAX_THICKNESS) ? (i+3) : hitIndex;
        }
        {
            float thickness = rayPosInTS2.z - depth2;
            hitIndex = (thickness>=0 && thickness < MAX_THICKNESS) ? (i+2) : hitIndex;
        }
        {
            float thickness = rayPosInTS1.z - depth1;
            hitIndex = (thickness>=0 && thickness < MAX_THICKNESS) ? (i+1) : hitIndex;
        }
        {
            float thickness = rayPosInTS0.z - depth0;
            hitIndex = (thickness>=0 && thickness < MAX_THICKNESS) ? (i+0) : hitIndex;
        }
        
        if(hitIndex != -1) break;
        
        rayPosInTS = rayPosInTS3 + vRayDirInTS;
    }

    bool intersected = hitIndex >= 0;
    Inter resultInter; 
    resultInter.intersection = rayStartPos.xyz + vRayDirInTS.xyz * hitIndex;
	
    resultInter.intensity = intersected ? 1 : 0;
	
    return resultInter;
}

float4 ComputeReflectedColor(Inter inter,
			                float4 skyColor)
{
	float4 ssr_color = tex_scene_color.SampleLevel(pointSampler, inter.intersection.xy, 0);

    return lerp(skyColor, ssr_color, inter.intensity);
}

float4 main(VertexOut vertexOut) : SV_TARGET
{    
    float4 finalColor = 0;
    
    float x = vertexOut.uv.x;
    float y = vertexOut.uv.y;

    //float4 NormalAndReflectionMask = tex_normal_refl_mask[int2(x, y)];
    float4 NormalAndReflectionMask = tex_normal_refl_mask.Sample(pointSampler, vertexOut.uv);
    float4 color = tex_scene_color.Sample(pointSampler, vertexOut.uv);
    float4 normalInWS = float4(normalize(NormalAndReflectionMask.xyz), 0);
    
    float3 normal = mul(normalInWS, sceneInfo.ViewMat).xyz;
    float reflection_mask = NormalAndReflectionMask.w;

    //float4 skyColor = float4(1,0,1,1);
    float4 skyColor = float4(0,0,0,1);
	
    float4 reflectionColor = 0;
    Inter inter;
    ComputePosAndReflectionOutput value;
    if(reflection_mask != 0)
    {
	    reflectionColor = skyColor; // skybox에서 sampling 해야할 듯
        value.outReflDirInTS = 0;
        value.outSamplePosInTS = 0;
        value.outMaxDistance = 0;

        // Compute the position, the reflection vector, maxTraceDistance of this sample in texture space.
        value = ComputePosAndReflection(float2(x, y), sceneInfo, normal, value);

        // Find intersection in texture space by tracing the reflection ray
        //float3 intersection = 0;
	    //float intensity = FindIntersection_Linear(value, intersection);
	    inter = FindIntersection_Linear(value);
			
	    // Compute reflection color if intersected
	    reflectionColor = ComputeReflectedColor(inter, skyColor);
    }

    // Add the reflection color to the color of the sample.
    finalColor = lerp(color, reflectionColor, reflection_mask);// color + reflectionColor;
    //finalColor = color + reflectionColor;

    //tex_output.write(color, tid);
    return float4(finalColor);
    //tex_output[dispatchThreadID.xy] = color;
    //tex_output[dispatchThreadID.xy] = reflectionColor;
    //tex_output[dispatchThreadID.xy] = float4(inter.intersection, inter.intensity);
    //tex_output[dispatchThreadID.xy] = float4(value.outReflDirInTS, inter.intensity);
}

