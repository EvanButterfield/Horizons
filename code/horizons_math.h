/* date = February 13th 2024 10:58 am */

#ifndef HORIZONS_MATH_H
#define HORIZONS_MATH_H

// Thank you to everyone that made Handmade_Math.h for a lot of the equations used
// https://github.com/HandmadeMath/HandmadeMath

#include <math.h>

// NOTE(evan): This exists because I don't want to
//             cast to f32 everytime I use fabs
internal inline f32 Abs(f32 Val)
{
  f32 Result = (f32)fabs(Val);
  return(Result);
}

internal inline vec2
Vec2Add(vec2 A, vec2 B)
{
  vec2 Result;
  Result.x = A.x + B.x;
  Result.y = A.y + B.y;
  
  return(Result);
}

internal inline vec2
Vec2Subtract(vec2 A, vec2 B)
{
  vec2 Result;
  Result.x = A.x - B.x;
  Result.y = A.y - B.y;
  
  return(Result);
}

internal inline vec2
Vec2Scale(vec2 A, f32 B)
{
  vec2 Result;
  Result.x = A.x*B;
  Result.y = A.y*B;
  
  return(Result);
}

internal inline vec2
Vec2DivideScalar(vec2 A, f32 B)
{
  vec2 Result;
  Result.x = A.x/B;
  Result.y = A.y/B;
  
  return(Result);
}

internal inline f32
Vec2MulDot(vec2 A, vec2 B)
{
  f32 Result = A.x*B.x + A.y*B.y;
  return(Result);
}

internal inline f32
Vec2Length(vec2 V)
{
  f32 Result = sqrtf(Vec2MulDot(V, V));
  return(Result);
}

internal inline vec2
Vec2Normalize(vec2 V)
{
  f32 Length = Vec2Length(V);
  if(Length == 0)
  {
    return(Vec2(0, 0));
  }
  f32 K = 1 / Length;
  vec2 Result = Vec2Scale(V, K);
  return(Result);
}

internal inline vec3
Vec3Add(vec3 A, vec3 B)
{
  vec3 Result;
  Result.x = A.x + B.x;
  Result.y = A.y + B.y;
  Result.z = A.z + B.z;
  
  return(Result);
}

internal inline vec3
Vec3AddScalar(vec3 A, f32 B)
{
  vec3 Result = Vec3Add(A, Vec3(B, B, B));
  return(Result);
}

internal inline vec3
Vec3Subtract(vec3 A, vec3 B)
{
  vec3 Result;
  Result.x = A.x - B.x;
  Result.y = A.y - B.y;
  Result.z = A.z - B.z;
  
  return(Result);
}

internal inline vec3
Vec3SubtractScalar(vec3 A, f32 B)
{
  vec3 Result = Vec3Subtract(A, Vec3(B, B, B));
  return(Result);
}

internal inline vec3
Vec3Scale(vec3 A, f32 B)
{
  vec3 Result;
  Result.x = A.x*B;
  Result.y = A.y*B;
  Result.z = A.z*B;
  
  return(Result);
}

internal inline vec3
Vec3Mul(vec3 A, vec3 B)
{
  vec3 Result;
  Result.x = A.x*B.x;
  Result.y = A.y*B.y;
  Result.z = A.z*B.z;
  
  return(Result);
}

internal inline f32
Vec3MulDot(vec3 A, vec3 B)
{
  f32 Result = A.x*B.x + A.y*B.y + A.z*B.z;
  return(Result);
}

internal inline vec3
Vec3Cross(vec3 A, vec3 B)
{
  vec3 Result;
  Result.x = A.y*B.z - A.z*B.y;
  Result.y = A.z*B.x - A.x*B.z;
  Result.z = A.x*B.y + A.y*B.x;
  
  return(Result);
}

internal inline vec3
Vec3MulMat3(vec3 V, mat3 *M)
{
  vec3 Result;
  Result.x = V.x*M->Elements[0][0] + V.y*M->Elements[0][1] + V.z*M->Elements[0][2];
  Result.y = V.x*M->Elements[1][0] + V.y*M->Elements[1][1] + V.z*M->Elements[1][2];
  Result.z = V.x*M->Elements[2][0] + V.y*M->Elements[2][1] + V.z*M->Elements[2][2];
  
  return(Result);
}

internal inline f32
Vec3Length(vec3 V)
{
  f32 Result = sqrtf(Vec3MulDot(V, V));
  return(Result);
}

