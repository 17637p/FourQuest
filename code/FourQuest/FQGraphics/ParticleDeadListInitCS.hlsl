
AppendStructuredBuffer<uint> gDeadListToAddTo : register(u0);

[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    gDeadListToAddTo.Append(id.x);
}