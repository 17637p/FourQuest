Texture2D tex_normal_refl_mask : register(t0);
Texture2D tex_depth : register(t1);
Texture2D tex_scene_color : register(t2);

RWTexture2D<float4> tex_output : register(u0);

SamplerState defaultSampler : register(s0);

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
        float3 intersection = 0;
	    //float intensity = FindIntersection_Linear(value, tex_depth, sceneInfo, intersection);
			
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