internal vec3
Vec3Normalize(vec3 V)
{
  f32 Length = Vec3Length(V);
  if(Length == 0)
  {
    return((vec3){0, 0, 0});
  }
  f32 K = 1 / Length;
  vec3 Result = Vec3Scale(V, K);
  return(Result);
}

internal vec3
Vec3FPEulerToRotation(f32 Pitch, f32 Yaw)
{
  vec3 Direction;
  Direction.Pitch = cosf(Yaw)*cosf(Pitch);
  Direction.Yaw = sinf(Pitch);
  Direction.Roll = sinf(Yaw)*cosf(Pitch);
  Direction = Vec3Normalize(Direction);
  
  return(Direction);
}

internal vec4
Vec4Add(vec4 A, vec4 B)
{
  vec4 Result;
  Result.x = A.x + B.x;
  Result.y = A.y + B.y;
  Result.z = A.z + B.z;
  Result.w = A.w + B.w;
  
  return(Result);
}

internal inline vec4
Vec4Scale(vec4 A, f32 B)
{
  vec4 Result;
  Result.x = A.x*B;
  Result.y = A.y*B;
  Result.z = A.z*B;
  Result.w = A.w*B;
  
  return(Result);
}

internal inline vec4
Vec4Mul(vec4 A, vec4 B)
{
  vec4 Result;
  Result.x = A.x*B.x;
  Result.y = A.y*B.y;
  Result.z = A.z*B.z;
  Result.w = A.w*B.w;
  
  return(Result);
}

internal vec4
Vec4MulMat4(vec4 V, mat4 *M)
{
  vec4 Result;
  Result.x = V.x*M->Elements[0][0] + V.y*M->Elements[0][1] + V.z*M->Elements[0][2] + V.w*M->Elements[0][3];
  Result.y = V.x*M->Elements[1][0] + V.y*M->Elements[1][1] + V.z*M->Elements[1][2] + V.w*M->Elements[1][3];
  Result.z = V.x*M->Elements[2][0] + V.y*M->Elements[2][1] + V.z*M->Elements[2][2] + V.w*M->Elements[2][3];
  Result.w = V.x*M->Elements[3][0] + V.y*M->Elements[3][1] + V.z*M->Elements[3][2] + V.w*M->Elements[3][3];
  
  return(Result);
}

internal inline mat3
Mat3FromMat4(mat4 *M)
{
  mat3 Result =
  {
    M->Rows[0].x, M->Rows[0].y, M->Rows[0].z,
    M->Rows[1].x, M->Rows[1].y, M->Rows[1].z,
    M->Rows[2].x, M->Rows[2].y, M->Rows[2].z,
  };
  
  return(Result);
}

internal inline mat4
Mat4Identity(void)
{
  mat4 Result =
  {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  };
  return(Result);
}

