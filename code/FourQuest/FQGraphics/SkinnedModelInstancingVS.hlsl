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

float4 Slerp(float4 p0, float4 p1, float t)
{
    float dotp = dot(normalize(p0), normalize(p1));
    if ((dotp > 0.9999) || (dotp < -0.9999))
    {
        if (t <= 0.5)
            return p0;
        return p1;
    }
    float theta = acos(dotp * 3.14159 / 180.0); // float theta = acos(dotp);
    float4 P = ((p0 * sin((1 - t) * theta) + p1 * sin(t * theta)) / sin(theta));
    P.w = 1;
    return P;
}

// HLSL 함수: T * R * S (translation * rotation * scale)
float4x4 CreateTRSMatrix(float3 translation, float4 quaternion, float3 scale)
{
    // Scale Matrix
    float4x4 scaleMatrix = float4x4(
        scale.x, 0.0f, 0.0f, 0.0f,
        0.0f, scale.y, 0.0f, 0.0f,
        0.0f, 0.0f, scale.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // Quaternion to Rotation Matrix
    float xx = quaternion.x * quaternion.x;
    float yy = quaternion.y * quaternion.y;
    float zz = quaternion.z * quaternion.z;
    float xy = quaternion.x * quaternion.y;
    float xz = quaternion.x * quaternion.z;
    float yz = quaternion.y * quaternion.z;
    float wx = quaternion.w * quaternion.x;
    float wy = quaternion.w * quaternion.y;
    float wz = quaternion.w * quaternion.z;

    float4x4 rotationMatrix = float4x4(
        1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
        2.0f * (xy + wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx), 0.0f,
        2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (xx + yy), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // Translation Matrix
    float4x4 translationMatrix = float4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        translation.x, translation.y, translation.z, 1.0f
    );

    // Combined TRS matrix: T * R * S
    return mul(translationMatrix, mul(rotationMatrix, scaleMatrix));
}

float4x4 Transpose(float4x4 mat)
{
    return float4x4(
        mat._m00, mat._m10, mat._m20, mat._m30, // 첫 번째 행 -> 첫 번째 열
        mat._m01, mat._m11, mat._m21, mat._m31, // 두 번째 행 -> 두 번째 열
        mat._m02, mat._m12, mat._m22, mat._m32, // 세 번째 행 -> 세 번째 열
        mat._m03, mat._m13, mat._m23, mat._m33 // 네 번째 행 -> 네 번째 열
    );
}

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

    float4 currTranslate, currRotation, currScale;
    float4 nextTranslate, nextRotation, nextScale;
    float4 resultTranslate, resultRotation, resultScale;
    float4 nextResultTranslate, nextResultRotation, nextResultScale;
    matrix result = 0;
    matrix transform = 0;

    for (int i = 0; i < 4; i++)
    {
        // 현재 프레임 보간
        currTranslate = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[0], animIndex[0], 0));
        currRotation = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[0], animIndex[0], 0));
        currScale = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[0], animIndex[0], 0));

        nextTranslate = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], animIndex[0], 0));
        nextRotation = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], animIndex[0], 0));
        nextScale = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], animIndex[0], 0));

        resultTranslate = lerp(currTranslate, nextTranslate, ratio[0]);
        resultRotation = Slerp(currRotation, nextRotation, ratio[0]);
        resultScale = lerp(currScale, nextScale, ratio[0]);

		// 다음 프레임 보간
        if (animIndex[1] >= 0)
        {
            currTranslate = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[1], animIndex[1], 0));
            currRotation = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[1], animIndex[1], 0));
            currScale = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[1], animIndex[1], 0));

            nextTranslate = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], animIndex[1], 0));
            nextRotation = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], animIndex[1], 0));
            nextScale = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], animIndex[1], 0));
            
            nextResultTranslate = lerp(currTranslate, nextTranslate, ratio[0]);
            nextResultRotation = Slerp(currRotation, nextRotation, ratio[0]);
            nextResultScale = lerp(currScale, nextScale, ratio[0]);
            
            resultTranslate = lerp(resultTranslate, nextResultTranslate, TweenFrames[input.InstanceID].tweenRatio);
            resultRotation = Slerp(resultRotation, nextResultRotation, TweenFrames[input.InstanceID].tweenRatio);
            resultScale = lerp(resultScale, nextResultScale, TweenFrames[input.InstanceID].tweenRatio);
        }
        
        result = (CreateTRSMatrix(currTranslate.xyz, currRotation, currScale.xyz));

        transform += mul(weights[i], result);
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