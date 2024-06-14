//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer cbParticleObject
// {
//
//   struct ParticleInstance
//   {
//       
//       float4x4 Transform;            // Offset:    0
//       float TimePos;                 // Offset:   64
//       float FrameTime;               // Offset:   68
//       int NumToEmit;                 // Offset:   72
//       float RandomSeed;              // Offset:   76
//
//   } gParticleInstance;               // Offset:    0 Size:    80
//   
//   struct ParticleMain
//   {
//       
//       float4 StartColor[2];          // Offset:   80
//       float2 StartLifeTime;          // Offset:  112
//       float2 StartSpeed;             // Offset:  120
//       float2 StartRotation;          // Offset:  128
//       float2 StartSize;              // Offset:  136
//       float2 GravityModfier;         // Offset:  144
//       float SimulationSpeed;         // Offset:  152
//       int MaxParticleSize;           // Offset:  156
//
//   } gParticleMain;                   // Offset:   80 Size:    80
//   
//   struct ParticleShape
//   {
//       
//       float4x4 Transform;            // Offset:  160
//       float3 Postion;                // Offset:  224
//       float pad0;                    // Offset:  236
//       float3 Rotation;               // Offset:  240
//       float pad1;                    // Offset:  252
//       float3 Scale;                  // Offset:  256
//       float pad2;                    // Offset:  268
//       int ShapeType;                 // Offset:  272
//       int ModeType;                  // Offset:  276
//       float Angle;                   // Offset:  280
//       float Radius;                  // Offset:  284
//       float DountRadius;             // Offset:  288
//       float Arc;                     // Offset:  292
//       float Speed;                   // Offset:  296
//       float Spread;                  // Offset:  300
//       float RadiusThickness;         // Offset:  304
//
//   } gParticleShape;                  // Offset:  160 Size:   148
//   
//   struct ParticleVelocityOverLifetime
//   {
//       
//       float3 Velocity;               // Offset:  320
//       float pad0;                    // Offset:  332
//       float3 Orbital;                // Offset:  336
//       float pad1;                    // Offset:  348
//       float3 Offset;                 // Offset:  352
//       float pad2;                    // Offset:  364
//       int bIsUsed;                   // Offset:  368
//
//   } gParticleVelocityOverLifetime;   // Offset:  320 Size:    52 [unused]
//   
//   struct ParticleLimitVelocityOverLifetime
//   {
//       
//       float Speed;                   // Offset:  384
//       float Dampen;                  // Offset:  388
//       int bIsUsed;                   // Offset:  392
//
//   } gParticleLimitVelocityOverLifetime;// Offset:  384 Size:    12 [unused]
//   
//   struct ParticleForceOverLifeTime
//   {
//       
//       float3 Force;                  // Offset:  400
//       int bIsUsed;                   // Offset:  412
//
//   } gParticleForceOverLifeTime;      // Offset:  400 Size:    16 [unused]
//   
//   struct ParticleColorOverLifetime
//   {
//       
//       float4 ColorRatios[8];         // Offset:  416
//       float4 AlphaRatios[8];         // Offset:  544
//       uint AlphaRatioCount;          // Offset:  672
//       uint ColorRatioCount;          // Offset:  676
//       int bIsUsed;                   // Offset:  680
//
//   } gParticleColorOverLifetime;      // Offset:  416 Size:   268 [unused]
//   
//   struct ParticleSizeOverLifetime
//   {
//       
//       float2 PointA;                 // Offset:  688
//       float2 PointB;                 // Offset:  696
//       float2 PointC;                 // Offset:  704
//       float2 PointD;                 // Offset:  712
//       int bIsUsed;                   // Offset:  720
//
//   } gParticleSizeOverLifetime;       // Offset:  688 Size:    36 [unused]
//   
//   struct ParticleRotationOverLifetime
//   {
//       
//       float AngularVelocity;         // Offset:  736
//       int bIsUsed;                   // Offset:  740
//
//   } gParticleRotationOverLifetime;   // Offset:  736 Size:     8 [unused]
//   
//   struct ParticleRender
//   {
//       
//       int bHasTexture;               // Offset:  752
//       int bUseMultiplyAlpha;         // Offset:  756
//       int bUseAlphaClip;             // Offset:  760
//       float AlphaClipThreshold;      // Offset:  764
//
//   } gParticleRender;                 // Offset:  752 Size:    16 [unused]
//
// }
//
// cbuffer cbDeadList
// {
//
//   uint gNumDeadParticles;            // Offset:    0 Size:     4
//   uint3 DeadListCount_pad;           // Offset:    4 Size:    12 [unused]
//
// }
//
// Resource bind info for g_ParticleBuffer
// {
//
//   struct Particle
//   {
//       
//       float3 PositionW;              // Offset:    0
//       float Size;                    // Offset:   12
//       float3 Velocity;               // Offset:   16
//       float Rotation;                // Offset:   28
//       float4 StartColor;             // Offset:   32
//       float4 Color;                  // Offset:   48
//       float3 EmitterPosition;        // Offset:   64
//       float GravityModfier;          // Offset:   76
//       float LifeTime;                // Offset:   80
//       float Age;                     // Offset:   84
//
//   } $Element;                        // Offset:    0 Size:    88
//
// }
//
// Resource bind info for g_DeadListToAllocFrom
// {
//
//   uint $Element;                     // Offset:    0 Size:     4
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// g_samWrapLinear                   sampler      NA          NA             s0      1 
// gRandomTexture                    texture  float4          2d             t0      1 
// g_ParticleBuffer                      UAV  struct         r/w             u0      1 
// g_DeadListToAllocFrom                 UAV  struct     consume             u1      1 
// cbParticleObject                  cbuffer      NA          NA            cb1      1 
// cbDeadList                        cbuffer      NA          NA            cb2      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// no Input
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// no Output
cs_5_0
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB1[20], immediateIndexed
dcl_constantbuffer CB2[1], immediateIndexed
dcl_sampler s0, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_uav_structured u0, 88
dcl_uav_structured u1, 4
dcl_input vThreadID.x
dcl_temps 11
dcl_thread_group 1024, 1, 1
ult r0.x, vThreadID.x, cb2[0].x
ult r0.y, vThreadID.x, cb1[4].z
and r0.x, r0.y, r0.x
if_nz r0.x
  utof r0.x, vThreadID.x
  mul r0.y, r0.x, l(0.000977)
  add r0.z, cb1[4].w, cb1[4].x
  sample_l_indexable(texture2d)(float,float,float,float) r1.xyzw, r0.yzyy, t0.xyzw, s0, l(0.000000)
  iadd r0.y, vThreadID.x, l(1)
  utof r0.y, r0.y
  mul r0.x, r0.y, l(0.000977)
  sample_l_indexable(texture2d)(float,float,float,float) r0.xyzw, r0.xzxx, t0.xyzw, s0, l(0.000000)
  add r2.x, r1.x, l(1.000000)
  mul r2.x, r2.x, l(0.500000)
  add r2.yz, -cb1[7].zzxz, cb1[7].wwyw
  mad r2.x, r2.x, r2.y, cb1[7].z
  mul r3.xyzw, cb1[0].yyyy, cb1[11].xyzw
  mad r3.xyzw, cb1[10].xyzw, cb1[0].xxxx, r3.xyzw
  mad r3.xyzw, cb1[12].xyzw, cb1[0].zzzz, r3.xyzw
  mad r3.xyzw, cb1[13].xyzw, cb1[0].wwww, r3.xyzw
  mul r4.xyzw, cb1[1].yyyy, cb1[11].xyzw
  mad r4.xyzw, cb1[10].xyzw, cb1[1].xxxx, r4.xyzw
  mad r4.xyzw, cb1[12].xyzw, cb1[1].zzzz, r4.xyzw
  mad r4.xyzw, cb1[13].xyzw, cb1[1].wwww, r4.xyzw
  mul r5.xyzw, cb1[2].yyyy, cb1[11].xyzw
  mad r5.xyzw, cb1[10].xyzw, cb1[2].xxxx, r5.xyzw
  mad r5.xyzw, cb1[12].xyzw, cb1[2].zzzz, r5.xyzw
  mad r5.xyzw, cb1[13].xyzw, cb1[2].wwww, r5.xyzw
  if_z cb1[17].x
    dp3 r2.y, r1.xyzx, r1.xyzx
    rsq r2.y, r2.y
    mul r6.xyz, r1.xyzx, r2.yyyy
    dp2 r2.y, r6.xzxx, r6.xzxx
    sqrt r2.y, r2.y
    div r2.w, r6.x, r2.y
    lt r6.w, l(0.000000), r6.z
    add r7.x, -|r2.w|, l(1.000000)
    sqrt r7.x, r7.x
    mad r7.y, |r2.w|, l(-0.018729), l(0.074261)
    mad r7.y, r7.y, |r2.w|, l(-0.212114)
    mad r7.y, r7.y, |r2.w|, l(1.570729)
    mul r7.z, r7.x, r7.y
    mad r7.z, r7.z, l(-2.000000), l(3.141593)
    lt r2.w, r2.w, -r2.w
    and r2.w, r2.w, r7.z
    mad r2.w, r7.y, r7.x, r2.w
    add r7.x, -r2.w, l(6.283184)
    movc r2.w, r6.w, r2.w, r7.x
    mul r2.w, r2.w, cb1[18].y
    mul r2.w, r2.w, l(0.159155)
    sincos r7.x, r8.x, r2.w
    mul r6.x, r2.y, r8.x
    mul r6.z, r2.y, r7.x
    mul r7.xyz, r2.xxxx, r6.xyzx
    mul r8.xyz, r6.xyzx, cb1[17].wwww
    dp3 r2.y, r8.xyzx, r8.xyzx
    sqrt r2.y, r2.y
    mov_sat r2.w, cb1[19].x
    add r6.w, -r2.w, l(1.000000)
    add r8.x, r1.w, l(1.000000)
    mul_sat r8.x, r8.x, l(0.500000)
    mul r6.w, r6.w, r8.x
    mul r6.w, r2.y, r6.w
    mad r2.y, r2.w, r2.y, r6.w
    mul r6.xyz, r2.yyyy, r6.xyzx
    mov r6.w, l(1.000000)
    dp4 r8.x, r6.xyzw, r3.xyzw
    dp4 r8.y, r6.xyzw, r4.xyzw
    dp4 r8.z, r6.xyzw, r5.xyzw
  else 
    ieq r2.y, cb1[17].x, l(1)
    if_nz r2.y
      dp3 r2.y, r1.xyzx, r1.xyzx
      rsq r2.y, r2.y
      mul r6.xyw, r1.xzxy, r2.yyyy
      dp2 r2.y, r6.xyxx, r6.xyxx
      sqrt r2.y, r2.y
      div r2.w, r6.x, r2.y
      lt r9.x, l(0.000000), r6.y
      add r9.y, -|r2.w|, l(1.000000)
      sqrt r9.y, r9.y
      mad r9.z, |r2.w|, l(-0.018729), l(0.074261)
      mad r9.z, r9.z, |r2.w|, l(-0.212114)
      mad r9.z, r9.z, |r2.w|, l(1.570729)
      mul r9.w, r9.y, r9.z
      mad r9.w, r9.w, l(-2.000000), l(3.141593)
      lt r2.w, r2.w, -r2.w
      and r2.w, r2.w, r9.w
      mad r2.w, r9.z, r9.y, r2.w
      add r9.y, -r2.w, l(6.283184)
      movc r2.w, r9.x, r2.w, r9.y
      mul r2.w, r2.w, cb1[18].y
      mul r2.w, r2.w, l(0.159155)
      sincos r9.x, r10.x, r2.w
      mul r6.x, r2.y, r10.x
      mul r6.z, r2.y, r9.x
      mov r6.y, |r6.w|
      mul r7.xyz, r2.xxxx, r6.xyzx
      mul r9.xyz, r6.xwzx, cb1[17].wwww
      dp3 r2.y, r9.xyzx, r9.xyzx
      sqrt r2.y, r2.y
      mov_sat r2.w, cb1[19].x
      add r6.y, -r2.w, l(1.000000)
      add r9.x, r1.w, l(1.000000)
      mul_sat r9.x, r9.x, l(0.500000)
      mul r6.y, r6.y, r9.x
      mul r6.y, r2.y, r6.y
      mad r2.y, r2.w, r2.y, r6.y
      mul r6.xyz, r2.yyyy, r6.xwzx
      mov r6.y, |r6.y|
      mov r6.w, l(1.000000)
      dp4 r8.x, r6.xyzw, r3.xyzw
      dp4 r8.y, r6.xyzw, r4.xyzw
      dp4 r8.z, r6.xyzw, r5.xyzw
    else 
      ieq r2.y, cb1[17].x, l(2)
      if_nz r2.y
        sincos r6.x, r9.x, cb1[17].z
        div r2.y, l(1.000000, 1.000000, 1.000000, 1.000000), r9.x
        mul r2.y, r2.y, r6.x
        add r2.y, |r2.y|, cb1[17].w
        div r2.y, r2.y, cb1[17].w
        dp3 r2.w, r1.xyzx, r1.xyzx
        rsq r2.w, r2.w
        mul r6.xy, r1.xzxx, r2.wwww
        dp2 r2.w, r6.xyxx, r6.xyxx
        sqrt r2.w, r2.w
        div r6.x, r6.x, r2.w
        lt r6.y, l(0.000000), r6.y
        add r6.z, -|r6.x|, l(1.000000)
        sqrt r6.z, r6.z
        mad r6.w, |r6.x|, l(-0.018729), l(0.074261)
        mad r6.w, r6.w, |r6.x|, l(-0.212114)
        mad r6.w, r6.w, |r6.x|, l(1.570729)
        mul r9.x, r6.z, r6.w
        mad r9.x, r9.x, l(-2.000000), l(3.141593)
        lt r6.x, r6.x, -r6.x
        and r6.x, r6.x, r9.x
        mad r6.x, r6.w, r6.z, r6.x
        add r6.z, -r6.x, l(6.283184)
        movc r6.x, r6.y, r6.x, r6.z
        mul r6.x, r6.x, cb1[18].y
        mul r6.x, r6.x, l(0.159155)
        sincos r6.x, r9.x, r6.x
        mul r9.x, r2.w, r9.x
        mul r9.y, r2.w, r6.x
        mul r6.xy, r9.xyxx, cb1[17].wwww
        dp2 r2.w, r6.xyxx, r6.xyxx
        sqrt r2.w, r2.w
        mov_sat r6.x, cb1[19].x
        add r6.y, -r6.x, l(1.000000)
        add r6.z, r1.w, l(1.000000)
        mul_sat r6.z, r6.z, l(0.500000)
        mul r6.y, r6.z, r6.y
        mul r6.y, r2.w, r6.y
        mad r2.w, r6.x, r2.w, r6.y
        dp2 r6.x, r9.xyxx, r9.xyxx
        rsq r6.x, r6.x
        mul r6.xy, r6.xxxx, r9.xyxx
        mul r6.xy, r2.wwww, r6.xyxx
        mul r9.xz, r2.yyyy, r6.xxyx
        mov r9.yw, l(0,0,0,1.000000)
        add r9.xyz, r9.xyzx, l(0.000000, 1.000000, 0.000000, 0.000000)
        dp4 r10.x, r9.xyzw, r3.xyzw
        dp4 r10.y, r9.xyzw, r4.xyzw
        dp4 r10.z, r9.xyzw, r5.xyzw
        mov r6.z, l(1.000000)
        dp3 r8.x, r6.xyzx, r3.xzwx
        dp3 r8.y, r6.xyzx, r4.xzwx
        dp3 r8.z, r6.xyzx, r5.xzwx
        add r6.xyz, -r8.xyzx, r10.xyzx
        dp3 r2.y, r6.xyzx, r6.xyzx
        rsq r2.y, r2.y
        mul r6.xyz, r2.yyyy, r6.xyzx
        mul r7.xyz, r2.xxxx, r6.xyzx
      else 
        ieq r2.y, cb1[17].x, l(3)
        if_nz r2.y
          dp3 r2.y, r1.xyzx, r1.xyzx
          rsq r2.y, r2.y
          mul r2.yw, r1.xxxz, r2.yyyy
          dp2 r6.x, r2.ywyy, r2.ywyy
          sqrt r6.x, r6.x
          div r2.y, r2.y, r6.x
          lt r2.w, l(0.000000), r2.w
          add r6.y, -|r2.y|, l(1.000000)
          sqrt r6.y, r6.y
          mad r6.z, |r2.y|, l(-0.018729), l(0.074261)
          mad r6.z, r6.z, |r2.y|, l(-0.212114)
          mad r6.z, r6.z, |r2.y|, l(1.570729)
          mul r6.w, r6.y, r6.z
          mad r6.w, r6.w, l(-2.000000), l(3.141593)
          lt r2.y, r2.y, -r2.y
          and r2.y, r2.y, r6.w
          mad r2.y, r6.z, r6.y, r2.y
          add r6.y, -r2.y, l(6.283184)
          movc r2.y, r2.w, r2.y, r6.y
          mul r2.y, r2.y, cb1[18].y
          mul r2.y, r2.y, l(0.159155)
          sincos r9.x, r10.x, r2.y
          mul r10.x, r6.x, r10.x
          mul r10.z, r6.x, r9.x
          dp2 r2.y, r10.xzxx, r10.xzxx
          rsq r2.y, r2.y
          mul r2.yw, r2.yyyy, r10.xxxz
          mov_sat r6.x, cb1[19].x
          add r6.y, -r6.x, l(1.000000)
          add r6.z, r1.w, l(1.000000)
          mul_sat r6.z, r6.z, l(0.500000)
          mul r6.y, r6.z, r6.y
          mul r6.y, r6.y, cb1[18].x
          mad r6.x, r6.x, cb1[18].x, r6.y
          dp2 r6.y, r1.xyxx, r1.xyxx
          rsq r6.y, r6.y
          mul r6.yz, r1.xxyx, r6.yyyy
          mul r6.xy, r6.xxxx, r6.yzyy
          mul r9.xz, r2.yywy, cb1[17].wwww
          mov r9.y, l(0)
          mov r6.zw, l(0,0,0,1.000000)
          add r6.xyz, r6.xyzx, r9.xyzx
          dp4 r8.x, r6.xyzw, r3.xyzw
          dp4 r8.y, r6.xyzw, r4.xyzw
          dp4 r8.z, r6.xyzw, r5.xyzw
          dp3 r2.y, r1.yzwy, r1.yzwy
          rsq r2.y, r2.y
          mul r6.xyz, r1.yzwy, r2.yyyy
          mul r7.xyz, r2.xxxx, r6.xyzx
        else 
          mov r6.x, r3.y
          mov r6.y, r4.y
          mov r6.z, r5.y
          dp3 r2.y, r6.xyzx, r6.xyzx
          rsq r2.y, r2.y
          mul r6.xyz, r2.yyyy, r6.xyzx
          mul r7.xyz, r2.xxxx, r6.xyzx
          ieq r2.y, cb1[17].x, l(4)
          if_nz r2.y
            mov r1.w, l(1.000000)
            dp4 r8.x, r1.xyzw, r3.xyzw
            dp4 r8.y, r1.xyzw, r4.xyzw
            dp4 r8.z, r1.xyzw, r5.xyzw
          else 
            ieq r2.y, cb1[17].x, l(5)
            if_nz r2.y
              dp3 r2.y, r1.xyzx, r1.xyzx
              rsq r2.y, r2.y
              mul r2.yw, r1.xxxz, r2.yyyy
              dp2 r3.y, r2.ywyy, r2.ywyy
              sqrt r3.y, r3.y
              div r2.y, r2.y, r3.y
              lt r2.w, l(0.000000), r2.w
              add r4.y, -|r2.y|, l(1.000000)
              sqrt r4.y, r4.y
              mad r5.y, |r2.y|, l(-0.018729), l(0.074261)
              mad r5.y, r5.y, |r2.y|, l(-0.212114)
              mad r5.y, r5.y, |r2.y|, l(1.570729)
              mul r6.x, r4.y, r5.y
              mad r6.x, r6.x, l(-2.000000), l(3.141593)
              lt r2.y, r2.y, -r2.y
              and r2.y, r2.y, r6.x
              mad r2.y, r5.y, r4.y, r2.y
              add r4.y, -r2.y, l(6.283184)
              movc r2.y, r2.w, r2.y, r4.y
              mul r2.y, r2.y, cb1[18].y
              mul r2.y, r2.y, l(0.159155)
              sincos r6.x, r9.x, r2.y
              mul r9.x, r3.y, r9.x
              mul r9.y, r3.y, r6.x
              mov_sat r2.y, cb1[19].x
              add r2.w, -r2.y, l(1.000000)
              add r1.w, r1.w, l(1.000000)
              mul_sat r1.w, r1.w, l(0.500000)
              mul r1.w, r1.w, r2.w
              mul r1.w, r1.w, cb1[17].w
              mad r1.w, r2.y, cb1[17].w, r1.w
              dp2 r2.y, r9.xyxx, r9.xyxx
              rsq r2.y, r2.y
              mul r2.yw, r2.yyyy, r9.xxxy
              mul r6.xy, r1.wwww, r2.ywyy
              mov r6.z, l(1.000000)
              dp3 r8.x, r6.xyzx, r3.xzwx
              dp3 r8.y, r6.xyzx, r4.xzwx
              dp3 r8.z, r6.xyzx, r5.xzwx
              dp2 r6.x, r9.xyxx, r3.xzxx
              dp2 r6.y, r9.xyxx, r4.xzxx
              dp2 r6.z, r9.xyxx, r5.xzxx
              dp3 r1.w, r6.xyzx, r6.xyzx
              rsq r1.w, r1.w
              mul r6.xyz, r1.wwww, r6.xyzx
              mul r7.xyz, r2.xxxx, r6.xyzx
            else 
              ieq r1.w, cb1[17].x, l(6)
              mov r1.y, l(1.000000)
              dp3 r3.x, r1.xzyx, r3.xzwx
              dp3 r3.y, r1.xzyx, r4.xzwx
              dp3 r3.z, r1.xzyx, r5.xzwx
              mov r1.x, cb1[0].w
              mov r1.y, cb1[1].w
              mov r1.z, cb1[2].w
              movc r8.xyz, r1.wwww, r3.xyzx, r1.xyzx
            endif 
          endif 
        endif 
      endif 
    endif 
  endif 
  add r0.xyzw, r0.xyzw, l(1.000000, 1.000000, 1.000000, 1.000000)
  mul_sat r0.xyzw, r0.xyzw, l(0.500000, 0.500000, 0.500000, 0.500000)
  add r1.xy, -cb1[8].zxzz, cb1[8].wyww
  mad r1.xy, r0.xyxx, r1.xyxx, cb1[8].zxzz
  add r3.xyzw, -cb1[5].xyzw, cb1[6].xyzw
  mad r3.xyzw, r0.zzzz, r3.xyzw, cb1[5].xyzw
  mad r2.x, r0.w, r2.z, cb1[7].x
  add r0.y, -cb1[9].x, cb1[9].y
  mad r0.w, r0.x, r0.y, cb1[9].x
  imm_atomic_consume r4.x, u1
  ld_structured_indexable(structured_buffer, stride=4)(mixed,mixed,mixed,mixed) r1.z, r4.x, l(0), u1.xxxx
  mov r8.w, r1.x
  store_structured u0.xyzw, r1.z, l(0), r8.xyzw
  mov r7.w, r1.y
  store_structured u0.xyzw, r1.z, l(16), r7.xyzw
  store_structured u0.xyzw, r1.z, l(32), r3.xyzw
  store_structured u0.xyzw, r1.z, l(48), r3.xyzw
  mov r0.x, cb1[0].w
  mov r0.y, cb1[1].w
  mov r0.z, cb1[2].w
  store_structured u0.xyzw, r1.z, l(64), r0.xyzw
  mov r2.y, l(0)
  store_structured u0.xy, r1.z, l(80), r2.xyxx
endif 
ret 
// Approximately 331 instruction slots used
