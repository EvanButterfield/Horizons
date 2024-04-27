/* date = December 8th 2023 9:06 am */

#ifndef HORIZONS_TYPES_H
#define HORIZONS_TYPES_H

#if !defined(COMPILER_MSVC)
#define COMPILER_MSVC 0
#endif

#if !defined(COMPILER_LLVM)
#define COMPILER_LLVM 0
#endif

#if !COMPILER_MSVC && !COMPILER_LLVM
#if _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#else
#undef COMPILER_LLVM
#define COMPILER_LLVM
#endif
#endif

#if _WIN32|_WIN64
#define OS_WIN 1
#endif

#if unix|__unix|__unix__
#define OS_UNIX 1
#endif

#if __APPLE__
#define OS_MAC 1
#endif

#define internal static
#define global static

#define true 1
#define false 0

#define Kibibytes(Value) (Value*1024)
#define Mebibytes(Value) (Kibibytes(Value)*1024LL)
#define Gibibytes(Value) (Mebibytes(Value)*1024LL)
#define Tebibytes(Value) (Gibibytes(Value)*1024LL)

#define Pi32 3.14159265359f
#define DEG_TO_RAD (Pi32 / 180)
#define EPSILON 1.19209290E-07F

#if HORIZONS_DEBUG
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif

#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))

#define STR2(X) #X
#define STR(X) STR2(X)

#define MAX_ENTITIES 128

#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t  s64;

typedef float f32;
typedef double f64;

typedef s32 b32;

typedef size_t memory_index;

typedef struct string8
{
  s8 *Str;
  s32 Length;
} string8;
#define String8(Str, Length) (string8){(Str), (Length)}
#define String8Plain(Str) (string8){(Str), ArrayCount(Str) - 1}

#pragma pack(push, 1)
typedef union vec2
{
  struct
  {
    f32 x, y;
  };
  struct
  {
    f32 x_, z;
  };
  f32 Elements[2];
} vec2;

#define Vec2(x, y) (vec2){(f32)(x), (f32)(y)}

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
  struct
  {
    f32 Pitch, Yaw, Roll;
  };
  f32 Elements[3];
} vec3;

#define Vec3(x, y, z) (vec3){(x), (y), (z)}
#define Vec3GreaterThan(A, B) (((A).x > (B).x) && ((A).y > (B).y) && ((A).z > (B).z))
#define Vec3LessThan(A, B) (((A).x < (B).x) && ((A).y < (B).y) && ((A).z < (B).z))

typedef union vec4
{
  struct
  {
    f32 x, y, z, w;
  };
  struct
  {
    vec3 xyz;
    f32 w_;
  };
  struct
  {
    f32 r, g, b, a;
  };
  f32 Elements[4];
} vec4;

#define Vec4(x, y, z, w) (vec4){(x), (y), (z), (w)}
#define Vec4FromVec3(xyz, w) (vec4){(xyz).x, (xyz).y, (xyz).z, w}

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
#pragma pack(pop)

internal inline u32
SafeTruncateUInt64(u64 Value)
{
  Assert(Value <= 0xFFFFFFFF);
  u32 Result = (u32)Value;
  return(Result);
}

#endif //HORIZONS_TYPES_H