internal mat4
Mat4Mul(mat4 A, mat4 B)
{
  mat4 Result =
  {
    A.Elements[0][0] * B.Elements[0][0] + A.Elements[0][1] * B.Elements[1][0] + A.Elements[0][2] * B.Elements[2][0] + A.Elements[0][3] * B.Elements[3][0],
    A.Elements[0][0] * B.Elements[0][1] + A.Elements[0][1] * B.Elements[1][1] + A.Elements[0][2] * B.Elements[2][1] + A.Elements[0][3] * B.Elements[3][1],
    A.Elements[0][0] * B.Elements[0][2] + A.Elements[0][1] * B.Elements[1][2] + A.Elements[0][2] * B.Elements[2][2] + A.Elements[0][3] * B.Elements[3][2],
    A.Elements[0][0] * B.Elements[0][3] + A.Elements[0][1] * B.Elements[1][3] + A.Elements[0][2] * B.Elements[2][3] + A.Elements[0][3] * B.Elements[3][3],
    A.Elements[1][0] * B.Elements[0][0] + A.Elements[1][1] * B.Elements[1][0] + A.Elements[1][2] * B.Elements[2][0] + A.Elements[1][3] * B.Elements[3][0],
    A.Elements[1][0] * B.Elements[0][1] + A.Elements[1][1] * B.Elements[1][1] + A.Elements[1][2] * B.Elements[2][1] + A.Elements[1][3] * B.Elements[3][1],
    A.Elements[1][0] * B.Elements[0][2] + A.Elements[1][1] * B.Elements[1][2] + A.Elements[1][2] * B.Elements[2][2] + A.Elements[1][3] * B.Elements[3][2],
    A.Elements[1][0] * B.Elements[0][3] + A.Elements[1][1] * B.Elements[1][3] + A.Elements[1][2] * B.Elements[2][3] + A.Elements[1][3] * B.Elements[3][3],
    A.Elements[2][0] * B.Elements[0][0] + A.Elements[2][1] * B.Elements[1][0] + A.Elements[2][2] * B.Elements[2][0] + A.Elements[2][3] * B.Elements[3][0],
    A.Elements[2][0] * B.Elements[0][1] + A.Elements[2][1] * B.Elements[1][1] + A.Elements[2][2] * B.Elements[2][1] + A.Elements[2][3] * B.Elements[3][1],
    A.Elements[2][0] * B.Elements[0][2] + A.Elements[2][1] * B.Elements[1][2] + A.Elements[2][2] * B.Elements[2][2] + A.Elements[2][3] * B.Elements[3][2],
    A.Elements[2][0] * B.Elements[0][3] + A.Elements[2][1] * B.Elements[1][3] + A.Elements[2][2] * B.Elements[2][3] + A.Elements[2][3] * B.Elements[3][3],
    A.Elements[3][0] * B.Elements[0][0] + A.Elements[3][1] * B.Elements[1][0] + A.Elements[3][2] * B.Elements[2][0] + A.Elements[3][3] * B.Elements[3][0],
    A.Elements[3][0] * B.Elements[0][1] + A.Elements[3][1] * B.Elements[1][1] + A.Elements[3][2] * B.Elements[2][1] + A.Elements[3][3] * B.Elements[3][1],
    A.Elements[3][0] * B.Elements[0][2] + A.Elements[3][1] * B.Elements[1][2] + A.Elements[3][2] * B.Elements[2][2] + A.Elements[3][3] * B.Elements[3][2],
    A.Elements[3][0] * B.Elements[0][3] + A.Elements[3][1] * B.Elements[1][3] + A.Elements[3][2] * B.Elements[2][3] + A.Elements[3][3] * B.Elements[3][3],
  };
  
  return(Result);
}

