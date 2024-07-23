Texture2D tex_no    rmal_refl_mask : register(t0);
Texture2D tex_depth : register(t1);
Texture2D tex_scene_color : register(t2);

RWTexture2D<float4> tex_output : register(u0);

SamplerState pointSampler : register(s0);

struct SceneInfo
{
    float4x4 ViewMat;
    float4x4 ProjMat;
    float4x4 InvProjMat;
    uint2 ViewSize; // 화면 해상도
};

struct ComputePosAndReflectionOutput
{
    float3 outSamplePosInTS;
    float3 outReflDirInTS;
    float outMaxDistance;
};

struct Inter
{
    float3 intersection;
    float intensity;
};

cbuffer cbSceneInfo
{
    SceneInfo sceneInfo;
};

// Compute the position, the reflection direction, maxTraceDistance of the sample in texture space.
void ComputePosAndReflection(uint2 tid,
                             const SceneInfo sceneInfo,
                             float3 vSampleNormalInVS,
                             ComputePosAndReflectionOutput output)
{
    float sampleDepth = tex_depth[tid].r;
    float4 samplePosInCS =  float4(((float2(tid)+0.5)/sceneInfo.ViewSize)*2-1.0f, sampleDepth, 1);
    samplePosInCS.y *= -1;

    float4 samplePosInVS = mul(samplePosInCS, sceneInfo.InvProjMat);
    samplePosInVS /= samplePosInVS.w;

    float3 vCamToSampleInVS = normalize(samplePosInVS.xyz);
    float4 vReflectionInVS = float4(reflect(vCamToSampleInVS.xyz, vSampleNormalInVS.xyz),0);

    float4 vReflectionEndPosInVS = samplePosInVS + vReflectionInVS * 1000;
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
}

Inter FindIntersection_Linear(ComputePosAndReflectionOutput posAndReflection)
{
    float3 vReflectionEndPosInTS = samplePosInTS + vReflDirInTS * maxTraceDistance;
    float3 vReflectionEndPosInTS = posAndReflection.outSamplePosInTS + 
    
    float3 dp = vReflectionEndPosInTS.xyz - samplePosInTS.xyz;
    int2 sampleScreenPos = int2(samplePosInTS.xy * sceneInfo.ViewSize.xy);
    int2 endPosScreenPos = int2(vReflectionEndPosInTS.xy * sceneInfo.ViewSize.xy);
    int2 dp2 = endPosScreenPos - sampleScreenPos;
    const int max_dist = max(abs(dp2.x), abs(dp2.y));
    dp /= max_dist;
    
    float4 rayPosInTS = float4(samplePosInTS.xyz + dp, 0);
    float4 vRayDirInTS = float4(dp.xyz, 0);
	float4 rayStartPos = rayPosInTS;

    int32_t hitIndex = -1;
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

        depth3 = tex_depth.sample(pointSampler, rayPosInTS3.xy).x;
        depth2 = tex_depth.sample(pointSampler, rayPosInTS2.xy).x;
        depth1 = tex_depth.sample(pointSampler, rayPosInTS1.xy).x;
        depth0 = tex_depth.sample(pointSampler, rayPosInTS0.xy).x;

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
    intersection = rayStartPos.xyz + vRayDirInTS.xyz * hitIndex;
	
    float intensity = intersected ? 1 : 0;
	
    return intensity;
}

[numthreads(16, 16, 1)] //Todo: 다시 채워야 함
void main(int3 tid : SV_GroupThreadID
    , int3 dispatchThreadID : SV_DispatchThreadID)
{    
    float4 finalColor = 0;
    
    int x = dispatchThreadID.x;
    int y = dispatchThreadID.y;

    float4 NormalAndReflectionMask = tex_normal_refl_mask[int2(x, y)];
    float4 color = tex_scene_color[int2(x, y)];
    float4 normalInWS = float4(normalize(NormalAndReflectionMask.xyz), 0);
    
    float3 normal = mul(normalInWS, sceneInfo.ViewMat).xyz;
    float reflection_mask = NormalAndReflectionMask.w;

    float4 skyColor = float4(0,0,1,1);
	
    float4 reflectionColor = 0;
    if(reflection_mask != 0)
    {
	    reflectionColor = skyColor; // skybox에서 sampling 해야할 듯
        ComputePosAndReflectionOutput value;
        value.outReflDirInTS = 0;
        value.outSamplePosInTS = 0;
        value.outMaxDistance = 0;
        
        //float3 samplePosInTS = 0;
        //float3 vReflDirInTS = 0;
        //float maxTraceDistance = 0;

        // Compute the position, the reflection vector, maxTraceDistance of this sample in texture space.
        ComputePosAndReflection(uint2(x, y), sceneInfo, normal, value);

        // Find intersection in texture space by tracing the reflection ray
        //float3 intersection = 0;
	    //float intensity = FindIntersection_Linear(value, intersection);
	    Inter inter = FindIntersection_Linear(value);
			
	    // Compute reflection color if intersected
	    //reflectionColor = ComputeReflectedColor(intensity, intersection, skyColor, tex_scene_color);
    }

    // Add the reflection color to the color of the sample.
    finalColor = color + reflectionColor;

    //tex_output.write(color, tid);
}


