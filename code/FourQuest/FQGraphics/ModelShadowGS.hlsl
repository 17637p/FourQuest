
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
    int cLightmodes0;
    int cLightmodes1;
    int cLightmodes2;
};

cbuffer cbLightCondition : register(b1)
{
    bool bIsStatic;
}

[maxvertexcount(27)]
void main(triangle VertexIn vin[3], inout TriangleStream<VertexOut> vout)
{
    int lightmodes[3];
    lightmodes[0] = cLightmodes0;
    lightmodes[1] = cLightmodes1;
    lightmodes[2] = cLightmodes2;
    
    for (uint shadowIndex = 0; shadowIndex < cShadowCount; ++shadowIndex)
    {
        if (lightmodes[shadowIndex] == LIGHT_MODE_MIXED && bIsStatic)
        {
            continue;
        }
        
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