internal mat4
Mat4Inverse(mat4 *M)
{
  f32 S[6];
  S[0] = M->Elements[0][0]*M->Elements[1][1] - M->Elements[1][0]*M->Elements[0][1];
  S[1] = M->Elements[0][0]*M->Elements[1][2] - M->Elements[1][0]*M->Elements[0][2];
  S[2] = M->Elements[0][0]*M->Elements[1][3] - M->Elements[1][0]*M->Elements[0][3];
  S[3] = M->Elements[0][1]*M->Elements[1][2] - M->Elements[1][1]*M->Elements[0][2];
  S[4] = M->Elements[0][1]*M->Elements[1][3] - M->Elements[1][1]*M->Elements[0][3];
  S[5] = M->Elements[0][2]*M->Elements[1][3] - M->Elements[1][2]*M->Elements[0][3];
  
  f32 C[6];
  C[0] = M->Elements[2][0]*M->Elements[3][1] - M->Elements[3][0]*M->Elements[2][1];
  C[1] = M->Elements[2][0]*M->Elements[3][2] - M->Elements[3][0]*M->Elements[2][2];
  C[2] = M->Elements[2][0]*M->Elements[3][3] - M->Elements[3][0]*M->Elements[2][3];
  C[3] = M->Elements[2][1]*M->Elements[3][2] - M->Elements[3][1]*M->Elements[2][2];
  C[4] = M->Elements[2][1]*M->Elements[3][3] - M->Elements[3][1]*M->Elements[2][3];
  C[5] = M->Elements[2][2]*M->Elements[3][3] - M->Elements[3][2]*M->Elements[2][3];
  
  f32 Idet = 1.0f/(S[0]*C[5] - S[1]*C[4] + S[2]*C[3] + S[3]*C[2] - S[4]*C[1] + S[5]*C[0]);
  
  mat4 Result;
  Result.Elements[0][0] = ( M->Elements[1][1]*C[5] - M->Elements[1][2]*C[4] + M->Elements[1][3]*C[3])*Idet;
  Result.Elements[0][1] = (-M->Elements[0][1]*C[5] + M->Elements[0][2]*C[4] - M->Elements[0][3]*C[3])*Idet;
  Result.Elements[0][2] = ( M->Elements[3][1]*S[5] - M->Elements[3][2]*S[4] + M->Elements[3][3]*S[3])*Idet;
  Result.Elements[0][3] = (-M->Elements[2][1]*S[5] + M->Elements[2][2]*S[4] - M->Elements[2][3]*S[3])*Idet;
  
  Result.Elements[1][0] = (-M->Elements[1][0]*C[5] + M->Elements[1][2]*C[2] - M->Elements[1][3]*C[1])*Idet;
  Result.Elements[1][1] = ( M->Elements[0][0]*C[5] - M->Elements[0][2]*C[2] + M->Elements[0][3]*C[1])*Idet;
  Result.Elements[1][2] = (-M->Elements[3][0]*S[5] + M->Elements[3][2]*S[2] - M->Elements[3][3]*S[1])*Idet;
  Result.Elements[1][3] = ( M->Elements[2][0]*S[5] - M->Elements[2][2]*S[2] + M->Elements[2][3]*S[1])*Idet;
  
  Result.Elements[2][0] = ( M->Elements[1][0]*C[4] - M->Elements[1][1]*C[2] + M->Elements[1][3]*C[0])*Idet;
  Result.Elements[2][1] = (-M->Elements[0][0]*C[4] + M->Elements[0][1]*C[2] - M->Elements[0][3]*C[0])*Idet;
  Result.Elements[2][2] = ( M->Elements[3][0]*S[4] - M->Elements[3][1]*S[2] + M->Elements[3][3]*S[0])*Idet;
  Result.Elements[2][3] = (-M->Elements[2][0]*S[4] + M->Elements[2][1]*S[2] - M->Elements[2][3]*S[0])*Idet;
  
  Result.Elements[3][0] = (-M->Elements[1][0]*C[3] + M->Elements[1][1]*C[1] - M->Elements[1][2]*C[0])*Idet;
  Result.Elements[3][1] = ( M->Elements[0][0]*C[3] - M->Elements[0][1]*C[1] + M->Elements[0][2]*C[0])*Idet;
  Result.Elements[3][2] = (-M->Elements[3][0]*S[3] + M->Elements[3][1]*S[1] - M->Elements[3][2]*S[0])*Idet;
  Result.Elements[3][3] = ( M->Elements[2][0]*S[3] - M->Elements[2][1]*S[1] + M->Elements[2][2]*S[0])*Idet;
  
  return(Result);
}

internal mat4
Mat4Transpose(mat4 *M)
{
  mat4 Result =
  {
    M->Rows[0].x, M->Rows[1].x, M->Rows[2].x, M->Rows[3].x,
    M->Rows[0].y, M->Rows[1].y, M->Rows[2].y, M->Rows[3].y,
    M->Rows[0].z, M->Rows[1].z, M->Rows[2].z, M->Rows[3].z,
    M->Rows[0].w, M->Rows[1].w, M->Rows[2].w, M->Rows[3].w
  };
  
  return(Result);
}

internal mat4
Mat4Translate(vec3 Translation)
{
  mat4 Result = Mat4Identity();
  Result.Elements[3][0] = Translation.x;
  Result.Elements[3][1] = Translation.y;
  Result.Elements[3][2] = Translation.z;
  
  return(Result);
}

internal mat4
Mat4Rotate(f32 Angle, vec3 Axis)
{
  mat4 Result = Mat4Identity();
  Angle = -Angle;
  Axis = Vec3Normalize(Axis);
  
  f32 S = sinf(Angle);
  f32 C = cosf(Angle);
  f32 CVal = 1 - C;
  
  Result.Elements[0][0] = (Axis.x*Axis.x*CVal) + C;
  Result.Elements[0][1] = (Axis.x*Axis.y*CVal) + (Axis.z*S);
  Result.Elements[0][2] = (Axis.x*Axis.z*CVal) - (Axis.y*S);
  
  Result.Elements[1][0] = (Axis.y*Axis.x*CVal) - (Axis.z*S);
  Result.Elements[1][1] = (Axis.y*Axis.y*CVal) + C;
  Result.Elements[1][2] = (Axis.y*Axis.z*CVal) + (Axis.x*S);
  
  Result.Elements[2][0] = (Axis.z*Axis.x*CVal) + (Axis.y*S);
  Result.Elements[2][1] = (Axis.z*Axis.y*CVal) - (Axis.x*S);
  Result.Elements[2][2] = (Axis.z*Axis.z*CVal) + C;
  
  return(Result);
}

