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
//   float4x4 cView;                    // Offset:    0 Size:    64
//   float4x4 cViewProj;                // Offset:   64 Size:    64
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
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// cbModelTransform                  cbuffer      NA          NA            cb0      1 
// cbSceneTransform                  cbuffer      NA          NA            cb1      1 
// cbBoneTransform                   cbuffer      NA          NA            cb2      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// POSITION                 0   xyz         0     NONE   float   xyz 
// NORMAL                   0   xyz         1     NONE   float   xyz 
// TANGENT                  0   xyz         2     NONE   float   xyz 
// UV                       0   xy          3     NONE   float   xy  
// INDICES                  0   xyzw        4     NONE     int   xyzw
// WEIGHTS                  0   xyzw        5     NONE   float   xyzw
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float   xyzw
// POSITION                 0   xyz         1     NONE   float   xyz 
// TEXCOORD                 3      w        1     NONE   float      w
// NORMAL                   0   xyz         2     NONE   float   xyz 
// TEXCOORD                 0   xyz         3     NONE   float   xyz 
// TEXCOORD                 1   xy          4     NONE   float   xy  
// TEXCOORD                 2     z         4     NONE   float     z 
// TEXCOORD                 4   xyz         5     NONE   float   xyz 
// TEXCOORD                 5   xyz         6     NONE   float   xyz 
//
vs_5_0
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[4], immediateIndexed
dcl_constantbuffer CB1[8], immediateIndexed
dcl_constantbuffer CB2[512], dynamicIndexed
dcl_input v0.xyz
dcl_input v1.xyz
dcl_input v2.xyz
dcl_input v3.xy
dcl_input v4.xyzw
dcl_input v5.xyzw
dcl_output_siv o0.xyzw, position
dcl_output o1.xyz
dcl_output o1.w
dcl_output o2.xyz
dcl_output o3.xyz
dcl_output o4.xy
dcl_output o4.z
dcl_output o5.xyz
dcl_output o6.xyz
dcl_temps 20
//
// Initial variable locations:
//   v0.x <- vin.Position.x; v0.y <- vin.Position.y; v0.z <- vin.Position.z; 
//   v1.x <- vin.NormalL.x; v1.y <- vin.NormalL.y; v1.z <- vin.NormalL.z; 
//   v2.x <- vin.TangentL.x; v2.y <- vin.TangentL.y; v2.z <- vin.TangentL.z; 
//   v3.x <- vin.UV.x; v3.y <- vin.UV.y; 
//   v4.x <- vin.Indices.x; v4.y <- vin.Indices.y; v4.z <- vin.Indices.z; v4.w <- vin.Indices.w; 
//   v5.x <- vin.Weights.x; v5.y <- vin.Weights.y; v5.z <- vin.Weights.z; v5.w <- vin.Weights.w; 
//   o6.x <- <main return value>.TangentV.x; o6.y <- <main return value>.TangentV.y; o6.z <- <main return value>.TangentV.z; 
//   o5.x <- <main return value>.NormalV.x; o5.y <- <main return value>.NormalV.y; o5.z <- <main return value>.NormalV.z; 
//   o4.x <- <main return value>.UV.x; o4.y <- <main return value>.UV.y; o4.z <- <main return value>.ClipSpacePosZ; 
//   o3.x <- <main return value>.TangentW.x; o3.y <- <main return value>.TangentW.y; o3.z <- <main return value>.TangentW.z; 
//   o2.x <- <main return value>.NormalW.x; o2.y <- <main return value>.NormalW.y; o2.z <- <main return value>.NormalW.z; 
//   o1.x <- <main return value>.PositionW.x; o1.y <- <main return value>.PositionW.y; o1.z <- <main return value>.PositionW.z; o1.w <- <main return value>.DepthView; 
//   o0.x <- <main return value>.PositionH.x; o0.y <- <main return value>.PositionH.y; o0.z <- <main return value>.PositionH.z; o0.w <- <main return value>.PositionH.w
//
#line 58 "C:\Users\user\Documents\FourQuest\code\FourQuest\FQGraphics\ModelVS.hlsl"
ishl r0.xyzw, v4.xyzw, l(2, 2, 2, 2)

#line 55
mul r1.x, v5.x, cb2[r0.x + 0].w  // r1.x <- worldMat._m30
mul r1.y, v5.x, cb2[r0.x + 1].w  // r1.y <- worldMat._m31
mul r1.z, v5.x, cb2[r0.x + 2].w  // r1.z <- worldMat._m32
mul r1.w, v5.x, cb2[r0.x + 3].w  // r1.w <- worldMat._m33

#line 56
mul r2.x, v5.y, cb2[r0.y + 0].w
mul r2.y, v5.y, cb2[r0.y + 1].w
mul r2.z, v5.y, cb2[r0.y + 2].w
mul r2.w, v5.y, cb2[r0.y + 3].w
add r1.xyzw, r1.xyzw, r2.xyzw