/*
kernel void kernel_screen_space_reflection_linear(texture2d<float, access::sample> tex_normal_refl_mask [[texture(0)]],
                                                  texture2d<float, access::sample> tex_depth [[texture(1)]],
                                                  texture2d<float, access::sample> tex_scene_color [[texture(2)]],
                                                  texture2d<float, access::write> tex_output [[texture(3)]],
                                                  const constant SceneInfo& sceneInfo [[buffer(0)]],
                                                  uint2 tid [[thread_position_in_grid]])
{
    float4 finalColor = 0;

    float4 NormalAndReflectionMask = tex_normal_refl_mask.read(tid);
    float4 color = tex_scene_color.read(tid);
    float4 normalInWS = float4(normalize(NormalAndReflectionMask.xyz), 0);
    float3 normal = (sceneInfo.ViewMat * normalInWS).xyz;
    float reflection_mask = NormalAndReflectionMask.w;

    float4 skyColor = float4(0,0,1,1);
	
    float4 reflectionColor = 0;
    if(reflection_mask != 0)
    {
	reflectionColor = skyColor;
        float3 samplePosInTS = 0;
        float3 vReflDirInTS = 0;
        float maxTraceDistance = 0;

        // Compute the position, the reflection vector, maxTraceDistance of this sample in texture space.
        ComputePosAndReflection(tid, sceneInfo, normal, tex_depth, samplePosInTS, vReflDirInTS, maxTraceDistance);

        // Find intersection in texture space by tracing the reflection ray
        float3 intersection = 0;
	float intensity = FindIntersection_Linear(samplePosInTS, vReflDirInTS, maxTraceDistance, tex_depth, sceneInfo, intersection);
			
	// Compute reflection color if intersected
	reflectionColor = ComputeReflectedColor(intensity, intersection, skyColor, tex_scene_color);
    }

    // Add the reflection color to the color of the sample.
    finalColor = color + reflectionColor;

    tex_output.write(color, tid);
}*/

/*
// Compute the position, the reflection direction, maxTraceDistance of the sample in texture space.
void ComputePosAndReflection(uint2 tid,
                             const constant SceneInfo& sceneInfo,
                             float3 vSampleNormalInVS,
                             texture2d<float, access::sample> tex_depth,
                             thread float3& outSamplePosInTS,
                             thread float3& outReflDirInTS,
                             thread float& outMaxDistance)
{
    float sampleDepth = tex_depth.read(tid).x;
    float4 samplePosInCS =  float4(((float2(tid)+0.5)/sceneInfo.ViewSize)*2-1.0f, sampleDepth, 1);
    samplePosInCS.y *= -1;

    float4 samplePosInVS = sceneInfo.InvProjMat * samplePosInCS;
    samplePosInVS /= samplePosInVS.w;

    float3 vCamToSampleInVS = normalize(samplePosInVS.xyz);
    float4 vReflectionInVS = float4(reflect(vCamToSampleInVS.xyz, vSampleNormalInVS.xyz),0);

    float4 vReflectionEndPosInVS = samplePosInVS + vReflectionInVS * 1000;
    vReflectionEndPosInVS /= (vReflectionEndPosInVS.z < 0 ? vReflectionEndPosInVS.z : 1);
    float4 vReflectionEndPosInCS = sceneInfo.ProjMat * float4(vReflectionEndPosInVS.xyz, 1);
    vReflectionEndPosInCS /= vReflectionEndPosInCS.w;
    float3 vReflectionDir = normalize((vReflectionEndPosInCS - samplePosInCS).xyz);

    // Transform to texture space
    samplePosInCS.xy *= float2(0.5f, -0.5f);
    samplePosInCS.xy += float2(0.5f, 0.5f);
    
    vReflectionDir.xy *= float2(0.5f, -0.5f);
    
    outSamplePosInTS = samplePosInCS.xyz;
    outReflDirInTS = vReflectionDir;
    
	// Compute the maximum distance to trace before the ray goes outside of the visible area.
    outMaxDistance = outReflDirInTS.x>=0 ? (1 - outSamplePosInTS.x)/outReflDirInTS.x  : -outSamplePosInTS.x/outReflDirInTS.x;
    outMaxDistance = min(outMaxDistance, outReflDirInTS.y<0 ? (-outSamplePosInTS.y/outReflDirInTS.y)  : ((1-outSamplePosInTS.y)/outReflDirInTS.y));
    outMaxDistance = min(outMaxDistance, outReflDirInTS.z<0 ? (-outSamplePosInTS.z/outReflDirInTS.z) : ((1-outSamplePosInTS.z)/outReflDirInTS.z));
}*/