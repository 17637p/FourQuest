//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer cbSceneTransform
// {
//
//   float gMinDist;                    // Offset:    0 Size:     4
//   float gMaxDist;                    // Offset:    4 Size:     4
//   float gMinTess;                    // Offset:    8 Size:     4
//   float gMaxTess;                    // Offset:   12 Size:     4
//   float3 gEyePosW;                   // Offset:   16 Size:    12
//   float pad;                         // Offset:   28 Size:     4 [unused]
//   float4 gWorldFrustumPlanes[6];     // Offset:   32 Size:    96 [unused]
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// cbSceneTransform                  cbuffer      NA          NA            cb0      1 
//
//
//
// Patch Constant signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_TessFactor            0   x           0 QUADEDGE   float   x   
// SV_TessFactor            1   x           1 QUADEDGE   float   x   
// SV_TessFactor            2   x           2 QUADEDGE   float   x   
// SV_TessFactor            3   x           3 QUADEDGE   float   x   
// SV_InsideTessFactor      0   x           4  QUADINT   float   x   
// SV_InsideTessFactor      1   x           5  QUADINT   float   x   
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// POSITION                 0   xyz         0     NONE   float   xyz 
// NORMAL                   0   xyz         1     NONE   float   xyz 
// TANGENT                  0   xyz         2     NONE   float   xyz 
// TEXCOORD                 0   xy          3     NONE   float   xy  
// TEXCOORD                 1     zw        3     NONE   float     zw
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// POSITION                 0   xyz         0     NONE   float   xyz 
// NORMAL                   0   xyz         1     NONE   float   xyz 
// TANGENT                  0   xyz         2     NONE   float   xyz 
// TEXCOORD                 0   xy          3     NONE   float   xy  
// TEXCOORD                 1     zw        3     NONE   float     zw
//
// Tessellation Domain   # of control points
// -------------------- --------------------
// Quadrilateral                           4
//
// Tessellation Output Primitive  Partitioning Type 
// ------------------------------ ------------------
// Clockwise Triangles            Even Fractional   
//
hs_5_0
hs_decls 
dcl_input_control_point_count 4
dcl_output_control_point_count 4
dcl_tessellator_domain domain_quad
dcl_tessellator_partitioning partitioning_fractional_even
dcl_tessellator_output_primitive output_triangle_cw
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[2], immediateIndexed
hs_fork_phase 
dcl_hs_fork_phase_instance_count 4
dcl_input vForkInstanceID
dcl_input vicp[4][0].x
dcl_output_siv o0.x, finalQuadUeq0EdgeTessFactor
dcl_output_siv o1.x, finalQuadVeq0EdgeTessFactor
dcl_output_siv o2.x, finalQuadUeq1EdgeTessFactor
dcl_output_siv o3.x, finalQuadVeq1EdgeTessFactor
dcl_temps 1
dcl_indexrange o0.x 4
iadd r0.x, vForkInstanceID.x, l(-1)
ult r0.yz, vForkInstanceID.xxxx, l(0, 1, 2, 0)
movc r0.x, r0.y, l(0), r0.x
iadd r0.y, -vForkInstanceID.x, l(1)
movc r0.y, r0.z, r0.y, l(2)
add r0.x, vicp[r0.y + 1][0].x, vicp[r0.x + 0][0].x
mad r0.xyz, r0.xxxx, l(0.500000, 0.500000, 0.500000, 0.000000), -cb0[1].xyzx
dp3 r0.x, r0.xyzx, r0.xyzx
sqrt r0.x, r0.x
add r0.x, r0.x, -cb0[0].x
add r0.yz, -cb0[0].xxwx, cb0[0].yyzy
div_sat r0.x, r0.x, r0.y
mad r0.x, r0.x, r0.z, cb0[0].w
exp o[r0.y + 0].x, r0.x
mov r0.y, vForkInstanceID.x
ret 
hs_fork_phase 
dcl_hs_fork_phase_instance_count 2
dcl_input vForkInstanceID
dcl_input vicp[4][0].x
dcl_output_siv o4.x, finalQuadUInsideTessFactor
dcl_output_siv o5.x, finalQuadVInsideTessFactor
dcl_temps 1
dcl_indexrange o4.x 2
add r0.x, vicp[1][0].x, vicp[0][0].x
add r0.x, r0.x, vicp[2][0].x
add r0.x, r0.x, vicp[3][0].x
mad r0.xyz, r0.xxxx, l(0.250000, 0.250000, 0.250000, 0.000000), -cb0[1].xyzx
dp3 r0.x, r0.xyzx, r0.xyzx
sqrt r0.x, r0.x
add r0.x, r0.x, -cb0[0].x
add r0.yz, -cb0[0].xxwx, cb0[0].yyzy
div_sat r0.x, r0.x, r0.y
mad r0.x, r0.x, r0.z, cb0[0].w
exp o[r0.y + 4].x, r0.x
mov r0.y, vForkInstanceID.x
ret 
// Approximately 29 instruction slots used
