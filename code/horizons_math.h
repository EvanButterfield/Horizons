/* date = February 13th 2024 10:58 am */

#ifndef HORIZONS_MATH_H
#define HORIZONS_MATH_H

typedef union vec2
{
  struct
  {
    f32 x, y;
  };
  f32 Elements[2];
} vec2;

typedef union vec3
{
  struct
  {
    f32 x, y, z;
  };
  struct
  {
    f32 r, g, b;
  };
  f32 Elements[3];
} vec3;

typedef union vec4
{
  struct
  {
    f32 x, y, z, w;
  };
  struct
  {
    f32 r, g, b, a;
  };
  f32 Elements[4];
} vec4;

typedef union mat3
{
  vec3 Rows[3];
  f32 Elements[3][3];
} mat3;

typedef union mat4
{
  vec4 Rows[4];
  f32 Elements[4][4];
} mat4;

#if !MATH_NO_FUNCS

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
Vec2DivideScalar(vec2 A, f32 B)
{
  vec2 Result;
  Result.x = A.x/B;
  Result.y = A.y/B;
  
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

internal inline f32
Vec3MulDot(vec3 A, vec3 B)
{
  f32 Result = A.x*B.x + A.y*B.y + A.z*B.z;
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
  f32 K = 1 / Vec3Length(V);
  vec3 Result = Vec3Scale(V, K);
  return(Result);
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
Vec4MulParts(vec4 A, vec4 B)
{
  vec4 Result;
  Result.x = A.x*B.x;
  Result.y = A.y*B.y;
  Result.z = A.z*B.z;
  Result.w = A.w*B.w;
  
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
Mat4Translate(mat4 M, vec3 Translation)
{
  vec4 First = Vec4Scale(M.Rows[0], Translation.x);
  vec4 Second = Vec4Scale(M.Rows[1], Translation.y);
  vec4 Third = Vec4Scale(M.Rows[2], Translation.z);
  
  vec4 Final = Vec4Add(First, Second);
  Final = Vec4Add(Final, Third);
  Final = Vec4Add(Final, M.Rows[3]);
  
  mat4 Result;
  Result.Rows[0] = M.Rows[0];
  Result.Rows[1] = M.Rows[1];
  Result.Rows[2] = M.Rows[2];
  Result.Rows[3] = Final;
  
  return(Result);
}

internal mat4
Mat4Rotate(mat4 M, f32 Angle, vec3 RawAxis)
{
  f32 C = cosf(Angle);
  f32 S = sinf(Angle);
  
  vec3 Axis = Vec3Normalize(RawAxis);
  vec3 Temp = Vec3Scale(Axis, 1 - C);
  
  mat3 Rotate;
  Rotate.Elements[0][0] = C + Temp.Elements[0]*Axis.Elements[0];
  Rotate.Elements[0][1] = Temp.Elements[0]*Axis.Elements[1] + S*Axis.Elements[2];
  Rotate.Elements[0][2] = Temp.Elements[0]*Axis.Elements[2] - S*Axis.Elements[1];
  
  Rotate.Elements[1][0] = Temp.Elements[1]*Axis.Elements[0] - S*Axis.Elements[2];
  Rotate.Elements[1][1] = C + Temp.Elements[1]*Axis.Elements[1];
  Rotate.Elements[1][2] = Temp.Elements[1]*Axis.Elements[2] + S*Axis.Elements[0];
  
  Rotate.Elements[2][0] = Temp.Elements[2]*Axis.Elements[0] + S*Axis.Elements[1];
  Rotate.Elements[2][1] = Temp.Elements[2]*Axis.Elements[1] - S*Axis.Elements[0];
  Rotate.Elements[2][2] = C + Temp.Elements[2]*Axis.Elements[2];
  
  vec4 First, Second, Third;
  
  mat4 Result;
  First = Vec4Scale(M.Rows[0], Rotate.Elements[0][0]);
  Second = Vec4Scale(M.Rows[1], Rotate.Elements[0][1]);
  Third = Vec4Scale(M.Rows[2], Rotate.Elements[0][2]);
  Result.Rows[0] = Vec4Add(Vec4Add(First, Second), Third);
  
  First = Vec4Scale(M.Rows[0], Rotate.Elements[1][0]);
  Second = Vec4Scale(M.Rows[1], Rotate.Elements[1][1]);
  Third = Vec4Scale(M.Rows[2], Rotate.Elements[1][2]);
  Result.Rows[1] = Vec4Add(Vec4Add(First, Second), Third);
  
  First = Vec4Scale(M.Rows[0], Rotate.Elements[2][0]);
  Second = Vec4Scale(M.Rows[1], Rotate.Elements[2][1]);
  Third = Vec4Scale(M.Rows[2], Rotate.Elements[2][2]);
  Result.Rows[2] = Vec4Add(Vec4Add(First, Second), Third);
  
  Result.Rows[3] = M.Rows[3];
  return(Result);
}

internal mat4
Mat4Scale(mat4 M, vec3 Scale)
{
  mat4 Result;
  Result.Rows[0] = Vec4Scale(M.Rows[0], Scale.x);
  Result.Rows[1] = Vec4Scale(M.Rows[1], Scale.y);
  Result.Rows[2] = Vec4Scale(M.Rows[2], Scale.z);
  Result.Rows[3] = M.Rows[3];
  
  return(Result);
}

internal mat4
Mat4CreateTransform(b32 UseTopLeft, vec2 Pos, f32 Angle, vec2 Scale, window_dimension _WindowDimension)
{
  vec2 WindowDimension = {(f32)_WindowDimension.Width, (f32)_WindowDimension.Height};
  Pos = Vec2Add(Pos, Vec2DivideScalar(WindowDimension, 2));
  if(UseTopLeft)
  {
    Pos = Vec2Add(Pos, Vec2DivideScalar(Scale, 2));
  }
  
  mat4 Result = Mat4Identity();
  Result = Mat4Translate(Result, (vec3){Pos.x, Pos.y, 0});
  Result = Mat4Rotate(Result, Angle, (vec3){0, 0, 1});
  Result = Mat4Scale(Result, (vec3){Scale.x, Scale.y, 1});
  
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

#endif

#endif //HORIZONS_MATH_H