#line 57
mul r2.x, v5.z, cb2[r0.z + 0].w
mul r2.y, v5.z, cb2[r0.z + 1].w
mul r2.z, v5.z, cb2[r0.z + 2].w
mul r2.w, v5.z, cb2[r0.z + 3].w
add r1.xyzw, r1.xyzw, r2.xyzw

#line 58
mul r2.x, v5.w, cb2[r0.w + 0].w
mul r2.y, v5.w, cb2[r0.w + 1].w
mul r2.z, v5.w, cb2[r0.w + 2].w
mul r2.w, v5.w, cb2[r0.w + 3].w
add r1.xyzw, r1.xyzw, r2.xyzw

#line 59
dp4 r2.w, r1.xyzw, cb0[3].xyzw  // r2.w <- worldMat._m33

#line 55
mul r3.xyz, v5.xxxx, cb2[r0.x + 0].yxzy  // r3.x <- worldMat._m10; r3.y <- worldMat._m00; r3.z <- worldMat._m20

#line 56
mov r4.x, r3.y

#line 55
mul r5.xyz, v5.xxxx, cb2[r0.x + 1].xzyx  // r5.x <- worldMat._m01; r5.y <- worldMat._m21; r5.z <- worldMat._m11

#line 56
mov r4.y, r5.x

#line 55
mul r6.xyz, v5.xxxx, cb2[r0.x + 2].xyzx  // r6.x <- worldMat._m02; r6.y <- worldMat._m12; r6.z <- worldMat._m22

#line 56
mov r4.z, r6.x

#line 55
mul r7.xyz, v5.xxxx, cb2[r0.x + 3].xyzx  // r7.x <- worldMat._m03; r7.y <- worldMat._m13; r7.z <- worldMat._m23

#line 56
mov r4.w, r7.x
mul r8.xyz, v5.yyyy, cb2[r0.y + 0].yxzy
mov r9.x, r8.y
mul r10.xyz, v5.yyyy, cb2[r0.y + 1].xzyx
mov r9.y, r10.x
mul r11.xyz, v5.yyyy, cb2[r0.y + 2].xyzx
mov r9.z, r11.x
mul r12.xyz, v5.yyyy, cb2[r0.y + 3].xyzx
mov r9.w, r12.x
add r4.xyzw, r4.xyzw, r9.xyzw  // r4.x <- worldMat._m00; r4.y <- worldMat._m01; r4.z <- worldMat._m02; r4.w <- worldMat._m03

#line 57
mul r9.xyz, v5.zzzz, cb2[r0.z + 0].yxzy
mov r13.x, r9.y
mul r14.xyz, v5.zzzz, cb2[r0.z + 1].xzyx
mov r13.y, r14.x
mul r15.xyz, v5.zzzz, cb2[r0.z + 2].xyzx
mov r13.z, r15.x
mul r0.xyz, v5.zzzz, cb2[r0.z + 3].xyzx
mov r13.w, r0.x
add r4.xyzw, r4.xyzw, r13.xyzw

#line 58
mul r13.xyz, v5.wwww, cb2[r0.w + 0].yxzy
mov r16.x, r13.y
mul r17.xyz, v5.wwww, cb2[r0.w + 1].xzyx
mov r16.y, r17.x
mul r18.xyz, v5.wwww, cb2[r0.w + 2].xyzx
mul r19.xyz, v5.wwww, cb2[r0.w + 3].xyzx
mov r16.z, r18.x
mov r16.w, r19.x
add r4.xyzw, r4.xyzw, r16.xyzw

#line 59
dp4 r2.x, r4.xyzw, cb0[3].xyzw  // r2.x <- worldMat._m03

#line 56
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
add r5.xyzw, r5.xyzw, r10.xyzw  // r5.x <- worldMat._m20; r5.z <- worldMat._m22; r5.w <- worldMat._m23
add r3.xyzw, r3.xyzw, r8.xyzw  // r3.y <- worldMat._m11; r3.z <- worldMat._m12; r3.w <- worldMat._m13

#line 57
mov r14.x, r9.z
mov r9.y, r14.z
mov r9.z, r15.y
mov r14.z, r15.z
mov r9.w, r0.y
mov r14.w, r0.z
add r0.xyzw, r5.xyzw, r14.xyzw  // r0.x <- worldMat._m20; r0.y <- worldMat._m21; r0.z <- worldMat._m22; r0.w <- worldMat._m23
add r3.xyzw, r3.xyzw, r9.xyzw

#line 58
mov r17.x, r13.z
mov r13.y, r17.z
mov r13.z, r18.y
mov r17.z, r18.z
mov r13.w, r19.y
mov r17.w, r19.z
add r0.xyzw, r0.xyzw, r17.xyzw
add r3.xyzw, r3.xyzw, r13.xyzw

