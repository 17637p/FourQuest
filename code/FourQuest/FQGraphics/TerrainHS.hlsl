struct VertexOut
{
    float3 PositionW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 UV : TEXCOORD0;
    float2 BoundsY : TEXCOORD1;
};

struct PatchTess
{
    float EdgeTess[4]   : SV_TessFactor;
    float InsideTess[2] : SV_InsideTessFactor;
};

struct HullOut
{
    float3 PositionW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 UV : TEXCOORD0;
    float2 BoundsY : TEXCOORD1;
};

cbuffer cbSceneTransform : register(b0)
{
    float gMinDist;
    float gMaxDist;

    float gMinTess;
    float gMaxTess;

    float3 gEyePosW;
    float pad;
    float4 gWorldFrustumPlanes[6];
};

bool AabbBehindPlaneTest(float3 center, float3 extents, float4 plane)
{
    float3 n = abs(plane.xyz);

    float r = dot(extents, n);

    float s = dot(float4(center, 1.0f), plane);

    //return (s + e) < 0.0f; e가 뭔지 책에 안나와있다.? 745
    return (s + r) < 0.0f;
}

bool AabbOutsideFrustumTest(float3 center, float3 extents, float4 frustumPlanes[6])
{
    for(int i = 0; i < 6; i++)
    {
        if(AabbBehindPlaneTest(center, extents, frustumPlanes[i]))
        {
            return true;
        }
    }

    return false;
}

float CalcTessFactor(float3 p)
{
    float d = distance(p, gEyePosW);
    
    float s = saturate((d - gMinDist) / (gMaxDist - gMinDist));

    return pow(2, (lerp(gMaxTess, gMinTess, s)));
}

PatchTess ConstantHS(InputPatch<VertexOut, 4> patch, uint patchID : SV_PrimitiveID)
{
    PatchTess pt;
    
    float minY = patch[0].BoundsY.x;
    float maxY = patch[0].BoundsY.y;

    float3 vMin = float3(patch[2].PositionW.x, minY, patch[2].PositionW.z);
    float3 vMax = float3(patch[1].PositionW.x, maxY, patch[1].PositionW.z);

    float3 boxCenter = 0.5f * (vMin + vMax);
    float3 boxExtents = 0.5f * (vMax - vMin);

    //if (AabbOutsideFrustumTest(boxCenter, boxExtents, gWorldFrustumPlanes))
    //{
    //    pt.EdgeTess[0] = 0.0f;
    //    pt.EdgeTess[1] = 0.0f;
    //    pt.EdgeTess[2] = 0.0f;
    //    pt.EdgeTess[3] = 0.0f;
    //
    //    pt.InsideTess[0] = 0.0f;
    //    pt.InsideTess[1] = 0.0f;
    //
    //    return pt;
    //}
    //// 통상적인 방식으로, 거리에 근거해서 테셀레이션 계수들을 구한다.
    //else
    {
        float e0 = 0.5f * (patch[0].PositionW + patch[2].PositionW);
        float e1 = 0.5f * (patch[0].PositionW + patch[1].PositionW);
        float e2 = 0.5f * (patch[1].PositionW + patch[3].PositionW);
        float e3 = 0.5f * (patch[2].PositionW + patch[3].PositionW);
        float c = 0.25f * (patch[0].PositionW + patch[1].PositionW + patch[2].PositionW + patch[3].PositionW);

        //pt.EdgeTess[0] = CalcTessFactor(e0);
        //pt.EdgeTess[1] = CalcTessFactor(e1);
        //pt.EdgeTess[2] = CalcTessFactor(e2);
        //pt.EdgeTess[3] = CalcTessFactor(e3);
        //
        //pt.InsideTess[0] = CalcTessFactor(c);

        pt.EdgeTess[0] = 50;
        pt.EdgeTess[1] = 50;
        pt.EdgeTess[2] = 50;
        pt.EdgeTess[3] = 50;

        pt.InsideTess[0] = 50;
        pt.InsideTess[1] = pt.InsideTess[0];

        return pt;
    }
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
HullOut main(InputPatch<VertexOut, 4> patch, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HullOut hout;

    hout.PositionW = patch[i].PositionW;
    hout.NormalW = patch[i].NormalW;
    hout.TangentW = patch[i].TangentW;
    hout.UV = patch[i].UV;
    hout.BoundsY = patch[i].BoundsY;

    return hout;
}