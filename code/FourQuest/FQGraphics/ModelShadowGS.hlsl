
#include "Common.hlsli"

struct VertexIn
{
    float4 PositionW : SV_POSITION;
};

struct VertexOut
{
    float4 PositionH : SV_POSITION;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

cbuffer cbDirectionalShadow : register(b0)
{
    matrix cLightViewProj[CascadeCount * MaxDirectionalShadowCount];
    float4 cCascadeEnds[CascadeCount];
    int cShadowCount;
};

[maxvertexcount(27)]
void main(triangle VertexIn vin[3], inout TriangleStream<VertexOut> vout)
{
    for (uint shadowIndex = 0; shadowIndex < cShadowCount; ++shadowIndex)
    {
        for (uint faceIndex = 0; faceIndex < 3; ++faceIndex)
        {
            VertexOut element;
            element.RTIndex = shadowIndex * CascadeCount + faceIndex;
            
            for (int i = 0; i < 3; ++i)
            {
                element.PositionH = mul(vin[i].PositionW, cLightViewProj[element.RTIndex]);
                vout.Append(element);
            }
            
            vout.RestartStrip();
        }
    }
}

