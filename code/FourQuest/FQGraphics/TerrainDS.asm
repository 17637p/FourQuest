//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer $Globals
// {
//
//   float2 gTexScale;                  // Offset:    0 Size:     8
//      = 0x42480000 0x42480000 
//
// }
//
// cbuffer cbModelTransform
// {
//
//   float4x4 cWorld;                   // Offset:    0 Size:    64
//   float4x4 cWorldInvTranspose;       // Offset:   64 Size:    64 [unused]
//
// }
//
// cbuffer cbSceneTransform
// {
//
//   float4x4 cView;                    // Offset:    0 Size:    64 [unused]
//   float4x4 cViewProj;                // Offset:   64 Size:    64
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// samHeightmap                      sampler      NA          NA             s0      1 
// gHeightMap                        texture  float4          2d             t0      1 
// cbModelTransform                  cbuffer      NA          NA            cb0      1 
// cbSceneTransform                  cbuffer      NA          NA            cb1      1 
// $Globals                          cbuffer      NA          NA            cb2      1 
//
//
//
// Patch Constant signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_TessFactor            0   x           0 QUADEDGE   float       
// SV_TessFactor            1   x           1 QUADEDGE   float       
// SV_TessFactor            2   x           2 QUADEDGE   float       
// SV_TessFactor            3   x           3 QUADEDGE   float       
// SV_InsideTessFactor      0   x           4  QUADINT   float       
// SV_InsideTessFactor      1   x           5  QUADINT   float       
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// POSITION                 0   xyz         0     NONE   float   x z 
// NORMAL                   0   xyz         1     NONE   float   xyz 
// TANGENT                  0   xyz         2     NONE   float   xyz 
// TEXCOORD                 0   xy          3     NONE   float   xy  
// TEXCOORD                 1     zw        3     NONE   float       
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float   xyzw
// POSITION                 0   xyz         1     NONE   float   xyz 
// TEXCOORD                 2      w        1     NONE   float      w
// NORMAL                   0   xyz         2     NONE   float   xyz 
// TANGENT                  0   xyz         3     NONE   float   xyz 
// TEXCOORD                 0   xy          4     NONE   float   xy  
// TEXCOORD                 1     zw        4     NONE   float     zw
//
// Tessellation Domain   # of control points
// -------------------- --------------------
// Quadrilateral                           4
//
ds_5_0
dcl_input_control_point_count 4
dcl_tessellator_domain domain_quad
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB2[1], immediateIndexed
dcl_constantbuffer CB0[2], immediateIndexed
dcl_constantbuffer CB1[8], immediateIndexed
dcl_sampler s0, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_input vDomain.xy
dcl_input vicp[4][0].xz
dcl_input vicp[4][1].xyz
dcl_input vicp[4][2].xyz
dcl_input vicp[4][3].xy
dcl_output_siv o0.xyzw, position
dcl_output o1.xyz
dcl_output o1.w
dcl_output o2.xyz
dcl_output o3.xyz
dcl_output o4.xy
dcl_output o4.zw
dcl_temps 3
add r0.xy, -vicp[2][0].xzxx, vicp[3][0].xzxx
mad r0.xy, vDomain.xxxx, r0.xyxx, vicp[2][0].xzxx
add r0.zw, -vicp[0][0].xxxz, vicp[1][0].xxxz
mad r0.zw, vDomain.xxxx, r0.zzzw, vicp[0][0].xxxz
add r0.xy, -r0.zwzz, r0.xyxx
mad r0.xz, vDomain.yyyy, r0.xxyx, r0.zzwz
add r1.xy, -vicp[2][3].xyxx, vicp[3][3].xyxx
mad r1.xy, vDomain.xxxx, r1.xyxx, vicp[2][3].xyxx
add r1.zw, -vicp[0][3].xxxy, vicp[1][3].xxxy
mad r1.zw, vDomain.xxxx, r1.zzzw, vicp[0][3].xxxy
add r1.xy, -r1.zwzz, r1.xyxx
mad r1.xy, vDomain.yyyy, r1.xyxx, r1.zwzz
sample_l_indexable(texture2d)(float,float,float,float) r1.z, r1.xyxx, t0.yzxw, s0, l(0.000000)
add r0.y, r1.z, cb0[1].w
mov r0.w, l(1.000000)
dp4 o0.x, r0.xyzw, cb1[4].xyzw
dp4 o0.y, r0.xyzw, cb1[5].xyzw
dp4 o0.z, r0.xyzw, cb1[6].xyzw
dp4 r0.w, r0.xyzw, cb1[7].xyzw
mov o0.w, r0.w
mov o1.xyzw, r0.xyzw
add r0.xyz, -vicp[2][1].xyzx, vicp[3][1].xyzx
mad r0.xyz, vDomain.xxxx, r0.xyzx, vicp[2][1].xyzx
add r2.xyz, -vicp[0][1].xyzx, vicp[1][1].xyzx
mad r2.xyz, vDomain.xxxx, r2.xyzx, vicp[0][1].xyzx
add r0.xyz, r0.xyzx, -r2.xyzx
mad o2.xyz, vDomain.yyyy, r0.xyzx, r2.xyzx
add r0.xyz, -vicp[2][2].xyzx, vicp[3][2].xyzx
mad r0.xyz, vDomain.xxxx, r0.xyzx, vicp[2][2].xyzx
add r2.xyz, -vicp[0][2].xyzx, vicp[1][2].xyzx
mad r2.xyz, vDomain.xxxx, r2.xyzx, vicp[0][2].xyzx
add r0.xyz, r0.xyzx, -r2.xyzx
mad o3.xyz, vDomain.yyyy, r0.xyzx, r2.xyzx
mul o4.zw, r1.xxxy, cb2[0].xxxy
mov o4.xy, r1.xyxx
ret 
// Approximately 36 instruction slots used
