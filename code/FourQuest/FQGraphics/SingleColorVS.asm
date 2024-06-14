//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
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
//   float4x4 cViewProj;                // Offset:    0 Size:    64
//
// }
//
// cbuffer cbBoneTransform
// {
//
//   float4x4 cFinalTransforms[128];    // Offset:    0 Size:  8192
//
// }
//
// cbuffer cbOutLineColor
// {
//
//   float4 cColor;                     // Offset:    0 Size:    16
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// cbModelTransform                  cbuffer      NA          NA            cb0      1 
// cbSceneTransform                  cbuffer      NA          NA            cb1      1 
// cbBoneTransform                   cbuffer      NA          NA            cb2      1 
// cbOutLineColor                    cbuffer      NA          NA            cb3      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// POSITION                 0   xyz         0     NONE   float   xyz 
// NORMAL                   0   xyz         1     NONE   float       
// TANGENT                  0   xyz         2     NONE   float       
// UV                       0   xy          3     NONE   float       
// INDICES                  0   xyzw        4     NONE     int   xyzw
// WEIGHTS                  0   xyzw        5     NONE   float   xyzw
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float   xyzw
// COLOR                    0   xyzw        1     NONE   float   xyzw
//
vs_5_0
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[4], immediateIndexed
dcl_constantbuffer CB1[4], immediateIndexed
dcl_constantbuffer CB2[512], dynamicIndexed
dcl_constantbuffer CB3[1], immediateIndexed
dcl_input v0.xyz
dcl_input v4.xyzw
dcl_input v5.xyzw
dcl_output_siv o0.xyzw, position
dcl_output o1.xyzw
dcl_temps 20
ishl r0.xyzw, v4.xyzw, l(2, 2, 2, 2)
mul r1.x, v5.x, cb2[r0.x + 0].w
mul r1.y, v5.x, cb2[r0.x + 1].w
mul r1.z, v5.x, cb2[r0.x + 2].w
mul r1.w, v5.x, cb2[r0.x + 3].w
mul r2.x, v5.y, cb2[r0.y + 0].w
mul r2.y, v5.y, cb2[r0.y + 1].w
mul r2.z, v5.y, cb2[r0.y + 2].w
mul r2.w, v5.y, cb2[r0.y + 3].w
add r1.xyzw, r1.xyzw, r2.xyzw
mul r2.x, v5.z, cb2[r0.z + 0].w
mul r2.y, v5.z, cb2[r0.z + 1].w
mul r2.z, v5.z, cb2[r0.z + 2].w
mul r2.w, v5.z, cb2[r0.z + 3].w
add r1.xyzw, r1.xyzw, r2.xyzw
mul r2.x, v5.w, cb2[r0.w + 0].w
mul r2.y, v5.w, cb2[r0.w + 1].w
mul r2.z, v5.w, cb2[r0.w + 2].w
mul r2.w, v5.w, cb2[r0.w + 3].w
add r1.xyzw, r1.xyzw, r2.xyzw
dp4 r2.w, r1.xyzw, cb0[0].xyzw
mul r3.xyz, v5.xxxx, cb2[r0.x + 0].yxzy
mov r4.x, r3.y
mul r5.xyz, v5.xxxx, cb2[r0.x + 1].xzyx
mov r4.y, r5.x
mul r6.xyz, v5.xxxx, cb2[r0.x + 2].xyzx
mov r4.z, r6.x
mul r7.xyz, v5.xxxx, cb2[r0.x + 3].xyzx
mov r4.w, r7.x
mul r8.xyz, v5.yyyy, cb2[r0.y + 0].yxzy
mov r9.x, r8.y
mul r10.xyz, v5.yyyy, cb2[r0.y + 1].xzyx
mov r9.y, r10.x
mul r11.xyz, v5.yyyy, cb2[r0.y + 2].xyzx
mov r9.z, r11.x
mul r12.xyz, v5.yyyy, cb2[r0.y + 3].xyzx
mov r9.w, r12.x
add r4.xyzw, r4.xyzw, r9.xyzw
mul r9.xyz, v5.zzzz, cb2[r0.z + 0].yxzy
mov r13.x, r9.y
mul r14.xyz, v5.zzzz, cb2[r0.z + 1].xzyx
mov r13.y, r14.x
mul r15.xyz, v5.zzzz, cb2[r0.z + 2].xyzx
mov r13.z, r15.x
mul r0.xyz, v5.zzzz, cb2[r0.z + 3].xyzx
mov r13.w, r0.x
add r4.xyzw, r4.xyzw, r13.xyzw
mul r13.xyz, v5.wwww, cb2[r0.w + 0].yxzy
mov r16.x, r13.y
mul r17.xyz, v5.wwww, cb2[r0.w + 1].xzyx
mov r16.y, r17.x
mul r18.xyz, v5.wwww, cb2[r0.w + 2].xyzx
mul r19.xyz, v5.wwww, cb2[r0.w + 3].xyzx
mov r16.z, r18.x
mov r16.w, r19.x
add r4.xyzw, r4.xyzw, r16.xyzw
dp4 r2.x, r4.xyzw, cb0[0].xyzw
mov r5.x, r3.z
mov r3.y, r5.z
mov r3.z, r6.y
mov r5.z, r6.z
mov r3.w, r7.y
mov r5.w, r7.z
mov r10.x, r8.z
mov r8.y, r10.z
mov r8.z, r11.y
mov r10.z, r11.z
mov r8.w, r12.y
mov r10.w, r12.z
add r5.xyzw, r5.xyzw, r10.xyzw
add r3.xyzw, r3.xyzw, r8.xyzw
mov r14.x, r9.z
mov r9.y, r14.z
mov r9.z, r15.y
mov r14.z, r15.z
mov r9.w, r0.y
mov r14.w, r0.z
add r0.xyzw, r5.xyzw, r14.xyzw
add r3.xyzw, r3.xyzw, r9.xyzw
mov r17.x, r13.z
mov r13.y, r17.z
mov r13.z, r18.y
mov r17.z, r18.z
mov r13.w, r19.y
mov r17.w, r19.z
add r0.xyzw, r0.xyzw, r17.xyzw
add r3.xyzw, r3.xyzw, r13.xyzw
dp4 r2.y, r3.xyzw, cb0[0].xyzw
dp4 r2.z, r0.xyzw, cb0[0].xyzw
mov r5.xyz, v0.xyzx
mov r5.w, l(1.000000)
dp4 r2.x, r5.xyzw, r2.xyzw
dp4 r6.x, r4.xyzw, cb0[1].xyzw
dp4 r6.y, r3.xyzw, cb0[1].xyzw
dp4 r6.z, r0.xyzw, cb0[1].xyzw
dp4 r6.w, r1.xyzw, cb0[1].xyzw
dp4 r2.y, r5.xyzw, r6.xyzw
dp4 r6.x, r4.xyzw, cb0[2].xyzw
dp4 r4.x, r4.xyzw, cb0[3].xyzw
dp4 r6.y, r3.xyzw, cb0[2].xyzw
dp4 r4.y, r3.xyzw, cb0[3].xyzw
dp4 r6.z, r0.xyzw, cb0[2].xyzw
dp4 r4.z, r0.xyzw, cb0[3].xyzw
dp4 r6.w, r1.xyzw, cb0[2].xyzw
dp4 r4.w, r1.xyzw, cb0[3].xyzw
dp4 r2.w, r5.xyzw, r4.xyzw
dp4 r2.z, r5.xyzw, r6.xyzw
dp4 o0.x, r2.xyzw, cb1[0].xyzw
dp4 o0.y, r2.xyzw, cb1[1].xyzw
dp4 o0.z, r2.xyzw, cb1[2].xyzw
dp4 o0.w, r2.xyzw, cb1[3].xyzw
mov o1.xyzw, cb3[0].xyzw
ret 
// Approximately 113 instruction slots used
