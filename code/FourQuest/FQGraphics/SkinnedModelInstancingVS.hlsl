#include "Common.hlsli"

#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500
#define MAX_MODEL_INSTANCE 500

struct KeyframeDesc
{
    int animIndex;
    uint currFrame;
    uint nextFrame;
    float ratio;
};

struct TweenFrameDesc
{
    KeyframeDesc curr;
    KeyframeDesc next;
    float tweenRatio;
};

struct VertexIn
{
    float3 Position : POSITION;
    float3 NormalL : NORMAL;
    float3 TangentL : TANGENT;
    float2 UV : UV;
    int4 Indices : INDICES;
    float4 Weights : WEIGHTS;
    matrix World : WORLD;
    uint InstanceID : SV_INSTANCEID;
};

struct VertexOut
{
    float4 PositionH : SV_POSITION;
    float3 PositionW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TEXCOORD0;
    float2 UV : TEXCOORD1;
    float ClipSpacePosZ : TEXCOORD2;
    float DepthView : TEXCOORD3;
    float3 NormalV : TEXCOORD4;
    float3 TangentV : TEXCOORD5;
};

cbuffer cbSceneTransform : register(b1)
{
    float4x4 cView;
    float4x4 cViewProj;
};

cbuffer cbMaterial : register(b3)
{
    ModelMaterial gModelMaterial;
};

cbuffer cbTweenBuffer : register(b4)
{
    TweenFrameDesc TweenFrames[MAX_MODEL_INSTANCE];
};

Texture2DArray TransformMap : register(t0);

matrix GetAnimationMatrix(VertexIn input)
{
    float indices[4] = { input.Indices.x, input.Indices.y, input.Indices.z, input.Indices.w };
    float weights[4] = { input.Weights.x, input.Weights.y, input.Weights.z, input.Weights.w };

    int animIndex[2];
    int currFrame[2];
    int nextFrame[2];
    float ratio[2];

    animIndex[0] = TweenFrames[input.InstanceID].curr.animIndex;
    currFrame[0] = TweenFrames[input.InstanceID].curr.currFrame;
    nextFrame[0] = TweenFrames[input.InstanceID].curr.nextFrame;
    ratio[0] = TweenFrames[input.InstanceID].curr.ratio;

    animIndex[1] = TweenFrames[input.InstanceID].next.animIndex;
    currFrame[1] = TweenFrames[input.InstanceID].next.currFrame;
    nextFrame[1] = TweenFrames[input.InstanceID].next.nextFrame;
    ratio[1] = TweenFrames[input.InstanceID].next.ratio;
    
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    matrix curr = 0;
    matrix next = 0;
    matrix transform = 0;

    for (int i = 0; i < 4; i++)
    {
		c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[0], animIndex[0], 0));
		c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[0], animIndex[0], 0));
		c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[0], animIndex[0], 0));
		c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[0], animIndex[0], 0));
		curr = matrix(c0, c1, c2, c3);

		n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], animIndex[0], 0));
		n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], animIndex[0], 0));
		n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], animIndex[0], 0));
		n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], animIndex[0], 0));
		next = matrix(n0, n1, n2, n3);

		matrix result = lerp(curr, next, ratio[0]);

        // 다음 애니메이션
        if (animIndex[1] >= 0)
        {
            c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[1], animIndex[1], 0));
            c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[1], animIndex[1], 0));
            c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[1], animIndex[1], 0));
            c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[1], animIndex[1], 0));
            curr = matrix(c0, c1, c2, c3);
        
            n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], animIndex[1], 0));
            n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], animIndex[1], 0));
            n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], animIndex[1], 0));
            n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], animIndex[1], 0));
            next = matrix(n0, n1, n2, n3);
        
            matrix nextResult = lerp(curr, next, ratio[1]);
            result = lerp(result, nextResult, TweenFrames[input.InstanceID].tweenRatio);
        }
        
        transform += mul(weights[i], curr);
    }

    return transform;
}


VertexOut main(VertexIn vin)
{
    VertexOut vout;
    
    float4x4 worldMat;
    worldMat = GetAnimationMatrix(vin);
    worldMat = mul(worldMat, vin.World);
    
    vout.PositionH = mul(float4(vin.Position, 1.f), worldMat);
    vout.PositionH = mul(vout.PositionH, cViewProj);
    
    vout.PositionW = mul(float4(vin.Position, 1.f), worldMat);
    vout.DepthView = mul(float4(vout.PositionW, 1.f), cView).z;
    
    vout.NormalW = normalize(mul(vin.NormalL, (float3x3) worldMat));
    vout.NormalV = normalize(mul(vout.NormalW, (float3x3) cView));
    
    vout.TangentW = normalize(mul(vin.TangentL, (float3x3) worldMat));
    vout.TangentV = normalize(mul(vout.TangentW, (float3x3) cView));
    
    vout.UV = mul(float4(vin.UV, 0, 1), gModelMaterial.TexTransform);
    
    vout.ClipSpacePosZ = vout.PositionH.z;
    
    return vout;
}