internal mat4
Mat4Scale(vec3 Scale)
{
  mat4 Result = {0};
  Result.Elements[0][0] = Scale.x;
  Result.Elements[1][1] = Scale.y;
  Result.Elements[2][2] = Scale.z;
  Result.Elements[3][3] = 1;
  
  return(Result);
}

internal mat4
Mat4Orthographic(f32 Left, f32 Right, f32 Bottom, f32 Top, f32 Near, f32 Far)
{
  f32 RightMLeft = Right - Left;
  f32 TopMBottom = Top - Bottom;
  f32 FarMNear = Far - Near;
  
  mat4 Result = {0};
  Result.Elements[0][0] = 2 / RightMLeft;
  Result.Elements[1][1] = 2 / TopMBottom;
  Result.Elements[2][2] = -2 / FarMNear;
  Result.Elements[3][0] = -(Right + Left) / RightMLeft;
  Result.Elements[3][1] = -(Top + Bottom) / TopMBottom;
  Result.Elements[3][2] = -(Far + Near) / FarMNear;
  Result.Elements[3][3] = 1;
  
  return(Result);
}

internal mat4
Mat4Perspective(f32 Aspect, f32 FOV, f32 Near, f32 Far)
{
  FOV *= DEG_TO_RAD;
  // Assert(fabs(Aspect - EPSILON) > 0);
  
  mat4 Result = {0};
  f32 Cotan = 1 / tanf(FOV / 2);
  Result.Elements[0][0] = Cotan / Aspect;
  Result.Elements[1][1] = Cotan;
  Result.Elements[2][3] = 1;
  Result.Elements[2][2] = -(Far / (Near - Far));
  Result.Elements[3][2] = Near*Far / (Near - Far);
  
  return(Result);
}

internal mat4
Mat4LookAt(vec3 Eye, vec3 Center, vec3 Up)
{
  vec3 F = Vec3Normalize(Vec3Subtract(Center, Eye));
  vec3 S = Vec3Normalize(Vec3Cross(F, Up));
  vec3 U = Vec3Cross(S, F);
  
  mat4 Result = {0};
  Result.Elements[0][0] = S.x;
  Result.Elements[0][1] = U.x;
  Result.Elements[0][2] = -F.x;
  Result.Elements[0][3] = 0;
  
  Result.Elements[1][0] = S.y;
  Result.Elements[1][1] = U.y;
  Result.Elements[1][2] = -F.y;
  Result.Elements[1][3] = 0;
  
  Result.Elements[2][0] = S.z;
  Result.Elements[2][1] = U.z;
  Result.Elements[2][2] = -F.z;
  Result.Elements[2][3] = 0;
  
  Result.Elements[3][0] = -Vec3MulDot(S, Eye);
  Result.Elements[3][1] = -Vec3MulDot(U, Eye);
  Result.Elements[3][2] = Vec3MulDot(F, Eye);
  Result.Elements[3][3] = 1;
  
  return(Result);
}

internal mat4
Mat4View(vec3 Position, vec3 Rotation, vec3 Up)
{
  mat4 Result = Mat4LookAt(Position, Vec3Add(Position, Rotation), Up);
  return(Result);
}

internal mat4
Mat4CreateTransform3D(vec3 Position, vec3 Rotation, vec3 Scale)
{
  mat4 TranslateM = Mat4Translate(Position);
  mat4 RotateM;
  if(Rotation.x || Rotation.y || Rotation.z)
  {
    vec3 RotationRadians = Vec3Scale(Rotation, DEG_TO_RAD);
    RotateM = Mat4Rotate(Vec3Length(RotationRadians), Vec3Normalize(RotationRadians));
  }
  else
  {
    RotateM = Mat4Identity();
  }
  mat4 ScaleM = Mat4Scale(Scale);
  
  mat4 Result = Mat4Mul(ScaleM, RotateM);
  Result = Mat4Mul(Result, TranslateM);
  
  return(Result);
}

internal mat4
Mat4CreateTransform2D(vec3 Pos, vec3 Rotation, vec3 Scale, window_dimension _WindowDimension)
{
  vec3 NewScale = Vec3(Scale.x/_WindowDimension.Width,
                       Scale.y/_WindowDimension.Height,
                       Scale.z);
  mat4 Result = Mat4CreateTransform3D(Pos, Rotation, NewScale);
  
  return(Result);
}

#endif //HORIZONS_MATH_H
