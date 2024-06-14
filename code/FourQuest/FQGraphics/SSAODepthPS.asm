//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float       
// POSITION                 0   xyz         1     NONE   float     z 
// NORMAL                   0   xyz         2     NONE   float   xyz 
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_TARGET                0   xyzw        0   TARGET   float   xyzw
//
ps_5_0
dcl_globalFlags refactoringAllowed
dcl_input_ps linear v1.z
dcl_input_ps linear v2.xyz
dcl_output o0.xyzw
dcl_temps 1
dp3 r0.x, v2.xyzx, v2.xyzx
rsq r0.x, r0.x
mul o0.xyz, r0.xxxx, v2.xyzx
mov o0.w, v1.z
ret 
// Approximately 5 instruction slots used