#line 59
dp4 r2.y, r3.xyzw, cb0[3].xyzw  // r2.y <- worldMat._m13
dp4 r2.z, r0.xyzw, cb0[3].xyzw  // r2.z <- worldMat._m23

#line 66
mov r5.xyz, v0.xyzx
mov r5.w, l(1.000000)
dp4 r2.w, r5.xyzw, r2.xyzw  // r2.w <- vout.PositionH.w

#line 59
dp4 r6.w, r1.xyzw, cb0[0].xyzw  // r6.w <- worldMat._m30
dp4 r6.x, r4.xyzw, cb0[0].xyzw  // r6.x <- worldMat._m00
dp4 r6.y, r3.xyzw, cb0[0].xyzw  // r6.y <- worldMat._m10
dp4 r6.z, r0.xyzw, cb0[0].xyzw  // r6.z <- worldMat._m20

#line 66
dp4 r2.x, r5.xyzw, r6.xyzw  // r2.x <- vout.PositionH.x

#line 59
dp4 r7.w, r1.xyzw, cb0[1].xyzw  // r7.w <- worldMat._m31
dp4 r1.w, r1.xyzw, cb0[2].xyzw  // r1.w <- worldMat._m32
dp4 r7.x, r4.xyzw, cb0[1].xyzw  // r7.x <- worldMat._m01
dp4 r1.x, r4.xyzw, cb0[2].xyzw  // r1.x <- worldMat._m02
dp4 r7.y, r3.xyzw, cb0[1].xyzw  // r7.y <- worldMat._m11
dp4 r1.y, r3.xyzw, cb0[2].xyzw  // r1.y <- worldMat._m12
dp4 r7.z, r0.xyzw, cb0[1].xyzw  // r7.z <- worldMat._m21
dp4 r1.z, r0.xyzw, cb0[2].xyzw  // r1.z <- worldMat._m22

#line 66
dp4 r2.y, r5.xyzw, r7.xyzw  // r2.y <- vout.PositionH.y
dp4 r2.z, r5.xyzw, r1.xyzw  // r2.z <- vout.PositionH.z

#line 67
dp4 o0.x, r2.xyzw, cb1[4].xyzw
dp4 o0.y, r2.xyzw, cb1[5].xyzw
dp4 o0.w, r2.xyzw, cb1[7].xyzw
dp4 r0.x, r2.xyzw, cb1[6].xyzw  // r0.x <- vout.PositionH.z

#line 82
mov o0.z, r0.x
mov o4.z, r0.x

#line 70
mov r2.w, l(1.000000)
dp4 o1.w, r2.xyzw, cb1[2].xyzw

#line 82
mov o1.xyz, r2.xyzx

#line 72
dp3 r0.x, v1.xyzx, r6.xyzx

#line 75
dp3 r2.x, v2.xyzx, r6.xyzx

#line 72
dp3 r0.y, v1.xyzx, r7.xyzx

#line 75
dp3 r2.y, v2.xyzx, r7.xyzx

#line 72
dp3 r0.z, v1.xyzx, r1.xyzx

#line 75
dp3 r2.z, v2.xyzx, r1.xyzx

#line 72
dp3 r0.w, r0.xyzx, r0.xyzx
rsq r0.w, r0.w
mul r0.xyz, r0.wwww, r0.xyzx  // r0.x <- vout.NormalW.x; r0.y <- vout.NormalW.y; r0.z <- vout.NormalW.z

#line 82
mov o2.xyz, r0.xyzx

#line 75
dp3 r0.w, r2.xyzx, r2.xyzx
rsq r0.w, r0.w
mul r1.xyz, r0.wwww, r2.xyzx  // r1.x <- vout.TangentW.x; r1.y <- vout.TangentW.y; r1.z <- vout.TangentW.z

#line 82
mov o3.xyz, r1.xyzx
mov o4.xy, v3.xyxx

#line 73
dp3 r2.x, r0.xyzx, cb1[0].xyzx
dp3 r2.y, r0.xyzx, cb1[1].xyzx
dp3 r2.z, r0.xyzx, cb1[2].xyzx
dp3 r0.x, r2.xyzx, r2.xyzx
rsq r0.x, r0.x
mul o5.xyz, r0.xxxx, r2.xyzx

#line 76
dp3 r0.x, r1.xyzx, cb1[0].xyzx
dp3 r0.y, r1.xyzx, cb1[1].xyzx
dp3 r0.z, r1.xyzx, cb1[2].xyzx
dp3 r0.w, r0.xyzx, r0.xyzx
rsq r0.w, r0.w
mul o6.xyz, r0.wwww, r0.xyzx

#line 82
ret 
// Approximately 144 instruction slots used
