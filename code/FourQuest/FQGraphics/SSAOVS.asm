//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer cbFrustumCorners
// {
//
//   float4 gFrustumCorners[4];         // Offset:    0 Size:    64
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// cbFrustumCorners                  cbuffer      NA          NA            cb0      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// POSITION                 0   xyz         0     NONE   float   xyz 
// NORMAL                   0   xyz         1     NONE   float   x   
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float   xyzw
// TEXCOORD                 0   xyz         1     NONE   float   xyz 
// TEXCOORD                 1   xy          2     NONE   float   xy  
//
vs_5_0
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[4], dynamicIndexed
dcl_input v0.xyz
dcl_input v1.x
dcl_output_siv o0.xyzw, position
dcl_output o1.xyz
dcl_output o2.xy
dcl_temps 1
mov o0.xyz, v0.xyzx
mov o0.w, l(1.000000)
ftou r0.x, v1.x
mov o1.xyz, cb0[r0.x + 0].xyzx
add r0.xy, v0.xyxx, l(1.000000, -1.000000, 0.000000, 0.000000)
mul o2.xy, r0.xyxx, l(0.500000, -0.500000, 0.000000, 0.000000)
ret 
// Approximately 7 instruction slots used
